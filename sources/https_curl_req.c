#include <stdlib.h>
#include "https_curl_req.h"
#include "https_curl_helpers.h"
#include <curl/curl.h>


/*make GET request*/
REQ_RET get_https_data(ReqHTTPS *req, unsigned char* data, unsigned long data_len, long timeout){

    CURLcode res;
	ReqChunk bReq;
	ReqChunk bRes; //temp structure to arrange the data sending from server
    ReqChunk hRes; //temp structure to get headers

    //init  the temp structure
	bReq.buffer = data;
	bReq.len = data_len;
	bReq.pos = 0;

    //reser method
    res  = curl_easy_setopt(req->_curl, CURLOPT_CUSTOMREQUEST, NULL);
    res = curl_easy_setopt(req->_curl, CURLOPT_UPLOAD, 1); //say: i have data to send,  enable uploading

	res = curl_easy_setopt(req->_curl, CURLOPT_CUSTOMREQUEST, "GET");

    //set callback function to send data
	res = curl_easy_setopt(req->_curl, CURLOPT_READFUNCTION, read_callback);
	res = curl_easy_setopt(req->_curl, CURLOPT_READDATA, &bReq);


    //init temp ReqChunk to be ready to accept the HTTP body response.
	bRes.len = 0;
	bRes.buffer = (unsigned char*)malloc(1);
	if (bRes.buffer == NULL) {
		return REQ_FAIL_DMEMORY;
	}

    hRes.len = 0;
    hRes.buffer = (unsigned char* )malloc(1);
    if(hRes.buffer == NULL){
        return REQ_FAIL_DMEMORY;
    }

    //set the callback function to get headers
    res = curl_easy_setopt(req->_curl, CURLOPT_HEADERFUNCTION, headers_callback);
    res = curl_easy_setopt(req->_curl, CURLOPT_HEADERDATA, &hRes);

	//set the callback function to read data
	res = curl_easy_setopt(req->_curl, CURLOPT_WRITEFUNCTION, write_callback);
	res = curl_easy_setopt(req->_curl, CURLOPT_WRITEDATA, &bRes);

	curl_easy_setopt(req->_curl, CURLOPT_POSTFIELDSIZE, bReq.len);
	curl_easy_setopt(req->_curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
	
        
    //set a request timeout default 3 seconds
    res = curl_easy_setopt(req->_curl, CURLOPT_TIMEOUT, (timeout) > 0 ? (timeout) : 3L);

    //makes the request and returns the server HTTP STATUS CODE ref: https://curl.haxx.se
	res = curl_easy_perform(req->_curl);

	//check the request status 
	if (res != CURLE_OK) {
		free(bRes.buffer);
        free(hRes.buffer);
		return REQ_N_OK;
	}

	//check if the HTTP body was empty or not 
	if (bRes.len == 0) {
		free(bRes.buffer);
		free(hRes.buffer);
        return REQ_EMPTY_BODY;
	}

	req->_len = bRes.len;
	
	if (!copy_heap(&(req->_resp), bRes.buffer, bRes.len)) {
		free(bRes.buffer);
		free(hRes.buffer);
        return REQ_FAIL_DMEMORY;
	}

    if(!copy_heap(&(req->_headers), hRes.buffer, hRes.len)){
        free(bRes.buffer);
        free(hRes.buffer);
        return REQ_FAIL_DMEMORY;
    }

    req->_head_len = hRes.len;

	free(bRes.buffer);
    free(hRes.buffer);
	
    return REQ_SUCCESS;
}

/*make POST request*/
REQ_RET post_https_data(ReqHTTPS* req, unsigned char* data, unsigned long data_len, long timeout)
{      
	volatile CURLcode res;
	ReqChunk bReq;
	ReqChunk bRes; //temp structure to arrange the data sending from server
    ReqChunk hRes;	
	   
        //init  the temp structure
	bReq.buffer = data;
	bReq.len = data_len;
	bReq.pos = 0;

    //reset method
    curl_easy_setopt(req->_curl, CURLOPT_CUSTOMREQUEST, NULL);
    curl_easy_setopt(req->_curl, CURLOPT_HTTPGET, 1L);

	//set the method POST 
	curl_easy_setopt(req->_curl, CURLOPT_POST, 1);

    //set callback function to send data
	curl_easy_setopt(req->_curl, CURLOPT_READFUNCTION, read_callback);
	curl_easy_setopt(req->_curl, CURLOPT_READDATA, &bReq);

    
    hRes.len = 0;
    hRes.buffer = (unsigned char* )malloc(1);
    if(hRes.buffer == NULL){
        return REQ_FAIL_DMEMORY;
    }

    //set the callback function to get headers
    res = curl_easy_setopt(req->_curl, CURLOPT_HEADERFUNCTION, headers_callback);
    res = curl_easy_setopt(req->_curl, CURLOPT_HEADERDATA, &hRes);

    //init temp ReqChunk to be ready to accept the HTTP body response.
	bRes.len = 0;
	bRes.buffer = (unsigned char*)malloc(1); 
	if (bRes.buffer == NULL) {
		return REQ_FAIL_DMEMORY;
	}

	//set the callback function to read data	
	curl_easy_setopt(req->_curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(req->_curl, CURLOPT_WRITEDATA, &bRes);


	curl_easy_setopt(req->_curl, CURLOPT_POSTFIELDSIZE, bReq.len); 
	curl_easy_setopt(req->_curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

    //set a request timeout default 3 seconds
    curl_easy_setopt(req->_curl, CURLOPT_TIMEOUT, (timeout) > 0 ? (timeout) : 3L);

        //makes the request and returns the server HTTP STATUS CODE ref: https://curl.haxx.se
	res = curl_easy_perform(req->_curl);

	
	//check the request status 
	if (res != CURLE_OK) {
		free(bRes.buffer);
        free(hRes.buffer);
        return REQ_N_OK;
	}

	//check if the HTTP body was empty or not 
	if (bRes.len == 0) {
		free(bRes.buffer);
		free(hRes.buffer);
        return REQ_EMPTY_BODY;
	}

	req->_len = bRes.len;
	
	if (!copy_heap(&(req->_resp), bRes.buffer, bRes.len)) {
	    free(bRes.buffer);
		free(hRes.buffer);
        return REQ_FAIL_DMEMORY;
	}
    
    if(!copy_heap(&(req->_headers), hRes.buffer, hRes.len)){
        free(bRes.buffer);
        free(hRes.buffer);
        return REQ_FAIL_DMEMORY;
    }

    req->_head_len = hRes.len;

	free(bRes.buffer);
    free(hRes.buffer);
   
	return REQ_SUCCESS;
}


/*make PUT request*/
REQ_RET put_https_data(ReqHTTPS* req, unsigned char* data, unsigned long data_len, long timeout)
{
	CURLcode res;
	ReqChunk bReq;
	ReqChunk bRes;//temp structure to arrange the data sending 
    ReqChunk hRes;
	
        //init  the temp structure 
	bReq.buffer = data;
	bReq.len = data_len;
	bReq.pos = 0;

            //reset method
    curl_easy_setopt(req->_curl, CURLOPT_CUSTOMREQUEST, NULL);
    curl_easy_setopt(req->_curl, CURLOPT_HTTPGET, 1L);
        
	//set the method PUT 
    curl_easy_setopt(req->_curl, CURLOPT_PUT, 1L);
	curl_easy_setopt(req->_curl, CURLOPT_UPLOAD, 1L);

        //set callback function to send data
	curl_easy_setopt(req->_curl, CURLOPT_READFUNCTION, read_callback);
	curl_easy_setopt(req->_curl, CURLOPT_READDATA, &bReq);



        //init temp ReqChunk to be ready to accept the HTTP body response.
	bRes.len = 0;
	bRes.buffer = (unsigned char*)malloc(1); 
	if (bRes.buffer == NULL) {
		return REQ_FAIL_DMEMORY;
	}

    hRes.len = 0;
    hRes.buffer = (unsigned char* )malloc(1);
    if(hRes.buffer == NULL){
        return REQ_FAIL_DMEMORY;
    }

    //set the callback function to get headers
    res = curl_easy_setopt(req->_curl, CURLOPT_HEADERFUNCTION, headers_callback);
    res = curl_easy_setopt(req->_curl, CURLOPT_HEADERDATA, &hRes);

	//set the callback function to read data	
	curl_easy_setopt(req->_curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(req->_curl, CURLOPT_WRITEDATA, &bRes);

	curl_easy_setopt(req->_curl, CURLOPT_POSTFIELDSIZE, bReq.len); 
	curl_easy_setopt(req->_curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

    //set a request timeout default 3 seconds
    curl_easy_setopt(req->_curl, CURLOPT_TIMEOUT, (timeout) > 0 ? (timeout) : 3L);

    //makes the request and returns the server HTTP STATUS CODE ref: https://curl.haxx.se
	res = curl_easy_perform(req->_curl);

	//check the request status 
	if (res != CURLE_OK) {
		free(bRes.buffer);
		free(hRes.buffer);
        return REQ_N_OK;
	}

	//check if the HTTP body was empty or not 
	if (bRes.len == 0) {
		free(bRes.buffer);
		free(hRes.buffer);
        return REQ_EMPTY_BODY;
	}

	req->_len = bRes.len;
	
	if (!copy_heap(&(req->_resp), bRes.buffer, bRes.len)) {
		free(bRes.buffer);
        free(hRes.buffer);
		return REQ_FAIL_DMEMORY;
	}
    
    if(!copy_heap(&(req->_headers), hRes.buffer, hRes.len)){
        free(bRes.buffer);
        free(hRes.buffer);
        return REQ_FAIL_DMEMORY;
    }

    req->_head_len = hRes.len;

	free(bRes.buffer);
    free(hRes.buffer);

	return REQ_SUCCESS;
}


/*make PUT request*/
REQ_RET delete_https_data(ReqHTTPS* req, unsigned char* data, unsigned long data_len, long timeout){

    CURLcode res;
	ReqChunk bReq;
	ReqChunk bRes; //temp structure to arrange the data sending from server
    ReqChunk hRes;

    //init  the temp structure
	bReq.buffer = data;
	bReq.len = data_len;
	bReq.pos = 0;

    //reset method
    curl_easy_setopt(req->_curl, CURLOPT_CUSTOMREQUEST, NULL);
    curl_easy_setopt(req->_curl, CURLOPT_HTTPGET, 1L);

    curl_easy_setopt(req->_curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    curl_easy_setopt(req->_curl, CURLOPT_UPLOAD, 1); //say: i have data to send,  enable uploading

    hRes.len = 0;
    hRes.buffer = (unsigned char* )malloc(1);
    if(hRes.buffer == NULL){
        return REQ_FAIL_DMEMORY;
    }


    //set the callback function to get headers
    res = curl_easy_setopt(req->_curl, CURLOPT_HEADERFUNCTION, headers_callback);
    res = curl_easy_setopt(req->_curl, CURLOPT_HEADERDATA, &hRes);

    //set callback function to send data
	res = curl_easy_setopt(req->_curl, CURLOPT_READFUNCTION, read_callback);
	res = curl_easy_setopt(req->_curl, CURLOPT_READDATA, &bReq);

        //init temp ReqChunk to be ready to accept the HTTP body response.
	bRes.len = 0;
	bRes.buffer = (unsigned char*)malloc(1); 
	if (bRes.buffer == NULL) {
		return REQ_FAIL_DMEMORY;
	}

	//set the callback function to read data	
	curl_easy_setopt(req->_curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(req->_curl, CURLOPT_WRITEDATA, &bRes);


	curl_easy_setopt(req->_curl, CURLOPT_INFILESIZE,  bReq.len); 
	//curl_easy_setopt(req->_curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);


    //set a request timeout default 3 seconds
    curl_easy_setopt(req->_curl, CURLOPT_TIMEOUT, (timeout) > 0 ? (timeout) : 3L);

    //makes the request and returns the server HTTP STATUS CODE ref: https://curl.haxx.se
	res = curl_easy_perform(req->_curl);
	
	//check the request status 
	if (res != CURLE_OK) {
		free(bRes.buffer);
		free(hRes.buffer);
        return REQ_N_OK;
	}

	//check if the HTTP body was empty or not 
	if (bRes.len == 0) {
		free(bRes.buffer);
		free(hRes.buffer);
        return REQ_EMPTY_BODY;
	}

	req->_len = bRes.len;
	
	if (!copy_heap(&(req->_resp), bRes.buffer, bRes.len)) {
		free(bRes.buffer);
        free(hRes.buffer);
		return REQ_FAIL_DMEMORY;
	}
    
    if(!copy_heap(&(req->_headers), hRes.buffer, hRes.len)){
        free(bRes.buffer);
        free(hRes.buffer);
        return REQ_FAIL_DMEMORY;
    }

    req->_head_len = hRes.len;

	free(bRes.buffer);
    free(hRes.buffer);
    
    return REQ_SUCCESS;
}

