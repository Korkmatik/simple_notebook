#include "DeleteNotebookDialog.h"
#include "ShowAllNotebooksDialog.h"
#include "Console.h"
#include "Notebook_specifics.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum delete_notebook_failure_codes 
{
	SUCCESS, TEMPFILENAME_BUFFER_ALLOC_FAIL, TEMP_FILE_OPEN_FAIL, NB_LIST_OPEN_FAIL, LINE_BUFFER_ALLOC_FAIL
};
static int delete_notebook(unsigned line_to_delete, CURRENT_NOTEBOOK *current_notebook);
static int exit_delete_notebook(enum delete_notebook_failure_codes code, FILE** f1, FILE** f2, char * buffer1, char* buffer2);
static void handle_retVal(int retVal);

void delete_notebook_dialog(CURRENT_NOTEBOOK *current_notebook)
{
	print_existing_notebooks();
	
	// Getting the user's choice
	printf("Choose a notebook to delete (0 to return to menu)\nYour choice: ");
	flush_stdin();
	int user_input = getchar();
	user_input -= '0';

	if (user_input == 0)
		return;

	int retVal = delete_notebook(--user_input, current_notebook);

	handle_retVal(retVal);
}

static int delete_notebook(unsigned line_to_delete, CURRENT_NOTEBOOK *current_notebook)
{
	// Allocating memory for the name of the temporary file
	char* temp_filename = (char*)malloc(strlen(NOTEBOOKS_LIST) * sizeof(char) + 5);
	if (temp_filename == NULL)
		return exit_delete_notebook(TEMPFILENAME_BUFFER_ALLOC_FAIL, NULL, NULL, NULL, NULL);
	strcpy(temp_filename, NOTEBOOKS_LIST);
	strcat(temp_filename, ".tmp");

	FILE* temp_file = fopen(temp_filename, "wb");
	if (temp_file == NULL)
		return exit_delete_notebook(TEMP_FILE_OPEN_FAIL, NULL, NULL, temp_filename, NULL);

	FILE* f_notebooks = fopen(NOTEBOOKS_LIST, "rb");
	if (f_notebooks == NULL)
		return exit_delete_notebook(NB_LIST_OPEN_FAIL, temp_file, NULL, temp_filename, NULL);

	// Allocate memory for the buffer which will hold the notebook names
	size_t line_buffer_size = 2048;
	char* line_buffer = (char*)malloc(line_buffer_size * sizeof(char));
	if (line_buffer == NULL)
		return exit_delete_notebook(LINE_BUFFER_ALLOC_FAIL, temp_file, f_notebooks, temp_filename, NULL);
	int current_line = 0;
	char deleted_notebook_name[1024] = "";

	// Write all notebooks which won't be deleted into the temporary file
	while (fgets(line_buffer, line_buffer_size, f_notebooks) != NULL)
	{
		if (current_line != line_to_delete)
			fprintf(temp_file, "%s", line_buffer);
		else
			strcpy(deleted_notebook_name, line_buffer);
		current_line++;
	}

	// Delete new line character in the delted notebook name
	deleted_notebook_name[strlen(deleted_notebook_name) - 1] = '\0';

	// if the current opened notebook is deleted then close it
	if (strcmp(deleted_notebook_name, current_notebook->to_text) == 0) {
		fclose(current_notebook->fstream);
		current_notebook->fstream = NULL;
		strcpy(current_notebook->to_text, "");
	}

	// Delete the notebook
	remove(deleted_notebook_name);

	// Rename temp file into notebooks list file and delte the old file
	fclose(f_notebooks);
	fclose(temp_file);
	remove(NOTEBOOKS_LIST);
	rename(temp_filename, NOTEBOOKS_LIST);

	return exit_delete_notebook(SUCCESS, NULL, NULL, temp_filename, line_buffer);
}

static int exit_delete_notebook(enum delete_notebook_failure_codes code, FILE ** f1, FILE** f2, char * buffer1, char* buffer2)
{
	if (f1 != NULL)
		fclose(*f1);

	if (f2 != NULL)
		fclose(*f2);

	if (buffer1 != NULL)
		free(buffer1);

	if (buffer2 != NULL)
		free(buffer2);

	return code;
}

void handle_retVal(int retVal)
{
	switch (retVal)
	{
	case SUCCESS:
		printf("\nSUCCESS: Notebook was deleted!\nPress [ENTER] to return to menu.");
		flush_stdin();
		getchar();
		break;
	case TEMPFILENAME_BUFFER_ALLOC_FAIL:
		quit_submenu_with_error("Error: Allocating memory for temporary filename failed", NULL, NULL, NULL);
		break;
	case TEMP_FILE_OPEN_FAIL:
		quit_submenu_with_error("Error: Could not open temporary file", NULL, NULL, NULL);
		break;
	case NB_LIST_OPEN_FAIL:
		quit_submenu_with_error("Error: Could not open notebook list file", NULL, NULL, NULL);
		break;
	case LINE_BUFFER_ALLOC_FAIL:
		quit_submenu_with_error("Error: Allocating memory for line buffer failed", NULL, NULL, NULL);
		break;
	}
}

