#include "https_curl.h"
#include "https_curl_req.h"
#include "https_curl_helpers.h"
#include <string.h>
#include <stdlib.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

//=====================================================================================   
//=========================== FUNCTION DEFINITIONS ====================================
//=====================================================================================

//============ INITIALIZATION FUNCTIONS =============================
/*init the requester object*/
ReqHTTPS* https_init(char* url, char* cert, CertType certype) { //init post request
    
	ReqHTTPS* req = NULL;
    CURLcode res;
		
	req = (ReqHTTPS *)malloc(sizeof(ReqHTTPS));
	if (req == NULL) {
		return NULL;
	}
        
    req->_resp = NULL;
    req->_len = 0;
	req->_url = url;
	
    
    //init libcurl global lib
    curl_global_init(CURL_GLOBAL_ALL);

    if ((req->_curl = curl_easy_init()) == NULL) {
		curl_global_cleanup();
		return NULL;
	}
    
    //follow location if a redirect is sent from server
    curl_easy_setopt(req->_curl, CURLOPT_FOLLOWLOCATION, 1);
    
    //set url
    curl_easy_setopt(req->_curl, CURLOPT_URL, url);

#ifdef DEBUG
    curl_easy_setopt(req->_curl, CURLOPT_VERBOSE, 1L);
#endif

    if(cert != NULL){
        req->_ca_crt = cert;
         //turn off default ca_certificate source
        curl_easy_setopt(req->_curl, CURLOPT_CAINFO, NULL);
        curl_easy_setopt(req->_curl, CURLOPT_CAPATH, NULL);

        //authenticate  the certificate signature
        curl_easy_setopt(req->_curl, CURLOPT_SSL_VERIFYPEER, 1L);
        /* Set the default value: strict certificate check please */
        curl_easy_setopt(req->_curl, CURLOPT_PROXY_SSL_VERIFYPEER, 1L);
        //verify CN(Common Name) certificate parameter
        curl_easy_setopt(req->_curl, CURLOPT_SSL_VERIFYHOST, 2L);
        /* Set the default value: strict name check please */
        curl_easy_setopt(req->_curl, CURLOPT_PROXY_SSL_VERIFYHOST, 2L);
        //set the format of certificate
        if(certype == DER)
            curl_easy_setopt(req->_curl, CURLOPT_SSLCERTTYPE, "DER");
        else if(certype == PEM)
            curl_easy_setopt(req->_curl, CURLOPT_SSLCERTTYPE, "PEM");
            
        //set the function to load and check de certificate
        res = curl_easy_setopt(req->_curl, CURLOPT_SSL_CTX_FUNCTION, sslctx_callback);
        if(res != CURLE_OK){ //you must to recompile Curl lib to support OpenSSL or another valid SSL lib 
            #ifdef DEBUG
            printf("ERROR: cUrl without valid SSL dependence to use ssl-context callback,\
                    recompile cUrl with valid SSL lib");
            #endif
            
            return NULL;
        }
    
        //pass the certificate to be loaded
        res = curl_easy_setopt(req -> _curl, CURLOPT_SSL_CTX_DATA, req->_ca_crt);
        #ifdef DEBUG
        if(res == CURLE_OK)
            printf("SSL Certificate Suported\n");
        else if(res == CURLE_NOT_BUILT_IN)
            printf("SSL is not supported in backend, recompile\n");
        else if(res == CURLE_UNKNOWN_OPTION)
            printf("CURL option is not recognized\n");
        #endif
    }
        
	return req;
}

REQ_RET https_set_url(ReqHTTPS* req, char* url, char* ca_cert, CertType certype){

    CURLcode res;

    if(certype == DER)
        curl_easy_setopt(req->_curl, CURLOPT_SSLCERTTYPE, "DER");
    else if(certype == PEM)
        curl_easy_setopt(req->_curl, CURLOPT_SSLCERTTYPE, "PEM");
    
     //pass the certificate to be loaded
    res = curl_easy_setopt(req -> _curl, CURLOPT_SSL_CTX_DATA, ca_cert);
    #ifdef DEBUG
    if(res == CURLE_OK)
        printf("SSL Certificate Suported\n");
    else if(res == CURLE_NOT_BUILT_IN)
        printf("SSL is not supported in backend, recompile\n");
    else if(res == CURLE_UNKNOWN_OPTION)
        printf("CURL option is not recognized\n");
    #endif

    if(CURLE_OK != curl_easy_setopt(req->_curl, CURLOPT_URL, url)){
       return REQ_N_OK;
    }

   return REQ_SUCCESS;
}



//make the request depending on Method parameter value 
REQ_RET https_req_perform(ReqHTTPS* req, Method method, unsigned char* data, unsigned long data_len, long timeout, char* headersList[], size_t hd_am) {
        
    REQ_RET ret = REQ_N_OK;
        
	struct curl_slist *headers = NULL;
    
        //add header to the list
    if(headersList != NULL && hd_am > 0){
        
        while(hd_am--){
            //set header list to send in http method
            headers = curl_slist_append(headers, headersList[hd_am]); //don't matter the header order
            curl_easy_setopt(req->_curl, CURLOPT_HTTPHEADER, headers);
        }
     }
    
    //check the right method for this function
	if(method > DELETE || method < GET){
		return REQ_NDEF_METHOD;
	}

	switch (method) {
		
        case GET:
			ret = get_https_data(req, data, data_len, timeout);
			break;
		
        case POST:
            ret =  post_https_data(req, data, data_len, timeout);
			break;
		
        case PUT:
			ret =  put_https_data(req, data, data_len, timeout);
			break;
                        
        case DELETE:
            ret =  delete_https_data(req, data, data_len, timeout);
            break;
                
        default:
			ret =  REQ_NDEF_METHOD;
			break;
	}

        //clear header list
	if(headersList != NULL && hd_am > 0) curl_slist_free_all(headers);
        
    curl_easy_setopt(req->_curl, CURLOPT_HTTPHEADER, NULL); //clear headers
        
    return ret;
}

//get headers from request
unsigned char * https_resp_headers(ReqHTTPS* req, unsigned long * headers_len){
    (*headers_len) = req->_head_len;
    return req->_headers;
}

//this function return the body of https response
unsigned char * https_resp_body(ReqHTTPS* req, unsigned long* data_len){
    (*data_len) = req->_len;
	return req->_resp;

}


/*clean and free the memory*/
void https_curl_cleanup(ReqHTTPS *req) {

	curl_easy_cleanup(req->_curl);
        curl_global_cleanup();
    
	if (req != NULL) {
        free(req->_headers);
		free(req->_resp);
		free(req);
	}

	return;
}

