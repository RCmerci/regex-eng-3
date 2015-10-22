DEBUG = -DREGEX_DEBUG -g

data: data.hpp data.cpp
	g++ --std=c++11 $(DEBUG) -c data.cpp

parser: parser.hpp parser.cpp
	g++ --std=c++11 $(DEBUG) -c parser.cpp
regexAlgorithm: regexAlgorithm.hpp regexAlgorithm.cpp
	g++ --std=c++11 $(DEBUG) -c regexAlgorithm.cpp

test: test.cpp parser data regexAlgorithm
	g++ --std=c++11 $(DEBUG) -c test.cpp
	g++ --std=c++11 -o test regexAlgorithm.o  parser.o data.o test.o

clear:
	rm *.o
