#include "make.h"

#include <stdio.h>
#include <string.h>

#define DEST_SIZE 1024

static char file[DEST_SIZE];

static const char *makefile =
  "CC = gcc\n"
  "\n"
  "TARGET = %s\n"
  "\n"
  "LFLAGS =\n"
  "\n"
  "CFLAGS  = -std=c11\n"
  "CFLAGS += -g\n"
  "CFLAGS += -Wall\n"
  "CFLAGS += -Wextra\n"
  "CFLAGS += -pedantic\n"
  "CFLAGS += -Werror\n"
  "CFLAGS += -Wmissing-declarations\n"
  "CFLAGS += -DUNITY_SUPPORT_64\n"
  "\n"
  "ASANFLAGS  = -fsanitize=address\n"
  "ASANFLAGS += -fno-common\n"
  "ASANFLAGS += -fno-omit-frame-pointer\n"
  "\n"
  "all: build test main\n"
  "      @echo \"All finished!\"\n"
  "\n"
  "test: tests.out\n"
  "	@./tests.out\n"
  "\n"
  "memcheck: test/*.c src/*.c src/*.h\n"
  "    @echo Compiling $@\n"
  "    @$(CC) $(ASANFLAGS) $(CFLAGS) src/*.c test/vendor/unity.c test/*.c -o memcheck.out $(LIBS)\n"
  "    @./memcheck.out\n"
  "    @echo \"Memory check passed\"\n"
  "\n"
  "build.all: build.lib main\n"
  "    @echo \"Building library and binary for project: $(TARGET)...\"\n"
  "\n"
  "main: main.c src/*.h src/*.c\n"
  "    $(CC) $(CFLAGS) -O3 main.c src/*.c -o $(TARGET)\n"
  "\n"
  "build.lib: src/*.h src/*.c\n"
  "    $(CC) -combine -c $(CFLAGS) src/*.c -o lib$(TARGET).o\n"
  "    $(CC) -shared -o lib$(TARGET).so lib$(TARGET).o"
  "\n"
  "tests.out: test/*.c src/*.c src/*.h\n"
  "    @echo Compiling $@\n"
  "    @$(CC) $(CFLAGS) src/*.c test/vendor/unity.c test/*.c -o tests.out $(LIBS)\n"
  "\n"
  "clean:\n"
  "    @rm -rf *.out\n"
  "\n"
  "deep.clean: clean\n"
  "    @rm -rf *.o *.a *.S\n";

int write_makefile(const char *project)
{
  if (!project) {
    return -1;
  }
  if (strlen(project) + strlen("/Makefile") + 1 >  DEST_SIZE) {
    return -1;
  }
  if (snprintf(file, DEST_SIZE, "%s/%s", project, "Makefile") < 0) {
    return -1;
  }

  FILE *dest;
  dest = fopen(file, "wb");
  if (dest == NULL) {
    fprintf(stderr, "cman error: couldn't open %s\n", file);
    return -1;
  }
  fprintf(dest, makefile, project);
  fclose(dest);
  return 0;
}
