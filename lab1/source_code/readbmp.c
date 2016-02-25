#include <stdio.h>
#include <stdlib.h>

#define CAST(Class, obj) ((Class)(obj))

int main(int argc, char *argv[])
{

  if( argc != 2 )
    {
      fprintf(stderr, "Usage:\t %s [input_bmp_file]\n", argv[0]);
      return 7;
    }

   //write_binary_bmp(argv[1],512,512,image);
  read_bmp(argv[1]);
  return 0;
}
