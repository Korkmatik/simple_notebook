#include "Menu.h"

#include <stdio.h>
#include <string.h>

static char* menu_entries[] = { 
	"Create a new notebook", 
	"Show all notebooks",
	"Open an existing notebook", 
	"Show all notes in current notebook",
	"Create new note", 
	"Delete note", 
	"Quit"
};

void show_menu(CURRENT_NOTEBOOK* current_notebook) {
	printf(" Welcome to the Simple Console Notebook!\n");

	if (strcmp(current_notebook->to_text, "") != 0)
		printf(" Current Notebook: %s\n", current_notebook->to_text);
	printf("+------------------------------------------------------+\n");

	// Prints the menu
	for (int menu_entry = 0; menu_entry < NUMBER_MENU_ENTRIES - 1;
		++menu_entry) {
		printf(" %d - %s\n", menu_entry + 1, menu_entries[menu_entry]);
	}
	printf("+------------------------------------------------------+\n");

	printf("\nYour choice: ");
}
