version = release

DEBUG := -DREGEX_DEBUG -g -O0
INCLUDE := -I includes/
OBJECTS = source/parser.o source/data.o source/regexAlgorithm.o source/automaton.o source/utildata.o source/match.o source/regex.o

ifeq ($(version), debug)
FLAG = --std=c++11  $(DEBUG) $(INCLUDE) -c
else
FLAG = --std=c++11 -DNDEBUG $(INCLUDE) -c
endif

# .PHONY: release
release: $(OBJECTS)
	ar r libregex.a $(OBJECTS)

source/data.o: source/data.cpp includes/data.hpp 
	g++ $(FLAG) -o $@ $<

source/parser.o: source/parser.cpp  includes/parser.hpp
	g++ $(FLAG) -o $@ $<
source/regexAlgorithm.o:  source/regexAlgorithm.cpp includes/regexAlgorithm.hpp
	g++ $(FLAG) -o $@ $< 

source/automaton.o:   source/automaton.cpp includes/automaton.hpp
	g++ $(FLAG) -o $@ $< 
source/utildata.o: source/utildata.cpp includes/utildata.hpp 
	g++ $(FLAG) -o $@ $< 

source/match.o: source/match.cpp includes/match.hpp 
	g++ $(FLAG) -o $@ $< 
source/regex.o: source/regex.cpp includes/regex.hpp 
	g++ $(FLAG) -o $@ $< 
source/test/test.o : source/test/test.cpp
	g++ $(FLAG) -o $@ $< 


source/test/test:  $(OBJECTS) source/test/test.o
	g++ --std=c++11 -o $@ $^ 
	./source/test/test



test : source/test/test
clear:
	rm source/*.o
	rm source/test/*.o
	rm source/test/test
	rm libregex.a
