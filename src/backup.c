#include "lib.h"
#include <dirent.h>
#include <regex.h>

int check_path_type(const char* path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return path_type_unknown;
    }
    if (S_ISDIR(path_stat.st_mode)) {
      return path_type_directory;
    }
    return path_type_file;
}

int follows_ignore_rules(char* path, char* ignore_line) {
  char lastbitofthepath[PATH_MAX];
  int k = 0;
  for (int i = 0; path[i] != 0; i++) {
    if (path[i] == '/') {
      lastbitofthepath[k] = 0;
      k = 0;
      continue;
    }
    lastbitofthepath[k++] = path[i];
  }
  if (k != 0 && lastbitofthepath[k] != 0) {
    lastbitofthepath[k] = 0;
  }

  regex_t re;
  int ret;

  if (regcomp(&re, ignore_line, REG_EXTENDED) != 0) {
      return 0;
  }

  ret = regexec(&re, lastbitofthepath, (size_t) 0, NULL, 0);
  regfree(&re);

  if (ret == 0) {
      return 1;
  } else {
      return 0;
  }
}

int should_ignore(char* path, char* ignore_instructions, int VERBOSE) {
  int i = 0, k = 0;
  char line[PATH_MAX];
  while (ignore_instructions[i] != 0) {
    if (ignore_instructions[i] != '\n') {
      line[k++] = ignore_instructions[i];
      i++;
      continue;
    }
    line[k] = 0;
    k = 0;
    if (!follows_ignore_rules(path, line)) {
      i++;
      continue;
    }
    if (VERBOSE) {
      printf("Ignoring %s due to ignore rule %s\n", path, line);
    }
    return 1;
  }
  return 0;
}

void copy_to_backup(char* source_path, int VERBOSE) {
  char destination_path[PATH_MAX];
  int index_of_first_slash = -1;
  for (int i = 0; i < PATH_MAX; i++) {
    if (source_path[i] != '/') continue;
    index_of_first_slash = i;
    break;
  }

  if (index_of_first_slash == -1) {
    // I DON'T EVEN KNOW HOW TO HANDLE THIS
    fprintf(stderr, "Weird path issue encountered, failed to copy to source\nPath encountered: %s", source_path);
    return;
  }

  int i = 0;
  for (; i < index_of_first_slash; i++) {
    destination_path[i] = source_path[i];
  }
  destination_path[i++] = '.';
  destination_path[i++] = 'b';
  destination_path[i] = 'k';
  while (++i < PATH_MAX) {
    destination_path[i] = source_path[i - 3];
  }

  if (VERBOSE) {
    printf("%s -> %s\n", source_path, destination_path);
  }

  verify_parent_dirs_exist(destination_path, VERBOSE);

  char command[2 * PATH_MAX + 10];
  snprintf(command, 2 * PATH_MAX + 10, "cp \"%s\" \"%s\"", source_path, destination_path);
  system(command);
  if (VERBOSE) {
    printf("Executing: %s\n", command);
  }
}

void push(char* directories, int cursor, const char* to_add) {
  int i = 0;
  for (; to_add[i] != 0; i++) {
    directories[cursor * PATH_MAX + i] = to_add[i];
  }
  while (i < PATH_MAX) {
    directories[cursor * PATH_MAX + i++] = 0;
  }
}

void backup(char* target, char* ignore_instructions, int VERBOSE) {
  int file_type = check_path_type(target);
  if (file_type != path_type_directory) {
    fprintf(stderr, "Input not a directory\n");
    return;
  }

  int cursor = 0;
  char* directories = malloc(PATH_MAX * PATH_MAX * sizeof(*directories));
  push(directories, cursor, target);
  while (cursor != -1) {
    char* path = &directories[(cursor--) * PATH_MAX];
    DIR* dir_ptr = opendir(path);
    if (dir_ptr == NULL) {
      fprintf(stderr, "Couldn't open the directory: %s\n", path);
      continue;
    }

    struct dirent* entry_ptr;
    char full_path[PATH_MAX];
    int i = 0;
    for (; path[i] != 0; i++) {
      full_path[i] = path[i];
    }
    full_path[i++] = '/';
    while ((entry_ptr = readdir(dir_ptr)) != NULL) {
      if (
        strcmp(entry_ptr->d_name, ".") == 0 ||
        strcmp(entry_ptr->d_name, "..") == 0
      ) continue;
      int j = 0;
      for (; entry_ptr->d_name[j] != 0; j++) {
        full_path[i + j] = entry_ptr->d_name[j];
      }
      full_path[i + j] = 0;

      if (VERBOSE) {
        printf("Looking at: %s\n", full_path);
      }
      if (should_ignore(full_path, ignore_instructions, VERBOSE)) {
        continue;
      }

      if (entry_ptr->d_type == DT_REG) {
        copy_to_backup(full_path, VERBOSE);
        continue;
      }
      push(directories, ++cursor, full_path);
    }
    closedir(dir_ptr);
  }
  free(directories);
}
