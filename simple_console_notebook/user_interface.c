#include "user_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void show_menu();
static void handle_user_input();
static void flush_stdin();
static void create_new_notebook();
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
		fprintf(stderr,
				"Error: Couldn't allocate memory for buffer!\nPress [ENTER] to return to menu.\n\n");
		flush_stdin();
		getchar();
		return;
	}

	// Get the notebook name from the user
	printf("Enter notebook name: ");
	if (!fgets(buffer, buffer_size, stdin)) {
		fprintf(stderr,
				"Error: Couldn't get input!\nPress [ENTER] to return to menu\n\n");
		free(buffer);
		flush_stdin();
		getchar();
		return;
	}

	flush_stdin();

	// closing current opened notebook
	if (current_notebook.fstream != NULL) {
		if (fclose(current_notebook.fstream) != 0) {
			fprintf(stderr,
					"Error: Couldn't close file stream of the current opened notebook\nPress [ENTER] to return to menu.\n\n");
			free(buffer);
			flush_stdin();
			getchar();
			return;
		}
	}

	// Store the notebook name in a file
	FILE* f_notebooks = fopen("notebooks_list", "a");
	if (f_notebooks == NULL) {
		fprintf(stderr,
				"Error while opening notebooks_list!\nPress [ENTER] to return to menu.\n\n");
		free(buffer);
		flush_stdin();
		getchar();
		return;
	}
	fprintf(f_notebooks, "%s\n", buffer);

	// change current notebook to the new notebook
	strcpy(current_notebook.to_text, buffer);
	fclose(f_notebooks);

	// creating a new notebook
	current_notebook.fstream = fopen(buffer, "w");
	free(buffer);

	// Finishing the process with a succes message
	printf(
			"[*]SUCCESS: Notebook %s created!\nPress [ENTER] to return to menu ...",
			current_notebook.to_text);
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
