#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#define NULL_STR (char*)0
#define DEFAULT_CONFIG_CONTENTS "^node_modules$\n^target$\n^build$\n^dist$\n^\\.next$\n^\\.output$\n^\\.vercel$\n^\\.wrangler$\n^\\.venv$\n^\\.astro$\n^\\.devenv$\n^__pycache__$"

typedef enum {
  path_type_unknown,
  path_type_directory,
  path_type_file
} path_type;

char* get_default_config();
char* shift(int* argc, char*** argv);
char* get_ignore_file(char* ignore_file, int VERBOSE);
void mkdir_if_not_exist(char* path, int VERBOSE);
void verify_parent_dirs_exist(char* file_path, int VERBOSE);

void backup(char* target, char* ignore_instructions, int VERBOSE);
