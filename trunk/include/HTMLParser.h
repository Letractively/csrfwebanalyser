#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <list>
#include <string>
#include "includes.h"

/* parses html document and tries to
	identify CSRF defence pattern */
void parseHTML(const char* code, std::list<std::pair<CSRF_Defenses, std::string> > *result);

#endif
