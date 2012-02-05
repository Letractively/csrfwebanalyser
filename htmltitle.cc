/*****************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * $Id: htmltitle.cc,v 1.3 2005/02/09 15:15:01 giva Exp $
 */

// Get a web page, parse it with libxml.
//
// Written by Lars Nilsson , modified by Kasas and Dimos
//
// GNU C++ compile command line suggestion (edit paths accordingly):
//
// g++ -Wall -I/opt/curl/include -I/opt/libxml/include/libxml2 htmltitle.cc \
// -o htmltitle -L/opt/curl/lib -L/opt/libxml/lib -lcurl -lxml2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xmlstring.h>

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

//
//  libcurl variables for error strings and returned data

static char errorBuffer[CURL_ERROR_SIZE];
static std::string buffer;

//
//  libcurl write callback function
//

static int writer(char *data, size_t size, size_t nmemb,
                  std::string *writerData)
{
  if (writerData == NULL)
    return 0;

  writerData->append(data, size*nmemb);

  return size * nmemb;
}

//
//  libcurl connection initialization
//

static bool init(CURL *&conn, char *url)
{
  CURLcode code;
	char agent[128] = "Mozilla/4.73 [en] (X11; U; Linux 2.2.15 i686)";

  conn = curl_easy_init();

  if (conn == NULL)
  {
    fprintf(stderr, "Failed to create CURL connection\n");

    exit(EXIT_FAILURE);
  }

  code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
  if (code != CURLE_OK)
  {
    fprintf(stderr, "Failed to set error buffer [%d]\n", code);

    return false;
  }

  code = curl_easy_setopt(conn, CURLOPT_URL, url);
  if (code != CURLE_OK)
  {
    fprintf(stderr, "Failed to set URL [%s]\n", errorBuffer);

    return false;
  }
  //set user agent to fool server
	code = curl_easy_setopt(conn, CURLOPT_USERAGENT, agent);
	if(code != CURLE_OK)
	{
		fprintf(stderr, "Failed to set user-agent option [%s]\n", errorBuffer);

    return false;
	}

  code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1);
  if (code != CURLE_OK)
  {
    fprintf(stderr, "Failed to set redirect option [%s]\n", errorBuffer);

    return false;
  }

  code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
  if (code != CURLE_OK)
  {
    fprintf(stderr, "Failed to set writer [%s]\n", errorBuffer);

    return false;
  }

  code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer);
  if (code != CURLE_OK)
  {
    fprintf(stderr, "Failed to set write data [%s]\n", errorBuffer);

    return false;
  }

  return true;
}
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

bool FindToken(htmlNodePtr element)
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
												printf("hidden input tag found, how interesting...\n");
												is_hidden = true;
											}
                    }
                    else if(xmlStrcasecmp(attr->name, (const xmlChar*)"value") == 0 && is_hidden) {
												printf("hiddent value is %s\n", attr->children->content);
												//if pattern matches return true
										}
                }
            }
            if(node->children != NULL)
            {
                FindToken(node->children);
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
		xmlStrcasecmp(val, (const xmlChar*)"join now"))
			return true;
	
	if(xmlStrcasestr(val, (const xmlChar*)"signup") ||
		xmlStrcasestr(val, (const xmlChar*)"register") ||
		xmlStrcasestr(val, (const xmlChar*)"registration") || 
		xmlStrcasestr(val, (const xmlChar*)"login") ||
		xmlStrcasestr(val, (const xmlChar*)"join"))
		return true;

	return false;
}

void FindRegisterLink(htmlNodePtr element)
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
												printf("Submit button found, let's see...\n");
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
												printf("Register value found, let's see...\n");
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
                FindRegisterLink(node->children);
            }
        }
    }
}

void parseHTML(const char* code)
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
					  if(FindToken(root)) {
							//token found, add to possible csrf defences
							return;
						}
						else {
							//maybe we could check some links anyhow, to check if token is
							//indeed present throughout the session
							//we could also mark the sites that possibly use this defence, to see if
							//token changes when starting a new session
							FindRegisterLink(root);
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

int main(int argc, char *argv[])
{
  CURL *conn = NULL;
  CURLcode code;
  std::string title;

  // Ensure one argument is given

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <url>\n", argv[0]);

    exit(EXIT_FAILURE);
  }

  curl_global_init(CURL_GLOBAL_DEFAULT);

  // Initialize CURL connection

  if (!init(conn, argv[1]))
  {
    fprintf(stderr, "Connection initializion failed\n");

    exit(EXIT_FAILURE);
  }

  // Retrieve content for the URL

  code = curl_easy_perform(conn);
  curl_easy_cleanup(conn);

  if (code != CURLE_OK)
  {
    fprintf(stderr, "Failed to get '%s' [%s]\n", argv[1], errorBuffer);

    exit(EXIT_FAILURE);
  }

  // Parse the (assumed) HTML code

  //parseHtml(buffer, title);
	parseHTML(buffer.c_str());
  // Display the extracted title

  //printf("Title: %s\n", title.c_str());
  printf("Program Exited Normally\n");
  return EXIT_SUCCESS;
}
 
