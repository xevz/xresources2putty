/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <mikael@distopic.net> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Mikael Svantesson
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <X11/Xresource.h>

#include "literal_colors.h"

#define SESSION_DELIM ","

struct xres {
	const char *name;
	const char *putty_name;
	char       *value;
};

struct xres colors[] = {
	{ "color0"     , "Colour6" , NULL },
	{ "color1"     , "Colour8" , NULL },
	{ "color2"     , "Colour10", NULL },
	{ "color3"     , "Colour12", NULL },
	{ "color4"     , "Colour14", NULL },
	{ "color5"     , "Colour16", NULL },
	{ "color6"     , "Colour18", NULL },
	{ "color7"     , "Colour20", NULL },
	{ "color8"     , "Colour7" , NULL },
	{ "color9"     , "Colour9" , NULL },
	{ "color10"    , "Colour11", NULL },
	{ "color11"    , "Colour13", NULL },
	{ "color12"    , "Colour15", NULL },
	{ "color13"    , "Colour17", NULL },
	{ "color14"    , "Colour19", NULL },
	{ "color15"    , "Colour21", NULL },
	{ "foreground" , "Colour0" , NULL },
	{ "background" , "Colour2" , NULL },
	{ "cursorColor", "Colour5" , NULL }
};

#define COLORS_LENGTH (sizeof(colors) / sizeof(struct xres))

#define RED(c)   ((c >> 16) & 0xff)
#define GREEN(c) ((c >>  8) & 0xff)
#define BLUE(c)   (c        & 0xff)

void usage() {
	(void)fprintf(stderr, "xdefaults2putty -class CLASS [-sessions SESSION[,...]] FILE\n");
	(void)fprintf(stderr, "    -class       X resource class (e.g. XTerm)\n");
	(void)fprintf(stderr, "    -sessions    PuTTY sessions, if not given \"default\" is used\n");
}

int translate_color(const char *hex_color, int *red, int *green, int *blue) {
	long color;

	assert(hex_color != NULL);

	if (sscanf(hex_color + 1, "%lx", &color)) {
		*red   = RED(color);
		*green = GREEN(color);
		*blue  = BLUE(color);
	} else {
		return -1;
	}

	return 1;
}

int check_file(const char *file) {
	int size;
	char *name;
	struct stat st;

	if (stat(file, &st) == -1) {
		size = strlen(file) + 17 + 1;
		name = (char *)malloc(size);

		snprintf(name, size, "xdefaults2putty: %s", file);
		perror(name);

		free(name);

		return 0;
	}

	return 1;
}

int parse_file(const char *file, char *class) {
	int         i;
	size_t      resource_len;
	size_t      class_len;
	char        *type;
	char        buf[512];
	XrmDatabase db;
	XrmValue    value;

	assert(file != NULL);
	assert(class != NULL);

	class_len = strlen(class);

	XrmInitialize();

	db = XrmGetFileDatabase(file);

	for (i = 0; i < COLORS_LENGTH; i++) {
		resource_len = strlen(colors[i].name);

		snprintf(buf, class_len + 1 + resource_len + 1, "%s.%s", class, colors[i].name);

		if (XrmGetResource(db, buf, NULL, &type, &value) != True)
			continue;

		colors[i].value = (char *)malloc(value.size + 1);
		if (colors[i].value == NULL)
			continue;

		strncpy(colors[i].value, value.addr, value.size + 1);
	}

	XrmDestroyDatabase(db);

	return 1;
}

long find_color_by_name(const char *name) {
	int i;

	assert(name != NULL);

	for (i = 0; i < LITERAL_COLORS_LENGTH; i++)
		if (!strncmp(literal_colors[i].name, name, strlen(name)))
			return literal_colors[i].color;

	return -1;
}

void generate_registry(char **sessions, int session_count) {
	int i, index;
	long result;
	int red, green, blue;

	assert(sessions != NULL);

	printf("Windows Registry Editor Version 5.00\r\n\r\n");

	for (i = 0; i < session_count; i++) {
		printf("[HKEY_CURRENT_USER\\Software\\SimonTatham\\PuTTY\\Sessions\\%s]\r\n", sessions[i]);

		for (index = 0; index < COLORS_LENGTH; index++) {
			red = 0; green = 0; blue = 0;

			if (colors[index].value) {
				if (colors[index].value[0] == '#') {
					result = (long)translate_color(colors[index].value, &red, &green, &blue);
				} else if ((result = find_color_by_name(colors[index].value)) > -1) {
					red   = RED(result);
					green = GREEN(result);
					blue  = BLUE(result);
				}

				if (result > -1)
					printf("\"%s\"=\"%d,%d,%d\"\r\n", colors[index].putty_name, red, green, blue);
			}
		}
	}
}

int main(int argc, char *argv[]) {
	int i, j, session_count, return_code;
	char *tok = NULL;

	char *input = NULL;
	char *output = NULL;
	char *class = NULL;
	char *sessions[512];

	input = NULL;
	output = NULL;
	class = NULL;
	*sessions = NULL;

	session_count = 0;
	return_code   = 0;

	(void)argc--;
	(void)*argv++;

	if (argc < 3) {
		usage();
		return 1;
	}

	for (i = 0, j = 0; i < argc; i++) {
		if (strncmp(argv[0], "-class", 6) == 0 && strlen(argv[1]) < 512) {
			i++;

			class = (char *)malloc(strlen(argv[1]) + 1);
			memset(class, 0, strlen(argv[1]) + 1);

			strncpy(class, argv[1], strlen(argv[1]));

			argc -= 2;
			argv += 2;
		}

		if (strncmp(argv[0], "-sessions", 9) == 0 && strlen(argv[1]) < 512) {
			i++;

			tok = strtok(argv[1], SESSION_DELIM);
			while (tok != NULL) {
				sessions[j] = (char *)malloc(strlen(tok) + 1);
				memset(sessions[j], 0, strlen(tok) + 1);

				strncpy(sessions[j++], tok, strlen(tok) + 1);
				session_count++;

				tok = strtok(NULL, SESSION_DELIM);
			}

			argc -= 2;
			argv += 2;
		}
	}

	if (class == NULL) {
		fprintf(stderr, "You need to define Xresource class.\n");

		return_code = 1;
		goto cleanup;
	}

	/* If no PuTTY sessions are defined, fall back to "default" */
	if (!session_count) {
		sessions[0] = (char *)malloc(8);

		strncpy(sessions[0], "default", 8);
		session_count++;
	}

	/* Input */
	if (*argv) {
		if (!check_file(*argv)) {
			return_code = 1;
			goto cleanup;
		}

		input = (char *)malloc(strlen(*argv) + 1);

		strncpy(input, *argv, strlen(*argv) + 1);

		(void)argc--;
		(void)*argv++;
	} else {
		fprintf(stderr, "You need to define a input file.\n");

		return_code = 1;
		goto cleanup;
	}

	/* Output */
	if (*argv) {
		output = (char *)malloc(strlen(*argv) + 1);

		strncpy(output, *argv, strlen(*argv) + 1);
	}

	/* Main program */
	if (parse_file(input, class))
		generate_registry(sessions, session_count);

cleanup:
	/* Clean up */
	if (input)
		free(input);

	if (output)
		free(output);

	if (class)
		free(class);

	for (i = 0; i < session_count; i++)
		free(sessions[i]);

	for (i = 0; i < COLORS_LENGTH; i++)
		free(colors[i].value);

	return return_code;
}

