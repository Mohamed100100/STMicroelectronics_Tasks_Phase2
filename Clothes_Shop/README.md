# Clothes Shop System

A concurrent shared-memory communication system for retail shop management built with POSIX IPC (Inter-Process Communication) in C.

## Overview

This system simulates a retail shop environment with three main components:
- **Shop Service**: Central process that maintains inventory and processes transactions
- **Customer Interface**: Allows users to browse inventory and purchase items
- **Inventory Manager**: Enables administrators to restock items, update prices, and add new products

The system uses various IPC mechanisms for real-time communication between processes:
- Shared memory for inventory data
- Semaphores for synchronization
- Message queues for transaction processing

## Features

- **Real-time Inventory Management**
  - View current stock levels and prices
  - Restock existing items
  - Update product pricing
  - Add new products to inventory

- **Customer Shopping Experience**
  - Browse available products
  - Place orders with custom quantities
  - Receive immediate transaction feedback

- **Concurrent Process Architecture**
  - Multiple customers can shop simultaneously
  - Inventory managers can update stock while the shop is running
  - Thread-safe operations via semaphore synchronization

- **Robust Error Handling**
  - Graceful handling of invalid inputs
  - Signal handling for clean shutdown
  - Comprehensive error messaging

## System Requirements

- POSIX-compliant system (Linux/Unix)
- GCC compiler
- POSIX IPC libraries

## Installation

```bash
# Compile the program
gcc -o shop_system clothes.c
```

## Usage

The system provides four operational modes:

### 1. Start the Shop Service (must be started first)

```bash
./shop_system shop
```

### 2. Launch Customer Interface

```bash
./shop_system customer
```

### 3. Launch Inventory Manager

```bash
./shop_system manager
```

### 4. Clean Up System Resources

```bash
./shop_system cleanup
```

## How It Works

### Shared Memory Structure

The system uses shared memory to maintain a consistent view of inventory data across all processes:

```c
typedef struct {
    char item_names[MAX_ITEMS][50];  // Product names
    int stock[MAX_ITEMS];            // Stock levels
    float prices[MAX_ITEMS];         // Product prices
    int item_count;                  // Total number of products
    bool is_active;                  // Shop status flag
} SharedMemory;
```

### Message Queue Communication

Transactions and updates are communicated via message queues:

- **Customer Order Queue**: For sending purchase requests to the shop
- **Inventory Update Queue**: For notifying about inventory changes
- **Response Queues**: For sending transaction results back to customers

### Synchronization

Semaphores protect shared memory from race conditions during concurrent access.

## Example Workflow

1. Start the shop service
2. Launch multiple customer terminals to simulate concurrent shoppers
3. Launch inventory manager to update stock and prices
4. Observe real-time updates across all processes
5. Clean up resources when finished

## Technical Implementation

- **POSIX Shared Memory**: `shm_open()`, `mmap()` for shared data structure
- **POSIX Semaphores**: `sem_open()`, `sem_wait()`, `sem_post()` for synchronization
- **POSIX Message Queues**: `mq_open()`, `mq_send()`, `mq_receive()` for IPC
- **Signal Handling**: Graceful termination with `SIGINT` handling

## Limitations

- Maximum of 50 different items in inventory (configurable via `MAX_ITEMS`)
- Product names limited to 50 characters
- Message size limited to 256 bytes

