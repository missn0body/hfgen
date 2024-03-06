#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////
// DEFINITIONS START HERE
////////////////////////////////////////////////////////////////////////////////////

const char *recipe = "%s_%s_%s";
const char *header_format = "#ifndef %s\n#define %s\n\n%s\n\n#endif /*%s*/";
const char *cpp_extern_c = "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n#ifdef __cplusplus\n}\n#endif\n";
const char *cpp_only = "#ifndef __cplusplus\n#error \"This header is to only be used with C++ code.\"\n";
const char *ext[] = { ".h", ".hpp", ".hh" };

constexpr size_t buffer_size = 64;
constexpr size_t name_size = 24;

typedef unsigned char flag_t;
enum { IS_CPP = (1 << 1), ALT_CPP = (1 << 2), ONLY_CPP = (1 << 3) };

static inline void setbit(flag_t *in, short what) { *in |= what; }
static inline bool   test(flag_t *in, short what) { return ((*in & what) != 0); }

typedef struct hfgen_context
{
	flag_t wants;
	char *filename, *projectname;
	char *formatted_header;

} hfgen;

static hfgen *context = nullptr;

////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS START HERE
////////////////////////////////////////////////////////////////////////////////////

void show_usage(void) { printf("usage.\n"); }

void free_struct(void)
{
	free(context->filename);
	free(context->projectname);
	free(context->formatted_header);
	free(context);
	return;
}

int main(int argc, char *argv[])
{
	// This program is supposed to output
	// to a file, but if it doesn't get any arguments
	// or passed filenames, then it defaults to standard
	// output
	FILE *stream = stdout;

	// Example strings, to be deleted
	char file[] = "error", project[] = "usgen";

	// There's a good chance that this fits on the stack,
	// but I have no idea where this code will end up compiled
	// on, so it's best to be sure
	context = malloc(sizeof(struct hfgen_context));
	if(context == nullptr) { perror(argv[0]); exit(EXIT_FAILURE); }

	context->wants = 0x00;
	context->filename = malloc(name_size);
	context->projectname = malloc(name_size);
	context->formatted_header = malloc(buffer_size);
	if(context->filename == nullptr || context->projectname == nullptr || context->formatted_header == nullptr)
	{
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}

	// I don't want to clog up the end of this program with
	// free() statements so let's just make a function do it
	// for us
	atexit(free_struct);

	puts(cpp_extern_c);
	puts(cpp_only);

	// Clean up
	if(stream != stdout) fclose(stream);
	return 0;
}
