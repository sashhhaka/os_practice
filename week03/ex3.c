#include <stdio.h>
#include <string.h>

#define MAX_LENGTH_NAME 63
#define MAX_LENGTH_DATA 2048
#define MAX_PATH 1024
#define UNSIGNED_CHAR_MAX 255

struct File;
struct Directory;

struct File {
    unsigned long long id;
    char name[MAX_LENGTH_NAME + 1];
    int size;
    char data[MAX_LENGTH_DATA];
    struct Directory *directory;
};

struct Directory {
    char name[MAX_LENGTH_NAME + 1];
    struct File *files[UNSIGNED_CHAR_MAX + 1];
    struct Directory *subdirectories[UNSIGNED_CHAR_MAX + 1];
    unsigned char nf, nd;
    char path[MAX_PATH];
};


void overwrite_to_file(struct File *file, const char *str) {
    int i = 0;
    while (str[i] != '\0') {
        file->data[i] = str[i];
        i++;
    }
    file->data[i] = '\0';
    file->size = strlen(file->data) + 1;
}

void append_to_file(struct File *file, const char *str) {
    int i = 0;
    while (file->data[i] != '\0') {
        i++;
    }
    int j = 0;
    while (str[j] != '\0') {
        file->data[i] = str[j];
        i++;
        j++;
    }
    file->data[i] = '\0';
    file->size = strlen(file->data) + 1;
}


void printp_file(struct File *file) {
    printf("%s/%s\n", file->directory->path, file->name);
}

void add_file(struct File *file, struct Directory *dir) {
    if (file && dir) {
        dir->files[dir->nf] = file;
        dir->nf++;
        file->directory = dir;
    }
    dir->nf++;
}

void show_file(struct File *file) {
    printf("%s ", file->name);
}

void add_dir(struct Directory *dir1, struct Directory *dir2) {
    if (dir1 && dir2) {
        dir2->subdirectories[dir2->nd] = dir1;
        dir2->nd++;
        char tempPath[MAX_PATH];
        strcpy(tempPath, dir2->path);
        if (strcmp(dir2->path, "/")) {
            strcat(tempPath, "/");
        }
        strcat(tempPath, dir1->name);
        strcpy(dir1->path, tempPath);
    }
    dir2->nd++;
}


void show_dir(struct Directory *dir) {
    printf("\nDIRECTORY\n");
    printf("path: %s\n", dir->path);
    printf("files:\n");
    printf(" [ ");
    for (int i = 0; i < dir->nf; i++) {
        show_file(dir->files[i]);
    }
    printf("]\n");
    printf(" directories:\n");
    printf(" { ");
    for (int i = 0; i < dir->nd; i++) {
        show_dir(dir->subdirectories[i]);
    }
    printf("}\n");
}

int main() {

    struct Directory root = {"/", {NULL}, {NULL}, 0, 0, "/"};
    struct Directory home = {"home", {NULL}, {NULL}, 0, 0, "/home"};
    struct Directory bin = {"bin", {NULL}, {NULL}, 0, 0, "/bin"};
    add_dir(&home, &root);
    add_dir(&bin, &root);



    struct File bash = {1, "bash", 0, "", &bin};
    bash.size = strlen(bash.data) + 1;
    add_file(&bash, &bin);


    struct File ex3_1 = {2, "ex3_1.c", 0, "int printf(const char * format, ...);",
                         &home};
    struct File ex3_2 = {3, "ex3_2.c", 0, "//This is a comment in C language",
                         &home};
    add_file(&ex3_1, &home);
    add_file(&ex3_2, &home);

    append_to_file(&bash, "Bourne Again Shell!!");


    append_to_file(&ex3_1, "int main(){printf(\"Hello World!\")}");


    printp_file(&bash);
    printp_file(&ex3_1);
    printp_file(&ex3_2);


    return 0;
}





