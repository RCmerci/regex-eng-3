#include "regex.hpp"
#include "match.hpp"
#include "data.hpp"
#include "parser.hpp"
#include "automaton.hpp"
#include "exception.hpp"

namespace regex
{
	/*----------------------------------------
	  RegexResult ： 匹配的结果
	  ----------------------------------------*/
	RegexResult::RegexResult(std::string& _testStr)
		:testStr(_testStr)
	{
	}
	
	void RegexResult::AddMatchRst(std::size_t begin, std::size_t length)
	{
		matchRst.push_back(std::make_pair(begin, length));
	}

	void RegexResult::AddCaptureRst(std::string& name, std::size_t begin, std::size_t length)
	{
		bool isExist = captureNameMap.end() != captureNameMap.find(name);
		if (isExist) {
			captureRst[captureNameMap[name]].push_back(std::make_pair(begin, length)); 
		}
		else {
			captureRst.push_back(std::list<std::pair<std::size_t, std::size_t>>(1, std::make_pair(begin, length)));
			captureNameMap[name] = captureRst.size() - 1;
		}
	}

	bool RegexResult::Bool()
	{
		if (matchRst.empty()) {
			return false;
		}
		return true;
	}

	std::vector<std::string> RegexResult::Group()
	{
		std::vector<std::string> rst;
		for (auto& i : matchRst) {
			rst.push_back(testStr.substr(i.first, i.second));
		}
		return rst;
	}
	
	std::vector<std::string> RegexResult::Group(std::size_t idx)
	{
		std::vector<std::string> rst;
		auto captureIdxIter = captureNameMap.find(std::to_string(idx));
		if (captureIdxIter == captureNameMap.end()) {
			return rst;
		}
		std::size_t captureIdx = captureIdxIter->second;
		for (auto& i : captureRst[captureIdx]) {
			rst.push_back(testStr.substr(i.first, i.second));
		}
		return rst;
	}

	std::vector<std::string> RegexResult::Group(std::string name)
	{
		std::vector<std::string> rst;
		auto captureIdxIter = captureNameMap.find(name);
		if (captureIdxIter == captureNameMap.end()) {
			return rst;
		}
		std::size_t captureIdx = captureIdxIter->second;
		for (auto& i : captureRst[captureIdx]) {
			rst.push_back(testStr.substr(i.first, i.second));
		}
		return rst;
	}

	std::vector<std::pair<std::size_t, std::size_t> > RegexResult::GroupIndex()
	{
		return matchRst;
	}
	std::list<std::pair<std::size_t, std::size_t> > RegexResult::GroupIndex(std::size_t idx)
	{
		auto captureIdxIter = captureNameMap.find(std::to_string(idx));
		if (captureIdxIter == captureNameMap.end()) {
			return std::list<std::pair<std::size_t, std::size_t> >();
		}
		std::size_t captureIdx = captureIdxIter->second;
		return captureRst[captureIdx];
	}
	std::list<std::pair<std::size_t, std::size_t> > RegexResult::GroupIndex(std::string name)
	{
		auto captureIdxIter = captureNameMap.find(name);
		if (captureIdxIter == captureNameMap.end()) {
			return std::list<std::pair<std::size_t, std::size_t> >();
		}
		std::size_t captureIdx = captureIdxIter->second;
		return captureRst[captureIdx];
	}
	
	
	
	/*----------------------------------------
	  Regex
	  ----------------------------------------*/ 
	Regex::Regex(std::string regexStr)
		:interpreter(nullptr)
	{
		using namespace regex_internal;
		Expression::ref exp = ParseRegexStr(regexStr);
		if (exp) {
			exp->TransferUsingExp();
			exp->MergeCharset();
			exp->SetNumberOnAnonymousCaptureExpression();
			Automaton::ref epsilonNfa = exp->GenerateEpsilonNfa();
			Automaton::ref nfa = epsilonNfa->EpsilonNfa2Nfa();
			delete epsilonNfa;
			exp->Delete();
			interpreter = Interpreter(nfa);
		}
		else {
			interpreter = Interpreter(nullptr);
		}
	}
	Regex::Regex(Regex&& regex)
		:interpreter(nullptr)
	{
		interpreter = std::move(regex.interpreter);
	}
	Regex::~Regex()
	{
	}
	
	RegexResult Regex::Match(std::string s, std::size_t idx)
	{
		
		RegexResult rst(s);
		interpreter.Match(s, idx, rst);
		return rst;
	}

	
	RegexResult Regex::Search(std::string s, std::size_t idx)
	{
		// idx 可以等于 s.size()    --->    因为空字符串也可以被匹配，而且idx最小是0
		while(idx <= s.size()) {
			RegexResult&& rst = Match(s, idx);
			if (Match(s, idx).Bool()) {
				return rst;
			}
			idx++;
		}
		return RegexResult(s);
	}
	
	RegexResult Regex::FindAll(std::string s, std::size_t idx)
	{
		RegexResult rst(s);
		while(idx <= s.size()) {
			std::size_t length = interpreter.Match(s, idx, rst);
			if (length > 0) {
				idx += length;
			}
			else {
				idx++;
			}
		}
		return rst;
	}

	std::string Regex::Replace(std::string& s, std::string replace, int replaceNum, std::size_t idx)
	{
		std::string copyS = s;
		std::size_t currentIdx = idx;
		std::size_t replaceIdx = currentIdx;
		std::size_t len;
		int i=0;		
		while ((i < replaceNum || replaceNum == -1) && currentIdx <= s.size()) {
			RegexResult rst(s);
			len = interpreter.Match(s, currentIdx, rst);
			if (rst.Bool()) {
				replaceIdx = copyS.size() - (s.size() - currentIdx);
				copyS.replace(replaceIdx, len, replace);
				i++;
				if (len > 0) {
					currentIdx += len;
				}
				else {
					currentIdx++;
				}
			}
			else {
				currentIdx++;
			}
		}
		return copyS;
	}

	std::string Regex::ReplaceAll(std::string& s, std::string replace, std::size_t idx)
	{
		return Replace(s, replace, -1, idx);
	}
}
