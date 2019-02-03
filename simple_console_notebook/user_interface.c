#include "user_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void show_menu();
static void handle_user_input();
static void flush_stdin();
static void create_new_notebook();
static void quit_creating_new_notebook(char* error_string, char* buffer);
static void show_all_notebooks();
static void show_all_entries();
static void create_new_entry();
static void quit();

// The menu
#define NUMBER_MENU_ENTRIES 5

static enum order_menu_entries {
	NEW_NOTEBOOK = 1, SHOW_NOTEBOOKS, SHOW_ALL_ENTRIES, CREATE_NEW_ENTRY, QUIT
};

static char* menu_entries[NUMBER_MENU_ENTRIES] = { "1 - Create a new notebook",
		"2 - Show all notebooks", "3 - Show all entries in current notebook",
		"4 - Create new entry", "5 - Quit" };

// Storing informations about the current opened notebook
typedef struct _current_notebook {
	char to_text[2048];
	FILE* fstream;
} CURRENT_NOTEBOOK;

CURRENT_NOTEBOOK current_notebook = { };

void start_application() {
	while (1) {
		show_menu();
		handle_user_input();
	}
}

static void show_menu() {
	printf(" Welcome to the Simple Console Notebook!\n");

	if (strcmp(current_notebook.to_text, "") != 0)
		printf(" Current Notebook: %s\n", current_notebook.to_text);
	printf("+------------------------------------------------------+\n");

	// Prints the menu
	for (int menu_entry = 0; menu_entry < NUMBER_MENU_ENTRIES; ++menu_entry) {
		printf("\t%s\n", menu_entries[menu_entry]);
	}

	printf("\nYour choice: ");
}

static void handle_user_input() {
	char user_choice;
	scanf("%c", &user_choice);

	flush_stdin();

	switch (user_choice - '0') {
	case NEW_NOTEBOOK:
		create_new_notebook();
		break;
	case SHOW_NOTEBOOKS:
		show_all_notebooks();
		break;
	case SHOW_ALL_ENTRIES:
		show_all_entries();
		break;
	case CREATE_NEW_ENTRY:
		create_new_entry();
		break;
	case QUIT:
		quit();
		break;
	default:
		printf("Sorry no such menu entry.\n\n");
		getchar();
		break;
	}
}

static void flush_stdin() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF)
		;
}

static void create_new_notebook() {
	// create a buffer on the heap for storing the notebook name
	unsigned buffer_size = 2048;
	char* buffer = malloc(buffer_size);
	if (buffer == NULL) {
		quit_creating_new_notebook("Error: Couldn't allocate memory for buffer!", buffer);
		return;
	}

	// Get the notebook name from the user
	printf("Enter notebook name: ");
	if (!fgets(buffer, buffer_size, stdin)) {
		quit_creating_new_notebook("Error: Couldn't get input!", buffer);
		return;
	}

	flush_stdin();

	// Checking if user entered anything for the notebook name
	if (strcmp(buffer, "") == 0) {
		quit_creating_new_notebook("Error: Notebook name must not be empty.", buffer);
		return;
	}

	// closing current opened notebook
	if (current_notebook.fstream != NULL) {
		if (fclose(current_notebook.fstream) != 0) {
			quit_creating_new_notebook("Error: Couldn't close file stream of the current opened notebook.", buffer);
			return;
		}
	}

	// check if the given notebook name already exists
	FILE* f_notebooks = fopen("notebooks_list", "rb");
	size_t check_buffer_size = 2048;
	char* check_buffer = malloc(check_buffer_size);
	int line = 0;
	while((line = getline(&check_buffer, &check_buffer_size, f_notebooks)) != -1) {
		if (strcmp(buffer, check_buffer) == 0) {
			quit_creating_new_notebook("Error: Notebook already exists!", buffer);
			free(check_buffer);
			fclose(f_notebooks);
			return;
		}
	}
	free(check_buffer);

	f_notebooks = freopen("notebooks_list", "ab", f_notebooks);

	if (f_notebooks == NULL) {
		quit_creating_new_notebook("Error while opening notebooks_list!", buffer);
		return;
	}

	// Store the notebook name in a file
	fprintf(f_notebooks, "%s", buffer);

	// change current notebook to the new notebook
	strcpy(current_notebook.to_text, buffer);
	fclose(f_notebooks);

	// creating a new notebook
	current_notebook.fstream = fopen(buffer, "w");
	free(buffer);

	// Finishing the process with a success message
	printf(
			"[*]SUCCESS: Notebook %s created!\nPress [ENTER] to return to menu ...",
			current_notebook.to_text);
	flush_stdin();
	getchar();
}

static void quit_creating_new_notebook(char* error_string, char* buffer) {
	fprintf(stderr, "%s\nPress [ENTER] to return to menu.\n\n", error_string);
	free(buffer);
	flush_stdin();
	getchar();
}

static void show_all_notebooks() {

}

static void show_all_entries() {

}

static void create_new_entry() {

}

static void quit() {
	printf("GOODBYE!");

	// Closing if there is an opened notebook
	if (current_notebook.fstream != NULL)
		fclose(current_notebook.fstream);

	exit(0);
}
