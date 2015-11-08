#ifndef REGEX_REGEXALGORITHM
#define REGEX_REGEXALGORITHM

#include <utility>
#include <map>
#include "data.hpp"
#include "automaton.hpp"

namespace regex
{
	namespace regex_internal
	{

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
			P parameter;
			R result;

			RegexAlgorithmBase()
				:parameter(),
				 result()
			{}
			R Call(Expression * expression, P parameter);
			
			virtual R Apply(OrExp * expression, P parameter)=0;
			virtual R Apply(JoinExp * expression, P parameter)=0;
			virtual R Apply(LoopExp * expression, P parameter)=0;
			virtual R Apply(CharsetExp * expression, P parameter)=0;
			virtual R Apply(UsingExp * expression, P parameter)=0;
			virtual R Apply(PositiveExp * expression, P parameter)=0;
			virtual R Apply(NegativeExp * expression, P parameter)=0;
			virtual R Apply(CaptureExp * expression, P parameter)=0;
			virtual R Apply(BeginExp * expression, P parameter)=0;
			virtual R Apply(EndExp * expression, P parameter)=0;
			
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
		R RegexAlgorithmBase<P, R>::Call(Expression* e, P _parameter)
		{
			parameter = _parameter;
			e->Accept(this);
			return result;
		}
		
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(OrExp * expression)
		{
			result = Apply(expression, parameter);
		}
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(JoinExp * expression)
		{
			result = Apply(expression, parameter);
		}
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(LoopExp * expression)
		{
			result = Apply(expression, parameter);
		}
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(CharsetExp * expression)
		{
			result = Apply(expression, parameter);
		}		
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(UsingExp * expression)
		{
			result = Apply(expression, parameter);
		}		
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(PositiveExp * expression)
		{
			result = Apply(expression, parameter);
		}		
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(NegativeExp * expression)
		{
			result = Apply(expression, parameter);
		}
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(CaptureExp * expression)
		{
			result = Apply(expression, parameter);
		}
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(BeginExp * expression)
		{
			result = Apply(expression, parameter);
		}
		template<typename P, typename R>
		void RegexAlgorithmBase<P, R>::Visit(EndExp * expression)
		{
			result = Apply(expression, parameter);
		}		


		/*-----------------------------------------------------------------
		  GenerateEpsilonNfaAlgorithm
		 ------------------------------------------------------------------*/
		
		class GenerateEpsilonNfaAlgorithm : public RegexAlgorithmBase<PAPSS, Automaton::ref>
		{
		public:
			//			GenerateEpsilonNfaAlgorithm(){}
			Automaton::ref Apply(OrExp * expression, PAPSS parameter);
			Automaton::ref Apply(JoinExp * expression, PAPSS parameter);
			Automaton::ref Apply(LoopExp * expression, PAPSS parameter);
			Automaton::ref Apply(CharsetExp * expression, PAPSS parameter);
			Automaton::ref Apply(UsingExp * expression, PAPSS parameter);
			Automaton::ref Apply(PositiveExp * expression, PAPSS parameter);
			Automaton::ref Apply(NegativeExp * expression, PAPSS parameter);
			Automaton::ref Apply(CaptureExp * expression, PAPSS parameter);
			Automaton::ref Apply(BeginExp * expression, PAPSS parameter);
			Automaton::ref Apply(EndExp * expression, PAPSS parameter);
		};

		
		/*-----------------------------------------------------------------
		  CompareExpressionAlgorithm
		 ------------------------------------------------------------------*/
		
		class CompareExpressionAlgorithm : public RegexAlgorithmBase<Expression::ref, bool>
		{
		public:
			//	CompareExpressionAlgorithm(){}
			bool Apply(OrExp * expression, Expression::ref parameter);
			bool Apply(JoinExp * expression, Expression::ref parameter);
			bool Apply(LoopExp * expression, Expression::ref parameter);
			bool Apply(CharsetExp * expression, Expression::ref parameter);
			bool Apply(UsingExp * expression, Expression::ref parameter);
			bool Apply(PositiveExp * expression, Expression::ref parameter);
			bool Apply(NegativeExp * expression, Expression::ref parameter);
			bool Apply(CaptureExp * expression, Expression::ref parameter);
			bool Apply(BeginExp * expression, Expression::ref parameter);
			bool Apply(EndExp * expression, Expression::ref parameter);
			
		};
		
		/*-----------------------------------------------------------------
		  MergeCharsetAlgorithm
		 ------------------------------------------------------------------*/

		class MergeCharsetAlgorithm : public RegexAlgorithmBase<void*, void*>
		{
		public:
			void* Apply(OrExp * expression, void* parameter);
			void* Apply(JoinExp * expression, void* parameter);
			void* Apply(LoopExp * expression, void* parameter);
			void* Apply(CharsetExp * expression, void* parameter);
			void* Apply(UsingExp * expression, void* parameter);
			void* Apply(PositiveExp * expression, void* parameter);
			void* Apply(NegativeExp * expression, void* parameter);
			void* Apply(CaptureExp * expression, void* parameter);
			void* Apply(BeginExp * expression, void* parameter);
			void* Apply(EndExp * expression, void* parameter);
			
		};
		
		/*-----------------------------------------------------------------
		  TransferUsingExpAlgorithm
		 ------------------------------------------------------------------*/
		
		class TransferUsingExpAlgorithm
			: public RegexAlgorithmBase<std::map<std::string, Expression::ref>*, Expression::ref>
		{
		public:
			using Name2Expression = std::map<std::string, Expression::ref>*;
			
			Expression::ref Apply(OrExp * expression, Name2Expression parameter);
			Expression::ref Apply(JoinExp * expression, Name2Expression parameter);
			Expression::ref Apply(LoopExp * expression, Name2Expression parameter);
			Expression::ref Apply(CharsetExp * expression, Name2Expression parameter);
			Expression::ref Apply(UsingExp * expression, Name2Expression parameter);
			Expression::ref Apply(PositiveExp * expression, Name2Expression parameter);
			Expression::ref Apply(NegativeExp * expression, Name2Expression parameter);
			Expression::ref Apply(CaptureExp * expression, Name2Expression parameter);
			Expression::ref Apply(BeginExp * expression, Name2Expression parameter);
			Expression::ref Apply(EndExp * expression, Name2Expression parameter);
		};
		
		/*-----------------------------------------------------------------
		  RecursiveCopyExpressionAlgorithm
		 ------------------------------------------------------------------*/
		class RecursiveCopyExpressionAlgorithm
			: public RegexAlgorithmBase<void*, Expression::ref>
		{
		public:
			Expression::ref Apply(OrExp * expression, void* parameter);
			Expression::ref Apply(JoinExp * expression, void* parameter);
			Expression::ref Apply(LoopExp * expression, void* parameter);
			Expression::ref Apply(CharsetExp * expression, void* parameter);
			Expression::ref Apply(UsingExp * expression, void* parameter);
			Expression::ref Apply(PositiveExp * expression, void* parameter);
			Expression::ref Apply(NegativeExp * expression, void* parameter);
			Expression::ref Apply(CaptureExp * expression, void* parameter);
			Expression::ref Apply(BeginExp * expression, void* parameter);
			Expression::ref Apply(EndExp * expression, void* parameter);			
		};

		/*-----------------------------------------------------------------
		  DeleteExpressionAlgorithm
		 ------------------------------------------------------------------*/
		class DeleteExpressionAlgorithm : public RegexAlgorithmBase<void*, void*>
		{
		public:
			void* Apply(OrExp * expression, void* parameter);
			void* Apply(JoinExp * expression, void* parameter);
			void* Apply(LoopExp * expression, void* parameter);
			void* Apply(CharsetExp * expression, void* parameter);
			void* Apply(UsingExp * expression, void* parameter);
			void* Apply(PositiveExp * expression, void* parameter);
			void* Apply(NegativeExp * expression, void* parameter);
			void* Apply(CaptureExp * expression, void* parameter);
			void* Apply(BeginExp * expression, void* parameter);
			void* Apply(EndExp * expression, void* parameter);
		};
		


		/*-----------------------------------------------------------------
		  SetNumberOnAnonymousCaptureExpressionAlgorithm
		  给匿名捕获的表达式加上序号作为名字(1,2,3...), 
		  因为仅仅 "1" 作为名字不符合命名捕获的名字规则，
		  所以序号和所命名的名字不会冲突
		 ------------------------------------------------------------------*/
		class SetNumberOnAnonymousCaptureExpressionAlgorithm
			:public RegexAlgorithmBase<int, void*>
		{
		public:
			void* Apply(OrExp * expression, int parameter);
			void* Apply(JoinExp * expression, int parameter);
			void* Apply(LoopExp * expression, int parameter);
			void* Apply(CharsetExp * expression, int parameter);
			void* Apply(UsingExp * expression, int parameter);
			void* Apply(PositiveExp * expression, int parameter);
			void* Apply(NegativeExp * expression, int parameter);
			void* Apply(CaptureExp * expression, int parameter);
			void* Apply(BeginExp * expression, int parameter);
			void* Apply(EndExp * expression, int parameter);
		};
		
	}
}


#endif
