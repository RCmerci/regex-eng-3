#include <iostream>
#include "parser.hpp"

using namespace regex::regex_internal;
int main()
{
	testAux();
	ParseRegexStr("abc()");
	ParseRegexStr("abc(a)");
	ParseRegexStr("abc(b)+?");
	ParseRegexStr("");
	ParseRegexStr("[^a-ff-f]abc()");
	ParseRegexStr("(yy*?)?");
	ParseRegexStr("a{1,}");
	ParseRegexStr("a{1,2}");
	ParseRegexStr("a{3,  1}");
	ParseRegexStr("a{ 1 , 2}");
	ParseRegexStr("a{1 ,   }");
	ParseRegexStr("(#<nnn>abd)(&<nnn>)+?");
	ParseRegexStr("^(=abc)(!bvc)$");

	
	std::cout<< "all parser test cases passed." <<std::endl;

	Expression::ref e1 = ParseRegexStr("abc");
	Expression::ref e2 = ParseRegexStr("bc");
	if (e1->Compare(*e2)) {
		std::cout<<"e1==e2"<<std::endl;
	}
	else {
		std::cout<<"e1!=e2"<<std::endl;
	}
}
