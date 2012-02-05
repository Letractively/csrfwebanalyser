#include <string>
#include <stdio.h>
#include "HTTPHeaderParser.h"

using namespace std;

void check_for_headers(char* header, const char* url){
	/*  CSRF defense headers */
	/***********************************************
	Custom HTTP Header
	X-Requested-By: XMLHttpRequest
	------------------------------------------------
	X-Frame-Options: SAMEORIGIN
	------------------------------------------------
	Access-Control-Allow-Origin Header: *
	------------------------------------------------
	************************************************/
	int location,i;
	if((location=string(header).find(string("X-Requested-By")))!=string::npos){
		fprintf(stderr, "---------------\n%s\n", url);
		i=location;
		while(header[i]!='\n'){
			fprintf(stderr, "%c", header[i]);
			i++;
		}
		fprintf(stderr, "\n");
	}
	if((location=string(header).find(string("X-Frame-Options")))!=string::npos){
		fprintf(stderr, "---------------\n%s\n", url);
		i=location;
		while(header[i]!='\n'){
			fprintf(stderr, "%c", header[i]);
			i++;
		}
		fprintf(stderr, "\n");
	}
	if((location=string(header).find(string("Access-Control-Allow-Origin")))!=string::npos){
		fprintf(stderr, "---------------\n%s\n", url);
		i=location;
		while(header[i]!='\n'){
			fprintf(stderr, "%c", header[i]);
			i++;
		}
		fprintf(stderr, "\n");
	}

}