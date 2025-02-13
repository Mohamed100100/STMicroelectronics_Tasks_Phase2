#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>

#include "HTTPclass.h"

static void http_PrintAllHTTPDetails(httpclass_t *HTTPObj) {
    if (HTTPObj == NULL) {
        printf("HTTP object is NULL.\n");
        return;
    }
    fwrite(HTTPObj->data_start, 1, HTTPObj->httpDataLength, stdout);
}


httpclass_t* ConstructHTTPObject( const unsigned char *packet) {
    httpclass_t *httpObj = (httpclass_t *)malloc(sizeof(httpclass_t));
    if (!httpObj) {
        fprintf(stderr, "Failed to allocate memory for HTTP object.\n");
        return NULL;
    }

    tcpclass_t *tcpObj = ConstructTCPObject(packet);
   if (!tcpObj) {
        free(httpObj);
        fprintf(stderr, "Failed to construct TCP object.\n");
        return NULL;
    }
    httpObj->data_start = packet + 14 + (tcpObj->ipPart->ihl)*4 + tcpObj->offset*4;

    uint16_t http_data_length = ntohs(tcpObj->ipPart->tot_len) - (tcpObj->ipPart->ihl* 4) - (tcpObj->offset * 4);
    httpObj->httpDataLength = http_data_length;

    httpObj->tcpPart = tcpObj;

    httpObj->PrintAllHTTPDetails = http_PrintAllHTTPDetails;

    return httpObj;
}

void DeconstructHTTPObject(httpclass_t *httpObj) {
  if (httpObj) {
    DeconstructTCPObject(httpObj->tcpPart);
    free(httpObj);
  }
}