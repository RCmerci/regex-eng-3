#include "regexAlgorithm.h"


namespace regex
{
	namespace regex_internal
	{
		/*-------------------------------------------------------
		  RegexAlgorithmBase
		-------------------------------------------------------*/
		template<typename ReturnType, typename ParameterType>
		ReturnType RegexAlgorithmBase::Call(Expression* e, ParameterType _parameter)
		{
			parameter = _parameter;
			e->Apply(this);
			return result;
		}
		template<typename ReturnType, typename ParameterType>
		void RegexAlgorithmBase::Visit(OrExp * expression)
		{
			Apply(expression, parameter);
		}
		template<typename ReturnType, typename ParameterType>
		void RegexAlgorithmBase::Visit(JoinExp * expression)
		{
			Apply(expression, parameter);
		}
		template<typename ReturnType, typename ParameterType>
		void RegexAlgorithmBase::Visit(LoopExp * expression)
		{
			Apply(expression, parameter);
		}
		template<typename ReturnType, typename ParameterType>
		void RegexAlgorithmBase::Visit(CharsetExp * expression)
		{
			Apply(expression, parameter);
		}		
		template<typename ReturnType, typename ParameterType>
		void RegexAlgorithmBase::Visit(UsingExp * expression)
		{
			Apply(expression, parameter);
		}		
		template<typename ReturnType, typename ParameterType>
		void RegexAlgorithmBase::Visit(PositiveExp * expression)
		{
			Apply(expression, parameter);
		}		
		template<typename ReturnType, typename ParameterType>
		void RegexAlgorithmBase::Visit(NegativeExp * expression)
		{
			Apply(expression, parameter);
		}		
		template<typename ReturnType, typename ParameterType>
		void RegexAlgorithmBase::Visit(BeginExp * expression)
		{
			Apply(expression, parameter);
		}		
		void RegexAlgorithmBase::Visit(EndExp * expression)
		{
			Apply(expression, parameter);
		}		

	}
}
