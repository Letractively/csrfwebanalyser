#include <string>
#include <stdio.h>
#include "HTTPHeaderParser.h"
#include "pcrecpp.h"
#include <iostream>
#include <sstream>

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

//check if first line of header returns 200 OK code
bool acceptHeader(const char* val) {
  string accept_code = ".*(200\\s*OK).*";
	pcrecpp::RE_Options opt;
	opt.set_caseless(true);
	pcrecpp::RE csp_field_re(accept_code, opt);
	if(csp_field_re.FullMatch((char *)val)) {
		//cout << "Header accepted, code: " << val << endl;	
		return true;
	}
	//cout << "Header rejected, code: " << val << endl;
	return false;	
}

bool isContentField(const char* val, const char * url, Results * results) {
	//printf("%s", val);
	string CSP_field = "(X-Requested-(By|With)|X-Frame-Options|Access-Control-Allow-Origin|\
	X*-Content-Security-Policy(-Report-Only|X-WebKit-CSP)*):(.*)";
	//string CSP_field = ".*[a-zA-Z]+(-+[a-zA-Z]+){2,}\\s*:.*";
	pcrecpp::RE_Options opt;
	opt.set_caseless(true);
	pcrecpp::RE csp_field_re(CSP_field, opt);
	if(csp_field_re.error().length() > 0) {
		std::cout << "PCRE: complilation failed with error: " << csp_field_re.error() << "\n";
	}
	string content = string(val);
	string policy, value;
	int pos;
	if(csp_field_re.FullMatch((char *)val)) {
		//printf("%s\n", val);
		//("buf1=%d\n", content.c_str());
		pos = content.find(":");
		policy = content.substr(0, pos);
		value = content.substr(pos+2);
		#if DEBUG_LEVEL > 0
		cout << policy << ":" << value << endl;
		std::cout << "++++++Link match for " << url << "!+++++++++\n";
		#endif
		results->AddUrlDefense(string(url), policy, value);
		results->AddDefenseUrl(policy, string(url), value);
		return true;
	}
	return false;
}

void check_for_headers(char* header, const char* url, Results* results){
	 //static cnt = 0;
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
	//X-CONTENT-TYPE-OPTIONS
	int location;
	#if DEBUG_LEVEL > 0
		fprintf(stderr, "---------------\n%s\n", url);
	#endif
	//char buff[1024];
	string buff;
	stringstream hds;
	hds << string(header);
	//get first line
	//cout << header << endl;
	//cout << "**********************************" << endl;
	getline(hds, buff);
	cout << "processign url: " << url << endl;
	//if(!acceptHeader(buff.c_str())) return;
	bool header_accepted = false;
	while(!hds.eof()) {
		getline(hds, buff);
		//cout << " Searching at " << buff <<endl;
		if(acceptHeader(buff.c_str())) 
			header_accepted = true;
		if (header_accepted) 
			isContentField(buff.c_str(), url, results);
	}
	
#if 0	
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
#endif
}