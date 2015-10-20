#include <iostream>
#include "parser.hpp"
int main()
{
	regex::regex_internal::testAux();
	regex::regex_internal::ParseRegexStr("abc()");
	regex::regex_internal::ParseRegexStr("abc(a)");
	regex::regex_internal::ParseRegexStr("abc(b)+?");
	regex::regex_internal::ParseRegexStr("");
	regex::regex_internal::ParseRegexStr("[^a-ff-f]abc()");
	regex::regex_internal::ParseRegexStr("(yy*?)?");
	regex::regex_internal::ParseRegexStr("a{1,}");
	regex::regex_internal::ParseRegexStr("a{1,2}");
	regex::regex_internal::ParseRegexStr("a{3,  1}");
	regex::regex_internal::ParseRegexStr("a{ 1 , 2}");
	regex::regex_internal::ParseRegexStr("a{1 ,   }");
	regex::regex_internal::ParseRegexStr("(#<nnn>abd)(&<nnn>)+?");
	regex::regex_internal::ParseRegexStr("^(=abc)(!bvc)$");

	

	std::cout<< "all parser test cases passed." <<std::endl;

}
