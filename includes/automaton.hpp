#ifndef REGEX_AUTOMATON
#define REGEX_AUTOMATON

#include <vector>
#include <list>
#include <utility>
#include "utildata.hpp"

namespace regex
{
	namespace regex_internal
	{
		class Edge;
		class Status;


		
		class Status
		{
		private:
			static unsigned long ID;
		public:
			using ref = Status*;
			unsigned long id;
			bool isFinal;
			std::vector<Edge*>  inEdges;
			std::vector<Edge*>  outEdges;
			Status(bool _isFinal=false);
			void AddInEdge(Edge* e);
			void AddOutEdge(Edge* e);
		};
		
		
		class Edge
		{
		public:
			using ref = Edge*;

			enum class Type
			{
				Head,
				Tail,
				Loop,
				End,
				Capture,
				Positive,
				Negative,
				//				        Using
			};
			using TypeListElem = std::pair<Edge::Type, std::size_t>;
			using TypeList = std::list<TypeListElem>;
			
			Status* source;
			Status* target;
			std::list<CharRange> content;
			bool isEpsilon;
			TypeList typeList;
			//			std::size_t index;
			// ---------------------------
			// name -> cap                            indexToName
			// pair<int, int> -> loop                 -
			// prefer -> loop , or                    -, 优先级高的在status里的列表前部分
			// atm* -> pos, neg                       indexToAtm
			// [X] usingCreate -> using                   -|合并
			// [X] usingUse -> using                      -|--> indexToAtm
			// -----------------------------
			
			Edge(Status* _source, Status* _target);
			Edge(std::list<CharRange>& _content, Status* _source, Status* _target);
			
			void MergeType(Edge* e);

			Edge&   Head();
			Edge&   Tail();
			Edge&   Loop();
			Edge&   End();
			Edge&   Capture(std::size_t index);
			Edge&   Positive(std::size_t index);
			Edge&   Negative(std::size_t index);
			Edge&   AddTypes(TypeList& tl);
			
			static Type TypeOfTypeListElem(TypeListElem& e);
			static std::size_t IndexOfTypeListElem(TypeListElem& e);

		private:
			TypeListElem makeTypePair(Type, std::size_t);
		};

		class Automaton
		{
		public:
			using ref = Automaton*;
			
			Automaton(bool _isEpsilonNfa=true);
			Automaton(Automaton&)=delete;
			Automaton(Automaton&&)=delete;
			void operator=(Automaton&)=delete;
			~Automaton();

			Status::ref NewStatus(bool isFinal=false, bool isStart=false);
			Edge&   NewEpsilonEdge(Status::ref source, Status::ref target);
			Edge&   NewEdge(CharRange& cr, Status::ref source, Status::ref target);
			Edge&   NewEdge(std::list<CharRange>& content, Status::ref source, Status::ref target);
			std::size_t NewSubAutomaton();
			std::size_t NewCaptureName(std::string& name);
			std::size_t NewCaptureName(std::string&& name);


			Automaton::ref EpsilonNfa2Nfa();
			
			std::list<Status::ref> statusList;
			std::list<Edge::ref>   edgeList;
			Status::ref beginStatus;
			std::list<Status::ref> endStatusList;
			std::vector<Automaton::ref> subAtmList;
			std::vector<std::string>    nameList;
			bool isEpsilonNfa;
		private:
			void AddStatus(Status::ref s);
			void AddEdge(Edge::ref e);
			void SetBeginStatus(Status::ref s);
		};


		class PAPSS
		{
		public:
			Automaton::ref atm;
			Status::ref s1;
			Status::ref s2;
			PAPSS(Automaton::ref atm, Status::ref s1, Status::ref s2);
			PAPSS()=default;
		};
		
	}
}

#endif
