#ifndef REGEX_REGEXALGORITHM
#define REGEX_REGEXALGORITHM

#include "data.hpp"

namespace regex
{
	namespace regex_internal
	{
		// class Expression;
		// class OrExp;
		// class JoinExp;
		// class LoopExp;
		// class CharsetExp;
		// class UsingExp;
		// class PositiveExp;
		// class NegativeExp;
		// class CaptureExp;
		// class BeginExp;
		// class EndExp;



		class ExpressionVisitor
		{
		public:
			virtual void Visit(OrExp * expression)=0;
			virtual void Visit(JoinExp * expression)=0;
			virtual void Visit(LoopExp * expression)=0;
			virtual void Visit(CharsetExp * expression)=0;
			virtual void Visit(UsingExp * expression)=0;
			virtual void Visit(PositiveExp * expression)=0;
			virtual void Visit(NegativeExp * expression)=0;
			virtual void Visit(CaptureExp * expression)=0;
			virtual void Visit(BeginExp * expression)=0;
			virtual void Visit(EndExp * expression)=0;
			
		};
		
		template<typename P, typename R>
		class RegexAlgorithmBase : public ExpressionVisitor
		{
		public:
			// typedef R*   ReturnType;
			// typedef P*   ParameterType;
			
			// ParameterType parameter;
			// ReturnType    result;

			P* parameterPtr;
			R result;
			
			R Call(Expression * expression, P& parameter);
			
			virtual R Apply(OrExp * expression, P& parameter)=0;
			virtual R Apply(JoinExp * expression, P& parameter)=0;
			virtual R Apply(LoopExp * expression, P& parameter)=0;
			virtual R Apply(CharsetExp * expression, P& parameter)=0;
			virtual R Apply(UsingExp * expression, P& parameter)=0;
			virtual R Apply(PositiveExp * expression, P& parameter)=0;
			virtual R Apply(NegativeExp * expression, P& parameter)=0;
			virtual R Apply(CaptureExp * expression, P& parameter)=0;
			virtual R Apply(BeginExp * expression, P& parameter)=0;
			virtual R Apply(EndExp * expression, P& parameter)=0;
			
			void Visit(OrExp * expression);
			void Visit(JoinExp * expression);
			void Visit(LoopExp * expression);
			void Visit(CharsetExp * expression);
			void Visit(UsingExp * expression);
			void Visit(PositiveExp * expression);
			void Visit(NegativeExp * expression);
			void Visit(CaptureExp * expression);
			void Visit(BeginExp * expression);
			void Visit(EndExp * expression);

		};

				/*-------------------------------------------------------
		  RegexAlgorithmBase
		-------------------------------------------------------*/
		template<typename P, typename R>
		R RegexAlgorithmBase<P, R>::Call(Expression* e, P& _parameter)
		{
			parameterPtr = &_parameter;
			e->Accept(this);
			return result;
		}
		
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(OrExp * expression)
		{
			result = Apply(expression, *parameterPtr);
		}
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(JoinExp * expression)
		{
			result = Apply(expression, *parameterPtr);
		}
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(LoopExp * expression)
		{
			result = Apply(expression, *parameterPtr);
		}
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(CharsetExp * expression)
		{
			result = Apply(expression, *parameterPtr);
		}		
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(UsingExp * expression)
		{
			result = Apply(expression, *parameterPtr);
		}		
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(PositiveExp * expression)
		{
			result = Apply(expression, *parameterPtr);
		}		
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(NegativeExp * expression)
		{
			result = Apply(expression, *parameterPtr);
		}
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(CaptureExp * expression)
		{
			result = Apply(expression, *parameterPtr);
		}
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(BeginExp * expression)
		{
			result = Apply(expression, *parameterPtr);
		}
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(EndExp * expression)
		{
			result = Apply(expression, *parameterPtr);
		}		


		/*-----------------------------------------------------------------
		  GenerateEpsilonNfaAlgorithm
		 ------------------------------------------------------------------*/
		
		class GenerateEpsilonNfaAlgorithm : public RegexAlgorithmBase<int, int>
		{
		public:
			int Apply(OrExp * expression, int& parameter);
			int Apply(JoinExp * expression, int& parameter);
			int Apply(LoopExp * expression, int& parameter);
			int Apply(CharsetExp * expression, int& parameter);
			int Apply(UsingExp * expression, int& parameter);
			int Apply(PositiveExp * expression, int& parameter);
			int Apply(NegativeExp * expression, int& parameter);
			int Apply(CaptureExp * expression, int& parameter);
			int Apply(BeginExp * expression, int& parameter);
			int Apply(EndExp * expression, int& parameter);
		};

		class CompareExpressionAlgorithm : public RegexAlgorithmBase<Expression, bool>
		{
			bool Apply(OrExp * expression, Expression& parameter);
			bool Apply(JoinExp * expression, Expression& parameter);
			bool Apply(LoopExp * expression, Expression& parameter);
			bool Apply(CharsetExp * expression, Expression& parameter);
			bool Apply(UsingExp * expression, Expression& parameter);
			bool Apply(PositiveExp * expression, Expression& parameter);
			bool Apply(NegativeExp * expression, Expression& parameter);
			bool Apply(CaptureExp * expression, Expression& parameter);
			bool Apply(BeginExp * expression, Expression& parameter);
			bool Apply(EndExp * expression, Expression& parameter);
			
		};
	}
}


#endif
