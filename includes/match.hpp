#ifndef REGEX_MATCH
#define REGEX_MATCH

#include <map>
#include <string>
#include <list>
#include <vector>
#include "automaton.hpp"


namespace regex
{
	class RegexResult;
	namespace regex_internal
	{
		/*----------------------------------------
		  Interpreter
		 ----------------------------------------*/ 
		class Interpreter
		{
		private:
			Automaton::ref atm;
		public:
			Interpreter(Automaton::ref atm);
			~Interpreter();
			Interpreter& operator=(Interpreter&& i);

			// return length
			std::size_t Match(std::string s, std::size_t begin, RegexResult& rst);
			// return length
			std::size_t PureMatch(Automaton::ref subAtm, std::string s, std::size_t begin);
		};

		/*----------------------------------------
		  MatchResult
		 ----------------------------------------*/ 
		// class MatchResult
		// {
		// public:
			
		// };
		/*----------------------------------------
		  CaptureItem
		 ----------------------------------------*/ 
		// class CaptureItem
		// {
		// public:
		// 	using ref = CaptureItem*;
			
		// private:
		// 	std::vector<std::string> captureList;
		// };

		// class MatchData
		// {
		// public:
		// 	using ref = MatchData*;

		// 	MatchData();
		// 	~MatchData();
		// 	CaptureItem* GetCapture(std::string s);
			
		// private:
		// 	std::map<std::string, CaptureItem*> captureGroups;
		// 	std::list<CaptureItem*> captureItems;
		// };


		/*----------------------------------------
		  CaptureStack, CommandStack, StatusStack
		 ----------------------------------------*/ 
		class CaptureStack
		{
		public:
			struct CaptureStackItem
			{
				std::string name;
				// int statusStackSize;
				std::size_t begin;
				std::size_t length;
			};
			void Push(std::string name, std::size_t begin, std::size_t length);
			void Pop(int size=1);
			CaptureStackItem* Top();
			void Resize(std::size_t size);
			std::size_t GetSize();


			std::list<CaptureStackItem>& GetInnerStack();
		private:
			std::list<CaptureStackItem> stack;
		};

		class CommandStack
		{
		public:
			struct CommandStackItem
			{
				Edge::Type type;
				std::size_t para;
				int lastNotFinishCmdIndex;
				std::size_t begin;
			};
			void Push(Edge::Type type, std::size_t para, std::size_t begin);
			void Pop(int size=1);
			CommandStackItem* Top();
			void Resize(std::size_t size);
			std::size_t GetSize();
			int GetLastNotFinishCmdIndex();
			CommandStackItem& At(std::size_t idx);
		private:
			std::vector<CommandStackItem> stack;
		};
		
		class StatusStack
		{
		public:
			struct StatusStackItem
			{
				Status::ref status;
				int edgeIndex;
				int cmdStackSize;
				int captureStackSize;
				std::size_t begin;
			};
			void Push(Status::ref status, int edgeIdx, int cmdStackSize, int captureStackSize, std::size_t begin);
			void Pop(int size=1);
			StatusStackItem* Top();
			void Resize(std::size_t size);
			std::size_t GetSize();

		private:
			std::list<StatusStackItem> stack;
		};

		
	}
}
#endif
