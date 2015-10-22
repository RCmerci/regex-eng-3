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
			// 注释回来
			//			Automaton::ref GenerateEpsilonNfa();
			bool Compare(Expression& expression);
			
			virtual void Accept(ExpressionVisitor* algorithm)=0;
		};

		class OrExp : public Expression
		{
		public:
			Expression::ref opt1;
			Expression::ref opt2;
			void Accept(ExpressionVisitor* algorithm);
		};
		class JoinExp : public Expression
		{
		public:
			Expression::ref left;
			Expression::ref right;
			void Accept(ExpressionVisitor* algorithm);
		};
		class LoopExp : public Expression
		{
		public:
			Expression::ref innerExp;
			int min;
			int max;
			bool greedy;
			void Accept(ExpressionVisitor* algorithm);
		};
		class CharsetExp : public Expression
		{
		public:
			std::list<std::pair<CharRange, bool> > charRangeL;
			bool reverse;
			void AddCharRange(CharRange& cr, bool isReverse);
			void AddCharRange(CharRange&& cr, bool isReverse);
			void Accept(ExpressionVisitor* algorithm);
		};
		class UsingExp : public Expression
		{
		public:
			bool isCreate;
			std::string name;
			Expression::ref innerExp;
			void Accept(ExpressionVisitor* algorithm);
		};
		class PositiveExp : public Expression
		{
		public:
			Expression::ref innerExp;
			void Accept(ExpressionVisitor* algorithm);
		};
		class NegativeExp : public Expression
		{
		public:
			Expression::ref innerExp;
			void Accept(ExpressionVisitor* algorithm);
		};
		class CaptureExp : public Expression
		{
		public:
			std::string name;
			Expression::ref innerExp;
			void Accept(ExpressionVisitor* algorithm);
		};
		class BeginExp : public Expression
		{
		public:
			void Accept(ExpressionVisitor* algorithm);
		};
		class EndExp : public Expression
		{
		public:
			void Accept(ExpressionVisitor* algorithm);
		};



		/* -----------------------------------------------------
		   CharRange 类
		 --------------------------------------------------------*/
		class CharRange
		{
		public:
			char begin;
			char end;
			CharRange(char begin, char end);
			CharRange();

			bool operator<(CharRange & i)const; /* end < i.begin */
			bool operator>(CharRange & i)const; /* begin > i.end */
			bool operator==(CharRange & i)const; /* begin == i.begin && end==i.end */
			bool operator<=(CharRange & i)const; /* this < i || this == i */
			bool operator>=(CharRange & i)const; /* this > i || this == i */
			bool operator!=(CharRange & i)const; /* !(this==i) */

			bool operator<(char c)const; /* end < c */
			bool operator>(char c)const; /* begin > c */
			bool operator==(char c)const; /* begin == end == c */
			bool operator<=(char c)const; /* this < c || this == c */
			bool operator>=(char c)const; /* this > c || this == c */
			bool operator!=(char c)const; /* !(this == c) */
		};
	}
}

#endif
