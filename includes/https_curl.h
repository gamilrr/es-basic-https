#ifndef HTTPS_CURL_H
#define HTTPS_CURL_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
//============================================================================
//========================== RETURN CODE ENUMS ===============================
//============================================================================

//define the status returned form function
typedef enum REQ_RET_E {
	REQ_SUCCESS,//all went fine, the data will be in ReqHTTPS.buffer and 
                //its length in Req.HTTP.len
	REQ_N_OK,   //the response from server was different from 200-OK
	REQ_NDEF_METHOD, //the method in ReqHTTPS.method is not defined 
	REQ_FAIL_CURL, //can't initialize CURL library
	REQ_FAIL_DMEMORY, //there was a failed attempt to allocate dynamic memory
	REQ_EMPTY_BODY, //the get request was fine but gave a empty body
}REQ_RET;

//============================================================================
//========================== OPTIONS INIT ENUMS ==============================
//============================================================================
//enum to define the method to make the request
typedef enum METHOD {
	GET,
	POST,
	PUT,
    DELETE
}Method;

//format of certificate
typedef enum CERT_TYPE{
    PEM,
    DER
}CertType;


//============================================================================
//========================= HTTPS PROPERTIES OBJECTS =========================
//============================================================================

//============ MAIN REQUESTER OBJECT ==============
/*structure to define the request properties and send/store the 
request/response HTTP body */
typedef struct {
    CURL* _curl;
	unsigned char *_headers; //store a list od headers
    unsigned long _head_len; //headers length
    unsigned char *_resp; //store the HTTP body of response	
    unsigned long _len;//buffer length to send
	char * _url;//URL to make the request
    char * _ca_crt; // set the CA certificate to validate ssl certificates, in format PEM
    
}ReqHTTPS;


//==============================================================================
//========================== EXPORTED FUNCTIONS ================================
//==============================================================================

/*Initialize the request properties and return a ReqHTTPS object pointer, the
 url must be https schema and cert must be PEM format*/
ReqHTTPS* https_init(char* url, char* cert, CertType certype);

/*Set url and certificates*/
REQ_RET https_set_url(ReqHTTPS* req, char* url, char* ca_cert, CertType certype);

/*This function is used to perform the request depending on the method 
 parameter*/
REQ_RET https_req_perform(ReqHTTPS* req, Method m, unsigned char* data, unsigned long data_len, long timeout, char* headersList[], size_t hd_am);

/*This function return a array of string with the headers the server response in request*/
unsigned char * https_resp_headers(ReqHTTPS* req, unsigned long * headers_len);


/*This function return the ReqHTTPS response body and set the length*/
unsigned char * https_resp_body(ReqHTTPS* req, unsigned long* data_len);

/*Released the memory and close the library*/
void https_curl_cleanup(ReqHTTPS *);


#endif

