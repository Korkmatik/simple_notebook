#ifndef _MENU_H
#define _MENU_H

#include "Notebook_specifics.h"

// The menu
enum order_menu_entries {
	NEW_NOTEBOOK = 1,
	SHOW_NOTEBOOKS,
	OPEN_NOTEBOOK,
	SHOW_ALL_NOTES,
	CREATE_NEW_NOTE,
	DELETE_NOTE,
	QUIT,
	NUMBER_MENU_ENTRIES
};

void show_menu(CURRENT_NOTEBOOK* current_notebook);

#endif // !_MENU_H
