#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
using namespace std;
 
#include <curl/curl.h>


typedef struct Website{
	char* header;
	char* body;
	size_t size;
}Website;



struct MemoryStruct {
  char *memory;
  size_t size;
};
 
 
static size_t
BodyDataCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
//  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  Website *mem = (Website *)userp;
 
  mem->body = (char*) realloc(mem->body, mem->size + realsize + 1);
  if (mem->body == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    exit(EXIT_FAILURE);
  }
 
  memcpy(&(mem->body[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->body[mem->size] = 0;
 
  return realsize;
}


static size_t
HeaderDataCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
//  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  Website *mem = (Website *)userp;
 
  mem->header = (char*) realloc(mem->header, mem->size + realsize + 1);
  if (mem->header == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    exit(EXIT_FAILURE);
  }
 
  memcpy(&(mem->header[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->header[mem->size] = 0;
 
  return realsize;
}

 
 
int main(int argc, char* argv[])
{

	  string alexa_top_1M = "top-1m.csv";

	  FILE* alexa_top_1M_filep ;

	  if( (alexa_top_1M_filep = fopen(alexa_top_1M.c_str(), "r")) != NULL){
		fprintf(stderr, "Error opening file %s\n", alexa_top_1M.c_str());
	  }

	  int site_id;

	  string url;

	  CURL *curl_handle;

	  Website website;

	  char url_buff[100];

  while(1){
	  url = "https://www.";

	  memset(url_buff, 0, 100 * sizeof(char));
 
	  fscanf(alexa_top_1M_filep, "%d,%s\n", &site_id, url_buff);

	  url+= string(url_buff);
	  printf("Processing website %s\n==============================================================", url.c_str());
	  if(site_id == 10 )break;


	  website.body = (char*)  malloc(1);  /* will be grown as needed by the realloc above */ 
	  website.header = (char*)  malloc(1);  /* will be grown as needed by the realloc above */ 
	  website.size = 0;    /* no data at this point */ 
 
	  curl_global_init(CURL_GLOBAL_ALL);


 
	  /* init the curl session */ 
	  curl_handle = curl_easy_init();
 
	  /* specify URL to get */ 
 	 curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
 
 	 /* send all body data to this function  */ 
  	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, BodyDataCallback);

	/* send all header data to this function  */
        curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, HeaderDataCallback);
 

	  /* we pass our 'chunk' struct to the callback function */ 
  	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&website);

	  /* we pass our 'chunk' struct to the callback function */ 
  	curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, (void *)&website);

 
	  /* some servers don't like requests that are made without a user-agent
    	 field, so we provide one */ 
  	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
 
 	 /* get it! */ 
  	curl_easy_perform(curl_handle);
 


	  /* cleanup curl stuff */ 
	  curl_easy_cleanup(curl_handle);
 
	  /*
   	* Now, our chunk.body points to a memory block that is chunk.size
 	  * bytes big and contains the remote file.
	  *
 	  * Do something nice with it!
   	*
	   * You should be aware of the fact that at this point we might have an
   	* allocated data block, and nothing has yet deallocated that data. So when
   	* you're done with it, you should free() it as a nice application.
  	 */ 
 
	  printf("%lu bytes retrieved\n", (long)website.size);

	  if(website.body){
		printf("=======================================%s\n==========================", website.header);
		free(website.body);
		free(website.header);
  	}
  }
 
	  /* we're done with libcurl, so clean it up */ 
	  curl_global_cleanup();
 
	  return 0;
}
