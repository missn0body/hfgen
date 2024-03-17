#include <docopt/docopt.h>
#include "../lib/print.hpp"

static const char *VERSION = "Header File GENerator by anson, v.2.0.0";
static const char USAGE[] =
R"(Header File GENerator (v.2.0.0): what it says on the tin
created by anson <thesearethethingswesaw@gmail.com>

usage:
	hfgen (-h | --help)
	hfgen --version
	hfgen -p [-ao] <C++_PROJECT_NAME> <C++_FILENAME>
	hfgen -c <C_PROJECTNAME> <C_FILENAME>

options:
	-a, --alt-cpp   use ".hh" instead of ".hpp" for C++ header files
	-c		generate a C makefile
	-h, --help      show this information
	-o, --only-cpp  include a guard to dissuade usage of C++ header files with C code
	-p, --cpp  	generate a C++ makefile
	--version       prints version information

copyright (c) 2024, see LICENSE for related details
this program uses docopt for argument parsing <http://docopt.org/>)";

int main(int argc, const char** argv)
{
	std::map<std::string, docopt::value> args = docopt::docopt(USAGE, { argv + 1, argv + argc }, true, VERSION);
	for(auto const& arg : args)
	{
		Println(arg.first, " ", arg.second);
	}

	return 0;
}
