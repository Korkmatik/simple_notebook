#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <stdio.h>

void flush_stdin();
void quit_submenu_with_error(const char* error_string, char* buffer1, char* buffer2, FILE** fstream);

#endif