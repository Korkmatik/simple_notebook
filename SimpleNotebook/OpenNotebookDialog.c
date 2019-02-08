#include "OpenNotebookDialog.h"
#include "ShowAllNotebooksDialog.h"
#include "Console.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void open_notebook(CURRENT_NOTEBOOK* current_notebook) {
	// Prints all existing notebooks
	int number_notebooks = print_existing_notebooks();

	// Get user's choice
	printf("Enter the number of the Notebook: ");
	flush_stdin();
	char user_choice = getchar();
	flush_stdin();
	int number_chosen_notebook = user_choice - '0' - 1;
	if (number_chosen_notebook < 0
		|| number_chosen_notebook > number_notebooks) {
		quit_submenu_with_error("No such notebook", NULL, NULL, NULL);
		return;
	}

	FILE* f_notebooks = fopen(NOTEBOOKS_LIST, "rb");
	if (f_notebooks == NULL) {
		quit_submenu_with_error("Could not open f_notebooks", NULL, NULL, &f_notebooks);
		return;
	}

	// Reading the chosen notebook as string
	size_t buffer_size = 2048;
	char* buffer = malloc(buffer_size * sizeof(char));
	int current_line = 0;
	while (current_line <= number_chosen_notebook) {
		fgets(buffer, buffer_size, f_notebooks);
		current_line++;
	}

	// Deleting new line character from buffer
	int str_size_buffer = strlen(buffer);
	buffer[str_size_buffer - 1] = '\0';

	fclose(f_notebooks);

	// Open the user specified notebook
	if (current_notebook->fstream != NULL)
		fclose(current_notebook->fstream);

	current_notebook->fstream = fopen(buffer, "rb");
	if (current_notebook->fstream == NULL) {
		quit_submenu_with_error("Could not open the specified notebook", buffer, NULL, NULL);
		return;
	}
	strcpy(current_notebook->to_text, buffer);
	free(buffer);

	printf("\nSUCCESS: %s notebook was opened\nPress [ENTER] to return to menu\n\n", buffer);
}
