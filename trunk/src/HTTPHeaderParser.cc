#include <string>
#include <stdio.h>
#include "HTTPHeaderParser.h"

using namespace std;

#include "includes.h"


void add_to_results(CSRF_Defenses header_type, char* header, int location, int offset, list< pair < CSRF_Defenses, string > > * results){
		int i=location;
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
		results->push_back(make_pair(header_type, firstline.substr(offset) ));
}


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
	int location;
	#if DEBUG_LEVEL > 0
		fprintf(stderr, "---------------\n%s\n", url);
	#endif
	if((location=string(header).find(string("X-Requested-By")))!=string::npos){
		add_to_results(CUSTOM_HTTP_HEADER, header, location, 15, results);
	}
	if((location=string(header).find(string("X-Frame-Options")))!=string::npos){
		add_to_results(X_FRAME_OPTIONS_HEADER, header, location, 16, results);
	}
	if((location=string(header).find(string("Access-Control-Allow-Origin")))!=string::npos){
		add_to_results(ACCESS_CONTROL_ALLOW_ORIGIN_HEADER, header, location, 28, results);
	}
	if((location=string(header).find(string("Content-Security-Policy")))!=string::npos){
		add_to_results(X_CONTENT_SECURITY_POLICY, header, location, 24, results);
	}
	if((location=string(header).find(string("Content-Security-Policy-Report-Only")))!=string::npos){
		add_to_results(CONTENT_SECURITY_POLICY_REPORT_ONLY, header, location, 36, results);
	}
	if((location=string(header).find(string("X-Content-Security-Policy")))!=string::npos){
		add_to_results(X_CONTENT_SECURITY_POLICY, header, location, 26, results);
	}
	if((location=string(header).find(string("X-WebKit-CSP")))!=string::npos){
		add_to_results(X_WEBKIT_CSP, header, location, 13, results);
	}



}