#include <iostream>
#include <cassert>
#include <memory>
#include <fstream>
#include <streambuf>
#include "../parser.hpp"
#include "../match.hpp"
#include "../regex.hpp"

using namespace regex::regex_internal;
using namespace regex;
std::shared_ptr<Regex> CreateInterpreter(std::string s)
{
	return std::make_shared<Regex>(s);
}

// std::shared_ptr<Interpreter> CreateInterpreter(std::string s)
// {
// 	Expression::ref exp = ParseRegexStr(s);
// 	if (exp) {
// 		exp->TransferUsingExp();
// 		exp->MergeCharset();
// 		Automaton::ref atm1 = exp->GenerateEpsilonNfa();
// 		auto nfa = atm1->EpsilonNfa2Nfa();
// 		delete atm1;
// 		// delete exp;
// 		exp->Delete();
// 		return std::make_shared<Interpreter>(nfa);
// 	}
// 	return std::make_shared<Interpreter>(nullptr);
	
// }


int main()
{
	testAux();
	ParseRegexStr("abc()")->Delete();
	ParseRegexStr("abc(a)")->Delete();
	ParseRegexStr("abc(b)+?")->Delete();
	ParseRegexStr("");
	ParseRegexStr("[^a-ff-f]abc()")->Delete();
	ParseRegexStr("(yy*?)?")->Delete();
	ParseRegexStr("a{1,}")->Delete();
	ParseRegexStr("a{1,2}")->Delete();
	ParseRegexStr("a{3,  1}")->Delete();
	ParseRegexStr("a{ 1 , 2}")->Delete();
	ParseRegexStr("a{1 ,   }")->Delete();
	ParseRegexStr("(#<nnn>abd)(&<nnn>)+?")->Delete();
	ParseRegexStr("^(=abc)(!bvc)$")->Delete();


	std::cout<< "all parser test cases passed." <<std::endl;
	// {
	// Expression::ref e1 = ParseRegexStr("|a()");
	// Expression::ref e2 = ParseRegexStr("|a");
	// if (e1->Compare(*e2)) {
	// 	std::cout<<"e1==e2"<<std::endl;
	// }
	// else {
	// 	std::cout<<"e1!=e2"<<std::endl;
	// }
	// }
	// Expression::ref e3 = ParseRegexStr("(?[0-85-9a-hb-xh-z])");
	// CaptureExp* e4 = dynamic_cast<CaptureExp*>(e3);
	// CharsetExp* e5 = dynamic_cast<CharsetExp*>(e4->innerExp);
	// if (e5) {
	// 	for (auto cr : e5->charRangeL) {
	// 		std::cout<<"("<<cr.begin<<","<<cr.end<<")"<<std::endl;
	// 	}
	// }
	// e3->MergeCharset();
	// for (auto cr : e5->charRangeL) {
	// 	std::cout<<"("<<cr.begin<<","<<cr.end<<")"<<std::endl;
	// }
	// Expression::ref e3 = ParseRegexStr("(#<name>fuck)(&<name>)");
	// auto e4 = dynamic_cast<JoinExp*>(e3);
	// assert(dynamic_cast<UsingExp*>(e4->left));
	// assert(dynamic_cast<UsingExp*>(e4->right));
	// e3->TransferUsingExp();
	// auto e5 = dynamic_cast<JoinExp*>(e3);
	// assert(!dynamic_cast<UsingExp*>(e5->left));
	// assert(!dynamic_cast<UsingExp*>(e5->right));
	// assert(dynamic_cast<JoinExp*>(e5->left));
	// auto p = dynamic_cast<JoinExp*>(e5->left);
	
	
	{
		Expression::ref exp = ParseRegexStr("ab");
		exp->TransferUsingExp();
		exp->MergeCharset();
		Automaton::ref atm1 = exp->GenerateEpsilonNfa();
		assert(atm1->beginStatus->outEdges.size() == 1);
		assert(atm1->beginStatus->outEdges[0]->isEpsilon == false);
		assert(atm1->beginStatus->outEdges[0]->content.front().begin == 'a');
		auto s2 = atm1->beginStatus->outEdges[0]->target;
		assert(s2->outEdges.size()==1);
		assert(s2->outEdges[0]->isEpsilon == false);
		assert(s2->outEdges[0]->content.front().begin == 'b');
		assert(s2->outEdges[0]->target->isFinal == true);
		auto s3 = s2->outEdges[0]->target;
		assert(s3->outEdges.size() == 0);
		// auto nfa = atm1->EpsilonNfa2Nfa();

		auto interpreter = CreateInterpreter("ab");
		// Interpreter interpreter(nfa);
		assert(interpreter->Match("ab").Bool() == true);
		assert(interpreter->Match("a").Bool()==false);
		delete atm1;
		exp->Delete();
	}
	{
		Expression::ref exp = ParseRegexStr("a?");
		exp->TransferUsingExp();
		exp->MergeCharset();
		Automaton::ref atm1 = exp->GenerateEpsilonNfa();
		//           e          0.a	       e
		//begin ---------->s1------->mid ---------+
		//         loop    |   1.e		  |     e
		//                 ------------------->  s2 -------> end
		//                                            end
		assert(atm1->beginStatus->outEdges.size()==1);
		assert(atm1->beginStatus->outEdges[0]->isEpsilon == true);
		assert(atm1->beginStatus->outEdges[0]->typeList.size()==1);
		assert(atm1->beginStatus->outEdges[0]->typeList.front().first == Edge::Type::Loop);
		auto s1 = atm1->beginStatus->outEdges[0]->target;
		assert(s1->outEdges.size()==2);
		assert(s1->outEdges[0]->isEpsilon == false);
		assert(s1->outEdges[0]->content.front().begin == 'a');
		assert(s1->outEdges[1]->isEpsilon == true);
		auto mid = s1->outEdges[0]->target;
		auto s2 = s1->outEdges[1]->target;
		assert(mid->outEdges.size() == 1);
		assert(mid->outEdges[0]->isEpsilon == true);
		assert(mid->outEdges[0]->target == s2);
		assert(s2->outEdges.size() == 1);
		assert(s2->outEdges[0]->isEpsilon==true);
		assert(s2->outEdges[0]->target->isFinal == true);
		auto nfa = atm1->EpsilonNfa2Nfa();
		//             1. e
		// begin ---------------->end
		//           loop,end
		//            0. a
		// begin --------------> mid
		//            loop
		//             e
		// mid --------------> end
		//           end
		assert(nfa->beginStatus->outEdges.size() == 2);
		assert(nfa->beginStatus->outEdges[1]->isEpsilon == true);
		assert(nfa->beginStatus->outEdges[1]->typeList.size()==2);
		assert(nfa->beginStatus->outEdges[1]->typeList.front().first==Edge::Type::Loop);
		assert(nfa->beginStatus->outEdges[1]->typeList.back().first==Edge::Type::End);
		assert(nfa->beginStatus->outEdges[0]->isEpsilon == false);
		assert(nfa->beginStatus->outEdges[0]->content.front().begin=='a');
		assert(nfa->beginStatus->outEdges[0]->typeList.size()==1);
		assert(nfa->beginStatus->outEdges[0]->typeList.front().first==Edge::Type::Loop);
		auto mid2 = nfa->beginStatus->outEdges[0]->target;
		assert(mid2->outEdges.size()==1);
		assert(mid2->outEdges[0]->isEpsilon==true);
		assert(mid2->outEdges[0]->typeList.size()==1);
		assert(mid2->outEdges[0]->typeList.front().first == Edge::Type::End);
		assert(mid2->outEdges[0]->target==nfa->beginStatus->outEdges[1]->target);
		assert(mid2->outEdges[0]->target->isFinal==true);
		auto interpreter = CreateInterpreter("a?");
		// Interpreter interpreter(nfa);
		assert(interpreter->Match("a").Bool() == true);


		exp->Delete();
		delete atm1;
	}
	{
		auto interpreter = CreateInterpreter("aa?");
		assert(interpreter->Match("a").Bool()==true);
		assert(interpreter->Match("a").Group().size()==1);
		assert(interpreter->Match("a").Group()[0]=="a");
		assert(interpreter->Match("a").Group(1).size()==0);
		
		assert(interpreter->Match("ab").Bool()==true);
		assert(interpreter->Match("ab").Group()[0]=="a");
		assert(interpreter->Match("ab").Group().size()==1);
		assert(interpreter->Match("ab").Group(1).size()==0);
		
		assert(interpreter->Match("aa").Bool()==true);
		assert(interpreter->Match("aa").Group().size()==1);
		assert(interpreter->Match("aa").Group()[0]=="aa");
		assert(interpreter->Match("aa").Group(1).size()==0);
		
		assert(interpreter->Match("ba").Bool()==false);
		assert(interpreter->Match("ba").Group().size()==0);
		
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("").Group().size()==0);

		
		assert(interpreter->Match("bb").Bool()==false);
		assert(interpreter->Match("bb").Group().size()==0);
		
	}
	
	{
		auto interpreter = CreateInterpreter("[2-9]");
		assert(interpreter->Match("19").Bool()==false);
		assert(interpreter->Match("19").Group().size()==0);
		assert(interpreter->Search("19").Bool()==true);
		assert(interpreter->Search("19").Group().size()==1);
		
		
		assert(interpreter->Match("29").Bool()==true);
		assert(interpreter->Match("29").Group().size()==1);
		assert(interpreter->Match("29").Group()[0]=="2");
		assert(interpreter->Match("29").Group(1).size()==0);
		assert(interpreter->Search("29").Group().size()==1);
		assert(interpreter->Search("29").Group()[0]=="2");
		assert(interpreter->Search("29", 1).Group()[0]=="9");
		assert(interpreter->FindAll("29").Group().size()==2);
		assert(interpreter->FindAll("29").Group()[0]=="2");
		assert(interpreter->FindAll("29").Group()[1]=="9");
		
		
		assert(interpreter->Match("222").Bool()==true);
		assert(interpreter->Match("222").Group().size()==1);
		assert(interpreter->Match("222").Group()[0]=="2");
		
		assert(interpreter->Match("au8").Bool()==false);
		assert(interpreter->Match("au8").Group().size()==0);
		
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("").Group().size()==0);
		
	}
	{
		auto interpreter = CreateInterpreter("a+");
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("").Group().size()==0);
		
		assert(interpreter->Match("a").Bool()==true);
		assert(interpreter->Match("a").Group().size()==1);
		assert(interpreter->Match("a").Group()[0]=="a");
		
		assert(interpreter->Match("aaaaaa").Bool()==true);
		assert(interpreter->Match("aaaaaa").Group().size()==1);
		assert(interpreter->Match("aaaaaa").Group()[0]=="aaaaaa");
		
		assert(interpreter->Match("aba").Bool()==true);
		assert(interpreter->Match("2aba").Bool()==false);
		assert(interpreter->Match("ba").Bool()==false);
	}
	{
		auto interpreter = CreateInterpreter("a+?");
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("a").Bool()==true);
		
		assert(interpreter->Match("aaaaaa").Bool()==true);
		assert(interpreter->Match("aaaaaa").Group().size()==1);
		assert(interpreter->Match("aaaaaa").Group()[0]=="a");
		
		assert(interpreter->Match("aba").Bool()==true);
		assert(interpreter->Match("2aba").Bool()==false);
		assert(interpreter->Match("ba").Bool()==false);

	}
	{
		auto interpreter = CreateInterpreter("a*");
		assert(interpreter->Match("").Bool()==true);
		assert(interpreter->Match("").Group().size()==1);
		assert(interpreter->Match("").Group()[0]=="");
		
		assert(interpreter->Match("a").Bool()==true);
		assert(interpreter->Match("aaaaaa").Bool()==true);
		assert(interpreter->Match("aaaaaa").Group().size()==1);
		assert(interpreter->Match("aaaaaa").Group()[0]=="aaaaaa");
		
		assert(interpreter->Match("aba").Bool()==true);
		assert(interpreter->Match("2aba").Bool()==true);
		assert(interpreter->Match("ba").Bool()==true);

	}
	{
		auto interpreter = CreateInterpreter("a*?");
		assert(interpreter->Match("").Bool()==true);
		assert(interpreter->Match("a").Bool()==true);
		assert(interpreter->Match("aaaaaa").Bool()==true);
		assert(interpreter->Match("aaaaaa").Group().size()==1);
		assert(interpreter->Match("aaaaaa").Group()[0]=="");
		
		assert(interpreter->Match("aba").Bool()==true);
		assert(interpreter->Match("2aba").Bool()==true);
		assert(interpreter->Match("ba").Bool()==true);

	}
	{
		auto interpreter = CreateInterpreter("a{2,5}");
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("a").Bool()==false);
		assert(interpreter->Match("aaaaa").Bool()==true);
		assert(interpreter->Match("aaaaa").Group().size()==1);
		assert(interpreter->Match("aaaaa").Group()[0]=="aaaaa");
		
		assert(interpreter->Match("aba").Bool()==false);
		assert(interpreter->Match("2aba").Bool()==false);
		assert(interpreter->Match("aaba").Bool()==true);
		assert(interpreter->Match("aaba").Group().size()==1);
		assert(interpreter->Match("aaba").Group()[0]=="aa");

	}
	{
		auto interpreter = CreateInterpreter("a{2, 5}?");
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("a").Bool()==false);
		assert(interpreter->Match("aaaaa").Bool()==true);
		assert(interpreter->Match("aaaaa").Group()[0]=="aa");
		
		assert(interpreter->Match("aba").Bool()==false);
		assert(interpreter->Match("2aba").Bool()==false);
		assert(interpreter->Match("aaba").Bool()==true);
		assert(interpreter->Match("aaba").Group()[0]=="aa");
		
	}	
	
	{
		auto interpreter = CreateInterpreter("a|b");
		assert(interpreter->Match("a").Bool()==true);
		assert(interpreter->Match("b").Bool()==true);
		assert(interpreter->Match("ab").Bool()==true);
		assert(interpreter->Match("cba").Bool()==false);
	}
	{
		auto interpreter = CreateInterpreter("(ab)");
		assert(interpreter->Match("ab").Bool()==true);
		assert(interpreter->Match("abb").Bool()==true);
		assert(interpreter->Match("a").Bool()==false);
		assert(interpreter->Match("ba").Bool()==false);
	}
	{
		auto interpreter = CreateInterpreter("(?ab)");
		assert(interpreter->Match("ab").Bool()==true);
		assert(interpreter->Match("ab").Group()[0]=="ab");
		assert(interpreter->Match("ab").Group(1).size()==1);
		assert(interpreter->Match("ab").Group(1)[0]=="ab");

		
		assert(interpreter->Match("abb").Bool()==true);
		assert(interpreter->Match("a").Bool()==false);
		assert(interpreter->Match("ba").Bool()==false);
	}
	{
		auto interpreter = CreateInterpreter("(?<name>ab)");
		assert(interpreter->Match("ab").Bool()==true);
		assert(interpreter->Match("ab").Group().size()==1);
		assert(interpreter->Match("ab").Group(1).size()==0);
		assert(interpreter->Match("ab").Group("name").size()==1);
		assert(interpreter->Match("ab").Group("name")[0]=="ab");
		
		assert(interpreter->Match("abb").Bool()==true);
		assert(interpreter->Match("a").Bool()==false);
		assert(interpreter->Match("ba").Bool()==false);
	}
	{
		auto interpreter=CreateInterpreter("(#<name>a)(&<name>)");
		assert(interpreter->Match("aa").Bool()==true);
		assert(interpreter->Match("ab").Bool()==false);
		assert(interpreter->Match("a").Bool()==false);
	}
	{
		auto interpreter=CreateInterpreter("(#<name>a?)(&<name>)");
		assert(interpreter->Match("").Bool()==true);
		assert(interpreter->Match("a").Bool()==true);
		assert(interpreter->Match("aa").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("\\d");
		assert(interpreter->Match("2").Bool()==true);
		assert(interpreter->Match("e2").Bool()==false);
		assert(interpreter->Match("233").Bool()==true);
		assert(interpreter->Match("").Bool()==false);
	}
	{
		auto interpreter = CreateInterpreter("\\D");
		assert(interpreter->Match("22").Bool()==false);
		assert(interpreter->Match("ww").Bool()==true);
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match(" ").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("\\s");
		assert(interpreter->Match("\t").Bool()==true);
		assert(interpreter->Match("\n").Bool()==true);
		assert(interpreter->Match(" ").Bool()==true);
		assert(interpreter->Match("\r").Bool()==true);
		assert(interpreter->Match("a").Bool()==false);
		assert(interpreter->Match("aaa").Bool()==false);
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("auud").Bool()==false);
	}
	{
		auto interpreter = CreateInterpreter("\\S");
		assert(interpreter->Match("\t").Bool()==false);
		assert(interpreter->Match("\n").Bool()==false);
		assert(interpreter->Match(" ").Bool()==false);
		assert(interpreter->Match("\r").Bool()==false);
		assert(interpreter->Match("a").Bool()==true);
		assert(interpreter->Match("aaa").Bool()==true);
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("auud").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("\\w");
		assert(interpreter->Match("1").Bool()==true);
		assert(interpreter->Match("a").Bool()==true);
		assert(interpreter->Match("3e").Bool()==true);
		assert(interpreter->Match("_sd").Bool()==true);
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("\t").Bool()==false);
		assert(interpreter->Match("\n").Bool()==false);
		
	}
	{
		auto interpreter = CreateInterpreter("\\W");
		assert(interpreter->Match("1").Bool()==false);
		assert(interpreter->Match("a").Bool()==false);
		assert(interpreter->Match("3e").Bool()==false);
		assert(interpreter->Match("_sd").Bool()==false);
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("\t").Bool()==true);
		assert(interpreter->Match("\n").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter(".");
		assert(interpreter->Match("1").Bool()==true);
		assert(interpreter->Match("a").Bool()==true);
		assert(interpreter->Match("3e").Bool()==true);
		assert(interpreter->Match("_sd").Bool()==true);
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("\t").Bool()==true);
		assert(interpreter->Match("\n").Bool()==false);
	}
	{
		auto interpreter = CreateInterpreter("[^\\s]");
		assert(interpreter->Match("\t").Bool()==false);
		assert(interpreter->Match("\n").Bool()==false);
		assert(interpreter->Match(" ").Bool()==false);
		assert(interpreter->Match("\r").Bool()==false);
		assert(interpreter->Match("a").Bool()==true);
		assert(interpreter->Match("aaa").Bool()==true);
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("auud").Bool()==true);
		assert(interpreter->Match("^").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("[ ^\\s]");
		assert(interpreter->Match("\t").Bool()==true);
		assert(interpreter->Match("\n").Bool()==true);
		assert(interpreter->Match(" ").Bool()==true);
		assert(interpreter->Match("\r").Bool()==true);
		assert(interpreter->Match("a").Bool()==false);
		assert(interpreter->Match("aaa").Bool()==false);
		assert(interpreter->Match("").Bool()==false);
		assert(interpreter->Match("auud").Bool()==false);
		assert(interpreter->Match("^").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("\\(\\)\\[\\]\\*\\?\\+\\{\\}\\|");
		assert(interpreter->Match("()[]*?+{}|").Bool()==true);
		assert(interpreter->Match("aaaaaaaaaa").Bool()==false);
	}
	{
		auto interpreter = CreateInterpreter("[][]");
		assert(interpreter->Match("]").Bool()==true);
		assert(interpreter->Match("[").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("[\\[]");
		assert(interpreter->Match("[").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("[^^]");
		assert(interpreter->Match("^").Bool()==false);
		assert(interpreter->Match("a").Bool()==true);
		assert(interpreter->Match("").Bool()==false);
	}
	{
		auto interpreter = CreateInterpreter("(#<ip>\\d{1,3})\\.(&<ip>)\\.(&<ip>)\\.(&<ip>)");
		assert(interpreter->Match("192.168.1.1").Bool()==true);
		assert(interpreter->Match("255.255.255.255").Bool()==true);
		assert(interpreter->Match("2333.22.33.22").Bool()==false);
	}
	{
		Expression::ref exp = ParseRegexStr(".+?a");
		exp->TransferUsingExp();
		exp->MergeCharset();
		exp->SetNumberOnAnonymousCaptureExpression();
		Automaton::ref nfa1 = exp->GenerateEpsilonNfa();
		assert(nfa1->beginStatus->outEdges.size()==1);
		assert(nfa1->beginStatus->outEdges[0]->typeList.front().first==Edge::Type::Loop);
		auto s1 = nfa1->beginStatus->outEdges[0]->target;
		assert(s1->outEdges.size()==1);
		assert(s1->outEdges[0]->typeList.size()==0);
		assert(s1->outEdges[0]->content.size()==2);
		auto s2 = s1->outEdges[0]->target;
		assert(s2->outEdges.size()==2);
		assert(s2->outEdges[0]->isEpsilon==true);
		assert(s2->outEdges[0]->typeList.size()==0);
		assert(s2->outEdges[1]->isEpsilon==true);
		assert(s2->outEdges[1]->typeList.size()==0);
		auto s3 = s2->outEdges[1]->target;
		assert(s3->outEdges.size()==1);
		assert(s3->outEdges[0]->content.size()==2);
		assert(s3->outEdges[0]->typeList.size()==0);
		auto s4 = s3->outEdges[0]->target;
		assert(s4->outEdges.size()==1);
		assert(s4->outEdges[0]->typeList.size()==0);
		auto interpreter1 = CreateInterpreter(".+?a");
		auto tmp1 = interpreter1->Match("666a");
	}
	{
		auto interpreter = CreateInterpreter("https?://.+?\\.(com|cn)(/(?.+))?");
		assert(interpreter->Match("http://666.com/233").Bool()==true);
		assert(interpreter->Match("http://666.com/233").Group()[0]=="http://666.com/233");
		auto tmp = interpreter->Match("http://666.com/233");
		assert(interpreter->Match("http://666.com/233").Group(1).size()==1);
		assert(interpreter->Match("http://666.com/233").Group(1)[0]=="233");
		
		
		assert(interpreter->Match("https://google.com").Bool()==true);
		assert(interpreter->Match("https://google.com").Group().size()==1);
		assert(interpreter->Match("https://google.com").Group(1).size()==0);

		assert(interpreter->Match("https://2333.cn").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("a??");
		assert(interpreter->Match("").Bool()==true);
		assert(interpreter->Match("a").Bool()==true);
		assert(interpreter->Match("a").Group()[0]=="");
		
	}
	{
		auto interpreter = CreateInterpreter("(\\?<name>)");
		assert(interpreter->Match("?<name>").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("(?<name>)");
		assert(interpreter->Match("").Bool()==true);
		assert(interpreter->Match("").Group("name").size()==0);
		assert(interpreter->Match("").Group("name1").size()==0);
	}
	{
		auto interpreter = CreateInterpreter("(?<name>233)");
		assert(interpreter->Match("233").Bool()==true);
		assert(interpreter->Match("233").Group(1).size()==0);
		assert(interpreter->Match("233").Group("name").size()==1);
		assert(interpreter->Match("233").Group("name")[0]=="233");
		
		assert(interpreter->Match("23").Bool()==false);
	}
	{
		auto interpreter = CreateInterpreter("(?)");
		assert(interpreter->Match("").Bool()==true);
		assert(interpreter->Match("").Group(1).size()==0);
		
	}
	{
		auto interpreter = CreateInterpreter("(?233)");
		assert(interpreter->Match("233").Bool()==true);
		assert(interpreter->Match("233").Group(1).size()==1);
	}
	{
		auto interpreter = CreateInterpreter("233$");
		assert(interpreter->Match("233ab").Bool()==false);
		assert(interpreter->Match("233").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("^233");
		assert(interpreter->Match("a233", 1).Bool()==false);
		assert(interpreter->Match("233233").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("^.+?@(?<type>.+?)\\.(?(com|cn))$");
		assert(interpreter->Match("647583736@qq.com").Bool()==true);
		assert(interpreter->Match("647583736@qq.com").Group(1).size()==1);
		assert(interpreter->Match("647583736@qq.com").Group(1)[0]=="com");
		assert(interpreter->Match("647583736@qq.com").Group("type")[0]=="qq");

		assert(interpreter->Match("647583736@qq.com ").Bool()==false);
		assert(interpreter->Match("647583736@qq.org").Bool()==false);
		assert(interpreter->Match("647583736@qq.comm").Bool()==false);
		
	}
	{
		auto interpreter = CreateInterpreter("(?(?<name>2..))+");
		assert(interpreter->Match("233244255").Bool()==true);
		assert(interpreter->Match("233244255").Group("name").size()==3);
		assert(interpreter->Match("233244255").Group("name")[0]=="233");
		assert(interpreter->Match("233244255").Group("name")[1]=="244");
		assert(interpreter->Match("233244255").Group("name")[2]=="255");
		assert(interpreter->Match("233244255").Group(1).size()==3);
		assert(interpreter->Match("233244255").Group(1)[0]=="233");
		assert(interpreter->Match("233244255").Group(1)[1]=="244");
		assert(interpreter->Match("233244255").Group(1)[2]=="255");
	}
	{
		auto interpreter = CreateInterpreter("aa(?(=66))");
		assert(interpreter->Match("aa66").Bool()==true);
		assert(interpreter->Match("aa66").Group(1).size()==1);
		assert(interpreter->Match("aa66").Group(1)[0]=="");
		
		assert(interpreter->Match("aa6").Bool()==false);
	}
	{
		auto interpreter = CreateInterpreter("2(!33)");
		assert(interpreter->Match("233").Bool()==false);
		assert(interpreter->Match("234").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("(?(=23(=45)))2345");
		assert(interpreter->Match("2345").Bool()==true);
		assert(interpreter->Match("2345").Group(1).size()==1);
		
		assert(interpreter->Match("1234").Bool()==false);
	}
	{
		auto interpreter = CreateInterpreter("(?(=23(!45)))2366");
		assert(interpreter->Match("2345").Bool()==false);
		assert(interpreter->Match("2366").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("a?");
		assert(interpreter->Match("").Bool()==true);
	}
	{
		auto interpreter = CreateInterpreter("");
		assert(interpreter->FindAll("asd").Bool()==true);
		assert(interpreter->FindAll("asd").Group().size()==4);
		assert(interpreter->FindAll("asd").Group()[0]=="");
		assert(interpreter->FindAll("asd").Group()[1]=="");
		assert(interpreter->FindAll("asd").Group()[2]=="");
		assert(interpreter->FindAll("asd").Group()[3]=="");
	}
	{
		auto interpreter = CreateInterpreter("(?a.)");
		assert(interpreter->FindAll("udyf a sat iike hisciaai ejaedjdbeaiaejdiaa").Bool()==true);
		assert(interpreter->FindAll("udyf a sat iike hisciaai ejaedjdbeaiaejdiaa").Group().size()==7);
		assert(interpreter->FindAll("udyf a sat iike hisciaai ejaedjdbeaiaejdiaa").Group(1).size()==7);
	}
	{
		auto interpreter = CreateInterpreter("(?(?(?(?(?(?.))))))");
		assert(interpreter->FindAll("qwqwqwqwqw").Group().size()==10);
		assert(interpreter->FindAll("qwqwqwqwqw").Group(1).size()==10);
		assert(interpreter->FindAll("qwqwqwqwqw").Group(2).size()==10);
		assert(interpreter->FindAll("qwqwqwqwqw").Group(3).size()==10);
		assert(interpreter->FindAll("qwqwqwqwqw").Group(4).size()==10);
		assert(interpreter->FindAll("qwqwqwqwqw").Group(5).size()==10);
		assert(interpreter->FindAll("qwqwqwqwqw").Group(6).size()==10);
		assert(interpreter->FindAll("qwqwqwqwqw").Group(7).size()==0);

	}
	{
		auto interpreter = CreateInterpreter("aa");
		std::string s = "aaaaa";
		assert(interpreter->ReplaceAll(s, "bb")=="bbbba");
		assert(Regex("aa").Replace(s, "bb")=="bbaaa");
		assert(Regex("aa").Replace(s, "bb", 2)=="bbbba");
	}
	{
		auto interpreter = CreateInterpreter("");
		std::string s = "aaa";
		assert(interpreter->ReplaceAll(s, "bb")=="bbabbabbabb");
		assert(interpreter->Replace(s, "bb")=="bbaaa");
		assert(interpreter->Replace(s, "bb", 2)=="bbabbaa");
	}
	{
		auto interpreter = CreateInterpreter("");
		std::string s = "aaa";
		assert(interpreter->ReplaceAll(s, "")=="aaa");
		assert(interpreter->Replace(s, "")=="aaa");
	}
	{
		auto interpreter = CreateInterpreter("\\d");
		std::string s = "r3tey37e7d";
		assert(interpreter->ReplaceAll(s, "")=="rteyed");
		assert(interpreter->Replace(s, "")=="rtey37e7d");
		assert(interpreter->Replace(s, "", 2)=="rtey7e7d");
		
	}


	{
		std::string testString =
			"<dt><a href=\"http://bbs.uestc.edu.cn/forum.php?mod=forumdisplay&fid=219\">出国留学</a><em class=\"xw0 xi1\" title=\"今日\"> (今日: 9)</em></dt>""<dd><a href=\"http://bbs.uestc.edu.cn/forum.php?mod=redirect&amp;tid=1570205&amp;goto=lastpost#lastpost\" class=\"xi2\">ETH网申</a></dd><dd><cite>by: <a href=\"http://bbs.uestc.edu.cn/home.php?mod=space&username=camelluxin\">camelluxin</a> [<span title=\"2015-11-8 21:30\">1&nbsp;小时前</span>]</cite></dd>";
		Regex regex("href=\"(?<url>https?://.+?)\"");
		auto result = regex.FindAll(testString);
		assert(result.Bool() == true);
		for (auto i : result.Group("url")) {
			std::cout<<i<<std::endl;
		}

	}
	std::cout<<"all test case passed."<<std::endl;
}
