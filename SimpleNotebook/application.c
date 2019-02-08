#include "application.h"
#include "Console.h"
#include "CurrentNotebook.h"
#include "Menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NOTEBOOKS_LIST "notebooks_list"

static void handle_user_input();
static void create_new_notebook();
static void quit_submenu_with_error(char*, char*, char*, FILE**);
static void show_all_notebooks();
static int print_existing_notebooks();
static void quit_show_all_notebooks(char*, FILE**, char*);
static void open_notebook();
static void show_notes_menu();
static int print_all_notes();
static bool is_notebook_opened();
static void create_new_entry();
static void delete_note();
static void quit();



CURRENT_NOTEBOOK current_notebook;

void start_application() {
	while (1) {
		show_menu(&current_notebook);
		handle_user_input();
	}
}

void handle_user_input() {
	char user_choice;
	scanf("%c", &user_choice);

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
	case SHOW_ALL_NOTES:
		show_notes_menu();
		break;
	case CREATE_NEW_NOTE:
		create_new_entry();
		break;
	case DELETE_NOTE:
		delete_note();
		break;
	case QUIT:
		quit();
		break;
	default:
		printf("Sorry, no such menu entry.\n\n");
		getchar();
		break;
	}

	flush_stdin();
}

static void create_new_notebook() {
	// create a buffer on the heap for storing the notebook name
	unsigned buffer_size = 2048;
	char* buffer = malloc(buffer_size);
	if (buffer == NULL) {
		quit_submenu_with_error("Error: Couldn't allocate memory for buffer!",
				buffer, NULL,
				NULL);
		return;
	}

	strcpy(buffer, "");
	flush_stdin();

	// Get the notebook name from the user
	printf("\nEnter notebook name: ");
	char* retVal = fgets(buffer, buffer_size, stdin);
	if (retVal == NULL) {
		quit_submenu_with_error("Error: Couldn't get input!", buffer, NULL,
		NULL);
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
	if (current_notebook.fstream != NULL) {
		if (fclose(current_notebook.fstream) != 0) {
			quit_submenu_with_error(
					"Error: Couldn't close file stream of the current opened notebook.",
					buffer, NULL, NULL);
			return;
		}
	}

	// check if the given notebook name already exists
	FILE* f_notebooks = fopen(NOTEBOOKS_LIST, "rb");
	if (f_notebooks != NULL) {
		size_t check_buffer_size = 1024;
		char* check_buffer = malloc(check_buffer_size);
		int strlen_check_buffer = 0;
		while ((fgets(check_buffer, check_buffer_size, f_notebooks)) != NULL) {
			// New line character has to be removed because in the text file the notebook name is stored with a new line
			strlen_check_buffer = strlen(check_buffer);
			check_buffer[strlen_check_buffer - 1] = '\0';
			if (strcmp(buffer, check_buffer) == 0) {
				quit_submenu_with_error("Error: Notebook already exists!",
						buffer, check_buffer, &f_notebooks);
				return;
			}
		}
		free(check_buffer);
		fclose(f_notebooks);
	}

	f_notebooks = fopen(NOTEBOOKS_LIST, "ab");

	if (f_notebooks == NULL) {
		quit_submenu_with_error("Error while opening notebooks_list!", buffer,
		NULL, NULL);
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
			"\nSUCCESS: Notebook %s created!\nPress [ENTER] to return to menu ...\n",
			current_notebook.to_text);
}

static void quit_submenu_with_error(char* error_string, char* buffer,
		char* check_buffer, FILE** fstream) {
	fprintf(stderr, "\n%s\nPress [ENTER] to return to menu.\n\n", error_string);

	if (buffer != NULL)
		free(buffer);
	if (check_buffer != NULL)
		free(check_buffer);
	if (fstream != NULL)
		fclose(*fstream);

	flush_stdin();
}

static void show_all_notebooks() {
	print_existing_notebooks();

	printf("Press any key to return to menu\n");
	flush_stdin();
}

static int print_existing_notebooks() {
	// Opens the file where the notebooks are stored
	FILE* f_notebooks = fopen(NOTEBOOKS_LIST, "rb");
	if (f_notebooks == NULL) {
		quit_show_all_notebooks("Could not open 'notebooks list'", &f_notebooks,
		NULL);
		return -1;
	}

	/* Reading and printing all notebooks*/

	// Allocating memory to store notebook names
	size_t buffer_size = 2048;
	char* buffer = malloc(buffer_size);
	if (buffer == NULL) {
		quit_show_all_notebooks("Could not allocate memory for buffer.",
				&f_notebooks, NULL);
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

static void quit_show_all_notebooks(char* error_string, FILE** fstream,
		char* buffer) {
	fprintf(stderr, "Error: %s\nPress [ENTER] to return to menu.\n\n",
			error_string);

	if (fstream != NULL)
		fclose(*fstream);

	if (buffer != NULL)
		free(buffer);
}

static void open_notebook() {
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
		quit_show_all_notebooks("No such notebook", NULL, NULL);
		return;
	}

	FILE* f_notebooks = fopen(NOTEBOOKS_LIST, "rb");
	if (f_notebooks == NULL) {
		quit_show_all_notebooks("Could not open f_notebooks", &f_notebooks,
		NULL);
		return;
	}

	// Reading the chosen notebook as string
	size_t buffer_size = 2048;
	char* buffer = malloc(buffer_size);
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
	if (current_notebook.fstream != NULL)
		fclose(current_notebook.fstream);

	current_notebook.fstream = fopen(buffer, "rb");
	if (current_notebook.fstream == NULL) {
		quit_show_all_notebooks("Could not open the specified notebook", NULL,
				buffer);
		return;
	}
	strcpy(current_notebook.to_text, buffer);
	free(buffer);

	printf(
			"\nSUCCESS: %s notebook was opened\nPress [ENTER] to return to menu\n\n",
			buffer);
}

static void show_notes_menu() {
	print_all_notes();

	printf("\nPress [ENTER] to return to menu");
	flush_stdin();
}

static int print_all_notes() {
	if (!is_notebook_opened()) {
		current_notebook.fstream = fopen(current_notebook.to_text, "rb");
		if (current_notebook.fstream == NULL) {
			quit_submenu_with_error(
					"No notebook opened yet. Please open a notebook first.",
					NULL, NULL, NULL);
			return -1;
		}
	} else {
		current_notebook.fstream = freopen(current_notebook.to_text, "rb",
				current_notebook.fstream);
	}

	// Buffer for storing a note
	size_t buffer_size = 4096;
	char* buffer = malloc(buffer_size);
	unsigned line = 0;

	// Printing note stored so far
	printf("\nYour notes in %s:\n"
			"----------------------------------\n", current_notebook.to_text);
	while ((fgets(buffer, buffer_size, current_notebook.fstream)) != NULL)
		printf("Note #%2d: %s", ++line, buffer);

	if (0 == line)
		printf("Sorry, no notes stored so far\n");

	free(buffer);

	return line;
}

static bool is_notebook_opened() {
	if (current_notebook.fstream == NULL)
		return false;
	else
		return true;
}

static void create_new_entry() {
	if (current_notebook.fstream == NULL) {
		current_notebook.fstream = fopen(current_notebook.to_text, "ab");
		if (current_notebook.fstream == NULL) {
			quit_submenu_with_error(
					"No notebook opened yet. Please open a notebook first.",
					NULL, NULL, NULL);
			return;
		}
	} else {
		current_notebook.fstream = freopen(current_notebook.to_text, "ab",
				current_notebook.fstream);
		if (current_notebook.fstream == NULL) {
			quit_submenu_with_error(
					"Error could not open specified file in write mode",
					NULL, NULL, NULL);
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
		quit_submenu_with_error("You haven't entered anything", buffer, NULL,
		NULL);
		return;
	}

	// Storing thshow_menue note in the notebook
	if (fprintf(current_notebook.fstream, "%s", buffer) < 0) {
		quit_submenu_with_error("Could not write into notebook", buffer, NULL,
		NULL);
		return;
	}
	fflush(current_notebook.fstream);
	free(buffer);

	printf("\nSUCCESS: Note was stored in %s\n"
			"Press [ENTER] to return to menu\n", current_notebook.to_text);

	getchar();
}

static void delete_note() {
	printf("\n");
	int number_notes = print_all_notes();
	if (number_notes <= 0)
		return;

	flush_stdin();
	printf(
			"\nWhich note do you want to delete? Enter the number of the note: ");
	char user_choice = getchar();
	int delete_line = user_choice - '0' - 1; // Transforming the char into an integer

	if (delete_line < 0 || delete_line > number_notes) {
		quit_submenu_with_error("No such note", NULL, NULL, NULL);
		return;
	}

	/* Deleting the note*/
	//Allocating memory for the name of the temporary file
	char* temp_filename = malloc(strlen(current_notebook.to_text) + 5);
	if (temp_filename == NULL) {
		quit_submenu_with_error(
				"Error: Could not allocate memory for temp_buffer", NULL, NULL,
				NULL);
		return;
	}
	strcpy(temp_filename, current_notebook.to_text);
	strcat(temp_filename, ".tmp");

	FILE* temp_file = fopen(temp_filename, "wb");
	if (temp_file == NULL) {
		quit_submenu_with_error("Error: Could not create temp_file",
				temp_filename, NULL, &temp_file);
		return;
	}

	current_notebook.fstream = freopen(current_notebook.to_text, "rb",
			current_notebook.fstream);

	if (current_notebook.fstream == NULL) {
		quit_submenu_with_error(
				"Error: Could not reopen current_notebook.fstream!",
				temp_filename, NULL, &temp_file);
		return;
	}

	// Allocate memory for the buffer which will hold the notes
	int current_line = 0;
	size_t line_buffer_size = 2048;
	char* line_buffer = malloc(line_buffer_size);
	if (line_buffer == NULL) {
		quit_submenu_with_error(
				"Error: Could not allocate memory for line_buffer",
				temp_filename, NULL, &temp_file);
		return;
	}

	// Write all notes which won't be deleted into the temporary file
	while (fgets(line_buffer, line_buffer_size, current_notebook.fstream) != NULL) {
		if (current_line != delete_line)
			fprintf(temp_file, "%s", line_buffer);
		current_line++;
	}

	// Rename temporary file into current notebook name
	fclose(current_notebook.fstream);
	fclose(temp_file);
	remove(current_notebook.to_text);
	rename(temp_filename, current_notebook.to_text);

	free(line_buffer);
	free(temp_filename);

	current_notebook.fstream = fopen(current_notebook.to_text, "rb");
	if (current_notebook.fstream == NULL) {
		quit_submenu_with_error("Error while trying to reopen current notebook",
				NULL, NULL, NULL);
		strcpy(current_notebook.to_text, "");
		return;
	}

	printf("\nSUCCESS: Note was deleted!\nPress [ENTER] to return to menu.");
	flush_stdin();
	getchar();
}

static void quit() {
	printf("GOODBYE!\n");

	// Closing if there is an opened notebook
	if (current_notebook.fstream != NULL)
		fclose(current_notebook.fstream);

	exit(0);
}
