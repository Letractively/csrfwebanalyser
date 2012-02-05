CC=g++
CFLAGS=-g
INCLUDE=-Iinclude/ -Ilibcurl/include/ -Ilibxml/include/libxml2/
LIB_DIR=-Llibcurl/lib -Llibxml/lib
LIB=-lcurl -lxml2
BIN_DIR=bin
OBJ_DIR=obj
SRC_DIR=src
INCLUDE_DIR=include

VPATH=$(SRC_DIR):$(OBJ_DIR):$(INCLUDE_DIR)

all: crawler

HTMLParser.o: HTMLParser.cc HTMLParser.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $(OBJ_DIR)/$@ 

HTTPHeaderParser.o: HTTPHeaderParser.cc HTTPHeaderParser.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $(OBJ_DIR)/$@ 

crawler: crawler.cc HTTPHeaderParser.o HTMLParser.o
	$(CC)  $(CFLAGS) $(INCLUDE) $^ -o $(BIN_DIR)/$@ $(LIB_DIR) $(LIB)

htmlParser:

clean: 
	rm -f $(BIN_DIR)/* $(OBJ_DIR)/*

clear: 
	rm -f $(SRC_DIR)/*~ $(INCLUDE_DIR)/*~ *~