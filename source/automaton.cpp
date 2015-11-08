#include <cassert>
#include <algorithm>
#include <list>
#include <utility>
#include <string>
#include <vector>
#include <stack>
#include "automaton.hpp"


namespace regex
{
	namespace regex_internal
	{
		/*----------------------------------------
		  AUX
		  ----------------------------------------*/
		static void CollectEpsilonEdge(Status::ref source, std::list<Edge::ref>& newEdges);
		/*----------------------------------------
		  Status
		  ----------------------------------------*/

		unsigned long Status::ID = 0;
		Status::Status(bool _isFinal)
			:id(ID),
			 isFinal(_isFinal)
		{
			++ID;
		}
		void Status::AddInEdge(Edge* e)
		{
			inEdges.push_back(e);
		}
		void Status::AddOutEdge(Edge* e)
		{
			outEdges.push_back(e);
		}

		/*----------------------------------------
		  Edge
		  ----------------------------------------*/
		Edge::TypeListElem Edge::makeTypePair(Type t, std::size_t s)
		{
			return std::make_pair(t, s);
		}
		
		Edge::Edge(Status* _source, Status* _target)
			:source(_source),
			 target(_target),
			 isEpsilon(true)
		{}

		Edge::Edge(std::list<CharRange>& _content, Status* _source, Status* _target)
			:source(_source),
			 target(_target),
			 content(_content),
			 isEpsilon(false)
		{}

		void Edge::MergeType(Edge* e)
		{
			for (auto t : e->typeList) {
				typeList.push_back(t);
			}
		}

		Edge& Edge::Head()
		{
			typeList.push_back(makeTypePair(Type::Head, 0));
			return *this;
		}
		Edge& Edge::Tail()
		{
			typeList.push_back(makeTypePair(Type::Tail, 0));
			return *this;
		}
		Edge& Edge::Loop()
		{
			typeList.push_back(makeTypePair(Type::Loop, 0));
			return *this;
		}
		Edge& Edge::End()
		{
			typeList.push_back(makeTypePair(Type::End, 0));
			return *this;
		}
		Edge& Edge::Capture(std::size_t _index)
		{
			typeList.push_back(makeTypePair(Type::Capture, _index));
			return *this;
		}
		Edge& Edge::Positive(std::size_t _index)
		{
			typeList.push_back(makeTypePair(Type::Positive, _index));
			return *this;
		}
		Edge& Edge::Negative(std::size_t _index)
		{
			typeList.push_back(makeTypePair(Type::Negative, _index));
			return *this;
		}
		Edge& Edge::AddTypes(TypeList& tl)
		{
			for (TypeListElem& t : tl) {
				typeList.push_back(t);
			}
			return *this;
		}
		

		
		Edge::Type Edge::TypeOfTypeListElem(TypeListElem& e)
		{
			return e.first;
		}
		std::size_t Edge::IndexOfTypeListElem(TypeListElem& e)
		{
			return e.second;
		}
		/*----------------------------------------
		  Automaton
		  ----------------------------------------*/
		Automaton::Automaton(bool _isEpsilonNfa)
			:isEpsilonNfa(_isEpsilonNfa)
		{
			// Status* start = new Status();
			// Status* end   = new Status(true);
			// statusList.push_back(start);
			// statusList.push_back(end);
			// beginStatus = start;
		}
		
		Automaton::~Automaton()
		{
			if (isEpsilonNfa) {
				// for (auto s : statusList) {
				// 	if (s == beginStatus) {
				// 		// skip begin status
				// 		continue;
				// 	}
				// 	if (endStatusList.end() != std::find(endStatusList.begin(), endStatusList.end(), s)) {
				// 		// skip end status
				// 		continue;
				// 	}
				// 	for (auto e : s->inEdges) {
				// 		if (!e->isEpsilon) {
				// 			s = nullptr;
				// 			break;
				// 		}
				// 	}
				// 	delete s;
				// }
				for (auto e : edgeList) {
					// epsilonNfa -> nfa 的时候，
					// 会重新new需要的边，原来的边不管是不是epsilon，
					// 直接delete
					// if (e->isEpsilon) {
					delete e;
					// }
				}
				for (auto atm : subAtmList) {
					delete atm;
				}
			}
			else {
				for (auto s : statusList) {
					delete s;
				}
				for (auto e : edgeList) {
					delete e;
				}
				for (auto atm : subAtmList) {
					delete atm;
				}
			}
		}

		Status::ref Automaton::NewStatus(bool isFinal, bool isStart)
		{
			Status::ref s = new Status(isFinal);
			statusList.push_back(s);
			if (isFinal) {
				endStatusList.push_back(s);
			}
			if (isStart) {
				beginStatus = s;
			}
			return s;
		}

		Edge& Automaton::NewEpsilonEdge(Status::ref source, Status::ref target)
		{
			Edge::ref e = new Edge(source, target);
			edgeList.push_back(e);
			source->AddOutEdge(e);
			target->AddInEdge(e);
			return *e;
		}

		Edge& Automaton::NewEdge(CharRange& cr, Status::ref source, Status::ref target)
		{
			std::list<CharRange> content = std::list<CharRange>(1, cr);
			Edge::ref e = new Edge(content, source, target);
			edgeList.push_back(e);
			source->AddOutEdge(e);
			target->AddInEdge(e);
			return *e;
		}
		Edge& Automaton::NewEdge(std::list<CharRange>& content, Status::ref source, Status::ref target)
		{

			Edge::ref e = new Edge(content, source, target);
			edgeList.push_back(e);
			source->AddOutEdge(e);
			target->AddInEdge(e);
			return *e;
		}

		std::size_t Automaton::NewSubAutomaton()
		{
			Automaton::ref subAtm = new Automaton;
			subAtmList.push_back(subAtm);
			return subAtmList.size()-1;
		}

		std::size_t Automaton::NewCaptureName(std::string& name)
		{
			nameList.push_back(name);
			return nameList.size()-1;
		}
		std::size_t Automaton::NewCaptureName(std::string&& name)
		{
			return NewCaptureName(name);
		}

		
		Automaton::ref Automaton::EpsilonNfa2Nfa()
		{
			Automaton::ref newAtm = new Automaton;
			//----------------------------------------
			//   step 1 收集消耗了字符的集合
			// ----------------------------------------
			std::vector<Status::ref> NoConsumeStatusList;
			std::vector<Status::ref> ConsumeStatusList;
			ConsumeStatusList.push_back(beginStatus);

			auto begin = statusList.begin();
			begin++; // except the startStatus
			auto end   = statusList.end();
			bool isConsumeStatus = false;
			for (auto i = begin; i != end; i++) {
				isConsumeStatus = false;
				if ((*i)->isFinal) {
					isConsumeStatus = true;
				}
				else {
					for (auto e : (*i)->inEdges) {
						if (!e->isEpsilon) {
							isConsumeStatus = true;
							break;
						}
					}
				}
				if (isConsumeStatus) {
					ConsumeStatusList.push_back((*i));
				}
				else {
					NoConsumeStatusList.push_back((*i));
				}

			}
			//----------------------------------------
			//  step 2 给newAtm加上压缩的边
			// ---------------------------------------
			for (auto s : ConsumeStatusList) { 
				newAtm->AddStatus(s);
			}
			newAtm->SetBeginStatus(beginStatus);
			std::list<Edge::ref> newEdges;
			for (auto s : ConsumeStatusList) {
				if (!s->isFinal || s->outEdges.size() > 0) {
					CollectEpsilonEdge(s, newEdges);
					s->inEdges.clear();
					s->outEdges.clear();
					for (auto e : newEdges) {
						newAtm->AddEdge(e);
					}
					newEdges.clear();
				}
			}
			// //----------------------------------------
			// //  step 3 处理终结状态的输入边
			// // ---------------------------------------
			// std::list<Edge::ref> mergeEdges;
			// Edge::ref newEdge = nullptr;
			// Status::ref statusBeforeEnd = nullptr;
			// Status::ref statusBeforeStatusBeforeEnd = nullptr;
			// for (auto s : newAtm->endStatusList) {
			// 	for (auto e : s->inEdges) {
			// 		if (!e->isEpsilon) {
			// 			continue;
			// 		}
			// 		statusBeforeEnd = e->source;
			// 		for (auto e1 : statusBeforeEnd->inEdges) {
			// 			statusBeforeStatusBeforeEnd = e1->source;
			// 			newEdge = new Edge(e1->content, statusBeforeStatusBeforeEnd, s);
			// 			newEdge->AddTypes(e1->typeList);
			// 			newEdge->AddTypes(e->typeList);
			// 			mergeEdges.push_back(newEdge);
			// 		}
			// 	}
			// }
			// for (auto e : mergeEdges) {
			// 	newAtm->AddEdge(e);
			// }
			// //----------------------------------------
			// //  step 4 先删除终结状态输入边中的空边
			// //         再
			// // ---------------------------------------
			// std::vector<Edge::ref> deleteEdges;
			// std::vector<Status::ref> deleteStatuses;
			// // 删除 目标是endStatus的空边
			// for (auto s : newAtm->endStatusList) { 
			// 	for (auto& e : s->inEdges) {
			// 		if (e->isEpsilon) {
			// 			deleteEdges.push_back(e);
			// 		}
			// 	}
			// }
			// std::size_t oldSize = deleteEdges.size();
			// while (true) {
			// 	for (auto s : newAtm->statusList) {
			// 		// 收集没有out edge 的状态（除了终止状态）
			// 		if (s->isFinal) {
			// 			continue;
			// 		}
			// 		bool isDeleteStatus = true;
			// 		for (auto e : s->outEdges) {
			// 			auto findEdge = std::find(deleteEdges.begin(),
			// 				  deleteEdges.end(),
			// 				  e);
			// 			if (deleteEdges.end() == findEdge) {
			// 				isDeleteStatus = false;
			// 				break;
			// 			}
			// 		}
			// 		if (isDeleteStatus) {
			// 			deleteStatuses.push_back(s);
			// 		}
			// 	}
			// 	for (auto s : deleteStatuses) {
			// 		// 收集将要删除的状态的in edge
			// 		for (auto e : s->inEdges) {
			// 			deleteEdges.push_back(e);
			// 		}
			// 	}
			// 	for (auto s : deleteStatuses) {
			// 		// 删除刚刚收集的要删除的状态
			// 		delete s;
			// 		std::remove(newAtm->statusList.begin(),
			// 			    newAtm->statusList.end(),
			// 			    s);
			// 	}
			// 	if (deleteEdges.size() == oldSize) {
			// 		// 要删除的边数量没有变化，
			// 		// 说明newAtm已经删完了要删的边和状态
			// 		// auto p = [&deleteEdges](Edge::ref e)
			// 		// 	{
			// 		// 		return
			// 		// 		deleteEdges.end() !=
			// 		// 		std::find(deleteEdges.begin(),
			// 		// 			  deleteEdges.end(),
			// 		// 			  e);
			// 		// 	};
			// 		for (auto e : deleteEdges) {
			// 			delete e;
			// 			std::remove(newAtm->edgeList.begin(),
			// 				       newAtm->edgeList.end(),
			// 				       e);
						
			// 		}
			// 		break;
			// 	}
			// 	oldSize = deleteEdges.size();
				
			// }

			//----------------------------------------
			//  step 5  (handle subAtm)
			// ---------------------------------------
			for (auto atm : subAtmList) {
				newAtm->subAtmList.push_back(atm->EpsilonNfa2Nfa());
			}
			
			//----------------------------------------
			//  step 6 复制Automaton里的其他资源
			// ---------------------------------------
			newAtm->nameList = nameList;


			//----------------------------------------
			//  step 7 删除没有消耗字符的状态
			// ---------------------------------------
			for (auto s : NoConsumeStatusList) {
				delete s;
			}


			
			newAtm->isEpsilonNfa = false;
			return newAtm;
		}

		void Automaton::AddStatus(Status::ref s)
		{
			statusList.push_back(s);
			if (s->isFinal) {
				endStatusList.push_back(s);
			}
		}
		void Automaton::AddEdge(Edge::ref e)
		{
			edgeList.push_back(e);
			e->source->outEdges.push_back(e);
			e->target->inEdges.push_back(e);
		}
		void Automaton::SetBeginStatus(Status::ref s)
		{
			beginStatus = s;
		}


		/*----------------------------------------
		  PAPSS
		  ----------------------------------------*/
		PAPSS::PAPSS(Automaton::ref _atm, Status::ref _s1, Status::ref _s2)
			:atm(_atm),
			 s1(_s1),
			 s2(_s2)
		{}

		/*----------------------------------------
		  Aux
		  ----------------------------------------*/
		static void CollectEpsilonEdge(Status::ref source, std::list<Edge::ref>& newEdges)
		{
			using StateStackElem = std::pair<Status::ref, std::pair<int, int> >;
			using StateStack = std::list<StateStackElem>;
			using TypeStackElem = Edge::TypeListElem;
			using TypeStack  = Edge::TypeList;
			
			auto makeStatePair = [](Status::ref s, int i, int j)
				{
					return std::make_pair(s, std::make_pair(i, j));
				};
			auto makeTypePair = [](Edge::Type t, std::size_t i)
				{
					return std::make_pair(t, i);
				};
			auto incrOutEdgeIndex = [](StateStackElem& e)
				{
					++(e.second.first);
					if (e.second.first >= e.first->outEdges.size()) {
						return false;
					}
					return true;
				};
			auto decrTypeIndex = [](StateStackElem& e, int i)
				{
					e.second.second -= i;
				};
			auto outEdgeIndex = [](StateStackElem& e)
				{
					return e.second.first;
				};
			auto typeSize = [](StateStackElem& e)
				{
					return e.second.second;
				};
			StateStack processStack;
			TypeStack typeStack;
			processStack.push_back(makeStatePair(source, 0, 0));
			
			auto addNewEdge = [&newEdges, &typeStack, &source](Status::ref target, std::list<CharRange>& content, bool isEpsilon)
				{
					Edge::ref newEdge;
					if (!isEpsilon) {
						newEdge = new Edge(content, source, target);
					}
					else {
						newEdge = new Edge(source, target);
					}
					newEdge->AddTypes(typeStack);
					newEdges.push_back(newEdge);
				};
			
			Status::ref currentStatus = nullptr;
			Edge::ref currentOutEdge = nullptr;
			while (true) {
				currentStatus = processStack.back().first;
				currentOutEdge = currentStatus->outEdges[outEdgeIndex(processStack.back())];
					
				for (auto& type : currentOutEdge->typeList) {
					typeStack.push_back(type);
				}
				
				if (currentOutEdge->isEpsilon && !currentOutEdge->target->isFinal) { // 边是epsilon && 边的target不是final
					StateStackElem elem = makeStatePair(currentOutEdge->target, 0, typeStack.size());
					processStack.push_back(elem);
				}
				else { // 边不是epsilon || 边的target是final
					addNewEdge(currentOutEdge->target, currentOutEdge->content, currentOutEdge->isEpsilon);
					while (true) {
						if (!incrOutEdgeIndex(processStack.back())) {
							processStack.pop_back();
							if (processStack.size() == 0) {
								return;
							}
							int newSize = typeSize(processStack.back());
							typeStack.resize(newSize);
							
						}
						else {
							typeStack.resize(typeSize(processStack.back()));
							break;
						}
						
					}
				}
			}
		}
	}
}
