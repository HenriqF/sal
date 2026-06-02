#ifndef WIN
#define WIN

#include <windows.h>
#include <stdio.h>
#define PROG_OS "windows"
#define MAX_PATH 260


int mkdir(const char* path);
int path_acessible(const char* path);
int is_folder(const char* path);
int copy_file(char* src, char* dst);

typedef struct DirIt {
    struct DirIt* prev;
    int status;
    char* path;

    HANDLE hFind;
    WIN32_FIND_DATA data;
    int init_call;
    int recursive;

    char* item_path;
    char* item_name;
    char* item_rel_path;

    int is_file;
    int depth;
} DirIt;

DirIt* init_dirit(char* path, int recursive);
int skip_folder(DirIt** dit);
int path_travel(DirIt** dit);

#endif