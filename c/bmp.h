/*
 * LCDK bitmap handling functions
 */

#ifndef BMP
#define BMP
#include "m_mem.h"

#pragma pack(1)
struct BitMap
{
   unsigned short int Type;
   unsigned int Size;
   unsigned short int Reserved1, Reserved2;
   unsigned int Offset;
} Header;

struct BitMapInfo
{
   unsigned int Size;
   int Width, Height;
   unsigned short int Planes;
   unsigned short int Bits;
   unsigned int Compression;
   unsigned int ImageSize;
   int xRes, yRes;
   unsigned int Colors;
   unsigned int ImptColors;
} InfoHeader;


unsigned char* imread(char* name)
{

	unsigned char *bitmap;
	FILE *BMP_in = fopen (name, "rb");
	int i, datasize;
	int padding;
	unsigned char *pad;

	if (BMP_in == NULL)
	{
	     printf("\nCannot open file\n");
	     exit(1);
	}

	printf("Reading the bmp header...\n");
	fread(&Header, sizeof(Header), 1, BMP_in);
	fread(&InfoHeader, sizeof(InfoHeader), 1, BMP_in);


	printf("size of Header = %d\n", sizeof(Header));
	printf("size of InfoHeader = %d\n", sizeof(InfoHeader));
	printf("Width: %d\n", InfoHeader.Width);
	printf("Height: %d\n", InfoHeader.Height);

	datasize = InfoHeader.Width*InfoHeader.Height*3;

	padding = (4 - ((InfoHeader.Width*3) % 4)) % 4 ;
	pad = malloc(padding*sizeof(unsigned char));

	bitmap = m_malloc(datasize);



    if (!bitmap)
     {
    	 printf("out of memory!\n");
     }
     else
     {
    	 printf("Successfully allocated memory for the bitmap\n");

     }

    printf("Reading BMP file...\n");
    for (i=0; i < datasize; i++)
    {
       	 fread(&bitmap[i], 1,1,  BMP_in);

       	 if (i % (3*InfoHeader.Width) == 3*InfoHeader.Width -1)
       	 {
       		fread(&pad, padding*sizeof(unsigned char),1,  BMP_in);
       	 }

    }


    fclose(BMP_in);

    printf("Finish reading BMP file...\n");

	return bitmap;
}

void imshow(char* name,unsigned char* bitmap)
{



    FILE *BMP_out;
    int filesize, datasize, i;
	int padding;
	unsigned char *pad;


    datasize = InfoHeader.Width*InfoHeader.Height*3;
    filesize = 54 + 3*InfoHeader.Width* InfoHeader.Height;

	padding = (4 - ((InfoHeader.Width*3) % 4)) % 4 ;
	pad = malloc(padding*sizeof(unsigned char));

	for (i = 0; i < padding; i++)
	{
		pad[i] = 0;
	}


    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};

    bmpfileheader[ 2] = (unsigned char)(filesize    );
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);
    bmpinfoheader[ 4] = (unsigned char)(       InfoHeader.Width    );
    bmpinfoheader[ 5] = (unsigned char)(       InfoHeader.Width>> 8);
    bmpinfoheader[ 6] = (unsigned char)(       InfoHeader.Width>>16);
    bmpinfoheader[ 7] = (unsigned char)(       InfoHeader.Width>>24);
    bmpinfoheader[ 8] = (unsigned char)(       InfoHeader.Height    );
    bmpinfoheader[ 9] = (unsigned char)(       InfoHeader.Height>> 8);
    bmpinfoheader[10] = (unsigned char)(       InfoHeader.Height>>16);
    bmpinfoheader[11] = (unsigned char)(       InfoHeader.Height>>24);

    BMP_out = fopen(name,"wb");
    fwrite(bmpfileheader,1,14,BMP_out);
    fwrite(bmpinfoheader,1,40,BMP_out);

    printf("Writing BMP file...\n");

    for(i=0; i < datasize; i++)
    {

    	if (i%(InfoHeader.Width*3) == 0)
    	{
    		printf("i = %d\n",i);
    	}

        fwrite(&bitmap[i], 1,1,  BMP_out);

        if (i % (3*InfoHeader.Width) == 3*InfoHeader.Width -1)
        {
        	fwrite(&pad, padding*sizeof(unsigned char),1,  BMP_out);
        }

     }
    printf("Finish writing BMP file...\n");
    fclose(BMP_out);

}

#endif
