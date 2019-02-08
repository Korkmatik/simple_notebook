#include "ShowAllNotebooksDialog.h"
#include "Console.h"
#include "Notebook_specifics.h"

#include <stdio.h>
#include <stdlib.h>

static void quit_show_all_notebooks(char *error_string, FILE **fstream, char *buffer);

void show_all_notebooks() {
	print_existing_notebooks();

	printf("Press any key to return to menu\n");
	flush_stdin();
}

int print_existing_notebooks() {
	// Opens the file where the notebooks are stored
	FILE* f_notebooks = fopen(NOTEBOOKS_LIST, "rb");
	if (f_notebooks == NULL) {
		quit_show_all_notebooks("Could not open 'notebooks list'", &f_notebooks, NULL);
		return -1;
	}

	/* Reading and printing all notebooks*/

	// Allocating memory to store notebook names
	size_t buffer_size = 2048;
	char* buffer = malloc(buffer_size);
	if (buffer == NULL) {
		quit_show_all_notebooks("Could not allocate memory for buffer.", &f_notebooks, NULL);
		return -1;
	}

	// Read notebook names and print them on console
	int number_notebooks = 0;
	printf("\nNR    NOTEBOOK NAME\n"
		"--------------------------------------\n");
	while ((fgets(buffer, buffer_size, f_notebooks)) != NULL)
		printf("%d - %s", ++number_notebooks, buffer);

	fclose(f_notebooks);

	printf("\n");

	if (number_notebooks <= 0)
		printf("Sorry, no notebooks stored so far.\n");

	printf("\n");

	return number_notebooks;
}

static void quit_show_all_notebooks(char *error_string, FILE **fstream, char *buffer) {
	fprintf(stderr, "Error: %s\nPress [ENTER] to return to menu.\n\n",
		error_string);

	if (fstream != NULL)
		fclose(*fstream);

	if (buffer != NULL)
		free(buffer);
}
