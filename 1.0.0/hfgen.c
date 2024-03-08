#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////
// DEFINITIONS START HERE
////////////////////////////////////////////////////////////////////////////////////

const char *VERSION = "1.0.0";

const char *recipe 	 = "%s_%s_%s";
const char *head_format  = "#ifndef %s\n#define %s\n\n%s\n#endif /*%s*/";
const char *cpp_extern_c = "#ifdef __cplusplus\nextern \"C\" {\n#endif /*__cplusplus*/\n\n\n\n\n\n#ifdef __cplusplus\n}\n#endif /*__cplusplus*/\n";
const char *cpp_only 	 = "#ifndef __cplusplus\n#error \"This header is to only be used with C++ code.\"\n\n\n";
const char *ext[] 	 = { ".h", ".hpp", ".hh" };

constexpr size_t header_size = 256;
constexpr size_t buffer_size = 64;
constexpr size_t name_size   = 24;

typedef unsigned char flag_t;
enum { IS_CPP = (1 << 1), ALT_CPP = (1 << 2), ONLY_CPP = (1 << 3) };

static inline void setbit(flag_t *in, short what) { *in |= what; }
static inline bool   test(flag_t *in, short what) { return ((*in & what) != 0); }

typedef struct hfgen_context
{
	flag_t wants;
	char *filename, *projectname, *stem, *extension;
	char *formatted_guard;
	char *header;

} hfgen;

static hfgen *context = nullptr;

////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS START HERE
////////////////////////////////////////////////////////////////////////////////////

void make_file(hfgen *in, const char *file)
{
	if(file == nullptr || in == nullptr) return;

	// Input the entire filename given...
	strncpy(in->stem, file, name_size);
	strncpy(in->filename, file, name_size);
	// Is it a C file?
	if(!test(&in->wants, IS_CPP))
	{
		strncpy(in->extension, ext[0], 6);
	}
	// Otherwise, it's either .hh or .hpp
	else strncpy(in->extension, (test(&in->wants, ALT_CPP)) ? ext[2] : ext[1], 6);
	strncat(in->filename, in->extension, name_size);
}

void make_guard(hfgen *in)
{
	if(in == nullptr || in->projectname == nullptr ||
	   in->filename == nullptr || in->extension == nullptr)
		return;

	// header: projectname_filename_extension
	snprintf(in->formatted_guard, buffer_size, recipe, in->projectname,
							   in->stem,
							   in->extension + 1);
	return;
}

void make_header(hfgen *in)
{
	if(in == nullptr || in->formatted_guard == nullptr) return;

	// These booleans here are just to make the sprintf statements below
	// a little bit shorter
	bool cpp_guard = test(&in->wants, IS_CPP);
	bool error_guard = test(&in->wants, ONLY_CPP) && cpp_guard;

	if(cpp_guard)
	{
		snprintf(in->header, header_size, head_format,  in->formatted_guard,
								in->formatted_guard,
								error_guard ? cpp_only : "",
								in->formatted_guard);
	}
	else
	{
		snprintf(in->header, header_size, head_format,  in->formatted_guard,
								in->formatted_guard,
								cpp_extern_c,
								in->formatted_guard);
	}

	return;
}

void show_usage(void)
{
	printf("Header File GENerator (%s): pretty much what it says, by anson.\n", VERSION);
	puts("usage:\n\thfgen -h / --help");
	puts("\thfgen [-acop] filename projectname");
	printf("options:\n\t%5s\t%s\n",	"-a, --alt-cpp",	"use \".hh\" instead of \".hpp\" for a C++ header file");
        printf("\t%5s\t%s\n",		"-c, --cpp",		"generate a C++ rather than C header file");
        printf("\t%5s\t%s\n",		"-o, --only-cpp",	"inserts a \"#warning\" directive to dissuade use with C code");
        printf("\t%5s\t%s\n",		"-p, --preview",	"outputs to stdout rather than a file");
	printf("\n\"--only-cpp\" will not have any effect unless it is paired with the \"--cpp\" flag\n");
}

void free_struct(void)
{
	free(context->filename);
	free(context->projectname);
	free(context->extension);
	free(context->formatted_guard);
	free(context);
	return;
}

int main(int argc, char *argv[])
{
	if(argc < 2 || !*argv[1]) { fprintf(stderr, "%s: unknown option. try \"--help\"\n", argv[0]); exit(EXIT_FAILURE); }

	// This program is supposed to output
	// to a file, but if it doesn't get any arguments
	// or passed filenames, then it defaults to standard
	// output
	FILE *stream = nullptr;

	// Strings to be populated and copied later
	char file[name_size], project[name_size];

	// There's a good chance that this fits on the stack,
	// but I have no idea where this code will end up compiled
	// on, so it's best to be sure
	context = malloc(sizeof(struct hfgen_context));
	if(context == nullptr) { perror(argv[0]); exit(EXIT_FAILURE); }

	// Allocating memory and assigning default values
	context->wants 		 = 0x00;
	context->filename 	 = malloc(name_size);
	context->projectname 	 = malloc(name_size);
	context->stem 		 = malloc(name_size);
	context->extension 	 = malloc(6);		// Really no need for big buffer sizes
	context->formatted_guard = malloc(buffer_size);
	context->header 	 = malloc(header_size);

	// And a memory check
	if(context->filename == nullptr || context->projectname == nullptr ||
	   context->stem == nullptr 	|| context->extension == nullptr   ||
	   context->formatted_guard == nullptr || context->header == nullptr )
	{
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}

	// I don't want to clog up the end of this program with
	// free() statements so let's just make a function do it
	// for us
	if(atexit(free_struct) < 0) { perror(argv[0]); exit(EXIT_FAILURE); }

	int c;
	while(--argc > 0 && (*++argv)[0] != '\0')
	{
		if((*argv)[0] != '-')
		{
			if(file[0] != '\0')
			{
				fprintf(stderr, "%s: discarded program input -- %s\n", argv[0], *argv);
				continue;
			}

			strncpy((project[0] == '\0') ? project : file, *argv, name_size);
		}

		if((*argv)[0] == '-')
		{
			// If there's another dash, then it's a long option.
			// Move the pointer up 2 places and compare the word itself.
			if((*argv)[1] == '-')
			{
				// Using continue statements here so that the user
				// can use both single character and long options
				// simultaniously, and the loop can test both.
				if(strcmp((*argv) + 2, "help")	   == 0) { show_usage(); exit(EXIT_SUCCESS); }
				if(strcmp((*argv) + 2, "alt-cpp")  == 0) { setbit(&context->wants, ALT_CPP);  continue; }
				if(strcmp((*argv) + 2, "cpp")	   == 0) { setbit(&context->wants, IS_CPP);   continue; }
				if(strcmp((*argv) + 2, "only-cpp") == 0) { setbit(&context->wants, ONLY_CPP); continue; }
				if(strcmp((*argv) + 2, "preview")  == 0) { stream = stdout; continue; }
			}
			while((c = *++argv[0]))
			{
				// Single character option testing here.
				switch(c)
				{
					case 'h': show_usage(); exit(EXIT_SUCCESS);
					case 'a': setbit(&context->wants, ALT_CPP);  break;
					case 'c': setbit(&context->wants, IS_CPP);   break;
					case 'o': setbit(&context->wants, ONLY_CPP); break;
					case 'p': stream = stdout; break;
					// This error flag can either be set by a
					// completely unrelated character inputted,
					// or you managed to put -option instead of
					// --option.
					default : fprintf(stderr, "%s: unknown option. try \"--help\"\n", argv[0]); exit(EXIT_FAILURE);
				}

			}

			continue;
		}

	}

	strncpy(context->projectname, project, name_size);
	strncpy(context->filename, file, name_size);

	make_file(context, file);
	make_guard(context);
	make_header(context);

	// And output it
	if(stream == nullptr) stream = fopen(context->filename, "w");
	fprintf(stream, "%s\n", context->header);

	// Clean up
	if(stream != stdout) fclose(stream);
	return 0;
}

