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

int getFileLines(FILE* f, char*** lines, size_t* qtd_linhas){//limite de 200 linhas!!!
    size_t size;
    char* content;
    readFile(f, &size, &content);
    size_t lc = 1;

    size_t line_size[200] = {0};
    size_t ls_index = 0;

    for (size_t i = 0; i < size; i++){
        line_size[ls_index]++;
        if(content[i] == '\n') {
            lc++;
            ls_index++;
        }
    }
    (*lines) = malloc(lc*sizeof(char*));

    size_t last_index = 0;
    for (size_t i = 0; i <= ls_index; i++){
        size_t offset = 0;
        size_t a = last_index+line_size[i];

        if (a-1 >= 0 && content[a-1] == '\n') offset++;
        if (a-2 >= 0 && content[a-2] == '\r') offset++;

        char* new_line = malloc(line_size[i]-offset+1);
        memcpy(new_line, content+last_index, line_size[i]-offset);
        new_line[line_size[i]-offset] = '\0';

        last_index += line_size[i];

        (*lines)[i] = new_line;
    }

    (*qtd_linhas) = ++ls_index;

    return 0;
}