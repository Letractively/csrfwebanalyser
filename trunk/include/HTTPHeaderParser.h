
#ifndef HTTPHEADERPARSER_H
#define HTTPHEADERPARSER_H


#include <list>
#include "includes.h"

using namespace std;

#define DEBUG_LEVEL 1


/* parses headers and tries to identify CSRF defence */
void check_for_headers(char* header, const char* url, list< pair < CSRF_Defenses, string > > * results);

#endif
