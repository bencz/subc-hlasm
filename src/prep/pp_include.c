/*
 * SubC Compiler - Preprocessor Include Processing
 *
 * Handles #include directive.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "prep.h"

FILE *try_open_include(char *file, char *path, int pathlen) {
	int	i;
	FILE	*f;

	/* Try each include directory in order */
	for (i = 0; i < Nincdirs; i++) {
		if (strlen(Incdirs[i]) + strlen(file) + 2 < (size_t)pathlen) {
			strcpy(path, Incdirs[i]);
			strcat(path, "/");
			strcat(path, file);
			if ((f = fopen(path, "r")) != NULL)
				return f;
		}
	}

	/* Try SCCDIR/include as fallback */
	if (strlen(SCCDIR) + strlen(file) + 10 < (size_t)pathlen) {
		strcpy(path, SCCDIR);
		strcat(path, "/include/");
		strcat(path, file);
		if ((f = fopen(path, "r")) != NULL)
			return f;
	}

	return NULL;
}

void include(void) {
	char	file[TEXTLEN+1], path[TEXTLEN+1];
	int	c, k;
	FILE	*inc, *oinfile;
	char	*ofile;
	int	oc, oline;

	if ((c = skip()) == '<')
		c = '>';
	k = getln(file, TEXTLEN-strlen(SCCDIR)-9);
	Line++;
	if (!k || file[k-1] != c)
		error("missing delimiter in '#include'", NULL);
	if (k) file[k-1] = 0;
	if (c == '"') {
		/* For "file", try current directory first */
		strcpy(path, file);
		inc = fopen(path, "r");
		/* If not found, try include directories */
		if (inc == NULL)
			inc = try_open_include(file, path, TEXTLEN);
	}
	else {
		/* For <file>, search include directories */
		inc = try_open_include(file, path, TEXTLEN);
	}
	if (inc == NULL)
		error("cannot open include file: %s", file);
	else {
		Inclev++;
		oc = next();
		oline = Line;
		ofile = File;
		oinfile = Infile;
		Line = 1;
		putback('\n');
		File = path;
		Infile = inc;
		Token = scan();
		while (XEOF != Token)
			top();
		Line = oline;
		File = ofile;
		Infile = oinfile;
		fclose(inc);
		putback(oc);
		Inclev--;
	}
}
