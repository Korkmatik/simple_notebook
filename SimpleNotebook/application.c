#include "application.h"

#include "Notebook_specifics.h"
#include "Menu.h"
#include "NewNotebookDialog.h"
#include "ShowAllNotebooksDialog.h"
#include "OpenNotebookDialog.h"
#include "ShowNotesDialog.h"
#include "NewEntryDialog.h"
#include "DeleteNoteDialog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LAST_NOTEBOOK_SAVE "last_notebook"

static void load_notebook(CURRENT_NOTEBOOK *current_notebook);
static void handle_user_input(CURRENT_NOTEBOOK *current_notebook);
static void quit(CURRENT_NOTEBOOK *current_notebook);


void start_application() {
	CURRENT_NOTEBOOK current_notebook;
	load_notebook(&current_notebook);
	
	while (1) {
		show_menu(&current_notebook);
		handle_user_input(&current_notebook);
	}
}

void load_notebook(CURRENT_NOTEBOOK *current_notebook) {
	FILE* last_notebook = fopen(LAST_NOTEBOOK_SAVE, "rb");
	if (last_notebook == NULL)
		return;
	
	fread(current_notebook, sizeof(CURRENT_NOTEBOOK), 1, last_notebook);
	fclose(last_notebook);

	current_notebook->fstream = fopen(current_notebook->to_text, "rb");
	if (current_notebook->fstream == NULL) {
		strcpy(current_notebook->to_text, "");
		return;
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

	if (current_notebook->fstream != NULL) {
		FILE* last_notebook = fopen(LAST_NOTEBOOK_SAVE, "wb");
		if (last_notebook != NULL) {
			fwrite(current_notebook, sizeof(CURRENT_NOTEBOOK), 1, last_notebook);
			fclose(last_notebook);
		}		
	}

	// Closing if there is an opened notebook
	if (current_notebook->fstream != NULL)
		fclose(current_notebook->fstream);

	exit(0);
}
