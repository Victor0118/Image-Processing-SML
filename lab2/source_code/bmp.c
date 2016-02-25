#include "bmp.h"

BITMAPFILEHEADER bmfhdr;
BITMAPINFOHEADER bmihdr;
int width, height, level;
BYTE** Y;
char** U;
char** V;
BYTE maxY, minY;


int read_bmp(const char *filename){

  BYTE ***image = rgb2yuv(filename);
  toGray("test.bmp");
  ChangeLuminance(30,"test2.bmp",image);
  ChangeLuminance(-60,"test3.bmp",image);  
  return 0;

}

BYTE *** rgb2yuv(const char *filename)
{
  BYTE ***image;
  FILE *file;
  int i, j, k;
  int biBitCount, offset;
  int *x;

  /*
    RGBQUAD aColors[];
    BYTE aBitmapBits[];
  */

  if (!(file=fopen(filename,"rb"))) 
    {
      printf("Cannot open file %s.\n", filename);
      return 0;
    }

  
  fread(&bmfhdr.bfType, sizeof(WORD), 1, file);
  fread(&bmfhdr.bfSize, sizeof(DWORD), 1, file);
  fread(&bmfhdr.bfReserved1, sizeof(WORD), 1, file);
  fread(&bmfhdr.bfReserved2, sizeof(WORD), 1, file);
  fread(&bmfhdr.bfOffBits, sizeof(DWORD), 1, file);
  fread(&bmihdr.biSize, sizeof(DWORD), 1, file);
  fread(&bmihdr.biWidth, sizeof(LONG), 1, file);
  fread(&bmihdr.biHeight, sizeof(LONG), 1, file);
  fread(&bmihdr.biPlanes, sizeof(WORD), 1, file);
  fread(&bmihdr.biBitCount, sizeof(WORD), 1, file);
  fread(&bmihdr.biCompression, sizeof(DWORD), 1, file);
  fread(&bmihdr.biSizeImage, sizeof(DWORD), 1, file);
  fread(&bmihdr.biXPelsPerMeter, sizeof(LONG), 1, file);
  fread(&bmihdr.biYPelsPerMeter, sizeof(LONG), 1, file);
  fread(&bmihdr.biClrUsed, sizeof(DWORD), 1, file);
  fread(&bmihdr.biClrImportant, sizeof(DWORD), 1, file);


  //fread(&bmfhdr,sizeof(bmfhdr),1,file);
  //fread(&bmihdr,sizeof(bmihdr),1,file);

  printf("+------------------+\n");    
  printf("| bfType: %d\n", bmfhdr.bfType);
  printf("| bfSize: %d\n", bmfhdr.bfSize);
  printf("| bfReserved1: %d\n", bmfhdr.bfReserved1);
  printf("| bfReserved2: %d\n", bmfhdr.bfReserved2);
  printf("| bfOffBits: %d\n", bmfhdr.bfOffBits);
  printf("| biSize: %d\n", bmihdr.biSize);
  printf("| biWidth: %d\n", bmihdr.biWidth);
  printf("| biHeight: %d\n", bmihdr.biHeight);
  printf("| biPlanes: %d\n", bmihdr.biPlanes);
  printf("| biBitCount: %d\n", bmihdr.biBitCount);
  printf("| biCompression: %d\n", bmihdr.biCompression);
  printf("| biSizeImage: %d\n", bmihdr.biSizeImage);
  printf("| biXPelsPerMeter: %d\n", bmihdr.biXPelsPerMeter);
  printf("| biYPelsPerMeter: %d\n", bmihdr.biYPelsPerMeter);
  printf("| biClrUsed: %d\n", bmihdr.biClrUsed);
  printf("| biClrImportant: %d\n", bmihdr.biClrImportant);
  printf("+------------------+\n");

  printf("+------------------+\n");
  printf("| biBitCount: %d\n", bmihdr.biBitCount);
  if (bmihdr.biBitCount == 1) {
    RGBQUAD aColors[2];
    for (i=0; i<2; i++) {
      fread(&aColors[i].rgbBlue, sizeof(BYTE), 1, file);
      fread(&aColors[i].rgbGreen, sizeof(BYTE), 1, file);
      fread(&aColors[i].rgbRed, sizeof(BYTE), 1, file);
      fread(&aColors[i].rgbReserved, sizeof(BYTE), 1, file);
      printf("| rgbBlue: %d\t rgbGreen: %d\t rgbRed: %d\t rgbReserved: %d\n", aColors[i].rgbBlue, aColors[i].rgbGreen, aColors[i].rgbRed, aColors[i].rgbReserved);
    }
  } else if (bmihdr.biBitCount == 8) {
    RGBQUAD aColors[256];
    for (i=0; i<8; i++) {
      fread(&aColors[i].rgbBlue, sizeof(BYTE), 1, file);
      fread(&aColors[i].rgbGreen, sizeof(BYTE), 1, file);
      fread(&aColors[i].rgbRed, sizeof(BYTE), 1, file);
      fread(&aColors[i].rgbReserved, sizeof(BYTE), 1, file);
      printf("| rgbBlue: %d\t rgbGreen: %d\t rgbRed: %d\t rgbReserved: %d\n", aColors[i].rgbBlue, aColors[i].rgbGreen, aColors[i].rgbRed, aColors[i].rgbReserved);
    }
  } else if (bmihdr.biBitCount == 24) {
    printf(" It's rgb bmp image without palette.\n");
  } else {
    printf("Something wrong with biBitCount of image %s.\n", filename);
    return 0;
  }
  printf("+------------------+\n");

  /* display bmp image attributes. */
  biBitCount = bmihdr.biBitCount;
  if (biBitCount == 1) {
    printf("%s is Binary image.\n", filename);
  } else if (biBitCount == 8) {
    printf("%s is Gray-Level or Indexed image.\n", filename);
  } else if (biBitCount == 24) {
    printf("%s is RGB image.\n", filename);
  } else {
      printf("Something wrong with biBitCount of image %s.\n", filename);
  }

  /* width and height of bmp image. */
  width = bmihdr.biWidth;
  height = bmihdr.biHeight;
  level = 3; /* for RGB only */
    
  /* malloc for 3-D array ***image and 1-D array *x. */
  image = (BYTE ***)malloc(height*sizeof(BYTE **));
  for (i=0; i<height; i++)
    image[i] = (BYTE **)malloc(width*sizeof(BYTE *));
  for(i=0; i<height; i++)
    for (j=0; j<width; j++)
      image[i][j] = (BYTE *)malloc(level*sizeof(BYTE));
  x = (int *)malloc(level*sizeof(int));
    
  /* initialize 3-D array ***image and 1-D array *x. */
  for (i=0; i<height; i++)
    for (j=0; j<width; j++)
      for (k=0; k<level; k++)
        image[i][j][k] = 0;
  for (k=0; k<level; k++)
    x[k] = 0;
  
  /* read pixel from file to array image. */
  /*
    bmihdr.biBitCount = 1  -> Binary image.
    bmihdr.biBitCount = 8  -> Gray-Level or Indexed image.
    bmihdr.biBitCount = 24 -> RGB image.
  */
  if ((bmihdr.biBitCount == 1) || (bmihdr.biBitCount == 8)) {
    offset = bmfhdr.bfOffBits - 54;
    fseek(file, offset, SEEK_CUR);
    for (i=height-1; i>=0; i--) {
      for (j=0; j<width; j++) {
        image[i][j][0] = fgetc(file);
      }
    }
  } else if (bmihdr.biBitCount == 24) {
    for (i=height-1; i>=0; i--) {
      for (j=0; j<width; j++)
        for (k=level-1; k>=0; k--)
          image[i][j][k] = fgetc(file);
      /*
        if(3*width%4 != 0)
        fseek(file, 4-width*3%4, SEEK_CUR);
      */
    }
  } else {
    printf("Something wrong with biBitCount of image %s.\n", filename);
    return 0;
  }

    printf("==================oldimage===================\n");  
  for (int i = 0; i < height; ++i)
  {	
  	  for (int j = 0; j < width; ++j)
  	  {
  	  	//Y[i][j] += (255 -maxY)/5;
  	  	printf("%d %d %d *", image[i][j][0],image[i][j][1],image[i][j][2]);
  	  }
  	  printf("\n");
  }
 

  Y = (BYTE **) malloc(height*sizeof(BYTE *));
  U = (char **) malloc(height*sizeof(char *));
  V = (char **) malloc(height*sizeof(char *));
  for (int i = 0; i < height; ++i)
  {	
	  Y[i] = (BYTE *) malloc(width*sizeof(BYTE));
	  U[i] = (char *) malloc(width*sizeof(char));
	  V[i] = (char *) malloc(width*sizeof(char));
  	  for (int j = 0; j < width; ++j)
  	  {
  	  	Y[i][j] = (BYTE)(0.299*image[i][j][0] + 0.587*image[i][j][1] + 0.114*image[i][j][2]);
  	  	U[i][j] = (BYTE)(-0.147*image[i][j][0] -0.289*image[i][j][1] + 0.435*image[i][j][2]);
  	  	V[i][j] = (BYTE)(0.615*image[i][j][0] -0.515*image[i][j][1] + -0.100*image[i][j][2]);
  	  	if (maxY < Y[i][j])
  	  	{
  	  		maxY = Y[i][j];
  	  		/* code */
  	  	}
  	  	if (minY > Y[i][j])
  	  	{
  	  		minY = Y[i][j];
  	  		/* code */
  	  	}
  	  }
  }
  fclose(file);
  return image;
}

int toGray(char * filename)
{

  printf("==================newY===================\n");  
  BYTE** newY;
  newY = (BYTE **) malloc(height*sizeof(BYTE *));
  for (int i = 0; i < height; ++i)
  {	
	  newY[i] = (BYTE *) malloc(width*sizeof(BYTE));
  	  for (int j = 0; j < width; ++j)
  	  {
  	  	newY[i][j] = (Y[i][j]-minY)*1.0/(maxY - minY)*255;
  	  	//printf("%d ", newY[i][j]);
  	  }
  	  //printf("\n");
  }

  FILE * file2;
  printf("==================Gray-Level-writing===================\n");
  if (!(file2=fopen("test.bmp","wb"))) 
    {
      printf("Cannot open file %s.\n", filename);
      return(7);
    }

  fwrite(&bmfhdr.bfType, sizeof(WORD), 1, file2);
  fwrite(&bmfhdr.bfSize, sizeof(DWORD), 1, file2);
  fwrite(&bmfhdr.bfReserved1, sizeof(WORD), 1, file2);
  fwrite(&bmfhdr.bfReserved2, sizeof(WORD), 1, file2);
  fwrite(&bmfhdr.bfOffBits, sizeof(DWORD), 1, file2);
  fwrite(&bmihdr.biSize, sizeof(DWORD), 1, file2);
  fwrite(&bmihdr.biWidth, sizeof(LONG), 1, file2);
  fwrite(&bmihdr.biHeight, sizeof(LONG), 1, file2);
  fwrite(&bmihdr.biPlanes, sizeof(WORD), 1, file2);
  fwrite(&bmihdr.biBitCount, sizeof(WORD), 1, file2);
  fwrite(&bmihdr.biCompression, sizeof(DWORD), 1, file2);
  fwrite(&bmihdr.biSizeImage, sizeof(DWORD), 1, file2);
  fwrite(&bmihdr.biXPelsPerMeter, sizeof(LONG), 1, file2);
  fwrite(&bmihdr.biYPelsPerMeter, sizeof(LONG), 1, file2);
  fwrite(&bmihdr.biClrUsed, sizeof(DWORD), 1, file2);
  fwrite(&bmihdr.biClrImportant, sizeof(DWORD), 1, file2);

  for (int i=height-1; i>=0; i--) {
      for (int j=0; j<width; j++)
        for (int k=level-1; k>=0; k--)
        	fwrite(&Y[i][j],sizeof(BYTE),1,file2);
          //image[i][j][k] = fgetc(file);
  }
  fclose(file2);
  return 0;

}

int ChangeLuminance(int delta, const char *filename, BYTE ***image)
{
  int temp=0;
  printf("==================newimage===================\n");
  for (int i = 0; i < height; ++i)
  {	
  	  for (int j = 0; j < width; ++j)
  	  {
  	  	
  	  	Y[i][j] =Y[i][j] > 255-delta?255:Y[i][j] < -delta?0:Y[i][j]+delta;  	  		

  	  	temp = (1.0*Y[i][j] + 0*U[i][j] + 1.1398*V[i][j]);
  	  	image[i][j][0] = temp>255?255:temp<0?0:temp;

  	  	temp = (0.9996*Y[i][j] -0.3954*U[i][j] -0.5805*V[i][j]);
  	  	image[i][j][1] = temp>255?255:temp<0?0:temp;

  	  	temp= (1.0020*Y[i][j] + 2.0361*U[i][j] -0.0005*V[i][j]);
  	  	image[i][j][2] = temp>255?255:temp<0?0:temp;

  	  	printf("%d %d %d *", image[i][j][0],image[i][j][1],image[i][j][2]);
  	  }
  	  printf("\n");
  }

  FILE * file2;
  printf("==========================================\n");
  if (!(file2=fopen(filename,"wb"))) 
    {
      printf("Cannot open file %s.\n", filename);
      return(7);
    }

	  fwrite(&bmfhdr.bfType, sizeof(WORD), 1, file2);
	  fwrite(&bmfhdr.bfSize, sizeof(DWORD), 1, file2);
	  fwrite(&bmfhdr.bfReserved1, sizeof(WORD), 1, file2);
	  fwrite(&bmfhdr.bfReserved2, sizeof(WORD), 1, file2);
	  fwrite(&bmfhdr.bfOffBits, sizeof(DWORD), 1, file2);
	  fwrite(&bmihdr.biSize, sizeof(DWORD), 1, file2);
	  fwrite(&bmihdr.biWidth, sizeof(LONG), 1, file2);
	  fwrite(&bmihdr.biHeight, sizeof(LONG), 1, file2);
	  fwrite(&bmihdr.biPlanes, sizeof(WORD), 1, file2);
	  fwrite(&bmihdr.biBitCount, sizeof(WORD), 1, file2);
	  fwrite(&bmihdr.biCompression, sizeof(DWORD), 1, file2);
	  fwrite(&bmihdr.biSizeImage, sizeof(DWORD), 1, file2);
	  fwrite(&bmihdr.biXPelsPerMeter, sizeof(LONG), 1, file2);
	  fwrite(&bmihdr.biYPelsPerMeter, sizeof(LONG), 1, file2);
	  fwrite(&bmihdr.biClrUsed, sizeof(DWORD), 1, file2);
	  fwrite(&bmihdr.biClrImportant, sizeof(DWORD), 1, file2);

  for (int i=height-1; i>=0; i--) {
      for (int j=0; j<width; j++)
        for (int k=level-1; k>=0; k--)
        	fwrite(&image[i][j][k],sizeof(BYTE),1,file2);
  }
  fclose(file2);
  return(0);
}