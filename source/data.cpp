#include <utility>
#include "data.hpp"
//#include "regexAlgorithm.hpp"

namespace regex
{
	namespace regex_internal
	{
		/*------------------------------------------
		  Aux
		  --------------------------------------------*/
		bool CharRangeBoolPairCompare(std::pair<CharRange, bool>& v1, std::pair<CharRange, bool>& v2)
		{
			return v1.first < v2.first;
		}

		
		/*------------------------------------------
		  CharRange
		  --------------------------------------------*/
		CharRange::CharRange()
			:begin(0),
			 end(0)
		{}

		CharRange::CharRange(char b, char e)
		{
			begin = b > e ? e : b;
			end   = b > e ? b : e;
		}

		bool CharRange::operator<(CharRange & item) const
		{
			if (end < item.begin) return true;
			return false;
		}

		bool CharRange::operator>(CharRange & item) const
		{
			if (begin > item.end) return true;
			return false;
		}

		bool CharRange::operator==(CharRange & item) const
		{
			if (begin == item.begin && end == item.end) return true;
			return false;
		}

		bool CharRange::operator<=(CharRange & item) const
		{
			if (*this < item) return true;
			if (*this == item) return true;
			return false;
		}

		bool CharRange::operator>=(CharRange & item) const
		{
			if (*this > item) return true;
			if (*this == item) return true;
			return false;
		}

		bool CharRange::operator!=(CharRange & item) const
		{
			return !(*this == item);
		}

		bool CharRange::operator<(char c) const
		{
			return end < c;
		}

		bool CharRange::operator>(char c) const
		{
			return begin > c;
		}

		bool CharRange::operator==(char c) const
		{
			if (begin == c && end == c) return true;
			return false;
		}

		bool CharRange::operator<=(char c) const
		{
			if (*this < c) return true;
			if (*this == c) return true;
			return false;
		}

		bool CharRange::operator>=(char c) const
		{
			if (*this > c) return true;
			if (*this == c) return true;
			return false;
		}

		bool CharRange::operator!=(char c) const
		{
			return !(*this == c);
		}



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
		//   注释回来
		// Automaton::ref Expression::GenerateEpsilonNfa()
		// {
		// 	GenerateEpsilonNfaAlgorithm().Call(this);
		// }
		
		// void Expression::Apply(RegexAlgorithmBase * algorithm)
		// {
		// 	algorithm->Visit(this);
		// }

		void CharsetExp::AddCharRange(CharRange& cr, bool isReverse)
		{
			charRangeL.push_back(std::make_pair(cr, isReverse));
			charRangeL.sort(CharRangeBoolPairCompare);
		}
		void CharsetExp::AddCharRange(CharRange&& cr, bool isReverse)
		{
			AddCharRange(cr, isReverse);
		}

		// void OrExp::Apply(RegexAlgorithmBase * algorithm)
		// {
		// 	algorithm->Apply(this);
		// }

		// void JoinExp::Apply(RegexAlgorithmBase * algorithm)
		// {
		// 	algorithm->Apply(this);
		// }

		// void LoopExp::Apply(RegexAlgorithmBase * algorithm)
		// {
		// 	algorithm->Apply(this);
		// }

		// void CharsetExp::Apply(RegexAlgorithmBase * algorithm)
		// {
		// 	algorithm->Apply(this);
		// }

		// void UsingExp::Apply(RegexAlgorithmBase * algorithm)
		// {
		// 	algorithm->Apply(this);
		// }

		// void PositiveExp::Apply(RegexAlgorithmBase * algorithm)
		// {
		// 	algorithm->Apply(this);
		// }

		// void NegativeExp::Apply(RegexAlgorithmBase * algorithm)
		// {
		// 	algorithm->Apply(this);
		// }
		
		// void CaptureExp::Apply(RegexAlgorithmBase * algorithm)
		// {
		// 	algorithm->Apply(this);
		// }
		
		// void BeginExp::Apply(RegexAlgorithmBase * algorithm)
		// {
		// 	algorithm->Apply(this);
		// }
		
		// void EndExp::Apply(RegexAlgorithmBase * algorithm)
		// {
		// 	algorithm->Apply(this);
		// }
				

	}
}









