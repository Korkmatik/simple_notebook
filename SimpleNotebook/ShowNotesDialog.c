#include "ShowNotesDialog.h"
#include "Console.h"

#include <stdbool.h>
#include <stdlib.h>

static bool is_notebook_opened(CURRENT_NOTEBOOK *current_notebook);


void show_notes_menu(CURRENT_NOTEBOOK *current_notebook){
	if (print_all_notes(current_notebook) == -1)
		return;

	printf("\nPress [ENTER] to return to menu");
	flush_stdin();
}

int print_all_notes(CURRENT_NOTEBOOK *current_notebook) {
	if (!is_notebook_opened(current_notebook)) {
		current_notebook->fstream = fopen(current_notebook->to_text, "rb");
		if (current_notebook->fstream == NULL) {
			quit_submenu_with_error("No notebook opened yet. Please open a notebook first.", NULL, NULL, NULL);
			return -1;
		}
	}
	else {
		current_notebook->fstream = freopen(current_notebook->to_text, "rb", current_notebook->fstream);
	}

	// Buffer for storing a note
	size_t buffer_size = 4096;
	char* buffer = malloc(buffer_size * sizeof(char));
	unsigned line = 0;

	// Printing note stored so far
	printf("\nYour notes in %s:\n----------------------------------\n", current_notebook->to_text);
	while ((fgets(buffer, buffer_size, current_notebook->fstream)) != NULL)
		printf("Note #%2d: %s", ++line, buffer);

	if (0 == line)
		printf("Sorry, no notes stored so far\n");

	free(buffer);

	return line;
}

static bool is_notebook_opened(CURRENT_NOTEBOOK *current_notebook) {
	if (current_notebook->fstream == NULL)
		return false;
	else
		return true;
}
