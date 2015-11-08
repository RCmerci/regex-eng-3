#include <cassert>
#include <utility>
#include "data.hpp"
#include "regexAlgorithm.hpp"
#include "automaton.hpp"

namespace regex
{
	namespace regex_internal
	{

		/*---------------------------------------------------
		  Expression
		  OrExp
		  JoinExp
		  LoopExp
		  CharsetExp
		  UsingExp
		  PositiveExp
		  NegativeExp
		  CaptureExp
		  BeginExp
		  EndExp
		  -----------------------------------------------------*/ 

		Expression::Expression()
		{}
		Automaton::ref Expression::GenerateEpsilonNfa()
		{
			Automaton::ref automaton = new Automaton;
			Status::ref begin = automaton->NewStatus(false, true);
			Status::ref end   = automaton->NewStatus(true, false);
			assert(automaton->endStatusList.size() == 1);
			PAPSS parameter = PAPSS(automaton, begin, end);
			Automaton::ref atm = GenerateEpsilonNfaAlgorithm().Call(this, parameter);


			// TODO : 删除以下
			for (auto e : atm->edgeList) {
				assert(e->typeList.size()<=1);
			}
			
			return atm;
		}

		bool Expression::Compare(Expression& expression)
		{
			return CompareExpressionAlgorithm().Call(this, &expression);
		}

		void Expression::MergeCharset()
		{
			MergeCharsetAlgorithm().Call(this, nullptr);
		}

		void Expression::TransferUsingExp()
		{
			TransferUsingExpAlgorithm::Name2Expression map = new std::map<std::string, Expression::ref>();
			TransferUsingExpAlgorithm().Call(this, map);
			delete map;
		}

		Expression::ref Expression::RecursiveCopyExpression()
		{
			return RecursiveCopyExpressionAlgorithm().Call(this, nullptr);
		}

		void Expression::Delete()
		{
			DeleteExpressionAlgorithm().Call(this, nullptr);
		}

		void Expression::SetNumberOnAnonymousCaptureExpression()
		{
			SetNumberOnAnonymousCaptureExpressionAlgorithm().Call(this, 1);
		}
		
		
		
		void CharsetExp::AddCharRange(CharRange& cr, bool isReverse)
		{
			charRangeL.push_back(std::make_pair(std::list<CharRange>(1, cr), isReverse));
		}
		
		void CharsetExp::AddCharRange(CharRange&& cr, bool isReverse)
		{
			AddCharRange(cr, isReverse);
		}

		void CharsetExp::AddCharRange(std::list<CharRange>& crl, bool isReverse)
		{
			charRangeL.push_back(std::make_pair(crl, isReverse));
		}
		/*----------------------------------------
		  Accept
		 ----------------------------------------*/ 
		
		void OrExp::Accept(ExpressionVisitor * algorithm)
		{
			algorithm->Visit(this);
		}

		void JoinExp::Accept(ExpressionVisitor * algorithm)
		{
			algorithm->Visit(this);
		}

		void LoopExp::Accept(ExpressionVisitor * algorithm)
		{
			algorithm->Visit(this);
		}

		void CharsetExp::Accept(ExpressionVisitor * algorithm)
		{
			algorithm->Visit(this);
		}

		void UsingExp::Accept(ExpressionVisitor * algorithm)
		{
			algorithm->Visit(this);
		}

		void PositiveExp::Accept(ExpressionVisitor * algorithm)
		{
			algorithm->Visit(this);
		}

		void NegativeExp::Accept(ExpressionVisitor * algorithm)
		{
			algorithm->Visit(this);
		}
		
		void CaptureExp::Accept(ExpressionVisitor * algorithm)
		{
			algorithm->Visit(this);
		}
		
		void BeginExp::Accept(ExpressionVisitor * algorithm)
		{
			algorithm->Visit(this);
		}
		
		void EndExp::Accept(ExpressionVisitor * algorithm)
		{
			algorithm->Visit(this);
		}


		// /*----------------------------------------
		//   destructor
		//   -----------------------------------------*/ 

		Expression::~Expression()
		{
		}

	}
}









