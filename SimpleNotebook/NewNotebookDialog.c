#include "NewNotebookDialog.h"

#include <stdlib.h>
#include <string.h>

void create_new_notebook(CURRENT_NOTEBOOK * current_notebook) {
	// create a buffer on the heap for storing the notebook name
	unsigned buffer_size = 2048;
	char* buffer = (char*)malloc(buffer_size);
	if (buffer == NULL) {
		quit_submenu_with_error("Error: Couldn't allocate memory for buffer!", buffer, NULL, NULL);
		return;
	}

	strcpy(buffer, "");
	flush_stdin();

	// Get the notebook name from the user
	printf("\nEnter notebook name: ");
	char* retVal = fgets(buffer, buffer_size, stdin);
	if (retVal == NULL) {
		quit_submenu_with_error("Error: Couldn't get input!", buffer, NULL, NULL);
		return;
	}

	// Delete new line character from notebook name which will be in it after the user has clicked [ENTER] to submit the input
	int new_line_pos = strlen(buffer) - 1;
	buffer[new_line_pos] = '\0';

	// Checking if user entered anything for the notebook name
	if (strcmp(buffer, "") == 0) {
		quit_submenu_with_error("Error: Notebook name must not be empty.",
			buffer, NULL, NULL);
		return;
	}

	// closing current opened notebook
	if (current_notebook->fstream != NULL) {
		if (fclose(current_notebook->fstream) != 0) {
			quit_submenu_with_error("Error: Couldn't close file stream of the current opened notebook.", buffer, NULL, NULL);
			return;
		}
	}

	// check if the given notebook name already exists
	FILE* f_notebooks = fopen(NOTEBOOKS_LIST, "rb");
	if (f_notebooks != NULL) {
		size_t check_buffer_size = 1024;
		char* check_buffer = (char*)malloc(check_buffer_size);
		int strlen_check_buffer = 0;
		while ((fgets(check_buffer, check_buffer_size, f_notebooks)) != NULL) {
			// New line character has to be removed because in the text file the notebook name is stored with a new line
			strlen_check_buffer = strlen(check_buffer);
			check_buffer[strlen_check_buffer - 1] = '\0';
			if (strcmp(buffer, check_buffer) == 0) {
				quit_submenu_with_error("Error: Notebook already exists!", buffer, check_buffer, &f_notebooks);
				return;
			}
		}
		free(check_buffer);
		fclose(f_notebooks);
	}

	f_notebooks = fopen(NOTEBOOKS_LIST, "ab");

	if (f_notebooks == NULL) {
		quit_submenu_with_error("Error while opening notebooks_list!", buffer, NULL, NULL);
		return;
	}

	// Store the notebook name in a file
	fprintf(f_notebooks, "%s\n", buffer);

	// change current notebook to the new notebook
	strcpy(current_notebook->to_text, buffer);
	fclose(f_notebooks);

	// creating a new notebook
	current_notebook->fstream = fopen(buffer, "w");
	free(buffer);

	// Finishing the process with a success message
	printf("\nSUCCESS: Notebook %s created!\nPress [ENTER] to return to menu ...\n", current_notebook->to_text);
}
