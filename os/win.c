#include "win.h"


int path_acessible(const char* path){
    return GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES;
}

int is_folder(const char* path){
    DWORD at = GetFileAttributesA(path);
    if (at == INVALID_FILE_ATTRIBUTES) return 0;
    return (at & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

int copy_file(const char* src, const char* dst){
    CopyFile(src, dst, 0);
}