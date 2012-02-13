
#ifndef HTTPHEADERPARSER_H
#define HTTPHEADERPARSER_H


#include <list>
#include "includes.h"
#include "Results.h"

using namespace std;

#define DEBUG_LEVEL 0


/* parses headers and tries to identify CSRF defence */
bool check_for_headers(char* header, const char* url, Results * results);

#endif

