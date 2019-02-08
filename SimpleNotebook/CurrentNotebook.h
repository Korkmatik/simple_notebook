#ifndef _CURRENT_NOTEBOOK_H
#define _CURRENT_NOTEBOOK_H

#include <stdio.h>

// Stores informations about the current opened notebook
typedef struct _current_notebook {
	char to_text[2048];
	FILE* fstream;
} CURRENT_NOTEBOOK;

#endif // !_CURRENT_NOTEBOOK_H
