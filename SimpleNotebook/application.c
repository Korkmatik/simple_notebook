#include "application.h"

#include "Notebook_specifics.h"
#include "Menu.h"
#include "NewNotebookDialog.h"
#include "ShowAllNotebooksDialog.h"
#include "OpenNotebookDialog.h"
#include "ShowNotesDialog.h"
#include "NewEntryDialog.h"
#include "DeleteNote.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static void handle_user_input(CURRENT_NOTEBOOK *current_notebook);
static void quit(CURRENT_NOTEBOOK *current_notebook);


void start_application() {
	CURRENT_NOTEBOOK current_notebook;
	current_notebook.fstream = NULL;
	strcpy(current_notebook.to_text, "");

	while (1) {
		show_menu(&current_notebook);
		handle_user_input(&current_notebook);
	}
}

void handle_user_input(CURRENT_NOTEBOOK *current_notebook) {
	char user_choice;
	scanf("%c", &user_choice);

	switch (user_choice - '0') {
	case NEW_NOTEBOOK:
		create_new_notebook(current_notebook);
		break;
	case SHOW_NOTEBOOKS:
		show_all_notebooks();
		break;
	case OPEN_NOTEBOOK:
		open_notebook(current_notebook);
		break;
	case SHOW_ALL_NOTES:
		show_notes_menu(current_notebook);
		break;
	case CREATE_NEW_NOTE:
		create_new_entry(current_notebook);
		break;
	case DELETE_NOTE:
		delete_note(current_notebook);
		break;
	case QUIT:
		quit(current_notebook);
		break;
	default:
		printf("Sorry, no such menu entry.\n\n");
		getchar();
		break;
	}

	flush_stdin();
}

static void quit(CURRENT_NOTEBOOK* current_notebook) {
	printf("GOODBYE!\n");

	// Closing if there is an opened notebook
	if (current_notebook->fstream != NULL)
		fclose(current_notebook->fstream);

	exit(0);
}
