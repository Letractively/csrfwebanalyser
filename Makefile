CC=g++
CFLAGS=-g -O3
INCLUDE=-Iinclude/ -Ilibcurl/include/ -Ilibxml/include/libxml2/ -Ilibpcre/include/
LIB_DIR=-Llibcurl/lib -Llibxml/lib -Llibpcre/lib
LIB=-lcurl -lxml2 -lpcre -lpcrecpp
BIN_DIR=bin
OBJ_DIR=obj
SRC_DIR=src
INCLUDE_DIR=include

VPATH=$(SRC_DIR):$(INCLUDE_DIR)

all: crawler

$(OBJ_DIR)/HTMLParser.o: HTMLParser.cc HTMLParser.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@ 

$(OBJ_DIR)/HTTPHeaderParser.o: HTTPHeaderParser.cc HTTPHeaderParser.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@ 

$(OBJ_DIR)/Results.o: Results.cc Results.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

crawler: crawler.cc $(OBJ_DIR)/Results.o $(OBJ_DIR)/HTTPHeaderParser.o $(OBJ_DIR)/HTMLParser.o
	$(CC)  $(CFLAGS) $(INCLUDE) $^ -o $(BIN_DIR)/$@ $(LIB_DIR) $(LIB)

htmlParser: htmlParserTest.cc Results.cc $(OBJ_DIR)/HTMLParser.o
	$(CC)  $(CFLAGS) $(INCLUDE) $^ -o $(BIN_DIR)/$@ $(LIB_DIR) $(LIB)

clean: 
	rm -f $(BIN_DIR)/* $(OBJ_DIR)/*

clear: 
	rm -f $(SRC_DIR)/*~ $(INCLUDE_DIR)/*~ *~