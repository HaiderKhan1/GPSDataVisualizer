UNAME := $(shell uname)
ROOT = ../
CC = gcc
CFLAGS = -Wall -std=c11 -g
LDFLAGS= -L.

INC = include/
SRC = src/
PARSER_SRC_FILES = $(wildcard src/GPX*.c)
PARSER_OBJ_FILES = $(patsubst src/GPX%.c,../GPX%.o,$(PARSER_SRC_FILES))

ifeq ($(UNAME), Linux)
	XML_PATH = /usr/include/libxml2
endif
ifeq ($(UNAME), Darwin)
	XML_PATH = /System/Volumes/Data/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/libxml2
endif

parser: $(ROOT)libgpxparser.so

$(ROOT)libgpxparser.so: $(PARSER_OBJ_FILES) $(ROOT)LinkedListAPI.o
	gcc -shared -o $(ROOT)libgpxparser.so $(PARSER_OBJ_FILES) $(ROOT)LinkedListAPI.o -lxml2 -lm

#Compiles all files named GPX*.c in src/ into object files, places all coresponding GPX*.o files in bin/
$(ROOT)GPX%.o: $(SRC)GPX%.c $(INC)LinkedListAPI.h $(INC)GPX*.h
	gcc $(CFLAGS) -I$(XML_PATH) -I$(INC) -c -fpic $< -o $@

$(ROOT)liblist.so: $(ROOT)LinkedListAPI.o
	$(CC) -shared -o $(ROOT)liblist.so $(ROOT)LinkedListAPI.o

$(ROOT)LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	$(CC) $(CFLAGS) -c -fpic -I$(INC) $(SRC)LinkedListAPI.c -o $(ROOT)LinkedListAPI.o

clean:
	rm -rf $(ROOT)StructListDemo $(ROOT)xmlExample $(ROOT)*.o $(ROOT)*.so


###################################################################################################
