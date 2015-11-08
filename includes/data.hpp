#ifndef REGEX_DATA
#define REGEX_DATA

#include <list>
#include <utility>
#include <string>
#include "automaton.hpp"

namespace regex
{
	namespace regex_internal
	{
		class CharRange;
		class ExpressionVisitor;
		
		class Expression
		{
		public:
			typedef Expression*    ref;

			Expression();
			Automaton::ref GenerateEpsilonNfa();
			bool Compare(Expression& expression);
			void MergeCharset();
			void TransferUsingExp();
			void SetNumberOnAnonymousCaptureExpression();
			Expression::ref RecursiveCopyExpression();
			void Delete();
			
			virtual void Accept(ExpressionVisitor* algorithm)=0;
			virtual ~Expression();
		};

		class OrExp : public Expression
		{
		public:
			Expression::ref opt1; // maybe nullptr
			Expression::ref opt2; // maybe nullptr
			void Accept(ExpressionVisitor* algorithm);
			// ~OrExp();
		};
		class JoinExp : public Expression
		{
		public:
			Expression::ref left;
			Expression::ref right;
			void Accept(ExpressionVisitor* algorithm);
			// ~JoinExp();
		};
		class LoopExp : public Expression
		{
		public:
			Expression::ref innerExp;
			int min;
			int max;
			bool greedy;
			void Accept(ExpressionVisitor* algorithm);
			// ~LoopExp();
		};
		class CharsetExp : public Expression
		{
		public:
			std::list<std::pair<std::list<CharRange>, bool> > charRangeL; // parser产生
			std::list<CharRange> charRangeListResult; // 把charRangeL整理后的结果(MergeCharsetAlgorithm产生)
			bool reverse;
			void AddCharRange(CharRange& cr, bool isReverse);
			void AddCharRange(CharRange&& cr, bool isReverse);
			void AddCharRange(std::list<CharRange>& crl, bool isReverse);
			
			void Accept(ExpressionVisitor* algorithm);
			// ~CharsetExp();
		};
		class UsingExp : public Expression
		{
		public:
			bool isCreate;
			std::string name;
			Expression::ref innerExp; // maybe nullptr in 'isCreate' mode
			void Accept(ExpressionVisitor* algorithm);
			// ~UsingExp();
		};
		class PositiveExp : public Expression
		{
		public:
			Expression::ref innerExp;
			void Accept(ExpressionVisitor* algorithm);
			// ~PositiveExp();
		};
		class NegativeExp : public Expression
		{
		public:
			Expression::ref innerExp;
			void Accept(ExpressionVisitor* algorithm);
			// ~NegativeExp();
		};
		class CaptureExp : public Expression
		{
		public:
			std::string name;
			Expression::ref innerExp; 
			void Accept(ExpressionVisitor* algorithm);
			// ~CaptureExp();
		};
		class BeginExp : public Expression
		{
		public:
			void Accept(ExpressionVisitor* algorithm);
			// ~BeginExp();
		};
		class EndExp : public Expression
		{
		public:
			void Accept(ExpressionVisitor* algorithm);
			// ~EndExp();
		};




	}
}

#endif
