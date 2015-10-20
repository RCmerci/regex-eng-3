DEBUG = -DREGEX_DEBUG -g

data: data.hpp data.cpp
	g++ --std=c++11 $(DEBUG) -c data.cpp

parser: parser.hpp parser.cpp
	g++ --std=c++11 $(DEBUG) -c parser.cpp


test: test.cpp parser data
	g++ --std=c++11 $(DEBUG) -c test.cpp
	g++ --std=c++11 -o test test.o parser.o data.o

clear:
	rm *.o
