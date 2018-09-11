#include <stdio.h>
#include "https_curl.h"
#include "ca_certificates.h"
#include <stdlib.h>

int main()
{
    //Request Handler init
    ReqHTTPS* req = https_init("https://www.google.com", ca_cert_google, DER);
    if(req == NULL){
        printf("Error: Trying to assign dynamic memory\n");
        return 0;
    }

    REQ_RET res;

    char* headers_list[] = {"Transfer-Encoding:"};
    res = https_req_perform(req, GET, NULL, 0, 3, headers_list, 1);
    if (res != REQ_SUCCESS) {
        printf("Error: the request was not good\n");
        return 0;
    }

    res = https_set_url(req, "https://twitter.com", ca_cert_twitter, PEM);

    res = https_req_perform(req, GET, NULL, 0, 3, headers_list, 1);
    if (res != REQ_SUCCESS) {
        printf("Error: the request was not good\n");
        return 0;
    }

    //  printf("This is the headers++++++++++++++:\n%s\n", req->_headers);
    //  printf("This is the response body+++++++++++++:\n%s\n", req->_resp);

    https_curl_cleanup(req);
}

