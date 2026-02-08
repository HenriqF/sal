#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <windows.h>
#include <openssl/sha.h>
#include <stdio.h>

#define BLUE "\x1b[34m"
#define RED "\033[31m"
#define PURPLE "\x1b[35m"
#define ORANGE "\x1b[38;5;208m"
#define RESET "\x1b[0m"

int endsWith(char* a, char* b);
int startsWith(char* a, char* b);

int readFile(FILE* f, size_t* size, char** content);
int writeFile(FILE* f, char* content);

int getFileLines(FILE* f, char*** lines, size_t* qtd_linhas);

#endif