#include "src/test.h"
#include "src/write_unity.h"

#include "vendor/cargs/include/cargs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>

#define ASSERT_CLI_CONF_REF(CONF) \
  assert((CONF) && "NULL pointer expected address of struct cli_config")

struct cli_config {
  const char *name;
};

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

char *conf_get_test_name(struct cli_config *conf)
{
  const char *error = NULL;
  char *test_name = NULL;

  ASSERT_CLI_CONF_REF(conf);

  if (!conf->name) {
    fputs("cman error: No project name provided!", stderr);
    return NULL;
  }

  size_t test_name_len = strlen(conf->name) + strlen("test_.c");
  test_name = malloc(test_name_len + 1);
  if (!test_name) {
    error = "cman error: failed to allocate space for test name!";
    goto FAIL;
  }

  int written;
  if ((written = sprintf(test_name, "test_%s.c", conf->name)) < 0) {
    error = "cman error: failed to create test name!";
    goto FAIL;
  }

  if ((size_t)written != test_name_len) {
    error = "cman error: failed to produce full test file name!";
    goto FAIL;
  }

  return test_name;

 FAIL:
  fputs(error, stderr);
  free(test_name);
  return NULL;
}

enum CMAN_CMD {
  CM_OK, CM_HELP, CM_ERR
};

void usage()
{
  fputs("Usage: cman [OPTION]...\n", stderr);
  fputs("Start sane C projects with ease!", stderr);
  cag_option_print(options, CAG_ARRAY_SIZE(options), stderr);
}

enum CMAN_CMD conf_init(struct cli_config *conf, int argc, char **argv)
{
  ASSERT_CLI_CONF_REF(conf);

  char ident;
  cag_option_context ctx;

  // Iterate through the options.
  cag_option_prepare(&ctx, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&ctx)) {
    ident = cag_option_get(&ctx);
    switch (ident) {
    case 'n':
      conf->name = cag_option_get_value(&ctx);
      break;
    case 'h':
      return CM_HELP;
    }
  }
  return CM_OK;
}

int main(int argc, char **argv)
{
  struct cli_config cli_conf = { NULL };
  switch (conf_init(&cli_conf, argc, argv)) {
  case CM_OK:
    break;
  case CM_ERR:
    fputs("Unexpected error processing command line arguments", stderr);
    return EXIT_FAILURE;
  case CM_HELP:
    usage();
    return EXIT_SUCCESS;
  }

  if (!cli_conf.name) {
    fputs("cman error: No project name provided!", stderr);
    return EXIT_FAILURE;
  }

  const char *testdir = mk_testdir(cli_conf.name);
  if (!testdir) {
    return EXIT_FAILURE;
  }

  char *test_name = conf_get_test_name(&cli_conf);

  fprintf(stderr, "Writing test file '%s/%s'...\n", testdir, test_name);

  if (write_test(testdir, test_name, cli_conf.name)) {
    fputs("Aborting ...", stderr);
    return EXIT_FAILURE;
  }

  fprintf(stderr, "Writing unity test files...\n");
  if (write_unity_files(testdir)) {
    fputs("Aborting ...", stderr);
    return EXIT_FAILURE;
  }

  fputs("Done!", stderr);

  free(test_name);
  return EXIT_SUCCESS;
}
