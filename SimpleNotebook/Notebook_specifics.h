#pragma once

#define NOTEBOOKS_LIST "notebooks_list"

#include <stdio.h>

// Stores informations about the current opened notebook
typedef struct _current_notebook {
	char to_text[2048];
	FILE* fstream;
} CURRENT_NOTEBOOK;
