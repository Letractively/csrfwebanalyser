#ifndef __INCLUDES_H__
#define __INCLUDES_H__

enum CSRF_Defenses{
	SECRET_VALIDATION_TOKEN = 0,
	REFERER_VALIDATION = 1,
	CUSTOM_HTTP_HEADER = 2,
	X_FRAME_OPTIONS_HEADER = 3,
	ACCESS_CONTROL_ALLOW_ORIGIN_HEADER = 4,
	CONTENT_SECURITY_POLICY = 5,
	CONTENT_SECURITY_POLICY_REPORT_ONLY = 6,
	X_CONTENT_SECURITY_POLICY = 7,
	X_WEBKIT_CSP = 8,
	NOT_ASSIGNED = 9,
	POSSIBLE_SECRET_VALIDATION_TOKEN = 10
};



extern bool header_check_on;
extern bool body_check_on;
extern bool referer_check_on;
extern bool add_header_results;
extern bool add_body_results;
extern bool add_referer_results;


#endif

