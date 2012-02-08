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
#include <map>
#include <unistd.h>

#include "HTTPHeaderParser.h"
#include "HTMLParser.h"
#include "Results.h"

using namespace std;

#define REDIRECTIONS 0
#define SAVE_ALL_SITES 0


FILE* websites_filep;
sem_t websites_file_sem;
sem_t url_id_sem;
int url_id = 0;



typedef struct Website{
	char* header;
	char* body;
	size_t header_size;
	size_t body_size;
}Website;

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


/* returns 0 if the websites file could not be opened */
int get_websites(string websites){
	  if(websites.empty())
	  		websites = "top-1m.csv";
	  FILE * websites_last_updated_filep;
	  time_t hours;
	  time_t nowhours;
	  nowhours = time(NULL)/3600;
	  unsigned int update = 0;

	  websites_last_updated_filep = fopen("websites_last_updated.txt", "r");

	  if (websites_last_updated_filep){
		  fscanf(websites_last_updated_filep, "%ld", &hours);
		  if(nowhours - hours > 24){
			update = 1;
			fprintf(stderr, "Updating websites...\n");
		  }
	  }
	  sem_wait(&websites_file_sem);
	  if( (websites_filep = fopen(websites.c_str(), "r")) == NULL){
		fprintf(stderr, "Error opening file %s\n", websites.c_str());
	  	sem_post(&websites_file_sem);
		return 0;
	  }
	  sem_post(&websites_file_sem);
	  return 1;
}
 
string get_next_url(){
	  string url = "http://www.";
	  int url_alexa_number;
	  char url_buff [100];
	  memset(url_buff, 0, 100 * sizeof(char));
    int url_id_alexa;
	  sem_wait(&websites_file_sem);
	  sem_wait(&url_id_sem);
	    fscanf(websites_filep, "%d,%s\n", &url_alexa_number, url_buff);
	    url_id++;
	  sem_post(&url_id_sem);
	  sem_post(&websites_file_sem);
	  if(string(url_buff).empty())
		return string("");
	  url+= string(url_buff);
	  return url;
}


int get_url_id(){
	int curr_url_id;
	sem_wait(&url_id_sem);
	  curr_url_id = url_id;
	sem_post(&url_id_sem);
	return curr_url_id;
}

void process_url(string url, Results *results, unsigned int currDepth){
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

		/* redirection limit is set to 10 redirections */
        curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 10);

        /* total transfer operation maximum time limit is set to 120 seconds */
        curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 120);

//        /* total time for the connection with the server is set to 20 seconds */
  //      curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 20);
	  	
        /* follow locations  */
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
 
		/* we pass our 'chunk' struct to the callback function */ 
 	 	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&website);

		/* we pass our 'chunk' struct to the callback function */ 
	  	curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, (void *)&website);

	  	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1);


 
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
			check_for_headers(website.header, url.c_str(), results);
			if(firstline.find(string("200"))==string::npos && firstline.find(string("301"))==string::npos && firstline.find(string("302"))==string::npos ) {
				fprintf(stderr,  "------------------\n");
				fprintf(stdout, "%s not 200, 301, 302, but %s\n", url.c_str(), firstline.c_str());
			}
			//printf(" --------%s-----------\n", url.c_str());
			//printf("=======================================%s\n==========================\n", website.header);
#if REDIRECTIONS
			int location_header_pos;
			if((location_header_pos = string(website.header).find(string("Location:")) ) != string::npos){
				string redirect_url = string(website.header+location_header_pos+10);
				fprintf(stderr, "Redirecting to %s\n", redirect_url.c_str());
				process_url(redirect_url, results, currDepth);
			}
#endif
			free(website.header);

  		}
		if(website.body){
			//printf("=======================================%s\n==========================", website.body);
			parseHTML(website.body, url, results, process_url, currDepth);
			free(website.body);
  		}
}

typedef struct Threadresults{
	Results results;
	int threadid;
}Threadresults;

void* do_crawl(void* threadresults){
	string url;
	fprintf(stderr, "Thread %d starting...\n",  ((Threadresults*)threadresults)->threadid);
	while(!(url=get_next_url()).empty()){
		#if DEBUG_LEVEL > 1
			fprintf(stderr, "Thread %d got url %s...\n",  ((Threadresults*)threadresults)->threadid, url.c_str());
		#endif
		/* adding map entry for the processed url */
		process_url( url, &((Threadresults*)threadresults)->results,  0 );
		#if DEBUG_LEVEL > 1
			fprintf(stderr, "Thread %d process_url completed!!...\n",  ((Threadresults*)threadresults)->threadid);
		#endif
		if (get_url_id() > 10000){
			fprintf(stderr, "Thread %d do_crawl THE END!!...\n",  ((Threadresults*)threadresults)->threadid);
			break;
		}
	}
}

string enum_to_str(int csrf_def){
	switch(csrf_def){
		case SECRET_VALIDATION_TOKEN:
			return "Secret Validation Token";
		case REFERER_VALIDATION:
			return "Referer Validation";
		case CUSTOM_HTTP_HEADER:
			return "Custom HTTP Header";
		case X_FRAME_OPTIONS_HEADER:
			return "X-Frame-Options Header";
		case ACCESS_CONTROL_ALLOW_ORIGIN_HEADER:
			return "Access Control Allow Origin Header";
		case CONTENT_SECURITY_POLICY:
			return "Content-Security-Policy";
		case CONTENT_SECURITY_POLICY_REPORT_ONLY:
			return "Content-Security-Policy-Report-Only";
		case X_CONTENT_SECURITY_POLICY:
			return "X-Content-Security-Policy";
		case X_WEBKIT_CSP:
			return "X-WebKit-CSP";
		case POSSIBLE_SECRET_VALIDATION_TOKEN:
			return "Possible Secrete Validation Token";
		case NOT_ASSIGNED:
			return "Defense not assigned";
		default:
			return "Defense not assigned";
	}
}

int main(int argc, char* argv[])
{
          pthread_t * threads;
          Threadresults * threadresults;
          int nthreads=1, thread_error, i;
          string websites;

          char url_buff[100];
          int c;
          const char * opstr = "f:n:";

          sem_init(&websites_file_sem, 0, 1);
          sem_init(&url_id_sem, 0, 1);

          curl_global_init(CURL_GLOBAL_ALL);
					initHTMLParser();
					
          while ((c = getopt(argc, argv, opstr)) != -1) {
                switch(c){
                        case 'f':
													websites = string(optarg);
													break;
                        case 'n':
                            nthreads = atoi(optarg);
                            break;
                        default:
                            break;
                }
          }

          if (!get_websites(websites)){
            fprintf(stderr, "Could not open websites file\n");
            exit(EXIT_FAILURE);
          }

          threads = (pthread_t*)  malloc(nthreads * sizeof(pthread_t));
          threadresults = (Threadresults*) malloc(nthreads * sizeof(Threadresults));

          for(i=0; i< nthreads; i++) {
								threadresults[i].results = Results();
                threadresults[i].threadid = i;
                thread_error = pthread_create(&threads[i], NULL, /* default attributes please */ do_crawl,(void *) &threadresults[i]);
                 if(thread_error != 0)
                      fprintf(stderr, "Couldn't run thread number %d, errno %d\n", i, thread_error);
          }

          /* now wait for all threads to terminate */
          for(i=0; i< nthreads; i++) {
	      thread_error = pthread_join(threads[i], NULL);
              fprintf(stderr, "Thread %d terminated\n", i);
          }

          #if DEBUG_LEVEL >= 0 //8a to kanw define -1 gia na ma8eis...
          /* print the results */
					Results total_results = Results();
          for(i=0; i< nthreads; i++) {
						total_results.MergeUrlDefensesMaps(threadresults[i].results.GetUrlDefensesMap());
						total_results.MergeDefenseUrlsMaps(threadresults[i].results.GetDefenseUrlsMap());
          }
          total_results.PrintUrlsMap();
					total_results.PrintDefensesMap();
          #endif

          /* we're done with libcurl, so clean it up */
          curl_global_cleanup();

          return 0;
}
