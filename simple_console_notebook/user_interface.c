#include "user_interface.h"

#include <stdio.h>
#include <string.h>

// The menu
#define NUMBER_MENU_ENTRIES 4
static void show_menu();

static enum order_menu_entries {
		NEW_NOTEBOOK = 1, SHOW_NOTEBOOKS, SHOW_ALL_ENTRIES, CREATE_NEW_ENTRY
};

static char* menu_entries[NUMBER_MENU_ENTRIES] = {
		"1 - Create a new notebook",
		"2 - Show all notebooks",
		"3 - Show all entries in current notebook",
		"4 - Create new entry"
};

static char current_notebook[100] = "test";

void start_application() {
	show_menu();
}

static void show_menu() {
	printf(" Welcome to the Simple Console Notebook!\n");
	if (strcmp(current_notebook, "") != 0)
		printf(" Current Notebook: %s\n", current_notebook);
	printf("+------------------------------------------------------+\n");
	for (int menu_entry = 0; menu_entry < NUMBER_MENU_ENTRIES; ++menu_entry) {
		printf("\t%s\n", menu_entries[menu_entry]);
	}
	printf("\nYour choice: ");
}

























