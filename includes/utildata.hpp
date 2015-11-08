#ifndef REGEX_UTILDATA
#define REGEX_UTILDATA

namespace regex
{
	namespace regex_internal
	{
		/* -----------------------------------------------------
		   CharRange 类
		 --------------------------------------------------------*/
#define CharRangeMAX         127
#define CharRangeMIN         0
		class CharRange
		{
		public:
			typedef char     RangeType;
			
			RangeType begin;
			RangeType end;
			static const RangeType Max;
			static const RangeType Min;
			CharRange(RangeType begin, RangeType end);
			CharRange();

			bool operator<(const CharRange & i)const; /* end < i.begin */
			bool operator>(const CharRange & i)const; /* begin > i.end */
			bool operator==(const CharRange & i)const; /* begin == i.begin && end==i.end */
			bool operator<=(const CharRange & i)const; /* this < i || this == i */
			bool operator>=(const CharRange & i)const; /* this > i || this == i */
			bool operator!=(const CharRange & i)const; /* !(this==i) */

			bool operator<(RangeType c)const; /* end < c */
			bool operator>(RangeType c)const; /* begin > c */
			bool operator==(RangeType c)const; /* begin == end == c */
			bool operator<=(RangeType c)const; /* this < c || this == c */
			bool operator>=(RangeType c)const; /* this > c || this == c */
			bool operator!=(RangeType c)const; /* !(this == c) */

			bool Include(RangeType c)const;
		};
	}
}
#endif
