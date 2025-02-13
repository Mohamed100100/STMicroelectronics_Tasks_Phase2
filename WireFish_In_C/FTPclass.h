#ifndef FTPCLASS_H
#define FTPCLASS_H

#include "TCPclass.h"

typedef struct ftpclass {
    tcpclass_t *tcpPart;            // inheritance
    
    const uint8_t *data_start;
    uint16_t ftpDataLength;


    // Function pointers for printing FTP details
    void (*PrintCommand)(struct ftpclass *FTPObj);
    void (*PrintArguments)(struct ftpclass *FTPObj);

    // method to call all print functions
    void (*PrintAllFTPDetails)(struct ftpclass *FTPObj);
} ftpclass_t;

// Constructor and Destructor
ftpclass_t *ConstructFTPObject(const unsigned char *packet);
void DeconstructFTPObject(ftpclass_t *ftpObj);

#endif // FTPCLASS_H
