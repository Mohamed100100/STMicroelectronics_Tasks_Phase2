#include <stdio.h>      
#include <stdlib.h>     
#include <fcntl.h>    
#include <sys/mman.h>   
#include <sys/stat.h>   
#include <semaphore.h>  
#include <unistd.h>    
#include <string.h>   
#include <signal.h>    
#include <stdbool.h>   
#include <mqueue.h>   
#include <errno.h>     


// Name of the shared memory object
#define SHM_NAME                                "/shop_memory"       
// Name of the semaphore for synchronization 
#define SEM_NAME                                "/shop_semaphore"     
// Maximum number of items the shop can hold
#define MAX_ITEMS 50                   

/* Message queue names */
// Queue for inventory updates
#define INVENTORY_QUEUE             "/inventory_queue"  
// Queue for customer orders 
#define CUSTOMER_QUEUE              "/customer_queue"   
// response queues
#define RESPONSE_QUEUE_PREFIX       "/response_queue_"   

/* Maximum size for a message in bytes */
#define MAX_MSG_SIZE 256

/**
 * Structure representing the shared memory between all processes
 * Contains all the shop's inventory data
 */
typedef struct {
    char item_names[MAX_ITEMS][50];  // Array of item names, each up to 50 chars
    int stock[MAX_ITEMS];            // Current stock level for each item
    float prices[MAX_ITEMS];         // Price of each item
    int item_count;                  // Number of different items in the shop
    bool is_active;                  // Flag indicating if the shop is open
} SharedMemory;


/**
 * Structure for customer order messages
 * Sent from customers to the shop
 */
typedef struct {
    int customer_id;             // Unique ID of the customer
    int item_id;                 // ID of the item being ordered
    int quantity;                // Quantity of the item being ordered
    char response_queue[64];     // Name of the queue to send the response to
} OrderMessage;


/**
 * Structure for order response messages
 * Sent from the shop back to customers
 */
typedef struct {
    bool success;            // Whether the order was successful
    char message[128];       // Detailed response message
} ResponseMessage;

/**
 * Structure for inventory update notifications
 * Sent from inventory managers to the shop
 */
typedef struct {
    int manager_id;          // ID of the manager making the update
} InventoryUpdateMessage;


/* Global flag for controlling program execution, modified by signal handler */
volatile int keep_running = 1;

/**
 * Signal handler for graceful termination
 * Sets the keep_running flag to 0 when a signal is received
 * 
 * @param sig Signal number
 */
void handle_signal(int sig) {
    // Set flag to terminate the main loop
    keep_running = 0;  
}

/**
 * Prints the current inventory to stdout
 * 
 * @param shm Pointer to the shared memory structure
 */
void print_inventory(SharedMemory *shm) {
    printf("\n--- Current Inventory ---\n");
    for (int i = 0; i < shm->item_count; i++) {
        printf("%d) %s - $%.2f (%d in stock)\n", 
               i + 1, shm->item_names[i], shm->prices[i], shm->stock[i]);
    }
    printf("------------------------\n");
}

/**
 * Initializes the shop process
 * Creates shared memory, semaphores, and message queues
 * Runs the main shop loop to process customer orders and inventory updates
 */
void initialize_shop() {
    // Create and configure the shared memory
    // Create or open shared memory object
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);  
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    
    // Set the size of the shared memory to be equal to the (SharedMemory object)
    if (ftruncate(fd, sizeof(SharedMemory)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    
    // Map the shared memory object into this process's address space
    // So the shm pointer can access the Shop database
    SharedMemory *shm = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    
    // Create and initialize semaphore for synchronizing access to shared memory
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    
    // Create message queues with specific attributes
    struct mq_attr attr;
    attr.mq_flags = 0;              // Blocking mode
    attr.mq_maxmsg = 10;            // Maximum number of messages in queue
    attr.mq_msgsize = MAX_MSG_SIZE; // Maximum message size
    attr.mq_curmsgs = 0;            // Current number of messages (initialized to 0)
    
    // Create queue for inventory managers to send updates
    mqd_t inventory_queue = mq_open(INVENTORY_QUEUE, O_CREAT | O_RDWR | O_NONBLOCK, 0666, &attr);
    if (inventory_queue == (mqd_t)-1) {
        perror("mq_open inventory_queue");
        exit(EXIT_FAILURE);
    }
    
    // Create queue for customers to send orders
    mqd_t customer_queue = mq_open(CUSTOMER_QUEUE, O_CREAT | O_RDWR | O_NONBLOCK, 0666, &attr);
    if (customer_queue == (mqd_t)-1) {
        perror("mq_open customer_queue");
        exit(EXIT_FAILURE);
    }
    
    // Initialize shop data with default items

    // Acquire semaphore before modifying shared memory
    sem_wait(sem);  
    
    // Add first item: T-Shirt
    strcpy(shm->item_names[0], "T-Shirt");
    shm->stock[0] = 10;
    shm->prices[0] = 20.0;
    
    // Add second item: Jeans
    strcpy(shm->item_names[1], "Jeans");
    shm->stock[1] = 5;
    shm->prices[1] = 50.0;
    
    // Initialize with 2 items
    shm->item_count = 2;       
    // Set shop as active (بسم الله الله اكبر اصطبحنا واصطبح الملك لله :))
    shm->is_active = true;     
    
    printf("Shop initialized with %d items\n", shm->item_count);

    // Print current inventory at startup
    print_inventory(shm);

    // Release semaphore after finishing initialization
    sem_post(sem);  
    
    // Set up signal handling for clean shutdown (e.g., Ctrl+C)
    signal(SIGINT, handle_signal);
    
    // Prepare message structs for communication
    InventoryUpdateMessage inv_update;
    OrderMessage order;
    ResponseMessage response;
       
    // Variable to store number of bytes read from message queues
    ssize_t bytes_read;
    
    // Main shop loop - processing messages from queues
    // loop while (Ctrl+C) is pressed and make keep_running = 0
    while (keep_running) {
       
        // Check inventory queue for updates from managers (non-blocking)
        bytes_read = mq_receive(inventory_queue, (char *)&inv_update, MAX_MSG_SIZE, NULL);
        if (bytes_read > 0) {
            printf("Received inventory update notification from manager #%d\n", inv_update.manager_id);
            
            // Print updated inventory
            sem_wait(sem);
            printf("\n--- UPDATED INVENTORY ---\n");
            print_inventory(shm);
            sem_post(sem);
        }
        
        // Check customer order queue for new orders (non-blocking)
        bytes_read = mq_receive(customer_queue, (char *)&order, MAX_MSG_SIZE, NULL);
        if (bytes_read > 0) {
            printf("Received order from customer #%d for item #%d, quantity %d\n", 
                   order.customer_id, order.item_id, order.quantity);
            
            // Process the order with exclusive access to shared memory
            sem_wait(sem);
            
            
            // Check if order is valid (item exists and enough stock available)
            if (order.item_id > 0 && order.item_id <= shm->item_count && 
                order.quantity > 0 && order.quantity <= shm->stock[order.item_id - 1]) {
                
                // Fulfill the order by reducing stock
                shm->stock[order.item_id - 1] -= order.quantity;
                response.success = true;
                
                // Create success message with transaction details
                snprintf(response.message, sizeof(response.message), 
                        "Order successful! Purchased %d %s(s) for $%.2f", 
                        order.quantity, shm->item_names[order.item_id - 1], 
                        order.quantity * shm->prices[order.item_id - 1]);
                printf("Successfully :) \n");
            } else {
                // Handle invalid orders (non-existent item or insufficient stock)
                response.success = false;
                strcpy(response.message, "Order failed! Invalid item or insufficient stock.");
                printf("Failed :( \n");
            }
            
            // Print updated inventory after processing the order
            print_inventory(shm);
            sem_post(sem);
            
            // Send response back to the specific customer via their response queue
            mqd_t response_queue = mq_open(order.response_queue, O_WRONLY);
            if (response_queue != (mqd_t)-1) {
                if (mq_send(response_queue, (char *)&response, sizeof(ResponseMessage), 0) == -1) {
                    perror("mq_send response");
                }
                mq_close(response_queue);         
            }
        }
    }
    
    // Cleanup when shop is shutting down
    sem_wait(sem);
    shm->is_active = false;  // Mark shop as inactive
    sem_post(sem);
    
    // Release all allocated resources
    munmap(shm, sizeof(SharedMemory));  // Unmap shared memory
    close(fd);                          // Close shared memory file descriptor
    sem_close(sem);                     // Close semaphore
    mq_close(inventory_queue);          // Close inventory message queue
    mq_close(customer_queue);           // Close customer message queue
    
    printf("Shop closed.\n");
}

/**
 * Implements the customer process functionality
 * Allows a customer to view inventory and place orders
 */
void customer_process() {
    // Generate a unique customer ID based on the process ID
    int customer_id = getpid() % 10000;
    
    // Create a unique response queue name for this customer
    char response_queue_name[64];
    snprintf(response_queue_name, sizeof(response_queue_name), "%s%d", RESPONSE_QUEUE_PREFIX, customer_id);
    
    // Set up message queue attributes
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    
    // Create a response queue for this customer
    mqd_t response_queue = mq_open(response_queue_name, O_CREAT | O_RDWR, 0666, &attr);
    if (response_queue == (mqd_t)-1) {
        perror("mq_open response_queue");
        exit(EXIT_FAILURE);
    }
    
    // Open the customer order queue for sending orders to the shop
    mqd_t customer_queue = mq_open(CUSTOMER_QUEUE, O_WRONLY);
    if (customer_queue == (mqd_t)-1) {
        perror("mq_open customer_queue");
        mq_close(response_queue);
        mq_unlink(response_queue_name);
        exit(EXIT_FAILURE);
    }
    
    // Connect to existing shared memory
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        printf("Error: Shop is not initialized. Please start the shop first.\n");
        mq_close(customer_queue);
        mq_close(response_queue);
        mq_unlink(response_queue_name);
        exit(EXIT_FAILURE);
    }
    
    // Map shared memory into this process's address space
    SharedMemory *shm = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        mq_close(customer_queue);
        mq_close(response_queue);
        mq_unlink(response_queue_name);
        exit(EXIT_FAILURE);
    }
    
    // Open the semaphore for synchronization
    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        mq_close(customer_queue);
        mq_close(response_queue);
        mq_unlink(response_queue_name);
        exit(EXIT_FAILURE);
    }
    
    // Set up signal handling for clean shutdown
    signal(SIGINT, handle_signal);

    // check if the shop is opened or not
    if (!shm->is_active) {
        printf("We are sory :( >>  Shop is closed.\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Customer #%d session started\n", customer_id);
    
    // Main customer interaction loop
    while (keep_running && shm->is_active) {
        // Take semaphore, check if shop is active, print inventory, then release
        sem_wait(sem);
        
        if (!shm->is_active) {
            printf("We are sory :( >>  Shop is closed.\n");
            sem_post(sem);
            break;
        }
        
        // Display customer portal with current inventory
        printf("\n--- Customer #%d Portal ---", customer_id);
        print_inventory(shm);
        
        sem_post(sem);
        
        // Get customer order input
        int choice, qty;
        printf("Enter item number to purchase (0 to Refresh ... -1 to exit): ");
        scanf("%d", &choice);
        
        if (choice == -1) {
            break;  // Exit if customer chooses 0
        }

        if (choice == 0) {
            continue;  // Exit if customer chooses 0
        }
        
        printf("Enter quantity: ");
        scanf("%d", &qty);
        
        // Create and send order message to the shop
        OrderMessage order;
        order.customer_id = customer_id;
        order.item_id = choice;
        order.quantity = qty;
        strcpy(order.response_queue, response_queue_name);  // Tell shop where to send response
        
        if (mq_send(customer_queue, (char *)&order, sizeof(OrderMessage), 0) == -1) {
            perror("mq_send order");
            continue;  // Try again on failure
        }
        
        printf("Order sent to shop. Waiting for response...\n");
        
        // Wait for response from the shop
        ResponseMessage response;
        ssize_t bytes_read = mq_receive(response_queue, (char *)&response, MAX_MSG_SIZE, NULL);
        
        if (bytes_read > 0) {
            // Display order response from shop
            printf("\n--- Order Status ---\n");
            printf("%s\n", response.message);
            printf("-------------------\n");
        } else {
            printf("Error receiving response from shop.\n");
        }
    }
    
    // Cleanup resources before exiting
    munmap(shm, sizeof(SharedMemory));
    close(fd);
    sem_close(sem);
    mq_close(customer_queue);
    mq_close(response_queue);
    mq_unlink(response_queue_name);  // Important: remove the customer-specific queue
    
    printf("Customer #%d session ended.\n", customer_id);
}

/**
 * Implements the inventory manager process functionality
 * Allows managers to restock items, update prices, and add new items
 */
void inventory_manager() {
    // Generate a unique manager ID based on the process ID
    int manager_id = getpid() % 10000;
    
    // Open inventory update queue for sending updates to the shop
    mqd_t inventory_queue = mq_open(INVENTORY_QUEUE, O_WRONLY);
    if (inventory_queue == (mqd_t)-1) {
        perror("mq_open inventory_queue");
        exit(EXIT_FAILURE);
    }
    
    // Connect to existing shared memory
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        printf("Error: Shop is not initialized. Please start the shop first.\n");
        mq_close(inventory_queue);
        exit(EXIT_FAILURE);
    }
    
    // Map shared memory into this process's address space
    SharedMemory *shm = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        mq_close(inventory_queue);
        exit(EXIT_FAILURE);
    }
    
    // Open the semaphore for synchronization
    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        mq_close(inventory_queue);
        exit(EXIT_FAILURE);
    }
    
    // Set up signal handling for clean shutdown
    signal(SIGINT, handle_signal);
    
    printf("Inventory Manager #%d session started\n", manager_id);
    
    // Main inventory manager interaction loop
    while (keep_running && shm->is_active) {
        sem_wait(sem);
        
        // Check if shop is still active
        if (!shm->is_active) {
            printf("Shop is closed.\n");
            sem_post(sem);
            break;
        }
        
        // Display inventory management interface
        printf("\n--- Inventory Management #%d ---\n", manager_id);
        print_inventory(shm);
        
        sem_post(sem);
        
        // Display available actions
        printf("Choose action:\n");
        printf("1. Restock item\n");
        printf("2. Update price\n");
        printf("3. Add new item\n");
        printf("4. Get the latest update\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        
        int action;
        scanf("%d", &action);
        
        if (action == 0) {
            break;  // Exit if manager chooses 0
        }
        
        // Take semaphore before modifying shared memory
        sem_wait(sem);
        
        bool updated = false;  // Track if an update was made
        
        // Process manager's action
        switch (action) {
            case 1: { // Restock an existing item
                int index, value;
                printf("Enter item number to restock: ");
                scanf("%d", &index);
                
                // Validate item number
                if (index < 1 || index > shm->item_count) {
                    printf("Invalid item number!\n");
                } else {
                    printf("Enter quantity to add: ");
                    scanf("%d", &value);
                    
                    // Validate quantity
                    if (value < 0) {
                        printf("Invalid quantity!\n");
                    } else {
                        // Update the stock
                        shm->stock[index - 1] += value;
                        printf("Stock updated! New stock for %s: %d\n", 
                               shm->item_names[index - 1], shm->stock[index - 1]);
                        updated = true;
                    }
                }
                break;
            }
            
            case 2: { // Update price of an existing item
                int index;
                float new_price;
                
                printf("Enter item number to update price: ");
                scanf("%d", &index);
                
                // Validate item number
                if (index < 1 || index > shm->item_count) {
                    printf("Invalid item number!\n");
                } else {
                    printf("Enter new price: ");
                    scanf("%f", &new_price);
                    
                    // Validate price
                    if (new_price < 0) {
                        printf("Invalid price!\n");
                    } else {
                        // Update the price
                        shm->prices[index - 1] = new_price;
                        printf("Price updated! New price for %s: $%.2f\n", 
                               shm->item_names[index - 1], shm->prices[index - 1]);
                        updated = true;
                    }
                }
                break;
            }
            
            case 3: { // Add a new item to the shop
                // Check if shop has reached maximum capacity
                if (shm->item_count >= MAX_ITEMS) {
                    printf("Cannot add more items. Maximum capacity reached!\n");
                } else {
                    // Get new item details
                    char name[50];
                    int stock;
                    float price;
                    
                    printf("Enter new item name: ");
                    scanf(" %49[^\n]", name);  // Read a string with spaces
                    
                    printf("Enter initial stock: ");
                    scanf("%d", &stock);
                    
                    printf("Enter price: ");
                    scanf("%f", &price);
                    
                    // Validate inputs
                    if (stock < 0 || price < 0) {
                        printf("Invalid stock or price values!\n");
                    } else {
                        // Add the new item
                        strcpy(shm->item_names[shm->item_count], name);
                        shm->stock[shm->item_count] = stock;
                        shm->prices[shm->item_count] = price;
                        shm->item_count++;
                        
                        printf("New item added successfully! Total items: %d\n", shm->item_count);
                        updated = true;
                    }
                }
                break;
            }
            case 4: { //Get the latest update
                // Do nothing Contintue
                break;
            }
            
            default:
                printf("Invalid option!\n");
        }
        
        sem_post(sem);
        
        // If inventory was updated, notify the shop
        if (updated) {
            InventoryUpdateMessage update;
            update.manager_id = manager_id;
            
            // Send update notification to shop
            if (mq_send(inventory_queue, (char *)&update, sizeof(InventoryUpdateMessage), 0) == -1) {
                perror("mq_send update");
            } else {
                printf("Inventory update notification sent to shop.\n");
            }
        }
    }
    
    // Cleanup resources before exiting
    munmap(shm, sizeof(SharedMemory));
    close(fd);
    sem_close(sem);
    mq_close(inventory_queue);
    
    printf("Inventory Manager #%d session ended.\n", manager_id);
}

/**
 * Cleans up all system resources used by the shop system
 * Should be called when the entire system is being shut down
 */
void cleanup_resources() {
    // Remove the shared memory object
    shm_unlink(SHM_NAME);
    
    // Remove the semaphore
    sem_unlink(SEM_NAME);
    
    // Remove message queues
    mq_unlink(INVENTORY_QUEUE);
    mq_unlink(CUSTOMER_QUEUE);
    
    // Note: Can't unlink all response queues here as they have dynamic names
    // They should be unlinked by each customer process upon exit
    
    printf("Resources cleaned up.\n");
}

/**
 * Main function : Parses command line arguments to determine which mode to run
 */
int main(int argc, char *argv[]) {
    // Check if a command line argument was provided
    if (argc != 2 ) {
        printf("Usage: %s [shop|customer|manager|cleanup]\n", argv[0]);
        return 1;
    }
    
    // Run the appropriate function based on the command line argument
    if (strcmp(argv[1], "shop") == 0) {
        initialize_shop();             // Start the shop process
    } else if (strcmp(argv[1], "customer") == 0) {
        customer_process();            // Start a customer process
    } else if (strcmp(argv[1], "manager") == 0) {
        inventory_manager();           // Start an inventory manager process
    } else if (strcmp(argv[1], "cleanup") == 0) {
        cleanup_resources();           // Clean up all resources
    } else {
        printf("Invalid option\n");    // Invalid command line argument
    }
    
    return 0;
}