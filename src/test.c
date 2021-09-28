#include "test.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <errno.h>

static char path[1024];
static char file[1024];

static const char *test_template =
  "#include \"vendor/unity.h\"\n"
  "#include \"../src/%s.h\"\n"
  "\n"
  "// The test header\n"
  "#include \"%s.h\"\n"
  "\n"
  "static void test_lib(void)\n"
  "{\n"
  "  TEST_ASSERT_EQUAL_STRING(\"Hello, World!\", hello());\n"
  "}\n"
  "\n"
  "int run_%s_suite(void)\n"
  "{\n"
  "   UnityBegin(\"%s suite\");\n"
  "\n"
  "   RUN_TEST(test_lib);\n"
  "\n"
  "   return UnityEnd();\n"
  "}\n";

static const char *test_main_template =
  "#include \"vendor/unity.h\"\n"
  "#include \"%s.h\"\n"
  "\n"
  "void setUp(void)\n"
  "{\n"
  "}\n"
  "\n"
  "void tearDown(void)\n"
  "{\n"
  "}\n"
  "\n"
  "int main(void)\n"
  "{\n"
  "  return run_%s_suite();\n"
  "}\n";

static const char *test_header_template =
  "#ifndef %s_H\n"
  "#define %s_H\n"
  "\n"
  "int run_%s_suite(void);\n"
  "\n"
  "#endif // %s_H\n";

static int write_test_main(const char *testdir, const char *project,
						   const char *snake_project)
{
  if (snprintf(file, 1024, "%s/main.c", testdir) < 0) {
	return -1;
  }
  FILE *dest;
  dest = fopen(file, "wb");
  if (dest == NULL) {
	fprintf(stderr, "cman error: couldn't open %s\n", file);
	return -1;
  }
  fprintf(dest, test_main_template, project, snake_project);
  fclose(dest);
  return 0;
}

static int write_test_header(const char *testdir, const char *project,
							 const char *snake_project)
{
  if (snprintf(file, 1024, "%s/%s.h", testdir, project) < 0) {
	return -1;
  }
  char test_project[1024];
  if (snprintf(test_project, 1024, "%s_test", snake_project) < 0) {
	return -1;
  }
  char *test_header = to_header_name(test_project);
  FILE *dest;
  dest = fopen(file, "wb");
  if (dest == NULL) {
	free(test_header);
	fprintf(stderr, "cman error: couldn't open %s\n", file);
	return -1;
  }
  fprintf(dest, test_header_template, test_header, test_header,
		  snake_project, test_header);
  fclose(dest);
  return 0;
}

static int write_test_impl(const char *testdir, const char *project,
						   const char *snake_project)
{
  if (snprintf(file, 1024, "%s/%s.c", testdir, project) < 0) {
    return -1;
  }

  FILE *dest;
  dest = fopen(file, "wb");
  if (dest == NULL) {
    fprintf(stderr, "cman error: couldn't open %s\n", file);
    return -1;
  }
  fprintf(dest, test_template, project, project, snake_project, project);
  fclose(dest);
  return 0;
}

int write_tests(const char *testdir, const char *project)
{
  char *err = "err";
  char *snake_project = string_snakify(project);
  if (write_test_main(testdir, project, snake_project) < 0) {
	err ="failed writing main test file";
	goto fail;;
  }
  if (write_test_header(testdir, project, snake_project) != 0) {
	err = "failed writing stub test header file";
    goto fail;
  }
  if (write_test_impl(testdir, project, snake_project) != 0) {
	err ="failed writing stub test implementation file";
	goto fail;
  }
  return 0;

 fail:
  free(snake_project);
  fprintf(stderr, "cman error: %s!\n", err);
  return -1;
}

const char *mk_testdir(const char *root)
{
  if (snprintf(path, 1024, "%s/%s", root, "test") < 0) {
    return NULL;
  }
  if (mkdir(path, 0777)) {
    switch (errno) {
    case EEXIST:
      break;
    default:
      return NULL;
    }
  }
  return path;

}
