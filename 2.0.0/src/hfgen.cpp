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
	hfgen -p [-ao] [--stdout] <project_name> <filename>
	hfgen -c [--stdout] <project_name> <filename>

options:
	-a, --alt-cpp   use ".hh" instead of ".hpp" for C++ header files
	-c		generate a C header file
	-h, --help      show this information
	-o, --only-cpp  include a guard to dissuade usage of C++ header files with C code
	-p, --cpp  	generate a C++ header file
	--stdout	prints output rather than sending to file
	--version       prints version information

copyright (c) 2024, see LICENSE for related details
this program uses docopt for argument parsing <http://docopt.org/>)";

int main(int argc, char *argv[])
{
	// This program should not be called by itself
	if(argc < 2) { PrintTo(std::cerr, argv[0], ": too few arguments, try --\"help\"\n"); return -1; }

	static std::ofstream fileobj;
	static std::map<std::string, docopt::value> args;
	// docopt parsing starts here, using the alternate parse function so I can capture exceptions
	// docopt errors are uppercased, which doesn't mesh well with the constant use of lowercase in
	// the messages to the user. We still display the details of the exception thrown to us, but I
	// change it just slightly, so that it doesn't feel too much like a completely different person
	// is talking to the user
	try { args = docopt::docopt_parse(USAGE, { argv + 1, argv + argc }, true, VERSION); }
	catch(const docopt::DocoptLanguageError &e)
	{
		PrintTo(std::cerr, argv[0], ": DocoptLanguageError caught: ", ToLower(e.what()), "\n");
		return -1;
	}
	catch(const docopt::DocoptArgumentError &e)
	{
		PrintTo(std::cerr, argv[0], ": ", ToLower(e.what()), ", try --\"help\"\n");
		return -1;
	}
	// I didn't realize this immediately but docopt will throw exceptions in cases of
	// the help or version flag being recieved. To be fair, I used the alternate parsing
	// function to get all the exceptions, but it is still a bit unusual not to just print
	// the information, methinks
	catch(const docopt::DocoptExitHelp &e)    { Print(USAGE, "\n");   return -1; }
	catch(const docopt::DocoptExitVersion &e) { Print(VERSION, "\n"); return -1; }

	// Header file generating starting here
	// Took me way too long to realize that docopt::value has a method called 'asString()'
	static const std::string project = args.at("<project_name>").asString();
	static const std::string file    = args.at("<filename>").asString();
	// Simplify the booleans for readability
	static const bool isCpp    = args.at("--cpp").asBool();
	static const bool onlyCpp  = args.at("--only-cpp").asBool();
	static const bool altCpp   = args.at("--alt-cpp").asBool();
	// TODO see below: static const bool toStdout = args.at("--stdout").asBool();

	// This is a bit confusing but simply checks isCpp and altCpp to determine
	// file extension
	static const std::string ext = (isCpp) ? ((altCpp) ? "hh" : "hpp") : "h";

	// Constructing filename and opening file object if need be
	// TODO find a way to switch streams between file object and std::cout
	/*
	static const std::string filename = AsString(file, ".", ext);
	if(!toStdout) fileobj.open(filename, std::ios::out);
	if(!fileobj) { std::perror(argv[0]); return -1; }
	*/

	// This makes the project__file__ext structure that will be copied a couple times
	std::string guard = MakeGuard(project, file, ext);
	// And here is the header. It is not a MakeIfBlock call because of the way
	// that I made that function; it does not allow an easy way for multiple lines
	// of text to be inputted between the #if directive and the #endif directive
	PrintTo(std::cout, MakePreProc("ifndef"s, guard), "\n");
	PrintTo(std::cout, MakePreProc("define"s, guard), Mult("\n"s, 2));

	// Guard for protection of C++ headers against C code
	if(isCpp && onlyCpp)
	{
		PrintTo(std::cout, MakeIfBlock("__cplusplus"s, MakePreProc("error", "This header file is only to be used with C++ code"), 1, true), "\n");
	}
	// Beginning guard against linking C code with C++
	// I wasn't going to use the '-c' bool more than once, so I didn't shorten it
	else if(!isCpp || args.at("-c").asBool())
	{
		PrintTo(std::cout, MakeIfBlock("__cplusplus"s, "extern \"C\" {"s), "\n");
	}

	// This is the comment block meant for a brief abstract of the header file
	// Strongly recommended to fill out and not delete, since it makes your life
	// and the lives around you better
	PrintTo(std::cout, MakeComment("Made by anson in <YEAR>, see LICENSE for related details", true), Mult("\n"s, 4));

	// Ending guard against linking C code with C++
	if(!isCpp || args.at("-c").asBool())
	{
		PrintTo(std::cout, MakeIfBlock("__cplusplus"s, "}", 1, false));
	}

	// And finally close out the file
	PrintTo(std::cout, "\n", MakePreProc("endif"s, MakeComment(guard)), "\n");
	//TODO see above: if(fileobj.is_open()) fileobj.close();
	return 0;
}
