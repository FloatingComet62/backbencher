#include "lib.h"
#include <stdio.h>
#include <stdlib.h>

char config[PATH_MAX] = {0};
char* get_default_config() {
  if (config[0] != 0) return config;

  const char* xdg_config_home = getenv("XDG_CONFIG_HOME");
  const char* home = getenv("HOME");

  if (home == NULL) {
    fprintf(stderr, "HOME environment variable not set, failed to determine the config path");
    return NULL;
  }

  if (xdg_config_home != NULL && xdg_config_home[0] != 0) {
    snprintf(config, PATH_MAX, "%s/backbencher/.backupignore", xdg_config_home);
  } else {
    snprintf(config, PATH_MAX, "%s/.config/backbencher/.backupignore", home);
  }

  return config;
}
