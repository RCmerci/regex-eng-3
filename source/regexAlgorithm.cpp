#include "regexAlgorithm.hpp"
//#include "data.hpp"

namespace regex
{
	namespace regex_internal
	{
		/*-------------------------------------------------------
		  RegexAlgorithmBase
		-------------------------------------------------------*/
		// template<typename P, typename R>
		// R RegexAlgorithmBase<P, R>::Call(Expression* e, P& _parameter)
		// {
		// 	parameterPtr = &_parameter;
		// 	e->Accept(this);
		// 	return result;
		// }
		
		// template<typename P, typename R>
		// void RegexAlgorithmBase<P, R>::Visit(OrExp * expression)
		// {
		// 	result = Apply(expression, *parameterPtr);
		// }
		// template<typename P, typename R>
		// void RegexAlgorithmBase<P, R>::Visit(JoinExp * expression)
		// {
		// 	result = Apply(expression, *parameterPtr);
		// }
		// template<typename P, typename R>
		// void RegexAlgorithmBase<P, R>::Visit(LoopExp * expression)
		// {
		// 	result = Apply(expression, *parameterPtr);
		// }
		// template<typename P, typename R>
		// void RegexAlgorithmBase<P, R>::Visit(CharsetExp * expression)
		// {
		// 	result = Apply(expression, *parameterPtr);
		// }		
		// template<typename P, typename R>
		// void RegexAlgorithmBase<P, R>::Visit(UsingExp * expression)
		// {
		// 	result = Apply(expression, *parameterPtr);
		// }		
		// template<typename P, typename R>
		// void RegexAlgorithmBase<P, R>::Visit(PositiveExp * expression)
		// {
		// 	result = Apply(expression, *parameterPtr);
		// }		
		// template<typename P, typename R>
		// void RegexAlgorithmBase<P, R>::Visit(NegativeExp * expression)
		// {
		// 	result = Apply(expression, *parameterPtr);
		// }
		// template<typename P, typename R>
		// void RegexAlgorithmBase<P, R>::Visit(CaptureExp * expression)
		// {
		// 	result = Apply(expression, *parameterPtr);
		// }
		// template<typename P, typename R>
		// void RegexAlgorithmBase<P, R>::Visit(BeginExp * expression)
		// {
		// 	result = Apply(expression, *parameterPtr);
		// }
		// template<typename P, typename R>
		// void RegexAlgorithmBase<P, R>::Visit(EndExp * expression)
		// {
		// 	result = Apply(expression, *parameterPtr);
		// }		

		/*-------------------------------------------------------
		  CompareExpressionAlgorithm
		-------------------------------------------------------*/
		
		bool CompareExpressionAlgorithm::Apply(OrExp * expression, Expression& parameter)
		{
			if (OrExp* p = dynamic_cast<OrExp*>(&parameter)) {
				return true;
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(JoinExp * expression, Expression& parameter)
		{
			if (JoinExp* p = dynamic_cast<JoinExp*>(&parameter)) {
				return true;
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(LoopExp * expression, Expression& parameter) {
			if (LoopExp* p = dynamic_cast<LoopExp*>(&parameter)) {
				return true;
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(CharsetExp * expression, Expression& parameter) {
			if (CharsetExp* p = dynamic_cast<CharsetExp*>(&parameter)) {
				return true;
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(UsingExp * expression, Expression& parameter) {
			if (UsingExp* p = dynamic_cast<UsingExp*>(&parameter)) {
				return true;
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(PositiveExp * expression, Expression& parameter) {
			if (PositiveExp* p = dynamic_cast<PositiveExp*>(&parameter)) {
				return true;
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(NegativeExp * expression, Expression& parameter) {
			if (NegativeExp* p = dynamic_cast<NegativeExp*>(&parameter)) {
				return true;
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(CaptureExp * expression, Expression& parameter) {
			if (CaptureExp* p = dynamic_cast<CaptureExp*>(&parameter)) {
				return true;
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(BeginExp * expression, Expression& parameter) {
			if (BeginExp* p = dynamic_cast<BeginExp*>(&parameter)) {
				return true;
			}
			return false;
		}
		bool CompareExpressionAlgorithm::Apply(EndExp * expression, Expression& parameter) {
			if (EndExp* p = dynamic_cast<EndExp*>(&parameter)) {
				return true;
			}
			return false;

		}
			

	}
}
