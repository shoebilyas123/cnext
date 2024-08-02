#include <stdio.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

char *file_list [4096];

int check_invalid_file(char *fname) {
    if(strcmp(fname, ".")==0 || strcmp(fname, "..")==0) {
        return 1;
    }
    return 0;
}

int get_filelist(int *fl_index) {
    DIR *dir;
    struct dirent *entry;

    // Open the current directory
    dir = opendir("./html");

    if (dir == NULL) {
        perror("Could not open directory");
        return 1;
    }
    // Read each directory entry
    while ((entry = readdir(dir)) != NULL) {
        if(check_invalid_file(entry->d_name)) {continue;}
        file_list[*fl_index] = entry->d_name;
        (*fl_index)++;
    }


    // Close the directory
    closedir(dir);
    return 0;
}

int main() {


    int fl_index = 0;
    if(get_filelist(&fl_index) != 0) {
        return 1;
    }

    printf("::HTML Files are::\n");
    for(int i=0;i<fl_index;i++) {
        printf("%s\n", file_list[i]);
    }


    return 0;
}
