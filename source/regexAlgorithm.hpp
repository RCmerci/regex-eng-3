#ifndef REGEX_REGEXALGORITHM
#define REGEX_REGEXALGORITHM

#include "data.hpp"

namespace regex
{
	namespace regex_internal
	{
		template<typename ReturnType, typename ParameterType>
		class RegexAlgorithmBase
		{
		public:
			ParameterType parameter;
			ReturnType    result;
			ReturnType Call(Expression * expression, ParameterType parameter);
			virtual void Apply(OrExp * expression, ParameterType parameter)=0;
			virtual void Apply(JoinExp * expression, ParameterType parameter)=0;
			virtual void Apply(LoopExp * expression, ParameterType parameter)=0;
			virtual void Apply(CharsetExp * expression, ParameterType parameter)=0;
			virtual void Apply(UsingExp * expression, ParameterType parameter)=0;
			virtual void Apply(PositiveExp * expression, ParameterType parameter)=0;
			virtual void Apply(NegativeExp * expression, ParameterType parameter)=0;
			virtual void Apply(BeginExp * expression, ParameterType parameter)=0;
			virtual void Apply(EndExp * expression, ParameterType parameter)=0;
			
			void Visit(OrExp * expression);
			void Visit(JoinExp * expression);
			void Visit(LoopExp * expression);
			void Visit(CharsetExp * expression);
			void Visit(UsingExp * expression);
			void Visit(PositiveExp * expression);
			void Visit(NegativeExp * expression);
			void Visit(BeginExp * expression); 
			void Visit(EndExp * expression);
		};


		/*-----------------------------------------------------------------
		  GenerateEpsilonNfaAlgorithm
		 ------------------------------------------------------------------*/

		class GenerateEpsilonNfaAlgorithm : public RegexAlgorithmBase
		{
		public:
			void Visit();
		};

	}
}


#endif
