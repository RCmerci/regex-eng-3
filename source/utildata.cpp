#include <cassert>
#include <utility>
#include "utildata.hpp"

namespace regex
{
	namespace regex_internal
	{
		
		/*------------------------------------------
		  CharRange
		  --------------------------------------------*/
		const CharRange::RangeType CharRange::Max = CharRangeMAX;
		const CharRange::RangeType CharRange::Min = CharRangeMIN;
		
		CharRange::CharRange()
			:begin(0),
			 end(0)
		{}

		CharRange::CharRange(RangeType b, RangeType e)
		{
			assert(b>=Min);
			assert(b<=Max);
			assert(e>=Min);
			assert(e<=Max);
			begin = b > e ? e : b;
			end   = b > e ? b : e;
		}

		bool CharRange::operator<(const CharRange & item) const
		{
			if (end < item.begin) return true;
			return false;
		}

		bool CharRange::operator>(const CharRange & item) const
		{
			if (begin > item.end) return true;
			return false;
		}

		bool CharRange::operator==(const CharRange & item) const
		{
			if (begin == item.begin && end == item.end) return true;
			return false;
		}

		bool CharRange::operator<=(const CharRange & item) const
		{
			if (*this < item) return true;
			if (*this == item) return true;
			return false;
		}

		bool CharRange::operator>=(const CharRange & item) const
		{
			if (*this > item) return true;
			if (*this == item) return true;
			return false;
		}

		bool CharRange::operator!=(const CharRange & item) const
		{
			return !(*this == item);
		}

		bool CharRange::operator<(RangeType c) const
		{
			return end < c;
		}

		bool CharRange::operator>(RangeType c) const
		{
			return begin > c;
		}

		bool CharRange::operator==(RangeType c) const
		{
			if (begin == c && end == c) return true;
			return false;
		}

		bool CharRange::operator<=(RangeType c) const
		{
			if (*this < c) return true;
			if (*this == c) return true;
			return false;
		}

		bool CharRange::operator>=(RangeType c) const
		{
			if (*this > c) return true;
			if (*this == c) return true;
			return false;
		}

		bool CharRange::operator!=(RangeType c) const
		{
			return !(*this == c);
		}

		bool CharRange::Include(RangeType c)const
		{
			return c <= end && c >= begin;
		}

	}
}
