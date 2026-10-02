#include <stdio.h>

int main(int /*argc*/, char **argv) {
  const char *filename = argv[1];
  printf("%s\n", filename);
  return 0;
}
