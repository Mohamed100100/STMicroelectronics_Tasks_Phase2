#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "FTPclass.h"

static void ftp_PrintAllFTPDetails(ftpclass_t *FTPObj) {
    if (FTPObj == NULL) {
        printf("FTP object is NULL.\n");
        return;
    }
    fwrite(FTPObj->data_start, 1, FTPObj->ftpDataLength, stdout);
}



ftpclass_t *ConstructFTPObject(const unsigned char *packet) {
    ftpclass_t *ftpObj = (ftpclass_t *)malloc(sizeof(ftpclass_t));
    if (!ftpObj) {
        fprintf(stderr, "Failed to allocate memory for FTP object.\n");
        return NULL;
    }

    // Create TCP object to handle TCP-level details
    tcpclass_t *tcpObj = ConstructTCPObject(packet);
    if (!tcpObj) {
        free(ftpObj);
        fprintf(stderr, "Failed to construct TCP object.\n");
        return NULL;
    }

    uint16_t data_length = ntohs(tcpObj->ipPart->tot_len) - (tcpObj->ipPart->ihl* 4) - (tcpObj->offset * 4);

    // Pointer to the start of FTP data in the packet
    ftpObj->data_start = packet + 14 + (tcpObj->ipPart->ihl) * 4 + tcpObj->offset*4;

    ftpObj->ftpDataLength = data_length;

    ftpObj->tcpPart = tcpObj;

    // Assign function pointers
    ftpObj->PrintAllFTPDetails = ftp_PrintAllFTPDetails;

    return ftpObj;
}

void DeconstructFTPObject(ftpclass_t *ftpObj) {
    if (ftpObj) {
        DeconstructTCPObject(ftpObj->tcpPart);
        free(ftpObj);
    }
}
