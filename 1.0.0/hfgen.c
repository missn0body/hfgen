#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////
// DEFINITIONS START HERE
////////////////////////////////////////////////////////////////////////////////////

const char *recipe = "%s_%s_%s\n";
const char *header_format = "#ifndef %s\n#define %s\n\n%s\n\n#endif /*%s*/";
const char *cpp_extern_c = "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n\n\n\n\n#ifdef __cplusplus\n}\n#endif\n";
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
	char *filename, *projectname, *stem, *extension;
	char *formatted_header;

} hfgen;

static hfgen *context = nullptr;

////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS START HERE
////////////////////////////////////////////////////////////////////////////////////

void make_file(hfgen *buffer, const char *input)
{
	if(input == nullptr || buffer == nullptr) return;

	// Input the entire filename given...
	strncpy(buffer->stem, input, name_size);
	strncpy(buffer->filename, input, name_size);
	// Is it a C file?
	if(!test(&buffer->wants, IS_CPP))
	{
		strncpy(buffer->extension, ext[0], name_size);
	}
	// Otherwise, it's either .hh or .hpp
	else strncpy(buffer->extension, (test(&buffer->wants, ALT_CPP)) ? ext[2] : ext[1], name_size);
	strncat(buffer->filename, buffer->extension, name_size);
}

void make_guard(hfgen *buffer)
{
	if(buffer == nullptr || buffer->projectname == nullptr ||
	   buffer->filename == nullptr || buffer->extension == nullptr)
		return;

	// header: projectname_filename_extension
	snprintf(buffer->formatted_header, buffer_size, recipe, buffer->projectname,
								buffer->stem,
								buffer->extension + 1);
	return;
}

void show_usage(void) { printf("usage.\n"); }

void free_struct(void)
{
	free(context->filename);
	free(context->projectname);
	free(context->extension);
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

	// Allocating memory and assigning default values
	context->wants = 0x00;
	context->filename = malloc(name_size);
	context->projectname = malloc(name_size);
	context->stem = malloc(name_size);
	context->extension = malloc(6);
	context->formatted_header = malloc(buffer_size);
	// And a memory check
	if(context->filename == nullptr || context->projectname == nullptr ||
	   context->stem == nullptr 	|| context->extension == nullptr   ||
	   context->formatted_header == nullptr)
	{
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}

	// I don't want to clog up the end of this program with
	// free() statements so let's just make a function do it
	// for us
	if(atexit(free_struct) < 0) { perror(argv[0]); exit(EXIT_FAILURE); }

	// Set up everything that the user wants
	setbit(&context->wants, IS_CPP);
	setbit(&context->wants, ALT_CPP);
	setbit(&context->wants, ONLY_CPP);

	strncpy(context->projectname, project, name_size);
	strncpy(context->filename, file, name_size);

	make_file(context, file);
	make_guard(context);

	// And print it for now
	fprintf(stream, "-- %s\n", context->filename);
	fprintf(stream, "-- %s\n", context->projectname);
	fprintf(stream, "-- %s\n", context->formatted_header);

	// Clean up
	if(stream != stdout) fclose(stream);
	return 0;
}
