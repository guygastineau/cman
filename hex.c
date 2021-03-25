#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

static unsigned char read_buff[1024];

size_t slurp(FILE *fh)
{
  return fread(read_buff, sizeof(unsigned char), sizeof(read_buff), fh);
}

int print_hex(unsigned char byte)
{
  return printf("0x%02X", byte);
}

int print_separator()
{
  return printf(", ");
}

// Does not return the number of chars written to stdout, but rather
// it returns the total number of bytes written as hex or the error.
int dump(size_t count)
{
  int err;
  size_t written;
  for (written = 0; written < count; ++written) {
    if ((err = print_hex(read_buff[written])) < 0) {
      return err;
    }
    if ((err = print_separator()) < 0) {
      return err;
    }
  }
  return written;
}

int error(const char *msg)
{
  fflush(stdout);
  fflush(stderr);
  fprintf(stderr,
          "hex error: %s\n"
          "=====================\n"
          "Usage: hex <filename>\n",
          msg);
  return EXIT_FAILURE;
}

int main(int argc, const char **argv)
{
  if (argc != 2) {
    return error("insufficient arguments");
  }

  const char *fname = argv[1];
  FILE *src = fopen(fname, "rb");

  if (!src) {
    return error("failed to open file");
  }

  size_t read;
  while ((read = slurp(src))) {
    // Check for error.
    if (read < sizeof(read_buff)) {
      if (ferror(src)) {
        fclose(src);
        return error("Failed reading file stream");
      }
      if (dump(read) < 0) {
        fclose(src);
        return error("Failed writing to stdout");
      }
    }
  }
  int err = ferror(src);
  fclose(src);
  if (err) {
    return error("Failed reading file stream");
  } else {
    // Add a new line to output
    puts("");
    // Flush in case running it for fun without redirection.
    fflush(stdout);
    fflush(stderr);
    fputs("========================================\n"
          "Finished converting file contents to hex\n"
          "========================================\n"
          , stderr);
  }
  return EXIT_SUCCESS;
}
