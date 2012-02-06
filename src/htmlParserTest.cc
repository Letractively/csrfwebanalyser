#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xmlstring.h>
#include "HTMLParser.h"
#include <list>
#include <includes.h>

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
	std::list<std::pair<CSRF_Defenses, std::string> > results;
	parseHTML(buffer.c_str(), &results);
  // Display the extracted title

  //printf("Title: %s\n", title.c_str());
  printf("Program Exited Normally\n");
  return EXIT_SUCCESS;
}

