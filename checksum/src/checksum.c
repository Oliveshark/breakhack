/*
**  CHECKSUM.C - Compute the checksum of a file
**
**  public somain demo by Bob Stout
*/

#include <stdlib.h>
#include "checksum.h"

unsigned
checksum(void *buffer, size_t len, unsigned int seed)
{
      unsigned char *buf = (unsigned char *)buffer;
      size_t i;

      for (i = 0; i < len; ++i)
            seed += (unsigned int)(*buf++);
      return seed;
}

unsigned
checksum_fp(FILE *fp)
{
      unsigned int seed = 0;
      char buf[4096];

      size_t len;
      do {
	      len = fread(buf, sizeof(char), sizeof(buf), fp);
	      seed = checksum(buf, len, seed);
      } while (len > 0);

      return seed;
}

#ifdef EXECUTABLE
#include <stdio.h>

int main(int argc, char *argv[])
{
      FILE *fp;
      const char *file;

      if (argc < 2) {
	      printf("You need to provide an input file\n");
	      printf("Example: %s <file>", argv[0]);
      }
      file = argv[1];

      if (NULL == (fp = fopen(file, "rb")))
      {
            printf("Unable to open %s for reading\n", file);
            return -1;
      }

      printf("The checksum of %s is %#x\n", file, checksum_fp(fp));

      return 0;
}

#endif
