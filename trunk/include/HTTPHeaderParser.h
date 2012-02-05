
#ifndef __CRAWLER_H__
#define __CRAWLER_H__

/* parses headers and tries to identify CSRF defence */
void check_for_headers(char* header, const char* url);

#endif
