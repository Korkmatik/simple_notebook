#include "application.h"
#include "Notebook_specifics.h"
#include "Menu.h"
#include "NewNotebookDialog.h"
#include "ShowAllNotebooksDialog.h"
#include "OpenNotebookDialog.h"
#include "ShowNotesDialog.h"
#include "NewEntryDialog.h"

#include "Console.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static void handle_user_input();

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
		create_new_entry(&current_notebook);
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
