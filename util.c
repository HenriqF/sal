#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int endsWith(char* a, char* b){
    size_t la = strlen(a);
    size_t lb = strlen(b);

    if (lb > la) return 0;

    return memcmp(a+(la-lb), b, lb) == 0;
}

int startsWith(char* a, char* b){
    size_t la = strlen(a);
    size_t lb = strlen(b);

    if (lb > la) return 0;

    return memcmp(a, b, lb) == 0;
}

int readFile(FILE* f, size_t* size, char** content){
    if (!f || !size || !content) return -1;

    if (fseek(f, 0, SEEK_END) != 0) return -1;
    long pos = ftell(f);
    if (pos < 0) return -1;

    *size = (size_t)pos;
    if (fseek(f, 0, SEEK_SET) != 0) return -1;

    *content = malloc(*size + 1);
    if (!*content) return -1;

    size_t nread = fread(*content, 1, *size, f);
    if (nread != *size){ free(*content); return -1; }

    (*content)[*size] = '\0';
    return 0;
}

int writeFile(FILE* f, char* content){
    if (!f) return -1;

    if(fprintf(f, "%s", content) < 0){
        return -1;
    }

    return 0;
}