CC=g++
CFLAGS=-g
INCLUDE=-I../../curl/include -I../../libxml/include/libxml2
LIB_DIR=-L../../curl/lib -L../../libxml/lib
LIB=-lcurl -lxml2

all: crawler htmltitle

crawler: crawler.cc
	g++ $(INCLUDE) crawler.cc -o crawler.out $(LIB_DIR) $(LIB)

htmltitle: htmltitle.cc
	$(CC)  $(CFLAGS) $(INCLUDE) htmltitle.cc -o htmltitle.out $(LIB_DIR) $(LIB)

clean: 
	rm -f *.out 