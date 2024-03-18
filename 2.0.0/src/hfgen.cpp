#include <docopt/docopt.h>

#include "../lib/print.hpp"
#include "../lib/hfgen.hpp"

static const char *VERSION = "Header File GENerator by anson, v.2.0.0";
static const char USAGE[] =
R"(Header File GENerator (v.2.0.0): what it says on the tin
created by anson <thesearethethingswesaw@gmail.com>

usage:
	hfgen (-h | --help)
	hfgen --version
	hfgen -p [-ao] <project_name> <filename>
	hfgen -c <project_name> <filename>

options:
	-a, --alt-cpp   use ".hh" instead of ".hpp" for C++ header files
	-c		generate a C header file
	-h, --help      show this information
	-o, --only-cpp  include a guard to dissuade usage of C++ header files with C code
	-p, --cpp  	generate a C++ header file
	--version       prints version information

copyright (c) 2024, see LICENSE for related details
this program uses docopt for argument parsing <http://docopt.org/>)";

int main(int argc, char *argv[])
{
	std::map<std::string, docopt::value> args = docopt::docopt(USAGE, { argv + 1, argv + argc }, true, VERSION);

	Println(args.at("--cpp"), "\n"s);

	Print(MakeIfBlock("__cplusplus"s, MakePreProc("error", "This header file is to be used for C++ code only"), 1, true));
	Print("\n"s, MakeComment("This is a generic header file", true), Mult("\n"s, 2));
	return 0;
}
