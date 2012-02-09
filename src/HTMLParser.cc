
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xmlstring.h>
#include <regex.h>
#include <iostream>
#include "pcrecpp.h"
#include "includes.h"
#include "HTMLParser.h"
//haven't seen any improvement from visiting links in greater depth, and takes significant more time for https
#define MAX_DEPTH 1 

pcrecpp::RE nonce_re(""), register_re(""), uri_re(""), authname_re(""); 
pcrecpp::RE_Options opt;

void initHTMLParser(void) {
	//normally this should be a hash value, but...
	std::string token_regex = "[A-Za-z0-9_=]{8,}";
	std::string register_regex = "sign(\\s*|_*)(up|in)|(regist(er|ration))|join(\\s*|_*)\\w*|login|create(\\s*|_*)account";
	std::string uri_regex = "^((http|https)://)+(www\\.)*.+";
	std::string authname_regex = "sign(\\s*|_*)(up|in)|(regist(er|ration))|auth|login|account|token|secret|refer+er|pass|id";
	opt.set_caseless(true);
	//compile all regural expressions once
	nonce_re = pcrecpp::RE(token_regex, opt);
	if(nonce_re.error().length() > 0) {
		std::cout << "PCRE: compilation failed with error: " << nonce_re.error() << "\n";
		exit(1);
	}
	register_re = pcrecpp::RE(register_regex, opt);
	if(register_re.error().length() > 0) {
		std::cout<< "PCRE: complilation failed with error: " << register_re.error() << "\n";
		exit(1);
	}
	uri_re = pcrecpp::RE(uri_regex,opt);
	if(uri_re.error().length() > 0) {
		std::cout<< "PCRE: complilation failed with error: " << uri_re.error() << "\n";
		exit(1);
	}
	authname_re = pcrecpp::RE(authname_regex, opt);
	if(authname_re.error().length() > 0) {
		std::cout<< "PCRE: complilation failed with error: " << authname_re.error() << "\n";
		exit(1);
	}
}

//function "borrowed" from chromium project
std::string XmlStringToStdString(const xmlChar* xmlstring) {
  // xmlChar*s are UTF-8, so this cast is safe.
  if (xmlstring)
    return std::string(reinterpret_cast<const char*>(xmlstring));
  else
    return "";
}

bool isNonce(const xmlChar* val) {
	//pcrecpp::RE re(token_regex);
  if(nonce_re.FullMatch(XmlStringToStdString(val))) {
		//printf("match!");
		return true;
	}
	return false;
}

bool isRegisterValue(const xmlChar* val) {	
	//pcrecpp::RE re(register_regex, opt);
	if(register_re.PartialMatch(XmlStringToStdString(val))) {
		//std::cout << "match!";
		return true;
	}
	return false;
}

bool isRegisterLink(const xmlChar* val) {
	//pcrecpp::RE re_link(uri_regex, opt);
	//pcrecpp::RE re_register(register_regex, opt);
	if(val == NULL) return false;
	//std::cout << XmlStringToStdString(val) << "\n";
	//if(register_regex.FullMatch(XmlStringToStdString(val))) {
	if(uri_re.FullMatch(XmlStringToStdString(val)) && 
		 register_re.PartialMatch(XmlStringToStdString(val))) {
		//std::cout << "++++++Link match!+++++++++\n";
		return true;
	}
	return false;
}

bool couldHoldNonce(const xmlChar* val) {
	//pcrecpp::RE re(name_regex, opt);
	if(authname_re.PartialMatch(XmlStringToStdString(val))) {
		//std::cout << "match!";
		return true;
	}
	return false;
}

#if 0
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
#endif

bool FindToken(htmlNodePtr element, string url, Results *results)
{
	  bool is_hidden = false;
		bool name_is_auth = false;
		bool token_found = false;
		std::string auth_name;
    for(htmlNodePtr node = element; node != NULL; node = node->next)
    {
        if(node->type == XML_ELEMENT_NODE)
        {
            if(xmlStrcasecmp(node->name, (const xmlChar*)"input") == 0)
            {
							  is_hidden = false;
								name_is_auth = false;
                for(xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next)
                {
                    if(xmlStrcasecmp(attr->name, (const xmlChar*)"type") == 0)
                    {
											if(xmlStrcasecmp(attr->children->content, (const xmlChar*)"hidden") == 0) {
												//printf("hidden input tag found, how interesting...\n");
												is_hidden = true;
											}
                    }
                    else if(is_hidden && xmlStrcasecmp(attr->name, (const xmlChar*)"value") == 0) {
											if(isNonce(attr->children->content)) {
												token_found = true;
												//printf("possible hidden token value tag is %s\n", attr->children->content);
												results->AddUrlDefense(url, "Secret Validation Token", XmlStringToStdString(attr->children->content) );
												results->AddDefenseUrl("Secret Validation Token", url, XmlStringToStdString(attr->children->content) );
												//if pattern matches return true
											}
											else if(xmlStrlen(attr->children->content) == 0 && name_is_auth) {
												token_found = true;
												//printf("possible hidden token name tag is %s\n", auth_name.c_str());
												results->AddUrlDefense(url, "Possible Secret Validation Token", auth_name );
												results->AddDefenseUrl("Possible Secret Validation Token", url, auth_name );
											}
										}
										else if(is_hidden && xmlStrcasecmp(attr->name, (const xmlChar*)"name") == 0) {
											if(couldHoldNonce(attr->children->content)) {
												auth_name = XmlStringToStdString(attr->children->content);
												name_is_auth = true;
											}
										}
                }
            }
            if(node->children != NULL)
            {
                FindToken(node->children, url, results);
            }
        }
    }
    return false;
}

void FindRegisterLink(htmlNodePtr element,
											string url,
											Results *results,
											void (*process_url)(std::string, Results *results, unsigned int),
											unsigned int currDepth)
{
	  bool visited_link = false;
	  xmlAttrPtr attr, attr1, attr2;
    for(htmlNodePtr node = element; node != NULL; node = node->next)
    {
        if(node->type == XML_ELEMENT_NODE)
        {
#if 0
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
															//printf("Ha, found register button, now how we click on it?\n");
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
															//printf("Ha, submit register button found, now what?\n");
														}
													}
												}
											}
                      break;
										}
                }
            }
#endif
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
								if(attr->children && xmlStrcasecmp(attr->name, (const xmlChar*)"href") == 0) {
									//printf("node=%s\n", attr->children->content);
									if(isRegisterLink(attr->children->content)) {
										if(currDepth <= MAX_DEPTH) {
											//printf("Register link found:%s\n", attr->children->content);
											visited_link = true;
											process_url(XmlStringToStdString(attr->children->content), results, currDepth+1);
											//printf("returned from depth=%d\n", currDepth+1);
											break;
										}
									}
									else if(node->children && isRegisterLink(node->children->content)) {
										if(currDepth <= MAX_DEPTH) {
											//printf("Register link found:%s\n", attr->children->content);
											visited_link = true;
											process_url(XmlStringToStdString(attr->children->content), results, currDepth+1);
											//printf("returned from depth=%d\n", currDepth+1);
											break;
										}
									}
								}
							}
						}
						if(visited_link) break;
            if(node->children != NULL)
            {
                FindRegisterLink(node->children, url, results, process_url, currDepth);
            }
        }
    }
}

void parseHTML(const char* code,
							 string url,
							 Results *results,
							 void (*process_url)(std::string, Results *results, unsigned int),
							 unsigned int currDepth)
{
	  if(currDepth == MAX_DEPTH) return;
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
					  //printf("searching for token (depth=%d)\n", currDepth);
					  if(FindToken(root, url, results)) {
							//token found, add to possible csrf defences
							return;
						}
						else {
							//maybe we could check some links anyhow, to check if token is
							//indeed present throughout the session
							//we could also mark the sites that possibly use this defence, to see if
							//token changes when starting a new session
							//printf("no token was found, searching for  singIn/signUp forms\n");
							FindRegisterLink(root, url, results, process_url, currDepth);
						}
        }
        xmlFreeDoc(doc);
        doc = NULL;
    }
}
