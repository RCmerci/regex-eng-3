#ifndef REGEX_AUTOMATON
#define REGEX_AUTOMATON

namespace regex
{
	namespace regex_internal
	{
		class Automaton
		{
		public:
			typedef Automaton* ref;

			Automaton();
			ref EpsilonNfaToNfa();
			ref NfaToDfa();
		};
	}
}

#endif
