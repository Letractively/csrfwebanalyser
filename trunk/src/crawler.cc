#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string>
#include <semaphore.h>
#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include <semaphore.h>
#include <string.h>

#include "HTTPHeaderParser.h"
#include "HTMLParser.h"

using namespace std;

#define REDIRECTIONS 0
#define DEBUG_LEVEL 0

FILE* alexa_top_1M_filep;
sem_t alexa_file_sem;
sem_t url_id_sem;
int url_id;

typedef struct Website{
	char* header;
	char* body;
	size_t header_size;
	size_t body_size;
}Website;

typedef enum CSRF_Defenses{
	SECRET_VALIDATION_TOKEN = 0;
	REFERER_VALIDATION = 1;
	CUSTOM_HTTP_HEADER = 2;
	X_FRAME_OPTIONS_HEADER = 3;
	ACCESS_CONTROL_ALLOW_ORIGIN_HEADER = 4;
}CSRF_Defenses;


static size_t
BodyDataCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
 
  Website *mem = (Website *)userp;
 
  mem->body = (char*) realloc(mem->body, mem->body_size + realsize + 1);
  if (mem->body == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    exit(EXIT_FAILURE);
  }
 
  memcpy(&(mem->body[mem->body_size]), contents, realsize);
  mem->body_size += realsize;
  mem->body[mem->body_size] = 0;
  return realsize;
}


static size_t
HeaderDataCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
 
  Website *mem = (Website *)userp;
 
  mem->header = (char*) realloc(mem->header, mem->header_size + realsize + 1);
  if (mem->header == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    exit(EXIT_FAILURE);
  }
 
  memcpy(&(mem->header[mem->header_size]), contents, realsize);
  mem->header_size += realsize;
  mem->header[mem->header_size] = 0;

#if REDIRECTIONS
  /* stop downloading if Location header was found. No other callbacks are called if 0 is returned */
  if(string(mem->header).find(string("Location"))!= string::npos){
	return 0;
  }
#endif
  return realsize;
}


/* returns 0 if the alexa top 1M sites file could not be opened */
int get_alexa_top_1M(){
	  string alexa_top_1M = "top-1m.csv";
	  FILE * alexa_top_1M_last_updated_filep;
	  time_t hours;
	  time_t nowhours;
	  nowhours = time(NULL)/3600;
	  unsigned int update = 0;

	  alexa_top_1M_last_updated_filep = fopen("alexa_top_1M_last_updated.txt", "r");

	  if (alexa_top_1M_last_updated_filep){
		  fscanf(alexa_top_1M_last_updated_filep, "%ld", &hours);
		  if(nowhours - hours > 24){
			update = 1;
			fprintf(stderr, "Updating alexa top 1M sites...\n");
		  }
	  }
	  sem_wait(&alexa_file_sem);
	  if( (alexa_top_1M_filep = fopen(alexa_top_1M.c_str(), "r")) == NULL){
		fprintf(stderr, "Error opening file %s\n", alexa_top_1M.c_str());
	  	sem_post(&alexa_file_sem);
		return 0;
	  }
	  sem_post(&alexa_file_sem);
	  return 1;
}
 
string get_next_url(){
	  string url = "http://www.";
	  char url_buff [100];
	  memset(url_buff, 0, 100 * sizeof(char));

	  sem_wait(&alexa_file_sem);
	  sem_wait(&url_id_sem);
	    fscanf(alexa_top_1M_filep, "%d,%s\n", &url_id, url_buff);
	  sem_post(&alexa_file_sem);
	  sem_post(&url_id_sem);
	  if(string(url_buff).empty())
		return string("");
	  url+= string(url_buff);
	  return url;
}

s
int get_url_id(){
	int curr_url_id;
	sem_wait(&url_id_sem);
	  curr_url_id = url_id;
	sem_post(&url_id_sem);
	return curr_url_id;
}

void process_url(string url){
		CURL* curl_handle;
		Website website;
		website.body = (char*)  malloc(1);  /* will be grown as needed by the realloc above */ 
 		website.header = (char*)  malloc(1);  /* will be grown as needed by the realloc above */ 
		website.body_size = 0; /* no data at this point */ 
		website.header_size = 0;    /* no data at this point */ 
		#if DEBUG_LEVEL > 0
		fprintf(stderr, "Processing %s\n", url.c_str());
		#endif
		/* init the curl session */ 
		curl_handle = curl_easy_init();
 
		/* specify URL to get */ 
 		curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
 
	 	/* send all body data to this function  */ 
  		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, BodyDataCallback);

		/* send all header data to this function  */
        	curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, HeaderDataCallback);

		/* follow locations  */
        	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
 
		/* we pass our 'chunk' struct to the callback function */ 
 	 	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&website);

		/* we pass our 'chunk' struct to the callback function */ 
	  	curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, (void *)&website);

 
		/* some servers don't like requests that are made without a user-agent
	    	field, so we provide one */ 
	  	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/4.73 [en] (X11; U; Linux 2.2.15 i686)");
 
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
		//printf("%lu bytes retrieved\n", (long)website.size);
		stringstream ss;
		ss<< string(website.header);
		string firstline;
		getline(ss, firstline);
		if(website.header){
			check_for_headers(website.header, url.c_str());
			if(firstline.find(string("200"))==string::npos && firstline.find(string("301"))==string::npos && firstline.find(string("302"))==string::npos ) {
				fprintf(stderr,  "------------------\n");
				fprintf(stdout, "%s not 200, 301, 302, but %s\n", url.c_str(), firstline.c_str());
			}
			//printf(" %s\n", url.c_str());
			//printf("=======================================%s\n==========================\n", website.header);
#if REDIRECTIONS
			int location_header_pos;
			if((location_header_pos = string(website.header).find(string("Location:")) ) != string::npos){
				string redirect_url = string(website.header+location_header_pos+10);
				fprintf(stderr, "Redirecting to %s\n", redirect_url.c_str());
				process_url(redirect_url);
			}
#endif
			free(website.header);

  		}
		if(website.body){
			//printf("=======================================%s\n==========================", website.body);
			parseHTML(website.body);
			free(website.body);
  		}
}

void* do_crawl(void* threadargs){
	string url;
//	fprintf(stderr, "Thread %d starting...\n",  threadargs);
	while(!(url=get_next_url()).empty()){
		#if DEBUG_LEVEL > 1
		 printf("Got url %s\n", url.c_str());
		#endif
		process_url(url);
	}
}

int main(int argc, char* argv[])
{
          pthread_t * threads;
          typedef struct Threadargs{
          	 map <string, pair<CSRF_Defenses, string > > CrawlResultsMap;
          	 const int threadid;
          }Threadargs;
          Threadargs * threadargs;
          int nthreads=1, thread_error, i;

          char url_buff[100];
          char c;

          sem_init(&alexa_file_sem, 0, 1);
          sem_init(&url_id_sem, 0, 1);

          if (!get_alexa_top_1M()){
                fprintf(stderr, "Could not open alexa top 1M sites file\n");
                exit(EXIT_FAILURE);
          }

          curl_global_init(CURL_GLOBAL_ALL);

          while ((c = getopt(argc, argv, "N")) != -1) {
                switch(c){
                        case 'N':
                                nthreads = atoi(argv[optind]);
                                break;
                        default:
                                break;
                }
          }

          threads = (pthread_t*)  malloc(nthreads * sizeof(pthread_t));
          threadargs = (Threadargs*) malloc(nthreads * sizeof(Threadargs));

          for(i=0; i< nthreads; i++) {
                threadargs[i].threadid = i;
                thread_error = pthread_create(&threads[i], NULL, /* default attributes please */ do_crawl,(void *) &threadargs[i]);
                 if(thread_error != 0)
                      fprintf(stderr, "Couldn't run thread number %d, errno %d\n", i, thread_error);
          }

          /* now wait for all threads to terminate */
          for(i=0; i< nthreads; i++) {
	      thread_error = pthread_join(threads[i], NULL);
              fprintf(stderr, "Thread %d terminated\n", i);
          }

          /* we're done with libcurl, so clean it up */
          curl_global_cleanup();

          return 0;
}

