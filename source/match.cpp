#include <cassert>
#include "regex.hpp"
#include "match.hpp"

namespace regex
{
	namespace regex_internal
	{
		/*----------------------------------------
		  Aux
		 ----------------------------------------*/
		static void BackTrace(CaptureStack* capS, CommandStack* cmdS, StatusStack* stsS, std::size_t& strIdx);
		static void PureBackTrace(StatusStack* stsS, std::size_t& strIdx);
		
		static bool CheckHead(int);
		static bool CheckTail(int, std::string&);
		// static bool CheckLoop(CommandStack&);
		// static bool CheckCapture(CommandStack&);
		// static bool CheckEnd(CommandStack&);
		// static bool CheckPositive(CommandStack&);
		// static bool CheckNegative(CommandStack&);
		static std::string& GetCaptureName(Automaton::ref atm, std::size_t idx);
		
		/*----------------------------------------
		  CaptureItem
		 ----------------------------------------*/

		/*----------------------------------------
		  CaptureStack, CommandStack, StatusStack
		 ----------------------------------------*/ 
		void CaptureStack::Push(std::string name,
					// int statusStackSize,
					std::size_t begin,
					std::size_t length)
		{
			CaptureStackItem item;
			item.name = name;
			// item.statusStackSize = statusStackSize;
			item.begin = begin;
			item.length = length;
			stack.push_back(item);
		}

		void CaptureStack::Pop(int size)
		{
			if (1 == size) {
				stack.pop_back();
			}
			else {
				int finalSize = stack.size() - size;
				finalSize = finalSize >= 0 ? finalSize : 0;
				stack.resize(finalSize);
			}
		}

		CaptureStack::CaptureStackItem* CaptureStack::Top()
		{
			if (0 == stack.size()) {
				return nullptr;
			}
			return &(stack.back());
		}
		
		void  CaptureStack::Resize(std::size_t size)
		{
			if (size > stack.size()) {
				return;
			}
			stack.resize(size);
		}

		std::size_t CaptureStack::GetSize()
		{
			return stack.size();
		}

		std::list<CaptureStack::CaptureStackItem>& CaptureStack::GetInnerStack()
		{
			return stack;
		}
		
		
		void CommandStack::Push(Edge::Type type,
					std::size_t para,
					std::size_t begin)
		{
			CommandStackItem item;
			item.type = type;
			item.para = para;
			item.begin = begin;

			switch (type) {
			case Edge::Type::Head:
			case Edge::Type::Tail:
			case Edge::Type::Positive:
			case Edge::Type::Negative:
				item.lastNotFinishCmdIndex = GetLastNotFinishCmdIndex();
				break;
			case Edge::Type::Loop:
			case Edge::Type::Capture:
				item.lastNotFinishCmdIndex = stack.size();
				break;
			case Edge::Type::End:
				int lastNotFinishCmdIndex = GetLastNotFinishCmdIndex() - 1;
				if (-1 != lastNotFinishCmdIndex) {
					CommandStackItem cmd = stack[lastNotFinishCmdIndex];
					item.lastNotFinishCmdIndex = cmd.lastNotFinishCmdIndex;
				}
				else {
					item.lastNotFinishCmdIndex = -1;
				}
				break;
			}
			stack.push_back(item);
		}

		void CommandStack::Pop(int size)
		{
			if (1 == size) {
				stack.pop_back();
			}
			else {
				int finalSize = stack.size() - size;
				finalSize = finalSize > 0 ? finalSize : 0;
				stack.resize(finalSize);
			}
		}
		
		void  CommandStack::Resize(std::size_t size)
		{
			if (size > stack.size()) {
				return;
			}
			stack.resize(size);
		}
		
		CommandStack::CommandStackItem* CommandStack::Top()
		{
			if (0 == stack.size()) {
				return nullptr;
			}
			return &(stack.back());
		}
		

		std::size_t CommandStack::GetSize()
		{
			return stack.size();
		}

		int CommandStack::GetLastNotFinishCmdIndex()
		{
			if (0 == stack.size()) {
				return -1;
			}
			return stack.back().lastNotFinishCmdIndex;
		}
		CommandStack::CommandStackItem& CommandStack::At(std::size_t idx)
		{
			return stack.at(idx);
		}

		void StatusStack::Push(Status::ref status,
				       int edgeIndex,
				       int cmdStackSize,
				       int captureStackSize,
				       std::size_t begin)
		{
			StatusStackItem item;
			item.status = status;
			item.edgeIndex = edgeIndex;
			item.cmdStackSize = cmdStackSize;
			item.captureStackSize = captureStackSize;
			item.begin = begin;
			stack.push_back(item);
		}

		void StatusStack::Pop(int size)
		{
			if (1 == size) {
				stack.pop_back();
			}
			else {
				int finalSize = stack.size() - size;
				finalSize = finalSize > 0 ? finalSize : 0;
				stack.resize(finalSize);
			}
		}

		void StatusStack::Resize(std::size_t size)
		{
			if (size > stack.size()) {
				return;
			}
			stack.resize(size);
		}
		
		StatusStack::StatusStackItem* StatusStack::Top()
		{
			if (0 == stack.size()) {
				return nullptr;
			}
			return &(stack.back());
		}

		
		std::size_t StatusStack::GetSize()
		{
			return stack.size();
		}


		/*----------------------------------------
		  Interpreter
		 ----------------------------------------*/ 

		Interpreter::Interpreter(Automaton::ref _atm)
		{
			atm = _atm;
			if (!atm) {
				atm = new Automaton(false); //nfa (not epsilon nfa)
				Status::ref s1 = atm->NewStatus(false, true);
				Status::ref s2 = atm->NewStatus(true, false);
				atm->NewEpsilonEdge(s1, s2);
			}
		}
		Interpreter::~Interpreter()
		{
			delete atm;
		}
		Interpreter& Interpreter::operator=(Interpreter&& i)
		{
			delete atm;
			atm = i.atm;
			i.atm = nullptr;
			return *this;
		}

		std::size_t Interpreter::Match(std::string s, std::size_t begin, RegexResult& rst)
		{
			CaptureStack captureStack;
			CommandStack commandStack;
			StatusStack  statusStack;
			
			StatusStack::StatusStackItem* statusElem=nullptr;
			int edgeIdx = 0;
			Edge::ref edge = nullptr;
			std::size_t idx = begin;
			std::size_t nextIdx = 0;
			
			statusStack.Push(atm->beginStatus, 0, 0, 0, idx);

			while (true) {// s.size() > idx) {
				statusElem = statusStack.Top();
				if (!statusElem) {
					return 0;
				}
				edgeIdx = statusElem->edgeIndex;
				edge = statusElem->status->outEdges[edgeIdx];
				if (edge->isEpsilon) {
					nextIdx = idx;
				}
				else {
					bool included = false;
					if (s.size() <= idx) {
						goto BackTrace;
					}
					for (CharRange& cr : edge->content) {
						if (cr.Include(s[idx])) {
							included = true;
							break;
						}
					}
					if (!included) {
						goto BackTrace;
					}
					nextIdx = idx+1;
				}
				
				for (Edge::TypeListElem& e : edge->typeList) {
					switch (e.first) {
					case Edge::Type::Head:
						if (! CheckHead(idx)) {
							goto BackTrace;
						}
						break;
					case Edge::Type::Tail:
						if (! CheckTail(idx, s)) {
							goto BackTrace;
						}
						break;
					case Edge::Type::Loop:
						commandStack.Push(Edge::Type::Loop, e.second, idx);
						break;
					case Edge::Type::Capture:
						commandStack.Push(Edge::Type::Capture, e.second, idx);
						break;
					case Edge::Type::End:
						{
							CommandStack::CommandStackItem& cmd = commandStack.At(commandStack.GetLastNotFinishCmdIndex());
							if (cmd.type == Edge::Type::Capture) {
								// idx - cmd.begin ->
								// 因为当前字符原本(epsilon nfa)在所有功能边之后，所以不是 [idx+1]
								captureStack.Push(GetCaptureName(atm, cmd.para), cmd.begin, idx-cmd.begin);
							}
							commandStack.Push(Edge::Type::End, e.second, idx);
						}
						break;
					case Edge::Type::Positive:
						if (! PureMatch(atm->subAtmList[e.second], s, idx)) {
							goto BackTrace;
						}
						break;
					case Edge::Type::Negative:
						if (PureMatch(atm->subAtmList[e.second], s, idx)) {
							goto BackTrace;
						}
						break;
					}
				}
				idx = nextIdx;
				if (edge->target->isFinal) { // match success
					auto captureItemList = captureStack.GetInnerStack();
					for (auto item : captureItemList) {
						rst.AddCaptureRst(item.name, item.begin, item.length);
					}
					rst.AddMatchRst(begin, nextIdx-begin);
					return idx-begin;
				}
				statusStack.Push(edge->target, 0, commandStack.GetSize(), captureStack.GetSize(), idx);
				continue;
			BackTrace:
				BackTrace(&captureStack, &commandStack, &statusStack, idx);
			}
			return 0;
		}
		std::size_t Interpreter::PureMatch(Automaton::ref subAtm, std::string s, std::size_t begin)
		{
			StatusStack  statusStack;
			
			StatusStack::StatusStackItem* statusElem=nullptr;
			int edgeIdx = 0;
			Edge::ref edge = nullptr;
			std::size_t idx = begin;
			std::size_t nextIdx = 0;
			
			statusStack.Push(subAtm->beginStatus, 0, 0, 0, idx);

			while (true) {
				statusElem = statusStack.Top();
				if (!statusElem) {
					return 0;
				}
				edgeIdx = statusElem->edgeIndex;
				edge = statusElem->status->outEdges[edgeIdx];
				if (edge->isEpsilon) {
					nextIdx = idx;
				}
				else {
					bool included = false;
					if (s.size() <= idx) {
						goto BackTrace;
					}
					for (CharRange& cr : edge->content) {
						if (cr.Include(s[idx])) {
							included = true;
							break;
						}
					}
					if (!included) {
						goto BackTrace;
					}
					nextIdx = idx+1;
				}
				
				for (Edge::TypeListElem& e : edge->typeList) {
					switch (e.first) {
					case Edge::Type::Head:
						if (! CheckHead(idx)) {
							goto BackTrace;
						}
						break;
					case Edge::Type::Tail:
						if (! CheckTail(idx, s)) {
							goto BackTrace;
						}
						break;
					case Edge::Type::Positive:
						if (! PureMatch(subAtm->subAtmList[e.second], s, idx)) {
							goto BackTrace;
						}
						break;
					case Edge::Type::Negative:
						if (PureMatch(subAtm->subAtmList[e.second], s, idx)) {
							goto BackTrace;
						}
						break;
					default:
						break;
					}
				}
				idx = nextIdx;
				if (edge->target->isFinal) { // match success
					return idx-begin;
				}
				statusStack.Push(edge->target, 0, 0, 0, idx);
				continue;
			BackTrace:
				PureBackTrace(&statusStack, idx);
			}
			return 0;
		}


		static void PureBackTrace(StatusStack* stsS, std::size_t& strIdx)
		{
			StatusStack::StatusStackItem* stsElem = stsS->Top();
			if (stsElem->edgeIndex+1 < stsElem->status->outEdges.size()) {
				// 换下一条边
				stsElem->edgeIndex++;
			}
			else { // 回溯到前一个状态
				stsS->Pop();
				stsElem = stsS->Top();
				if (stsElem) {
					stsElem->edgeIndex++;
					if (stsElem->edgeIndex >= stsElem->status->outEdges.size()) {
						return PureBackTrace(stsS, strIdx);
					}
					strIdx = stsElem->begin;
				}
			}
			
		}

		static void BackTrace(CaptureStack* capS, CommandStack* cmdS, StatusStack* stsS, std::size_t& strIdx)
		{
			StatusStack::StatusStackItem* stsElem = stsS->Top();
			if (stsElem->edgeIndex+1 < stsElem->status->outEdges.size()) {
				// 换下一条边
				stsElem->edgeIndex++;
				cmdS->Resize(stsElem->cmdStackSize);
				capS->Resize(stsElem->captureStackSize);
			}
			else { // 回溯到前一个状态
				stsS->Pop();
				stsElem = stsS->Top();
				if (stsElem) {
					cmdS->Resize(stsElem->cmdStackSize);
					capS->Resize(stsElem->captureStackSize);
					stsElem->edgeIndex++;
					if (stsElem->edgeIndex >= stsElem->status->outEdges.size()) {
						return BackTrace(capS, cmdS, stsS, strIdx);
					}
					strIdx = stsElem->begin;
				}
			}
		}


		static bool CheckHead(int strIdx)
		{
			return 0 == strIdx;
		}
		static bool CheckTail(int strIdx, std::string& s)
		{
			return s.size() == strIdx;
		}
		// static bool CheckLoop(CommandStack& cmdS)
		// {
		// 	return true;
		// }
		// static bool CheckCapture(CommandStack& cmdS)
		// {
		// 	return true;
		// }
		// static bool CheckEnd(CommandStack& cmdS)
		// {
		// 	return true;
		// }
		// static bool CheckPositive(CommandStack& cmdS)
		// {
		// 	// TODO
		// 	return true;
		// }
		// static bool CheckNegative(CommandStack& cmdS)
		// {
		// 	// TODO
		// 	return true;
		// }

		// static void HandleLoop(CommandStack& cmdS, std::size_t para, int idx)
		// {
		// 	cmdS.Push(Edge::Type::Loop, para, idx);
		// }
		// static void HandleCapture(CommandStack& cmdS, std::size_t para, int idx)
		// {
		// 	cmdS.Push(Edge::Type::Capture, para, idx);
		// }
		// static void HandleEnd(CommandStack& cmdS, std::size_t para, int idx)
		// {
			
		// }
		static std::string& GetCaptureName(Automaton::ref atm, std::size_t idx)
		{
			return atm->nameList.at(idx);
		}
	}
}
