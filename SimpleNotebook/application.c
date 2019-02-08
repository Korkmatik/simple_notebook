#include "application.h"
#include "Notebook_specifics.h"
#include "Menu.h"
#include "NewNotebookDialog.h"
#include "ShowAllNotebooksDialog.h"
#include "OpenNotebookDialog.h"
#include "ShowNotesDialog.h"

#include "Console.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static void handle_user_input();
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
		create_new_notebook(&current_notebook);
		break;
	case SHOW_NOTEBOOKS:
		show_all_notebooks();
		break;
	case OPEN_NOTEBOOK:
		open_notebook(&current_notebook);
		break;
	case SHOW_ALL_NOTES:
		show_notes_menu(&current_notebook);
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

static void create_new_entry() {
	if (current_notebook.fstream == NULL) {
		current_notebook.fstream = fopen(current_notebook.to_text, "ab");
		if (current_notebook.fstream == NULL) {
			quit_submenu_with_error("No notebook opened yet. Please open a notebook first.", NULL, NULL, NULL);
			return;
		}
	} else {
		current_notebook.fstream = freopen(current_notebook.to_text, "ab", current_notebook.fstream);
		if (current_notebook.fstream == NULL) {
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
	if (fprintf(current_notebook.fstream, "%s", buffer) < 0) {
		quit_submenu_with_error("Could not write into notebook", buffer, NULL, NULL);
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
	int number_notes = print_all_notes(&current_notebook);
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
