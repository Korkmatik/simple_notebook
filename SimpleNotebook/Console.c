#include "Console.h"

#include <stdlib.h>

void flush_stdin() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF)
		;
}

void quit_submenu_with_error(const char * error_string, char * buffer1, char * buffer2, FILE ** fstream) {
	fprintf(stderr, "\n%s\nPress [ENTER] to return to menu.\n\n", error_string);

	if (buffer1 != NULL)
		free(buffer1);
	if (buffer2 != NULL)
		free(buffer2);
	if (fstream != NULL)
		fclose(*fstream);

	flush_stdin();
}
