#include "user_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOTEBOOKS_LIST "notebooks_list"

static void show_menu();
static void handle_user_input();
static void flush_stdin();
static void create_new_notebook();
static void quit_creating_new_notebook(char* error_string, char* buffer, char* check_buffer, FILE** fstream);
static void show_all_notebooks();
static void open_notebook();
static void quit_show_all_notebooks(char* error_string, FILE** fstream, char* buffer);
static void show_all_entries();
static void create_new_entry();
static void quit();

// The menu

static enum order_menu_entries {
	NEW_NOTEBOOK = 1, SHOW_NOTEBOOKS, OPEN_NOTEBOOK, SHOW_ALL_ENTRIES, CREATE_NEW_ENTRY, QUIT, NUMBER_MENU_ENTRIES
};

static char* menu_entries[] = { "Create a new notebook",
		"Show all notebooks", "Open an existing notebook", "Show all entries in current notebook",
		"Create new entry", "Quit" };

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
	for (int menu_entry = 0; menu_entry < NUMBER_MENU_ENTRIES - 1; ++menu_entry) {
		printf("%d - \t%s\n", menu_entry + 1, menu_entries[menu_entry]);
	}

	printf("\nYour choice: ");
}

static void handle_user_input() {
	char user_choice;
	scanf("%c", &user_choice);

	//flush_stdin();

	switch (user_choice - '0') {
	case NEW_NOTEBOOK:
		create_new_notebook();
		break;
	case SHOW_NOTEBOOKS:
		show_all_notebooks();
		break;
	case OPEN_NOTEBOOK:
		open_notebook();
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
		quit_creating_new_notebook("Error: Couldn't allocate memory for buffer!", buffer, NULL, NULL);
		return;
	}

	strcpy(buffer, "");
	flush_stdin();

	// Get the notebook name from the user
	printf("Enter notebook name: ");
	char* retVal = fgets(buffer, buffer_size, stdin);
	if (retVal == NULL) {
		quit_creating_new_notebook("Error: Couldn't get input!", buffer, NULL, NULL);
		return;
	}

	// Delete new line character from notebook name which will be in it after the user has clicked [ENTER] to submit the input
	int new_line_pos = strlen(buffer) - 1;
	buffer[new_line_pos] = '\0';


	// Checking if user entered anything for the notebook name
	if (strcmp(buffer, "") == 0) {
		quit_creating_new_notebook("Error: Notebook name must not be empty.", buffer, NULL, NULL);
		return;
	}

	// closing current opened notebook
	if (current_notebook.fstream != NULL) {
		if (fclose(current_notebook.fstream) != 0) {
			quit_creating_new_notebook("Error: Couldn't close file stream of the current opened notebook.", buffer, NULL, NULL);
			return;
		}
	}

	// check if the given notebook name already exists
	FILE* f_notebooks = fopen(NOTEBOOKS_LIST, "rb");
	if (f_notebooks != NULL) {
		size_t check_buffer_size = 1024;
		char* check_buffer = malloc(check_buffer_size);
		int strlen_check_buffer = 0;
		while((getline(&check_buffer, &check_buffer_size, f_notebooks)) != -1) {
			// New line character has to be removed because in the text file the notebook name is stored with a new line
			strlen_check_buffer = strlen(check_buffer);
			check_buffer[strlen_check_buffer - 1] = '\0';
			if (strcmp(buffer, check_buffer) == 0) {
				quit_creating_new_notebook("Error: Notebook already exists!", buffer, check_buffer, &f_notebooks);
				return;
			}
		}
			free(check_buffer);
			fclose(f_notebooks);
	}

	f_notebooks = fopen(NOTEBOOKS_LIST, "ab");

	if (f_notebooks == NULL) {
		quit_creating_new_notebook("Error while opening notebooks_list!", buffer, NULL, NULL);
		return;
	}

	// Store the notebook name in a file
	fprintf(f_notebooks, "%s\n", buffer);

	// change current notebook to the new notebook
	strcpy(current_notebook.to_text, buffer);
	fclose(f_notebooks);

	// creating a new notebook
	current_notebook.fstream = fopen(buffer, "w");
	free(buffer);

	// Finishing the process with a success message
	printf(
			"SUCCESS: Notebook %s created!\nPress [ENTER] to return to menu ...\n",
			current_notebook.to_text);
	getchar();
}

static void quit_creating_new_notebook(char* error_string, char* buffer, char* check_buffer, FILE** fstream) {
	fprintf(stderr, "%s\nPress [ENTER] to return to menu.\n\n", error_string);
	if (buffer != NULL)
		free(buffer);
	if (check_buffer != NULL)
		free(check_buffer);
	if (fstream != NULL)
		fclose(*fstream);
	getchar();
}

static void show_all_notebooks() {
	// Opens the file where the notebooks are stored
	FILE* f_notebooks = fopen(NOTEBOOKS_LIST, "rb");
	if(f_notebooks == NULL) {
		quit_show_all_notebooks("Could not open 'notebooks list'", &f_notebooks, NULL);
		return;
	}

	/* Reading and printing all notebooks*/

	// Allocating memory to store notebook names
	size_t buffer_size = 2048;
	char* buffer = malloc(buffer_size);
	if(buffer == NULL) {
		quit_show_all_notebooks("Could not allocate memory for buffer.", &f_notebooks, NULL);
		return;
	}

	// Read notebook names and print them on console
	int line = 0;
	printf("NR    NOTEBOOK NAME\n"
		   "--------------------------------------\n");
	while((getline(&buffer, &buffer_size, f_notebooks)) != -1)
		printf("%d - %s", ++line, buffer);

	printf("\n");

	if(line <= 0)
		printf("Sorry, no notebooks stored so far.\n");

	printf("Press any key to return to menu\n");
	flush_stdin();
	getchar();
}

static void open_notebook() {

}

static void quit_show_all_notebooks(char* error_string, FILE** fstream, char* buffer) {
	fprintf(stderr, "Error: %s\nPress [ENTER] to return to menu.\n\n", error_string);
	fclose(*fstream);
	if(buffer != NULL)
		free(buffer);
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
