#include <cassert>
#include <vector>
#include <utility>
#include <string>

#ifdef REGEX_DEBUG
#include <iostream>
#endif

#include "parser.hpp"
#include "data.hpp"
#include "exception.hpp"


namespace regex
{
	namespace regex_internal
	{
		static Expression* ParseOr(std::string& regexStr);
		/*****************************************
		  辅助函数
		*****************************************/

		static bool isChar(std::string& regexStr, char c)
		{
			if (regexStr.length() == 0) return false;
			if (regexStr[0] == c) {
				regexStr.erase(0, 1);
				return true;
			}
			return false;
		}
		static bool isCharPure(const std::string& regexStr, char c)
		{
			if (regexStr.length() == 0) return false;
			if (regexStr[0] == c) return true;
			return false;
		}
		static bool isInChars(char c, const char* cs)
		{
			while(*cs) {
				if (c == *cs) {
					return true;
				}
				cs++;
			}
			return false;

		}

		// static bool isInChars(std::string& regexStr, const char* _cs)
		// {
		// 	if (regexStr.length() == 0) return false;
		// 	char c = regexStr[0];
		// 	const char* cs = _cs;
		// 	bool rst = isInChars(c, cs);
		// 	if (rst) {
		// 		regexStr.erase(0, 1);
		// 	}
		// 	return rst;
		// }
		static bool isInCharsPure(std::string& regexStr, const char* _cs)
		{
			if (regexStr.length() == 0) return false;
			char c = regexStr[0];
			const char* cs = _cs;
			return isInChars(c, cs);
		}
		static bool reduceChar(std::string& regexStr, int n)
		{
			if (regexStr.length() < n)
				return false;
			regexStr.erase(0, n);
			return true;
		}
		static bool isStr(std::string& regexStr, std::string s)
		{
			if (!regexStr.compare(0, s.length(), s)) {
				regexStr.erase(0, s.length());
				return true;
			}
			return false;
		}
		// static bool isStrPure(std::string& regexStr, std::string s)
		// {
		// 	if (!regexStr.compare(0, s.length(), s)) {
		// 		return true;
		// 	}
		// 	return false;
		// }
		static std::string isName(std::string& regexStr)
		{
			if (regexStr.length() == 0) {
				return "";
			}
			int i = 0;
			while (regexStr[i]) {
				if ((regexStr[i] >= 'a' && regexStr[i] <= 'z') ||
				    (regexStr[i] >= 'A' && regexStr[i] <= 'Z') ||
				    (regexStr[i] == '_') ||
				    (regexStr[i] >= '0' && regexStr[i] <= '9' && i != 0)) {
					i++;
				}
				else {
					break;
				}
			}
			std::string rst = regexStr.substr(0, i);
			regexStr.erase(0, i);
			return rst;
		}

#ifdef REGEX_DEBUG
		void testAux()
		{
			std::string s = "a";
			assert(isChar(s, 'a') == true);
			assert(isChar(s, 'a') == false);
			s = "a";
			assert(isCharPure(s, 'a') == true);
			assert(isCharPure(s, 'a') == true);
			assert(isInChars('a', "U(9)9Y") == false);
			assert(isInChars('a', "hasuu") == true);
			s = "a";
			assert(isInCharsPure(s, "Uyduus") == false);
			assert(isInCharsPure(s, "oooaie") == true);
			s = "a";
			assert(reduceChar(s, 1) == true);
			assert(reduceChar(s, 1) == false);
			s = "a";
			assert(isStr(s, "s") == false);
			assert(isStr(s, "a") == true);
			s = "a";
			assert(isStr(s, "as") == false);
			assert(isStr(s, "a") == true);
			assert(s.length() == 0);
			s = "aa";
			assert(isName(s) == "aa");
			assert(isName(s) == "");
			s = "sdd_A 0,";
			assert(isName(s) == "sdd_A");
			std::cout << "parser aux funcs test passed." << std::endl;
		}
#endif
		/*****************************************
		  Parser
		*****************************************/
		static std::string NormalizeRegexStr(const std::string& regexStr)
		{
			int i, len = regexStr.length();
			std::string rst;
			for (i=0;i<len;i++) {
				if (regexStr[i] == '\\' && len > i+1) {
					if (!isInChars(regexStr[i+1], "sSwWdD\\")) {
						rst.push_back(regexStr[i+1]);
						i++;
						continue;
					}
				}
				else if (isCharPure(regexStr, '.')) {
					rst.push_back('\\');
				}
				rst.push_back(regexStr[i]);
			}
			return rst;
		}
		static std::pair<int, int> HandleLoopRange(std::string& regexStr)
		{
			int min=0, max=-1;
			std::string minStr="", maxStr="";
			if (! isChar(regexStr, '{')) {
				throw ParserException("wrong loop expression.\n");
			}
			while (isChar(regexStr, ' ')) ;
			if (isChar(regexStr, ',')) {
				min = 0;
			}
			else {
				while(regexStr[0] && regexStr[0] >= '0' && regexStr[0] <= '9') {
					minStr.push_back(regexStr[0]);
					reduceChar(regexStr, 1);
				}
				while (isChar(regexStr, ' ')) ;
				if (! isChar(regexStr, ',')) {
					throw ParserException(" `,' expected in loop expression.\n");
				}
			}
			while (isChar(regexStr, ' ')) ;
			while (regexStr[0] && regexStr[0] >= '0' && regexStr[0] <= '9') {
				maxStr.push_back(regexStr[0]);
				reduceChar(regexStr, 1);
			}
			while (isChar(regexStr, ' ')) ;
			if (! isChar(regexStr, '}')) {
				throw ParserException(" } expected in loop expression.\n");
			}

			if (minStr.length() == 0) {
				minStr = "0";
			}
			if (maxStr.length() == 0) {
				maxStr = "-1";
			}
			min = std::stoi(minStr);
			max = std::stoi(maxStr);
			return std::make_pair(min, max);
		}
		static Expression* ParseLoop(std::string& regexStr)
		{
			int max=1, min=0;
			bool greedy = true;
			if (regexStr.length() == 0 ) {
				return nullptr;
			}
			else if (isChar(regexStr, '*')) {
				min = 0;
				max = -1;
			}
			else if (isChar(regexStr, '+')) {
				min = 1;
				max = -1;
			}
			else if (isCharPure(regexStr, '{')){
				std::pair<int, int> range = HandleLoopRange(regexStr);
				min = range.first;
				max = range.second;
			}
			else if (isCharPure(regexStr, '?')) {
				
			}
			else {
				//__2323
				return nullptr;
			}
			if (isChar(regexStr, '?')) {
				greedy = false;
			}
			LoopExp* loopExp = new LoopExp;
			loopExp->min = min;
			loopExp->max = max;
			loopExp->greedy = greedy;
			return loopExp;
			
		}
		static Expression* ParseGroup(std::string& regexStr)
		{
			if (regexStr.length() == 0) {
				return nullptr;
			}
			else if (isStr(regexStr, "(?<")) { // name capture
				std::string name = isName(regexStr);
				if (!name.length()) {
					goto ParseGroup_expected_name;
				}
				if (!isChar(regexStr, '>')) {
					goto ParseGroup_expected_right_angle_bracket;
				}
				Expression* expression = ParseOr(regexStr);
				if (!isChar(regexStr, ')')) {
					goto ParseGroup_expected_right_bracket;
				}
				CaptureExp* captureExp = new CaptureExp;
				captureExp->name = name;
				captureExp->innerExp = expression;
				return captureExp;
				
			}
			else if (isStr(regexStr, "(?")) { // anonymous capture
				Expression* expression = ParseOr(regexStr);
				if (!isChar(regexStr, ')')) {
					goto ParseGroup_expected_right_bracket;

				}
				CaptureExp* captureExp = new CaptureExp;
				captureExp->name = "";
				captureExp->innerExp = expression;
				return captureExp;
			}
			else if (isStr(regexStr, "(=")) { // positive
				Expression* expression = ParseOr(regexStr);
				if (!isChar(regexStr, ')')) {
					goto ParseGroup_expected_right_bracket;
				}
				PositiveExp* positiveExp = new PositiveExp;
				positiveExp->innerExp = expression;
				return positiveExp;
			}
			else if (isStr(regexStr, "(!")) { // negative
				Expression* expression = ParseOr(regexStr);
				if (!isChar(regexStr, ')')) {
					goto ParseGroup_expected_right_bracket;
				}
				NegativeExp* negativeExp = new NegativeExp;
				negativeExp->innerExp = expression;
				return negativeExp;
			}
			else if (isStr(regexStr, "(#<")) { // using create
				std::string name = isName(regexStr);
				if (!name.length()) {
					goto ParseGroup_expected_name;
				}
				if (!isChar(regexStr, '>')) {
					goto ParseGroup_expected_right_angle_bracket;
				}
				Expression* expression = ParseOr(regexStr);
				if (!isChar(regexStr, ')')) {
					goto ParseGroup_expected_right_bracket;
				}
				UsingExp* usingExp = new UsingExp;
				usingExp->name = name;
				usingExp->innerExp = expression;
				usingExp->isCreate = true;
				return usingExp;
			}
			else if (isStr(regexStr, "(&<")) { // using apply
				std::string name = isName(regexStr);
				if (!name.length()) {
					goto ParseGroup_expected_name;
				}
				if (!isChar(regexStr, '>')) {
					goto ParseGroup_expected_right_angle_bracket;
				}
				if (!isChar(regexStr, ')')) {
					goto ParseGroup_expected_right_bracket;
				}
				UsingExp* usingExp = new UsingExp;
				usingExp->name = name;
				usingExp->innerExp = nullptr;
				usingExp->isCreate = false;
				return usingExp;
			}
			else if (isStr(regexStr, "(")) { // common (
				Expression* expression = ParseOr(regexStr);
				if (!isChar(regexStr, ')')) {
					goto ParseGroup_expected_right_bracket;
				}
				return expression;
			}
			else {
				return nullptr;
			}
			// else {
			// 	std::string expStr = "[Panic] ";
			// 	throw ParserException(expStr+"file:"+__FILE__+",line:"+std::to_string(__LINE__)+",func:"+__func__+"\n");
			// }

		ParseGroup_expected_right_bracket:
			throw ParserException(") expected in expression.\n");
		ParseGroup_expected_right_angle_bracket:
			throw ParserException("> expected in expression.\n");
		ParseGroup_expected_name:
			throw ParserException("<name> expected in expression.\n");
		}


		static std::pair<std::vector<CharRange>, bool> HandleEscapeChar(char c)
		{
			bool isReverse = false;
			std::vector<CharRange> vcr;
			switch (c) {
			case 'D':
				isReverse = true;
			case 'd':
				vcr.push_back(CharRange('0', '9'));
				return std::make_pair(vcr, isReverse);
			case 'W':
				isReverse = true;
			case 'w':
				vcr.push_back(CharRange('a', 'z'));
				vcr.push_back(CharRange('A', 'Z'));
				vcr.push_back(CharRange('0', '9'));
				vcr.push_back(CharRange('_', '_'));
				return std::make_pair(vcr, isReverse);
			case 'S':
				isReverse = true;
			case 's':
				vcr.push_back(CharRange('\t', '\t'));
				vcr.push_back(CharRange(' ', ' '));
				vcr.push_back(CharRange('\n', '\n'));
				vcr.push_back(CharRange('\r', '\r'));
				return  std::make_pair(vcr, isReverse);
			case '.':
				vcr.push_back(CharRange(0, '\n'-1));
				vcr.push_back(CharRange('\n'+1, 127));
				return std::make_pair(vcr, isReverse);
			default:
				vcr.push_back(CharRange(c, c));
				return std::make_pair(vcr, isReverse);

			}
		}
		static bool HandleInnerCharset(std::string& regexStr, CharsetExp* charsetExp)
		{
			if (regexStr.length() == 0)
				return true;
			bool isReverse;
			while (true) {
				isReverse = false;
				if (isChar(regexStr, '\\')) {
					if (regexStr.length() == 0) {
						goto HandleInnerCharset_expression_expected_after_backslash;
					}
					char c = regexStr[0];
					auto vcrPair = HandleEscapeChar(c);
					for (int i=0;i<vcrPair.first.size();i++) {
						charsetExp->AddCharRange(vcrPair.first[i], vcrPair.second);
					}
					reduceChar(regexStr, 1);
				}
				else if (isChar(regexStr, '-')) {
					goto HandleInnerCharset_expression_expected_before_minus;
				}
				else {
					if (regexStr.length() == 0) {
						goto HandleInnerCharset_not_complete_charset;
					}
					char c = regexStr[0];
					switch (c) {
					case ']':
						return true;
					default:
						if (regexStr.length() > 1 && regexStr[1] == '-') {
							if (regexStr.length() == 2 || regexStr[2] == ']') {
								goto HandleInnerCharset_not_complete_charset;
							}
							if (regexStr[2] == '\\' && regexStr.length() == 3) {
								goto HandleInnerCharset_not_complete_charset;
							}
							if (regexStr[2] == '\\' &&
							    (regexStr[3] == 's' || regexStr[3] == 'S' || regexStr[3] == 'd' || regexStr[3] == 'D' || regexStr[3] == 'w' || regexStr[3] == 'W')) {
								goto HandleInnerCharset_bad_range;
							}
							//
							if (regexStr[2] == '\\') {
								assert(regexStr[3] == '\\');
								if (regexStr[3] < c) {
									goto HandleInnerCharset_bad_range;
								}
								charsetExp->AddCharRange(CharRange(c, regexStr[3]), false);
								reduceChar(regexStr, 4);
							}
							else {
								if (regexStr[2] < c) {
									goto HandleInnerCharset_bad_range;
								}
								charsetExp->AddCharRange(CharRange(c, regexStr[2]), false);
								reduceChar(regexStr, 3);
							}
						       
						}
						else {
							charsetExp->AddCharRange(CharRange(c, c), false);
							reduceChar(regexStr, 1);
						}
					}
				}
			}


		HandleInnerCharset_bad_range:
			throw ParserException("bad charset range.\n");
		HandleInnerCharset_not_complete_charset:
			throw ParserException("not complete charset expression.\n");
		HandleInnerCharset_expression_expected_after_backslash:
			throw ParserException("expression expected after \"\\\".\n");
		HandleInnerCharset_expression_expected_before_minus:
			throw ParserException("expression expected before \"-\".\n");
		}
		static Expression* ParseCharset(std::string& regexStr)
		{
			if (regexStr.length() == 0) {
				return nullptr;
			}
			else if (isChar(regexStr, '^')) {
				return new BeginExp;
			}
			else if (isChar(regexStr, '$')) {
				return new EndExp;
			}
			else if (isChar(regexStr, '[')) {
				CharsetExp* charsetExp = new CharsetExp;
				if (isChar(regexStr, '^')) {
					charsetExp->reverse = true;
				}
				HandleInnerCharset(regexStr, charsetExp);
				if (!isChar(regexStr, ']')) {
					throw ParserException("\"]\" expected for charset expression.\n");
				}
				return charsetExp;
			}
			else if (isInCharsPure(regexStr, "()]*?+{}|")) {
				return nullptr;
			}
			else if (isChar(regexStr, '\\')) {
				if (regexStr.length() == 0) {
					throw ParserException("not complete regular expression.\n");
				}
				CharsetExp* charsetExp = new CharsetExp;
				auto vcrPair = HandleEscapeChar(regexStr[0]);
				reduceChar(regexStr, 1);
				for (int i=0;i<vcrPair.first.size();i++) {
					charsetExp->AddCharRange(vcrPair.first[i], vcrPair.second);
				}
				return charsetExp;
			}
			else {
				CharsetExp* charsetExp = new CharsetExp;
				charsetExp->AddCharRange(CharRange(regexStr[0], regexStr[0]), false);
				reduceChar(regexStr, 1);
				return charsetExp;
			}
		}
		static Expression* ParsePar(std::string& regexStr)
		{
			Expression* rst = ParseCharset(regexStr);
			if (!rst) {
				rst = ParseGroup(regexStr);
			}
			if (rst) {
				Expression* loopExp_ = ParseLoop(regexStr);
				if (loopExp_) {
					LoopExp* loopExp = static_cast<LoopExp*>(loopExp_);
					loopExp->innerExp = rst;
					rst = loopExp;
				}
			}
			return rst;
		}
		static Expression* ParseJoin(std::string& regexStr)
		{
			Expression* rst = ParsePar(regexStr);
			while (true) {
				Expression* right = ParsePar(regexStr);
				if (right) {
					JoinExp* joinExp = new JoinExp;
					joinExp->left = rst;
					joinExp->right = right;
					rst = joinExp;
				}
				else {
					break;
				}
			}
			return rst;
		}
		static Expression* ParseOr(std::string& regexStr)
		{
			Expression * joinExp = ParseJoin(regexStr);
			Expression * rst = joinExp;
			while(true) {
				if (isChar(regexStr, '|')) {
					OrExp * orExp = new OrExp;
					Expression * joinExp2 = ParseJoin(regexStr);
					orExp->opt1 = rst;
					orExp->opt2 = joinExp2;
					rst = orExp;
				}
				else {
					break;
				}
			}
			return rst;
		}
		Expression::ref ParseRegexStr(const std::string& _regexStr)
		{
			std::string regexStr = NormalizeRegexStr(_regexStr);
			Expression* expression = ParseOr(regexStr);
			if (regexStr.length() != 0) {
				std::string expStr = "illegel expression: ";
				throw ParserException(expStr + regexStr);
			}
			return expression;
		}

	}
}
