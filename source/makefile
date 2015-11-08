
DEBUG = -DREGEX_DEBUG -g -O0

data: data.hpp data.cpp
	g++ --std=c++11 $(DEBUG) -c data.cpp

parser: parser.hpp parser.cpp
	g++ --std=c++11 $(DEBUG) -c parser.cpp
regexAlgorithm: regexAlgorithm.hpp regexAlgorithm.cpp
	g++ --std=c++11 $(DEBUG) -c regexAlgorithm.cpp

automaton: automaton.hpp automaton.cpp
	g++ --std=c++11 $(DEBUG) -c automaton.cpp
utildata: utildata.hpp utildata.cpp
	g++ --std=c++11 $(DEBUG) -c utildata.cpp

match: match.hpp match.cpp
	g++ --std=c++11 $(DEBUG) -c match.cpp
regex: regex.hpp regex.cpp
	g++ --std=c++11 $(DEBUG) -c regex.cpp

test: test/test.cpp parser data regexAlgorithm automaton utildata match regex
	g++ --std=c++11 $(DEBUG) -c test/test.cpp -o test/test.o
	g++ --std=c++11 -o test/test regexAlgorithm.o  parser.o data.o test/test.o utildata.o automaton.o match.o regex.o
	./test/test
clear:
	rm *.o
