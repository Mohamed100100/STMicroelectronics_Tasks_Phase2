#ifndef HTTPCLASS_H
#define HTTPCLASS_H

#include <stdint.h>
#include "TCPclass.h"

class httpclass_t : public tcpclass_t
{
    public:
   const uint8_t *data_start;
   uint16_t httpDataLength;

    httpclass_t();
    httpclass_t(const unsigned char *packet);

    // method to call all print functions
    void PrintAllHTTPDetails();
} ;


#endif
