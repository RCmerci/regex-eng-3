#ifndef REGEX_EXCEPTION
#define REGEX_EXCEPTION
namespace regex
{
	namespace regex_internal
	{

		class RegexException : public std::runtime_error
		{
		public:
			explicit RegexException(const std::string& s)
				:std::runtime_error(s)
				{}
		};

		class ParserException : public std::runtime_error
		{
		public:
			explicit ParserException(const std::string& s)
				:std::runtime_error(s)
				{}
		};

		class GenerateEpsilonNfaException : public std::runtime_error
		{
		public:
			explicit GenerateEpsilonNfaException(const std::string& s)
				:std::runtime_error(s)
				{}
		};

		class ExpressionAlgorithmException : public std::runtime_error
		{
		public:
			explicit ExpressionAlgorithmException(const std::string& s)
				:std::runtime_error(s)
				{}
		};

		
		
		
	}
}

#endif
