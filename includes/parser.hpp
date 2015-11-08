#ifndef REGEX_PARSER
#define REGEX_PARSER

#include <string>
#include "data.hpp"



namespace regex
{
	namespace regex_internal
	{
		#ifdef REGEX_DEBUG
		void testAux();
		#endif
		Expression::ref ParseRegexStr(const std::string& regStr);
	}
}


#endif
