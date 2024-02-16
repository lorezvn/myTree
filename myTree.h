#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

//Struct

//tree_options -> ogni campo indica una delle opzioni che possono essere indicate dall'utente
typedef struct {
    bool help;
    bool hidden_files;
    bool only_dirs;
    bool full_path;
    bool inodes;
    bool permissions;
    bool size;
    bool username;
    bool groupname;
    bool date;
    bool level_depth;
    bool reverse_order;
    bool last_modify;
    bool dirs_first;

} tree_options;

struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"inodes", no_argument, NULL, 'i'},
    {"dirsfirst", no_argument, NULL, 'F'},
    {0, 0, 0, 0}
};


//Prototipi
void blue();
void green();
void purple();
void red();
void cyan();
void reset();

void info_options(tree_options options, struct stat file_info);
bool is_image(char * ext);
bool is_compressed(char * ext);
time_t get_mtime(char * filename, char * dir_path);
void no_punctuation(char* str);

void display_help();
void display_inodes(struct stat file_info);
void display_perms(struct stat file_info);
void display_size(struct stat file_info);
void display_username(struct stat file_info);
void display_groupname(struct stat file_info);
void display_date(struct stat file_info);

int alpha_order(const void* a, const void* b);
void merge_sort(char ** arr, int l, int r, char * dir_path);
void merge(char ** arr, int l, int m, int r, char * dir_path);
void reverse(char ** files, int n);
void dirs_order(char ** files, int n, char * dir_path);

