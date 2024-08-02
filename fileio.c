#include <dirent.h>
#include <string.h>

int check_invalid_file(char *fname) {
    if(strcmp(fname, ".")==0 || strcmp(fname, "..")==0) {
        return 1;
    }
    return 0;
}
