#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <list>
#include <string>
#include "includes.h"

/* parses html document and tries to
	identify CSRF defence pattern */
void parseHTML(const char* code, 
							 std::list<std::pair<CSRF_Defenses, std::string> > *result,
							 std::list<std::pair<CSRF_Defenses, std::string> > (*process_url)(std::string url, unsigned int),
							 unsigned int currDepth);

#endif
