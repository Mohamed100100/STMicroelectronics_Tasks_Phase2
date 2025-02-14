#ifndef FTPCLASS_H
#define FTPCLASS_H

#include "TCPclass.h"

struct ftpclass_t : public tcpclass_t 
{
    public :
    const uint8_t *data_start;
    uint16_t ftpDataLength;

    ftpclass_t();
    ftpclass_t(const unsigned char *packet);

    // Function pointers for printing FTP details
    void PrintCommand();
    void PrintArguments();

    // method to call all print functions
    void PrintAllFTPDetails();
} ;

#endif // FTPCLASS_H
