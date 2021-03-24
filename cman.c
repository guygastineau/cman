#include "src/test.h"

#include "vendor/cargs/include/cargs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Command Line options parsing.
static struct cag_option options[] = {
  {
    .identifier = 'n',
    .access_letters = "nNpP",
    .access_name = "name",
    .value_name = "PROJECT_NAME",
    .description = "[REQUIRED] Provide a name for the project."
  },
  {
    .identifier = 'h',
    .access_letters = "hH?",
    .access_name = "help",
    .value_name = NULL,
    .description = "Print cli help for cman."
  }
};

struct project_config {
  const char *name;
};

int main(int argc, char **argv)
{
  // For use with FAIL
  const char *error = NULL;
  char *test_name = NULL;

  char ident;
  cag_option_context ctx;

  struct project_config conf = { NULL };

  // Iterate through the options.
  cag_option_prepare(&ctx, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&ctx)) {
    ident = cag_option_get(&ctx);
    switch (ident) {
    case 'n':
      conf.name = cag_option_get_value(&ctx);
      break;
    case 'h':
      puts("Usage: cman [OPTION]...");
      puts("");
      puts("Start sane C projects with ease!");
      puts("");
      cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
      return EXIT_SUCCESS;
    }
  }

  if (!conf.name) {
    fputs("cman error: No project name provided!", stderr);
    return EXIT_FAILURE;
  }

  size_t test_name_len = strlen(conf.name) + strlen("test_.c");
  test_name = malloc(test_name_len + 1);
  if (!test_name) {
    error = "cman error: failed to allocate space for test name!";
    goto FAIL;
  }

  int written;
  if ((written = sprintf(test_name, "test_%s.c", conf.name)) < 0) {
    error = "cman error: failed to create test name!";
    goto FAIL;
  }

  if ((size_t)written != test_name_len) {
    error = "cman error: failed to produce full test file name!";
    goto FAIL;
  }

  printf("Writing test file '%s'...\n", test_name);
  write_test(test_name, conf.name);

  puts("Done!");

  free(test_name);
  return EXIT_SUCCESS;

 FAIL:
  fputs(error, stderr);
  free(test_name);
  return EXIT_FAILURE;
}
