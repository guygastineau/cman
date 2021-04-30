#include "src/test.h"
#include "src/make.h"
#include "src/lib.h"
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
  int after_args;
};

// Command Line options parsing.
static struct cag_option options[] = {
  {
    .identifier = 'n',
    .access_letters = "nNpP",
    .access_name = "name",
    .value_name = "PROJECT_NAME",
    .description = "[Optional]\n\n"
                   "        "
                   "Provide a name for the project.\n"
                   "        "
                   "A name is required, but it may be provided\n"
                   "        "
                   "by as a bare argument following all options.\n"
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
  fputs("Usage: cman [OPTION]... <project-name>\n", stderr);
  fputs("Start sane C projects with ease!\n\n", stderr);
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
  conf->after_args = cag_option_get_index(&ctx);
  return CM_OK;
}

int mk_project_dir(const char *name)
{
  if (mkdir(name, 0777)) {
    switch (errno) {
    case EEXIST:
      break;
    default:
      return -1;
    }
  }
  return 0;
}

void post_process_args(struct cli_config *conf, int argc, char **argv)
{
  ASSERT_CLI_CONF_REF(conf);
  if (conf->after_args >= argc) {
    return;
  }
  if (!conf->name) {
    conf->name = argv[conf->after_args];
  }
  return;
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

  post_process_args(&cli_conf, argc, argv);

  if (!cli_conf.name) {
    fputs("cman error: No project name provided!", stderr);
    return EXIT_FAILURE;
  }

  if (mk_project_dir(cli_conf.name)) {
    fprintf(stderr, "Failed to create directory '%s' for new project!",
            cli_conf.name);
    return EXIT_FAILURE;
  }

  fprintf(stderr, "Writing Makefile: '%s/Makefile", cli_conf.name);
  if (write_makefile(cli_conf.name) < 0) {
    perror("Aborting cman. Failed to write Makefile");
    return EXIT_FAILURE;
  }

  const char *testdir = mk_testdir(cli_conf.name);
  if (!testdir) {
    return EXIT_FAILURE;
  }

  char *test_name = conf_get_test_name(&cli_conf);

  fprintf(stderr, "Writing test file '%s/%s'...\n", testdir, test_name);

  if (write_test(testdir, test_name, cli_conf.name)) {
    perror("Arborting test writing");
    free(test_name);
    return EXIT_FAILURE;
  }

  fprintf(stderr, "Writing unity test files...\n");
  if (write_unity_files(testdir)) {
    perror("Aborting unity writing");
    free(test_name);
    return EXIT_FAILURE;
  }

  free(test_name);

  fprintf(stderr, "Writing library stubs... \n");
  if (write_lib(cli_conf.name)) {
    perror("Aborting... error writing library stubs");
    return EXIT_FAILURE;
  }

  fputs("Done!\n", stderr);

  free(test_name);
  return EXIT_SUCCESS;
}
