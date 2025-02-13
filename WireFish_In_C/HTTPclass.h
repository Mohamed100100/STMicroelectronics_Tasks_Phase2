#ifndef HTTPCLASS_H
#define HTTPCLASS_H

#include <stdint.h>
#include "TCPclass.h"

typedef struct httpclass {

    tcpclass_t *tcpPart;                //inhertance 

   const uint8_t *data_start;
   uint16_t httpDataLength;

    // method to call all print functions
    void (*PrintAllHTTPDetails)(struct httpclass *HTTPObj);
} httpclass_t;


httpclass_t* ConstructHTTPObject( const unsigned char *packet);
void DeconstructHTTPObject(httpclass_t *httpclassObj);

#endif
