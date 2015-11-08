#ifndef REGEX
#define REGEX

#include <string>
#include <vector>
#include <utility>
#include <map>
#include "match.hpp"


namespace regex
{
	
	/*----------------------------------------
	  RegexResult ： 匹配的结果
	  ----------------------------------------*/ 
	
	class RegexResult
	{
	public:
		RegexResult(std::string& testStr);
		void AddMatchRst(std::size_t begin, std::size_t length);
		void AddCaptureRst(std::string& name, std::size_t begin, std::size_t length);
		
		bool Bool();
		std::vector<std::string> Group();
		std::vector<std::string> Group(std::size_t idx);
		std::vector<std::string> Group(std::string name);

		std::vector<std::pair<std::size_t, std::size_t> > GroupIndex();
		std::list<std::pair<std::size_t, std::size_t> > GroupIndex(std::size_t idx);
		std::list<std::pair<std::size_t, std::size_t> > GroupIndex(std::string name);
		
		
	private:
		// [<begin, length>, <0, 1>, ...]
		std::vector<std::pair<std::size_t, std::size_t> > matchRst;
		// {"name":idx, "a":1, ...}
		std::map<std::string, std::size_t> captureNameMap;
		// [[<begin, length>, <begin, length>,...], [<0, 1>, <1, 2>, ...]]
		std::vector<std::list<std::pair<std::size_t, std::size_t> > > captureRst;
		std::string testStr;
	};

	/*----------------------------------------
	  Regex
	  ----------------------------------------*/ 
	class Regex
	{
	public:
		Regex(std::string regexStr);
		Regex(Regex&& regex);
		~Regex();

		RegexResult Search(std::string s, std::size_t idx=0);
		RegexResult Match(std::string s, std::size_t idx=0);
		RegexResult FindAll(std::string s, std::size_t idx=0);
		std::string Replace(std::string& s, std::string replace, int replaceNum=1, std::size_t idx=0);
		std::string ReplaceAll(std::string& s, std::string replace, std::size_t idx=0);

	private:
		regex_internal::Interpreter interpreter;
	};
	
}

#endif
