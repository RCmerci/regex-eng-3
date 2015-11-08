#include <utility>
#include <cassert>
#include <bitset>
#include <string>
#include "exception.hpp"
#include "regexAlgorithm.hpp"
#include "data.hpp"
#include "automaton.hpp"


namespace regex
{
	namespace regex_internal
	{

		/*-------------------------------------------------------
		  GenerateEpsilonNfaAlgorithm
		-------------------------------------------------------*/
		Automaton::ref GenerateEpsilonNfaAlgorithm::Apply(OrExp* expression, PAPSS papss)
		{
			Automaton::ref atm = papss.atm;
			Status::ref begin  = papss.s1;
			Status::ref end    = papss.s2;
			Status::ref opt1s1 = atm->NewStatus();    
			Status::ref opt1s2 = atm->NewStatus();    
			Status::ref opt2s1 = atm->NewStatus();    
			Status::ref opt2s2 = atm->NewStatus();
			atm->NewEpsilonEdge(begin, opt1s1);
			atm->NewEpsilonEdge(opt1s2, end);
			atm->NewEpsilonEdge(begin, opt2s1);
			atm->NewEpsilonEdge(opt2s2, end);
			if (expression->opt1 == nullptr) {
				atm->NewEpsilonEdge(opt1s1, opt1s2);
			}
			else {
				Call(expression->opt1, PAPSS(atm, opt1s1, opt1s2));
			}
			if (expression->opt2 == nullptr) {
				atm->NewEpsilonEdge(opt2s1, opt2s2);
			}
			else {
				Call(expression->opt2, PAPSS(atm, opt2s1, opt2s2));
			}
			return atm;
		}
		Automaton::ref GenerateEpsilonNfaAlgorithm::Apply(JoinExp* expression, PAPSS papss)
		{
			Automaton::ref atm = papss.atm;
			Status::ref begin  = papss.s1;
			Status::ref end    = papss.s2;
			Status::ref midStatus   = atm->NewStatus();
			Call(expression->left, PAPSS(atm, begin, midStatus));
			Call(expression->right, PAPSS(atm, midStatus, end));
			return atm;
		}
		Automaton::ref GenerateEpsilonNfaAlgorithm::Apply(LoopExp* expression, PAPSS papss)
		{
			Automaton::ref atm = papss.atm;
			Status::ref begin  = papss.s1;
			Status::ref end    = papss.s2;
			Status::ref loopBegin = atm->NewStatus();
			Status::ref loopEnd   = atm->NewStatus();
			atm->NewEpsilonEdge(begin, loopBegin).Loop();
			atm->NewEpsilonEdge(loopEnd, end).End();
			Status::ref currentBegin = loopBegin;
			for (int i=0; i<expression->min; i++) {
				Status::ref mid = atm->NewStatus();
				Call(expression->innerExp, PAPSS(atm, currentBegin, mid));
				currentBegin = mid;
			}
			int leftTime = expression->max == -1 ? -1 : expression->max - expression->min;
			if (leftTime != -1) {
				if (expression->greedy) {
					for (int i=0; i<leftTime; i++) {
						Status::ref mid = atm->NewStatus();
						Call(expression->innerExp, PAPSS(atm, currentBegin, mid)); //prefer
						atm->NewEpsilonEdge(currentBegin, loopEnd); // not prefer
						currentBegin = mid;
					}
					atm->NewEpsilonEdge(currentBegin, loopEnd);
				}
				else {
					for (int i=0; i<leftTime; i++) {
						Status::ref mid = atm->NewStatus();
						atm->NewEpsilonEdge(currentBegin, loopEnd); // prefer
						Call(expression->innerExp, PAPSS(atm, currentBegin, mid)); // not prefer
						currentBegin = mid;
					}
					atm->NewEpsilonEdge(currentBegin, loopEnd);
				}
			}
			else {
				Status::ref midBegin = atm->NewStatus();
				Status::ref midEnd   = atm->NewStatus();
				if (expression->greedy) {
					atm->NewEpsilonEdge(currentBegin, midBegin); // prefer
					atm->NewEpsilonEdge(midEnd, currentBegin);
					Call(expression->innerExp, PAPSS(atm, midBegin, midEnd)); 
					atm->NewEpsilonEdge(currentBegin, loopEnd); // not prefer
				}
				else {
					Call(expression->innerExp, PAPSS(atm, midBegin, midEnd));
					atm->NewEpsilonEdge(currentBegin, loopEnd); // prefer
					atm->NewEpsilonEdge(currentBegin, midBegin); // not prefer
					atm->NewEpsilonEdge(midEnd, currentBegin);
				}
			}
			return atm;
		}
		Automaton::ref GenerateEpsilonNfaAlgorithm::Apply(CharsetExp* expression, PAPSS papss)
		{
			Automaton::ref atm = papss.atm;
			Status::ref begin  = papss.s1;
			Status::ref end    = papss.s2;
			// Status::ref s1     = atm->NewStatus();
			// Status::ref s2     = atm->NewStatus();
			// atm->NewEpsilonEdge(begin, s1);
			// atm->NewEpsilonEdge(s2, end);
			// atm->NewEdge(expression->charRangeL, s1, s2);
			atm->NewEdge(expression->charRangeListResult, begin, end);
			return atm;
		}
		Automaton::ref GenerateEpsilonNfaAlgorithm::Apply(UsingExp* expression, PAPSS papss)
		{
			throw GenerateEpsilonNfaException("UsingExp need to be handled before generate epsilonNfa.\n");
		}
		Automaton::ref GenerateEpsilonNfaAlgorithm::Apply(PositiveExp* expression, PAPSS papss)
		{
			Automaton::ref atm = papss.atm;
			Status::ref begin  = papss.s1;
			Status::ref end    = papss.s2;
			std::size_t index = atm->NewSubAutomaton();
			Automaton::ref subAtm = atm->subAtmList[index];
			Status::ref subBegin = subAtm->NewStatus(false, true);
			Status::ref subEnd   = subAtm->NewStatus(true, false);
			assert(subAtm->endStatusList.size() == 1);
			Call(expression->innerExp, PAPSS(subAtm, subBegin, subEnd));
			atm->NewEpsilonEdge(begin, end).Positive(index);
			return atm;
		}
		Automaton::ref GenerateEpsilonNfaAlgorithm::Apply(NegativeExp* expression, PAPSS papss)
		{
			Automaton::ref atm = papss.atm;
			Status::ref begin  = papss.s1;
			Status::ref end    = papss.s2;
			std::size_t index  = atm->NewSubAutomaton();
			Automaton::ref subAtm = atm->subAtmList[index];
			Status::ref subBegin = subAtm->NewStatus(false, true);
			Status::ref subEnd   = subAtm->NewStatus(true, false);
			assert(subAtm->endStatusList.size() == 1);
			Call(expression->innerExp, PAPSS(subAtm, subBegin, subEnd));
			atm->NewEpsilonEdge(begin, end).Negative(index);
			return atm;
		}
		Automaton::ref GenerateEpsilonNfaAlgorithm::Apply(CaptureExp* expression, PAPSS papss)
		{
			Automaton::ref atm = papss.atm;
			Status::ref begin  = papss.s1;
			Status::ref end    = papss.s2;
			Status::ref capBegin = atm->NewStatus();
			Status::ref capEnd   = atm->NewStatus();
			std::size_t index = atm->NewCaptureName(expression->name);
			atm->NewEpsilonEdge(begin, capBegin).Capture(index);
			atm->NewEpsilonEdge(capEnd, end).End();
			Call(expression->innerExp, PAPSS(atm, capBegin, capEnd));
			return atm;
		}
		Automaton::ref GenerateEpsilonNfaAlgorithm::Apply(BeginExp* expression, PAPSS papss)
		{
			Automaton::ref atm = papss.atm;
			Status::ref begin  = papss.s1;
			Status::ref end    = papss.s2;
			atm->NewEpsilonEdge(begin, end).Head();
			return atm;
		}
		Automaton::ref GenerateEpsilonNfaAlgorithm::Apply(EndExp* expression, PAPSS papss)
		{	
			Automaton::ref atm = papss.atm;
			Status::ref begin  = papss.s1;
			Status::ref end    = papss.s2;
			atm->NewEpsilonEdge(begin, end).Tail();
			return atm;
		}
		
		/*-------------------------------------------------------
		  CompareExpressionAlgorithm
		-------------------------------------------------------*/
		
		bool CompareExpressionAlgorithm::Apply(OrExp * expression, Expression::ref parameter)
		{
			if (OrExp* p = dynamic_cast<OrExp*>(parameter)) {
				bool opt1=false, opt2=false, opt1Flag=false, opt2Flag=false;
				if (expression->opt1 == nullptr) {
					opt1 = (p->opt1 == nullptr);
					opt1Flag = true;
				}
				else if (p->opt1 == nullptr) {
					opt1 = (expression->opt1 == nullptr);
					opt1Flag = true;
				}
				if (expression->opt2 == nullptr) {
					opt2 = (p->opt1 == nullptr);
					opt2Flag = true;
				}
				else if (p->opt2 == nullptr) {
					opt2 = (expression->opt2 == nullptr);
					opt2Flag = true;
				}
				if (!opt1Flag) {
					opt1 = Call(expression->opt1, p->opt1);
				}
				if (!opt2Flag) {
					opt2 = Call(expression->opt2, p->opt2);
				}
				return opt1 && opt2;
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(JoinExp * expression, Expression::ref parameter)
		{
			if (JoinExp* p = dynamic_cast<JoinExp*>(parameter)) {
				if (Call(expression->left, p->left) && Call(expression->right, p->right)) {
					return true;
				}
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(LoopExp * expression, Expression::ref parameter) {
			if (LoopExp* p = dynamic_cast<LoopExp*>(parameter)) {
				if (expression->min == p->min &&
				    expression->max == p->max &&
				    expression->greedy == p->greedy &&
				    Call(expression->innerExp, p->innerExp)) {
					return true;
				}
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(CharsetExp * expression, Expression::ref parameter) {
			if (CharsetExp* p = dynamic_cast<CharsetExp*>(parameter)) {
				auto equalBP = [](CharRange& i, CharRange& j)
					{
						return i==j;
					};
				auto equalBP2 = [](std::pair<std::list<CharRange>, bool>& i,
						   std::pair<std::list<CharRange>, bool>& j)
					{
						return i==j;
					};
				if (expression->charRangeListResult.size() == 0) {
					if (expression->charRangeL.size() == p->charRangeL.size() &&
					    expression->reverse == p->reverse &&
					    std::equal(expression->charRangeL.begin(), expression->charRangeL.end(), p->charRangeL.begin(), equalBP2)) {
						return true;
					}
				}
				else if (expression->charRangeListResult.size() == p->charRangeListResult.size() &&
					 expression->reverse == p->reverse &&
					 std::equal(expression->charRangeListResult.begin(), expression->charRangeListResult.end(), p->charRangeListResult.begin(), equalBP)) {
					return true;
				}
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(UsingExp * expression, Expression::ref parameter) {
			if (UsingExp* p = dynamic_cast<UsingExp*>(parameter)) {
				if (expression->isCreate == p->isCreate &&
				    expression->name == p->name &&
				    ((expression->innerExp == nullptr && p->innerExp == nullptr) ||
				     Call(expression->innerExp, p->innerExp))) {
					return true;
				}
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(PositiveExp * expression, Expression::ref parameter) {
			if (PositiveExp* p = dynamic_cast<PositiveExp*>(parameter)) {
				if (Call(expression->innerExp, p->innerExp)) {
					return true;
				}
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(NegativeExp * expression, Expression::ref parameter) {
			if (NegativeExp* p = dynamic_cast<NegativeExp*>(parameter)) {
				if (Call(expression->innerExp, p->innerExp)) {
					return true;
				}
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(CaptureExp * expression, Expression::ref parameter) {
			if (CaptureExp* p = dynamic_cast<CaptureExp*>(parameter)) {
				if (expression->name == p->name &&
				    Call(expression->innerExp, p->innerExp)) {
					return true;
				}
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(BeginExp * expression, Expression::ref parameter) {
			if (BeginExp* p = dynamic_cast<BeginExp*>(parameter)) {
				return true;
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(EndExp * expression, Expression::ref parameter) {
			if (EndExp* p = dynamic_cast<EndExp*>(parameter)) {
				return true;
			}
			return false;

		}

		/*-----------------------------------------------------------------
		  MergeCharsetAlgorithm
		 ------------------------------------------------------------------*/
		
		void* MergeCharsetAlgorithm::Apply(OrExp* expression, void* parameter)
		{
			if (expression->opt1) {
				Call(expression->opt1, nullptr);
			}
			if (expression->opt2) {
				Call(expression->opt2, nullptr);
			}
			return nullptr;
		}
		void* MergeCharsetAlgorithm::Apply(JoinExp* expression, void* parameter)
		{
			Call(expression->left, nullptr);
			Call(expression->right, nullptr);
			return nullptr;
		}
		void* MergeCharsetAlgorithm::Apply(LoopExp* expression, void* parameter)
		{
			Call(expression->innerExp, nullptr);
			return nullptr;
		}
		void* MergeCharsetAlgorithm::Apply(CharsetExp* expression, void* parameter)
		{
			std::bitset<CharRangeMAX-CharRangeMIN+1> rangeMap(0);
			for (std::pair<std::list<CharRange>, bool>& crlb : expression->charRangeL) {
				std::list<CharRange>& crl = crlb.first;
				bool reverse = crlb.second;
				for (auto& cr : crl) {
					for (int i=cr.begin;i<=cr.end;i++) {
						rangeMap.set(i);
					}
				}
				if (reverse) {
					rangeMap.flip();
				}
			}
			if (expression->reverse) {
				rangeMap.flip();
				expression->reverse = false;
			}
			std::list<CharRange> result;
			CharRange::RangeType begin, end;
			bool constructFlag = false;
			for (unsigned int i = CharRangeMIN; i <= CharRangeMAX; i++) {
				if (rangeMap[i]) {
					if (!constructFlag) {
						constructFlag = true;
						begin = i;
					}
				}
				else {
					if (constructFlag) {
						end = i - 1;
						result.push_back(CharRange(begin, end));
						constructFlag = false;
					}
				}
			}
			if (constructFlag) { // 如果bitmap从中间一直到最后都是1的话，在上面的循环里会少加这个charRange
				result.push_back(CharRange(begin, CharRangeMAX));
			}
			expression->charRangeListResult = result;
			return nullptr;
		}
		void* MergeCharsetAlgorithm::Apply(UsingExp* expression, void* parameter)
		{
			Call(expression->innerExp, nullptr);
			return nullptr;
		}
		void* MergeCharsetAlgorithm::Apply(PositiveExp* expression, void* parameter)
		{
			Call(expression->innerExp, nullptr);
			return nullptr;
		}
		void* MergeCharsetAlgorithm::Apply(NegativeExp* expression, void* parameter)
		{
			Call(expression->innerExp, nullptr);
			return nullptr;
		}
		void* MergeCharsetAlgorithm::Apply(CaptureExp* expression, void* parameter)
		{
			Call(expression->innerExp, nullptr);
			return nullptr;
		}
		void* MergeCharsetAlgorithm::Apply(BeginExp* expression, void* parameter)
		{
			return nullptr;
		}
		void* MergeCharsetAlgorithm::Apply(EndExp* expression, void* parameter)
		{
			return nullptr;
		}

		/*-----------------------------------------------------------------
		  TransferUsingExpAlgorithm
		 ------------------------------------------------------------------*/

		Expression::ref TransferUsingExpAlgorithm::Apply(OrExp* expression, Name2Expression parameter)
		{
			if (expression->opt1) {
				if (UsingExp* p = dynamic_cast<UsingExp*>(expression->opt1)) {
					Expression::ref tmp = Call(p, parameter);
					delete expression->opt1;
					expression->opt1 = tmp;
				}
				else {
					Call(expression->opt1, parameter);
				}
			}
			if (expression->opt2) {
				if (UsingExp* p = dynamic_cast<UsingExp*>(expression->opt2)) {
					Expression::ref tmp = Call(p, parameter);
					delete expression->opt2;
					expression->opt2 = tmp;
				}
				else {
					Call(expression->opt2, parameter);
				}
			}
			return expression;
		}
		Expression::ref TransferUsingExpAlgorithm::Apply(JoinExp* expression, Name2Expression parameter)
		{
			if (UsingExp* p = dynamic_cast<UsingExp*>(expression->left)) {
				Expression::ref tmp = Call(p, parameter);
				delete expression->left;
				expression->left = tmp;
			}
			else {
				Call(expression->left, parameter);
			}
			if (UsingExp* p = dynamic_cast<UsingExp*>(expression->right)) {
				Expression::ref tmp = Call(p, parameter);
				delete expression->right;
				expression->right = tmp;
			}
			else {
				Call(expression->right, parameter);
			}
			return expression;
		}
		Expression::ref TransferUsingExpAlgorithm::Apply(LoopExp* expression, Name2Expression parameter)
		{
			if (UsingExp* p = dynamic_cast<UsingExp*>(expression->innerExp)) {
				Expression::ref tmp = Call(p, parameter);
				delete expression->innerExp;
				expression->innerExp = tmp;
			}
			else {
				Call(expression->innerExp, parameter);
			}
			return expression;
		}
		Expression::ref TransferUsingExpAlgorithm::Apply(CharsetExp* expression, Name2Expression parameter)
		{
			return expression;
		}
		Expression::ref TransferUsingExpAlgorithm::Apply(UsingExp* expression, Name2Expression parameter)
		{
			if (expression->isCreate) {
				if (!expression->innerExp) {
					throw ExpressionAlgorithmException("expression expected in UsingCreateExpression.\n");
				}
				Expression::ref innerNoUsingExp = Call(expression->innerExp, parameter);
				(*parameter)[expression->name] = innerNoUsingExp;
				return (*parameter)[expression->name];
			}
			else {
				return (*parameter)[expression->name]->RecursiveCopyExpression();
			}
		}
		Expression::ref TransferUsingExpAlgorithm::Apply(PositiveExp* expression, Name2Expression parameter)
		{
			if (UsingExp* p = dynamic_cast<UsingExp*>(expression->innerExp)) {
				Expression::ref tmp = Call(p, parameter);
				delete expression->innerExp;
				expression->innerExp = tmp;
			}
			else {
				Call(expression->innerExp, parameter);
			}
			return expression;
		}
		Expression::ref TransferUsingExpAlgorithm::Apply(NegativeExp* expression, Name2Expression parameter)
		{
			if (UsingExp* p = dynamic_cast<UsingExp*>(expression->innerExp)) {
				Expression::ref tmp = Call(p, parameter);
				delete expression->innerExp;
				expression->innerExp = tmp;
			}
			else {
				Call(expression->innerExp, parameter);
			}
			return expression;
		}
		Expression::ref TransferUsingExpAlgorithm::Apply(CaptureExp* expression, Name2Expression parameter)
		{
			if (UsingExp* p = dynamic_cast<UsingExp*>(expression->innerExp)) {
				Expression::ref tmp = Call(p, parameter);
				delete expression->innerExp;
				expression->innerExp = tmp;
			}
			else {
				Call(expression->innerExp, parameter);
			}
			return expression;
		}
		Expression::ref TransferUsingExpAlgorithm::Apply(BeginExp* expression, Name2Expression parameter)
		{
			return expression;
		}
		Expression::ref TransferUsingExpAlgorithm::Apply(EndExp* expression, Name2Expression parameter)
		{
			return expression;
		}

	

		/*-------------------------------------------------------
		  RecursiveCopyExpressionAlgorithm
		  -------------------------------------------------------*/
		Expression::ref RecursiveCopyExpressionAlgorithm::Apply(OrExp* expression, void* _)
		{
			OrExp* newE = new OrExp;
			if (expression->opt1) {
				newE->opt1 = Call(expression->opt1, nullptr);
			}
			else {
				newE->opt1 = nullptr;
			}
			if (expression->opt2) {
				newE->opt2 = Call(expression->opt2, nullptr);
			}
			else {
				newE->opt2 = nullptr;
			}
			return newE;
		}
		Expression::ref RecursiveCopyExpressionAlgorithm::Apply(JoinExp* expression, void* _)
		{
			JoinExp* newE = new JoinExp;
			newE->left = Call(expression->left, nullptr);
			newE->right = Call(expression->right, nullptr);
			return newE;
		}
		Expression::ref RecursiveCopyExpressionAlgorithm::Apply(LoopExp* expression, void* _)
		{
			LoopExp* newE = new LoopExp;
			newE->innerExp = Call(expression->innerExp, nullptr);
			newE->min = expression->min;
			newE->max = expression->max;
			newE->greedy = expression->greedy;
			return newE;
		}
		Expression::ref RecursiveCopyExpressionAlgorithm::Apply(CharsetExp* expression, void* _)
		{
			CharsetExp* newE = new CharsetExp;
			newE->charRangeL = expression->charRangeL;
			newE->reverse = expression->reverse;
			return newE;
		}
		Expression::ref RecursiveCopyExpressionAlgorithm::Apply(UsingExp* expression, void* _)
		{
			UsingExp* newE = new UsingExp;
			newE->isCreate = expression->isCreate;
			newE->name = expression->name;
			newE->innerExp = Call(expression->innerExp, nullptr);
			return newE;
		}
		Expression::ref RecursiveCopyExpressionAlgorithm::Apply(PositiveExp* expression, void* _)
		{
			PositiveExp* newE = new PositiveExp;
			newE->innerExp = Call(expression->innerExp, nullptr);
			return newE;
		}
		Expression::ref RecursiveCopyExpressionAlgorithm::Apply(NegativeExp* expression, void* _)
		{
			NegativeExp* newE = new NegativeExp;
			newE->innerExp = Call(expression->innerExp, nullptr);
			return newE;
		}
		Expression::ref RecursiveCopyExpressionAlgorithm::Apply(CaptureExp* expression, void* _)
		{
			CaptureExp* newE = new CaptureExp;
			newE->innerExp = Call(expression->innerExp, nullptr);
			newE->name = expression->name;
			return newE;
		}
		Expression::ref RecursiveCopyExpressionAlgorithm::Apply(BeginExp* expression, void* _)
		{
			BeginExp* newE = new BeginExp;
			return newE;
		}
		Expression::ref RecursiveCopyExpressionAlgorithm::Apply(EndExp* expression, void* _)
		{
			EndExp* newE = new EndExp;
			return newE;
		}



		/*-----------------------------------------------------------------
		  DeleteExpressionAlgorithm
		 ------------------------------------------------------------------*/
		void* DeleteExpressionAlgorithm::Apply(OrExp* expression, void* _)
		{
			if (expression->opt1) {
				Call(expression->opt1, nullptr);
			}
			if (expression->opt2) {
				Call(expression->opt2, nullptr);
			}
			delete expression;
			return nullptr;
		}
		void* DeleteExpressionAlgorithm::Apply(JoinExp* expression, void* _)
		{
			Call(expression->left, nullptr);
			Call(expression->right, nullptr);
			delete expression;
			return nullptr;
		}
		void* DeleteExpressionAlgorithm::Apply(LoopExp* expression, void* _)
		{
			Call(expression->innerExp, nullptr);
			delete expression;
			return nullptr;
		}
		void* DeleteExpressionAlgorithm::Apply(CharsetExp* expression, void* _)
		{
			delete expression;
			return nullptr;
		}
		void* DeleteExpressionAlgorithm::Apply(UsingExp* expression, void* _)
		{
			if (expression->innerExp) {
				Call(expression->innerExp, nullptr);
			}
			delete expression;
			return nullptr;
		}
		void* DeleteExpressionAlgorithm::Apply(PositiveExp* expression, void* _)
		{
			Call(expression->innerExp, nullptr);
			delete expression;
			return nullptr;
		}
		void* DeleteExpressionAlgorithm::Apply(NegativeExp* expression, void* _)
		{
			Call(expression->innerExp, nullptr);
			delete expression;
			return nullptr;
		}
		void* DeleteExpressionAlgorithm::Apply(CaptureExp* expression, void* _)
		{
			Call(expression->innerExp, nullptr);
			delete expression;
			return nullptr;
		}
		void* DeleteExpressionAlgorithm::Apply(BeginExp* expression, void* _)
		{
			delete expression;
			return nullptr;
		}
		void* DeleteExpressionAlgorithm::Apply(EndExp* expression, void* _)
		{
			delete expression;
			return nullptr;
		}




		/*-----------------------------------------------------------------
		  SetNumberOnAnonymousCaptureExpressionAlgorithm
		 ------------------------------------------------------------------*/
		void* SetNumberOnAnonymousCaptureExpressionAlgorithm::Apply(OrExp* expression, int _)
		{
			if (expression->opt1) {
				Call(expression->opt1, parameter);
			}
			if (expression->opt2) {
				Call(expression->opt2, parameter);
			}
			return nullptr;
		}
		void* SetNumberOnAnonymousCaptureExpressionAlgorithm::Apply(JoinExp* expression, int _)
		{
			Call(expression->left, parameter);
			Call(expression->right, parameter);
			return nullptr;
		}
		void* SetNumberOnAnonymousCaptureExpressionAlgorithm::Apply(LoopExp* expression, int _)
		{
			Call(expression->innerExp, parameter);
			return nullptr;
		}
		void* SetNumberOnAnonymousCaptureExpressionAlgorithm::Apply(CharsetExp* expression, int _)
		{
			return nullptr;
		}
		void* SetNumberOnAnonymousCaptureExpressionAlgorithm::Apply(UsingExp* expression, int _)
		{
			if (expression->innerExp) {
				Call(expression->innerExp, parameter);
			}
			return nullptr;
		}
		void* SetNumberOnAnonymousCaptureExpressionAlgorithm::Apply(PositiveExp* expression, int _)
		{
			return nullptr;
		}
		void* SetNumberOnAnonymousCaptureExpressionAlgorithm::Apply(NegativeExp* expression, int _)
		{
			return nullptr;
		}
		void* SetNumberOnAnonymousCaptureExpressionAlgorithm::Apply(CaptureExp* expression, int _)
		{
			if (expression->name == "") {
				expression->name = std::to_string(parameter);
				parameter++;
			}
			Call(expression->innerExp, parameter);
			return nullptr;
		}
		void* SetNumberOnAnonymousCaptureExpressionAlgorithm::Apply(BeginExp* expression, int _)
		{
			return nullptr;
		}
		void* SetNumberOnAnonymousCaptureExpressionAlgorithm::Apply(EndExp* expression, int _)
		{
			return nullptr;
		}
		
	}
}
