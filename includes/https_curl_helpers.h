#ifndef HTTPS_CURL_HELPERS_H
#define HTTPS_CURL_HELPERS_H

#include "https_curl.h"

//structure to arrange the data sending 
typedef struct {
	unsigned char *buffer;
	unsigned long len;
	unsigned long pos;
} ReqChunk;

//snippet to copy from a var in heap to another one
int copy_heap(unsigned char **dest, unsigned char *src, unsigned long len);

//callback function such is called when the process receives the server response body
unsigned long write_callback(void *ptr, unsigned long size, unsigned long nmemb, void* s);

//callback function such is called each time a data chunk will be sent to server 
unsigned long read_callback(void *ptr, unsigned long size, unsigned long nitems, void *stream);

//callback to read headers from http request
unsigned long headers_callback(void *ptr, unsigned long size, unsigned long nitems, void *stream);

//callback to set and verify the certificate in SSL handshake
CURLcode sslctx_callback(CURL *curl, void *sslctx, void *parm);

#endif
