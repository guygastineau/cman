#include "write_unity.h"
#include "embed_common.h"

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

#define BUFF_SIZE 1024

// Not using any locks, so this module is NOT thread safe!
static char path_buff[BUFF_SIZE];
static char file_buff[BUFF_SIZE];

// Return 0 on success and -1 on failure. Check ERRNO for specifics.
static int mkvendor(const char *vendor_path)
{
  if (mkdir(vendor_path, 0777)) {
    switch (errno) {
    case EEXIST:
      break;
    default:
      return -1;
    }
  }
  return 0;
}

static int write_file(const char *dir, const char *name,
                      size_t len, const unsigned char *bytes)
{
  if (snprintf(file_buff, BUFF_SIZE, "%s/%s", dir, name)) {
    return -1;
  }
  FILE *dest = fopen(file_buff, "wb");
  if (!dest) {
    return -1;
  }
  size_t written = fwrite(bytes, sizeof(unsigned char), len, dest);
  fclose(dest);
  if (written < len) {
    return -1;
  }
  return 0;
}

// Return 0 on success and -1 on failure. Check ERRNO for specifics.
int write_unity_files(const char *testdir)
{
  if (snprintf(path_buff, BUFF_SIZE, "%s/%s", testdir, "vendor")) {
    return -1;
  }

  if (mkvendor(path_buff)) {
    return -1;
  }

  if (write_file(path_buff, "unity.h", sizeof(unity_h_bytes), unity_h_bytes)) {
    return -1;
  }
  if (write_file(path_buff, "unity.c", sizeof(unity_c_bytes), unity_c_bytes)) {
    return -1;
  }
  if (write_file(path_buff, "unity_internals.h",
                 sizeof(unity_internals_h_bytes),
                 unity_internals_h_bytes)) {
    return -1;
  }
  return 1;
}
