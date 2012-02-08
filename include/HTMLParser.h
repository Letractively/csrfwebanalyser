#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <list>
#include <string>
#include "includes.h"
#include "Results.h"

/* parses html document and tries to
	identify CSRF defence pattern */
void parseHTML(const char* code,
							 string url,
							 Results *result,
							 void (*process_url)(std::string, Results *result, unsigned int),
							 unsigned int currDepth);

#endif
