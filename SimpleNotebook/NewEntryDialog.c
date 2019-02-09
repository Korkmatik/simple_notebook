#include "NewEntryDialog.h"
#include "Notebook_specifics.h"
#include "Console.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_new_entry(CURRENT_NOTEBOOK* current_notebook) {
	if (current_notebook->fstream == NULL) {
		current_notebook->fstream = fopen(current_notebook->to_text, "ab");
		if (current_notebook->fstream == NULL) {
			quit_submenu_with_error("No notebook opened yet. Please open a notebook first.", NULL, NULL, NULL);
			return;
		}
	}
	else {
		current_notebook->fstream = freopen(current_notebook->to_text, "ab", current_notebook->fstream);
		if (current_notebook->fstream == NULL) {
			quit_submenu_with_error("Error could not open specified file in write mode", NULL, NULL, NULL);
			return;
		}
	}

	// Buffer for storing the user's note
	int buffer_size = 4096;
	char* buffer = malloc(buffer_size);

	// Getting user's note
	flush_stdin();
	printf("Type your note. Press [ENTER] if you are finished.\n"
		"--------------------------------------------------\n\n");
	fgets(buffer, buffer_size, stdin);

	if (strlen(buffer) == 0) {
		quit_submenu_with_error("You haven't entered anything", buffer, NULL, NULL);
		return;
	}

	// Storing thshow_menue note in the notebook
	if (fprintf(current_notebook->fstream, "%s", buffer) < 0) {
		quit_submenu_with_error("Could not write into notebook", buffer, NULL, NULL);
		return;
	}
	fflush(current_notebook->fstream);
	free(buffer);

	printf("\nSUCCESS: Note was stored in %s\nPress [ENTER] to return to menu\n", current_notebook->to_text);
	getchar();
}
