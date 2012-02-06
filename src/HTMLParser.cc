
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xmlstring.h>
#include "HTMLParser.h"
#include <regex.h>
#include <iostream>
#include "pcrecpp.h"
#include "includes.h"
//
//  Case-insensitive string comparison
//

#ifdef _MSC_VER
#define COMPARE(a, b) (!stricmp((a), (b)))
#else
#define COMPARE(a, b) (!strcasecmp((a), (b)))
#endif

//
//  libxml callback context structure
//

struct Context
{
  Context(): addTitle(false) { }

  bool addTitle;
  std::string title;
};

#if 0
//
//  libxml start element callback function
//

static void StartElement(void *voidContext,
                         const xmlChar *name,
                         const xmlChar **attributes)
{
  Context *context = (Context *)voidContext;

  if (COMPARE((char *)name, "TITLE"))
  {
    context->title = "";
    context->addTitle = true;
  }
}

//
//  libxml end element callback function
//

static void EndElement(void *voidContext,
                       const xmlChar *name)
{
  Context *context = (Context *)voidContext;

  if (COMPARE((char *)name, "TITLE"))
    context->addTitle = false;
}

//
//  Text handling helper function
//

static void handleCharacters(Context *context,
                             const xmlChar *chars,
                             int length)
{
  if (context->addTitle)
    context->title.append((char *)chars, length);
}

//
//  libxml PCDATA callback function
//

static void Characters(void *voidContext,
                       consCURLOPT_USERAGENTt xmlChar *chars,
                       int length)
{
  Context *context = (Context *)voidContext;

  handleCharacters(context, chars, length);
}

//
//  libxml CDATA callback function
//

static void cdata(void *voidContext,
                  const xmlChar *chars,
                  int length)
{
  Context *context = (Context *)voidContext;

  handleCharacters(context, chars, length);
}

//
//  libxml SAX callback structure
//

static htmlSAXHandler saxHandler =
{
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  StartElement,
  EndElement,
  NULL,
  Characters,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  cdata,
  NULL
};

//
//  Parse given (assumed to be) HTML text and return the title
//

static void parseHtml(const std::string &html,
                      std::string &title)
{
  htmlParserCtxtPtr ctxt;
  Context context;

  ctxt = htmlCreatePushParserCtxt(&saxHandler, &context, "", 0, "",
                                  XML_CHAR_ENCODING_NONE);

  htmlParseChunk(ctxt, html.c_str(), html.size(), 0);
  htmlParseChunk(ctxt, "", 0, 1);

  htmlFreeParserCtxt(ctxt);

  title = context.title;
}
#endif

//code "borrowed" from chromium project
std::string XmlStringToStdString(const xmlChar* xmlstring) {
  // xmlChar*s are UTF-8, so this cast is safe.
  if (xmlstring)
    return std::string(reinterpret_cast<const char*>(xmlstring));
  else
    return "";
}


bool isNonce(const xmlChar* val) {
#if 0
	regex_t regex;
  int reti;

	//reti = regcomp(&regex, "[A-Za-z0-9_^.^ \t\r\n\v\f]", 0);
	reti = regcomp(&regex, "^\\d*$", REG_EXTENDED);
	if(reti) { 
		fprintf(stderr, "Could not compile regex\n"); 
		exit(1); 
	}

	/* Execute regular expression */
  //reti = regexec(&regex, reinterpret_cast<const char*>(val), 0, NULL, 0);
  reti = regexec(&regex, "19216811", 0, NULL, 0);
	if(!reti) {
		printf("match!");
		return false;
  }
  else if(reti == REG_NOMATCH) {
		return false;
  }
  else {
		//regerror(reti, &regex, val, xmlStrlen(val));
    fprintf(stderr, "Regex match failed: %s\n", val);
    exit(1);
	}
	/* Free compiled regular expression if you want to use the regex_t again */
	regfree(&regex);
	return false;

	//if(xmlStrlen(val) <= 10) return false;
	//return true;
#endif
	int i;
	string s;
	pcrecpp::RE re("[A-Za-z0-9_=]{10,}");
	if (re.error().length() > 0) {
		std::cout << "PCRE compilation failed with error: " << re.error() << "\n";
  }
  if (re.FullMatch(XmlStringToStdString(val))) {
		//printf("match!");
		return true;
	}
	return false;
}

bool FindToken(htmlNodePtr element, std::list<std::pair<CSRF_Defenses, string> > *result)
{
	  bool is_hidden = false;
    for(htmlNodePtr node = element; node != NULL; node = node->next)
    {
        if(node->type == XML_ELEMENT_NODE)
        {
            if(xmlStrcasecmp(node->name, (const xmlChar*)"input") == 0)
            {
                for(xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next)
                {
                    if(xmlStrcasecmp(attr->name, (const xmlChar*)"type") == 0)
                    {
											if(xmlStrcasecmp(attr->children->content, (const xmlChar*)"hidden") == 0) {
												//printf("hidden input tag found, how interesting...\n");
												is_hidden = true;
											}
                    }
                    else if(xmlStrcasecmp(attr->name, (const xmlChar*)"value") == 0 && is_hidden) {
											if(isNonce(attr->children->content)) {	
												printf("hiddent value is %s\n", attr->children->content);
												result->push_back(make_pair(SECRET_VALIDATION_TOKEN, 
																					XmlStringToStdString(attr->children->content)));
												//if pattern matches return true
											}
										}
                }
            }
            if(node->children != NULL)
            {
                FindToken(node->children, result);
            }
        }
    }
    return false;
}

bool isRegisterValue(const xmlChar* val) {
	if(xmlStrcasecmp(val, (const xmlChar*)"Sign Up") == 0 ||
		xmlStrcasecmp(val, (const xmlChar*)"Register") == 0 ||
		xmlStrcasecmp(val, (const xmlChar*)"signup") == 0 ||
		xmlStrcasecmp(val, (const xmlChar*)"registration") == 0 ||
		xmlStrcasecmp(val, (const xmlChar*)"join") == 0 ||
		xmlStrcasecmp(val, (const xmlChar*)"join now") == 0)
		return true;

	if(xmlStrcasestr(val, (const xmlChar*)"signup") ||
		xmlStrcasestr(val, (const xmlChar*)"register") ||
		xmlStrcasestr(val, (const xmlChar*)"registration") || 
		xmlStrcasestr(val, (const xmlChar*)"login") ||
		xmlStrcasestr(val, (const xmlChar*)"join"))
		return true;
	
	return false;
}

void FindRegisterLink(htmlNodePtr element, std::list<std::pair<CSRF_Defenses, string> > *result)
{
	
	  xmlAttrPtr attr, attr1, attr2;
    for(htmlNodePtr node = element; node != NULL; node = node->next)
    {
        if(node->type == XML_ELEMENT_NODE)
        {
            if(xmlStrcasecmp(node->name, (const xmlChar*)"input") == 0)
            {
                for(attr1 = node->properties; attr1 != NULL; attr1 = attr1->next)
                {
									  //printf("input<%s>=%s\n", attr1->name, attr1->children->content);
                    if(xmlStrcasecmp(attr1->name, (const xmlChar*)"type") == 0)
                    {
											if(xmlStrcasecmp(attr1->children->content, (const xmlChar*)"submit") == 0) {
												//printf("Submit button found, let's see...\n");
												for(attr2 = node->properties; attr2 != NULL; attr2 = attr2->next) {
													if(xmlStrcasecmp(attr2->name, (const xmlChar*)"value") == 0)	{
														if(isRegisterValue(attr2->name)) {
															printf("Ha, found register button, now how we click on it?\n");
														}
													}
												}
											}
											break;
                    }
                    else if(xmlStrcasecmp(attr1->name, (const xmlChar*)"value") == 0)
                    {
											//printf("%s=%s\n", attr1->name, attr1->children->content);
											//printf("sign up\n");
											if(isRegisterValue(attr1->children->content)) {
												//printf("Register value found, let's see...\n");
												for(attr2 = node->properties; attr2 != NULL; attr2 = attr2->next) {
													if(xmlStrcasecmp(attr2->name, (const xmlChar*)"type") == 0) {
														if(xmlStrcasecmp(attr2->children->content, (const xmlChar*)"submit") == 0) {
															printf("Ha, submit register button found, now what?\n");
														}
													}
												}
											}
                      break;
										}
                }
            }
						//printf("Element=%s\n", node->name);
#if 0
					  if(xmlStrcasecmp(node->name, (const xmlChar*)"a") == 0 ||
							xmlStrcasecmp(node->name, (const xmlChar*)"button") == 0) {
							for(attr = node->properties; attr != NULL; attr = attr->next) {
								if(xmlStrcasecmp(attr->name, (const xmlChar*)"href") == 0) {
									if(isRegisterValue(attr->children->content)) {
										printf("Register link found\n");
									}
									if(node->children != NULL && isRegisterValue(node->children->content)) {
										printf("Register link found\n");
									}
								}
							}
						}
#endif
						//printf("Element=%s\n", node->name);
						if(node->properties != NULL) {
						//if(xmlStrcasecmp(node->name, (const xmlChar*)"a") == 0 ||
						//	xmlStrcasecmp(node->name, (const xmlChar*)"button") == 0) {
							for(attr = node->properties; attr != NULL; attr = attr->next) {
								if(xmlStrcasecmp(attr->name, (const xmlChar*)"href") == 0) {
									//printf("node=%s\n", attr->children->content);
									if(isRegisterValue(attr->children->content)) {
										printf("Register link found\n");
									}
									if(node->children != NULL && isRegisterValue(node->children->content)) {
										printf("Register link found\n");
									}
								}
							}
						}
            if(node->children != NULL)
            {
                FindRegisterLink(node->children, result);
            }
        }
    }
}

void parseHTML(const char* code, std::list<std::pair<CSRF_Defenses, std::string> > *result)
{
	  //paypal has a hidden referes input without value however
	  //printf("%s", code);
	  xmlChar *html = xmlCharStrdup((char *)code);
    //htmlDocPtr doc = htmlParseDoc(html, NULL);
    htmlDocPtr doc = htmlReadDoc(html, NULL, NULL, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
		if(doc != NULL)
    {
        htmlNodePtr root = xmlDocGetRootElement(doc);
        if(root != NULL)
        {
					  if(FindToken(root, result)) {
							//token found, add to possible csrf defences
							return;
						}
						else {
							//maybe we could check some links anyhow, to check if token is
							//indeed present throughout the session
							//we could also mark the sites that possibly use this defence, to see if
							//token changes when starting a new session
							FindRegisterLink(root, result);
						}
        }
        xmlFreeDoc(doc);
        doc = NULL;
    }
#if 0
    if(xmlStrcasestr((const xmlChar*)"/signup?next=%2Fchannels%3Ffeature%3Dsignup", (const xmlChar*)"signup")) {
			printf("haystack works!\n");
		}
#endif
}
