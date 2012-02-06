#include <string>
#include <stdio.h>
#include "HTTPHeaderParser.h"

using namespace std;

#include "includes.h"




void check_for_headers(char* header, const char* url, list< pair < CSRF_Defenses, string > > * results){
	/*  CSRF defense headers */
	/***********************************************
	Custom HTTP Header
	X-Requested-By: XMLHttpRequest
	------------------------------------------------
	X-Frame-Options: SAMEORIGIN, DENY, etc.
	------------------------------------------------
	Access-Control-Allow-Origin Header: *, smth
	------------------------------------------------
	************************************************/
	int location,i;
	if((location=string(header).find(string("X-Requested-By")))!=string::npos){
		#if DEBUG_LEVEL > 0
			fprintf(stderr, "---------------\n%s\n", url);
		#endif
		i=location;
		string firstline="";
		while(header[i]!='\n'){
			#if DEBUG_LEVEL > 0
				fprintf(stderr, "%c", header[i]);
			#endif
			firstline+=header[i];
			i++;
		}
		#if DEBUG_LEVEL > 0
			fprintf(stderr, "\n");
		#endif
		results->push_back(make_pair(CUSTOM_HTTP_HEADER, firstline.substr(15) ));
	}
	if((location=string(header).find(string("X-Frame-Options")))!=string::npos){
		#if DEBUG_LEVEL > 0
			fprintf(stderr, "---------------\n%s\n", url);
		#endif
		i=location;
		string firstline="";
		while(header[i]!='\n'){
			#if DEBUG_LEVEL > 0
				fprintf(stderr, "%c", header[i]);
			#endif
			firstline+=header[i];
			i++;
		}
		#if DEBUG_LEVEL > 0
			fprintf(stderr, "\n");
		#endif
		results->push_back(make_pair(X_FRAME_OPTIONS_HEADER, firstline.substr(16)));
	}
	if((location=string(header).find(string("Access-Control-Allow-Origin")))!=string::npos){
		#if DEBUG_LEVEL > 0
			fprintf(stderr, "---------------\n%s\n", url);
		#endif
		i=location;
		string firstline="";
		while(header[i]!='\n'){
			#if DEBUG_LEVEL > 0
				fprintf(stderr, "%c", header[i]);
			#endif
			firstline+=header[i];
			i++;
		}
		#if DEBUG_LEVEL > 0
			fprintf(stderr, "\n");
		#endif
		results->push_back(make_pair(ACCESS_CONTROL_ALLOW_ORIGIN_HEADER, firstline.substr(28)));
	}
}