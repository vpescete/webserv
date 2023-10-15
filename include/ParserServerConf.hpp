#ifndef PARSERSERVERCONF_HPP
# define PARSERSERVERCONF_HPP

# include <iostream>

class ParserServerConf {
	private:
		
	public:
		// constructor
		ParserServerConf();
		ParserServerConf(const std::string path);
		ParserServerConf(const ParserServerConf & cpy);

		// destructor
		~ParserServerConf();

		// override operator
		ParserServerConf& operator=(const ParserServerConf & rhs);

		// setter member

		// getter member
};

#endif