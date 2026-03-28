#include "lib.h"

int _;

void mkdir_if_not_exist(char* path, int VERBOSE) {
  if (mkdir(path, S_IRWXU) < 0) return;
  if (VERBOSE) {
    printf("(Created to create directory %s)", path);
  }
}

void verify_parent_dirs_exist(char* file_path, int VERBOSE) {
  for (int i = 0; file_path[i] != 0; i++) {
    if (file_path[i] == '/') file_path[i] = 0;
    if (file_path[i] != 0) continue;
    mkdir_if_not_exist(file_path, VERBOSE);
    file_path[i] = '/';
  }
}

char* get_ignore_file(char* ignore_file, int VERBOSE) {
  verify_parent_dirs_exist(ignore_file, VERBOSE);
  FILE* file = fopen(ignore_file, "r");
  char* default_config;

  if (file != 0) {
    float current_pointer = ftell(file);
    fseek(file, 0, SEEK_END);
    long int file_size = ftell(file);
    fseek(file, current_pointer, SEEK_SET);

    char* contents = malloc(file_size * sizeof(*contents));
    _ = fread(contents, 1, file_size, file);
    fclose(file);
    
    return contents;
  }

  if (strcmp(ignore_file, (default_config = get_default_config())) != 0) {
    return NULL;
  }

  // mainly used for first time running
  if (default_config == NULL) {
    return NULL;
  }

  file = fopen(default_config, "w");
  if (file == 0) {
    return NULL;
  }

  int content_length = strlen(DEFAULT_CONFIG_CONTENTS);
  _ = fwrite(DEFAULT_CONFIG_CONTENTS, 1, content_length, file);
  fclose(file);
  if (VERBOSE) {
    printf("Creating %s\n", default_config);
  }

  char* contents = malloc(content_length * sizeof(char));
  for (int i = 0; i < content_length; i++) {
    contents[i] = DEFAULT_CONFIG_CONTENTS[i];
  }
  return contents;
}

