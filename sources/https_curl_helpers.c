#include "https_curl_helpers.h"
#include <openssl/err.h>
#include <openssl/ssl.h>

//======================================================================================
//=========================== HELPERS FUNCTION =========================================
//======================================================================================
//callback function to get headers
unsigned long headers_callback(void *ptr, unsigned long size, unsigned long nitems, void *stream)
{
	
	unsigned long data_recv = size * nitems;
	ReqChunk* b = (ReqChunk*)stream;
	unsigned long new_len = b->len + data_recv;

	b->buffer = (unsigned char*)realloc(b->buffer, new_len);

	if (b->buffer == NULL) {
		return 0;
	}

	memcpy(b->buffer + b->len, ptr, data_recv);

	b->len = new_len;

	return data_recv;
}


//callback function such is called when the process receives the server response body
unsigned long write_callback(void *ptr, unsigned long size, unsigned long nmemb, void* s)
{
	
	unsigned long data_recv = size * nmemb;
	ReqChunk* b = (ReqChunk*)s;
	unsigned long new_len = b->len + data_recv;

	b->buffer = (unsigned char*)realloc(b->buffer, new_len);

	if (b->buffer == NULL) {
		return 0;
	}

	memcpy(b->buffer + b->len, ptr, data_recv);

	b->len = new_len;

	return data_recv;
}

//callback function such is called each time a data chunk will be sent to server 
unsigned long read_callback(void *ptr, unsigned long size, unsigned long nitems, void *stream)
{
	unsigned long data_recv = size * nitems;
	ReqChunk *rarg = (ReqChunk *)stream;
	unsigned long len = rarg->len - rarg->pos;

	if (len > data_recv)
		len = data_recv;

	memcpy(ptr, rarg->buffer + rarg->pos, len);
	rarg->pos += len;

	return len;
}



//callback function to be called in ssl handshake this will verify the server certificate
CURLcode sslctx_callback(CURL *curl, void *sslctx, void *parm)
{
  CURLcode rv = CURLE_ABORTED_BY_CALLBACK;
  X509_STORE *store = NULL;
  X509 *cert = NULL;
  BIO *bio = NULL;
  char* pem_cert = (char* ) parm;
  /* clear the current thread's OpenSSL error queue */ 
  ERR_clear_error();
 
  /* get a BIO */ 
  bio = BIO_new_mem_buf(pem_cert, -1);
  if(!bio)
    goto err;
 
  /* use it to read the PEM formatted certificate from memory into an X509
   * structure that SSL can use
   */ 
  if(!PEM_read_bio_X509(bio, &cert, 0, NULL))
    goto err;
 
  /* get a pointer to the X509 certificate store (which may be empty!) */ 
  store = SSL_CTX_get_cert_store((SSL_CTX *)sslctx);
  if(!store)
    goto err;
 
  /* add our certificate to this store */ 
  if(!X509_STORE_add_cert(store, cert)) {
    unsigned long error = ERR_peek_last_error();
 
    /* Ignore error X509_R_CERT_ALREADY_IN_HASH_TABLE which means the
     * certificate is already in the store. That could happen if
     * libcurl already loaded the certificate from a ca cert bundle
     * set at libcurl build-time or runtime.
     */ 
    if(ERR_GET_LIB(error) != ERR_LIB_X509 ||
       ERR_GET_REASON(error) != X509_R_CERT_ALREADY_IN_HASH_TABLE)
      goto err;
 
    ERR_clear_error();
  }
 
  rv = CURLE_OK;
 
err:
  if(rv != CURLE_OK) {
    char errbuf[256];
    unsigned long error = ERR_peek_last_error();
 
   
    if(error) {
      ERR_error_string_n(error, errbuf, sizeof errbuf);
      printf("%s\n", errbuf);
    }
  }
 
  X509_free(cert);
  BIO_free(bio);
  ERR_clear_error();
 
  return rv;
}

//function to copy memory dynamically, must be freed the first parameter
int copy_heap(unsigned char **dest, unsigned char *src, unsigned long len) {

    free(*dest);
    
	(*dest) = (unsigned char*)malloc(len);
	if ((*dest) == NULL) {
		return 0;
	}

	memcpy((*dest), src, len);

	return 1;
}
