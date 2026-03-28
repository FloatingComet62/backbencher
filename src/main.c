#include "lib.h"

#define VERSION "1.0"

void help() {
  printf("Backup your files\n");
  printf("Usage: Backbencher [OPTIONS] [input]\n");
  printf("Arguments:\n");
  printf("\t[input]... The path of the folder to backup\n");
  printf("Options:\n");
  printf("\t-i --ignore-file <filename> Specify which files to exclude\n\t\t(default = ~/.config/backbencher/.backupignore)\n");
  printf("\t-h --help \tPrint help\n");
  printf("\t-v --verbose \tVerbose logging \n");
  printf("\t-V --version \tPrint version\n");
}

void assign(char* to, char* from) {
  int i = 0;
  for (; from[i] != 0; i++) {
    to[i] = from[i];
  }
  to[i] = 0;
}

int source_main(int argc, char** argv) {
  shift(&argc, &argv);

  char* ignore_file = get_default_config();
  char* target = NULL_STR;
  int VERBOSE = 0;

  while (1) {
    char* segment = shift(&argc, &argv);
    if (segment == NULL_STR) {
      break;
    }
    else if (segment[0] == '-' && segment[1] == '-') {
      if (strcmp(&segment[2], "ignore-file") == 0) {
        assign(&ignore_file[0], shift(&argc, &argv));
        if (ignore_file == NULL_STR) {
          fprintf(stderr, "Missing <filename> for --ignore-file\n");
          return 1;
        }
      }
      else if (strcmp(&segment[2], "help") == 0) {
        help();
        return 0;
      }
      else if (strcmp(&segment[2], "verbose") == 0) {
        VERBOSE = 1;
      }
      else if (strcmp(&segment[2], "version") == 0) {
        printf(VERSION);
        printf("\n");
        return 0;
      }
    } else if (segment[0] == '-') {
      if (segment[1] == 'i') {
        assign(&ignore_file[0], shift(&argc, &argv));
        if (ignore_file == NULL_STR) {
          fprintf(stderr, "Missing <filename> for --ignore-file\n");
          return 1;
        }
      }
      else if (segment[1] == 'h') {
        help();
        return 0;
      }
      else if (segment[1] == 'v') {
        VERBOSE = 1;
      }
      else if (segment[1] == 'V') {
        printf(VERSION);
        printf("\n");
        return 0;
      }
    } else {
      target = segment;
    }
  }

  if (target == NULL_STR) {
    fprintf(stderr, "Missing [input] - The folder to backup\n");
    return 1;
  }

  if (VERBOSE) {
    printf("Target folder: %s\n", target);
    printf("Using ignore file: %s\n", ignore_file);
    printf("Using verbosity: %d\n", VERBOSE);
  }

  char* ignore_instructions = get_ignore_file(ignore_file, VERBOSE);
  if (ignore_instructions == NULL) {
    fprintf(stderr, "I don't have permissions to read the ignore file\n");
    return 1;
  }
  if (VERBOSE) {
    printf("Ignoring rules:\n%s\n", ignore_instructions);
  }

  backup(target, ignore_instructions, VERBOSE);

  free(ignore_instructions);
  return 0;
}

int main(int argc, char** argv) {
  int result = source_main(argc, argv);
  if (result != 0) {
    printf("\n");
    help();
  }
  return result;
}

char* shift(int* argc, char*** argv) {
  if (argc == 0) return NULL_STR;
  --(*argc);
  char* output = **argv;
  ++(*argv);
  return output;
}
