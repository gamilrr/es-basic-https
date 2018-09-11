#ifndef HTTPS_CURL_REQ_H
#define HTTPS_CURL_REQ_H

#include "https_curl.h"

/*theses functions make a CRUD request to server and store in ReqHTTPS.resp the server
response body and in ReqHTTPS.len its length*/
REQ_RET get_https_data(ReqHTTPS * req, unsigned char* data, unsigned long data_len, long timeout); //GET function
REQ_RET post_https_data(ReqHTTPS *, unsigned char* data, unsigned long data_len, long timeout); //POST function
REQ_RET put_https_data(ReqHTTPS *, unsigned char* data, unsigned long data_len, long timeout); //PUT function
REQ_RET delete_https_data(ReqHTTPS* req, unsigned char* data, unsigned long data_len, long timeout); //DELETE function

#endif
