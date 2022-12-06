#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "bmpHeader.h"

#define BYTE unsigned char

#define widthbytes(bits) (((bits)+31)/32*4)

//typedef struct tagRGBQUAD{
//	BYTE rgbBlue; 
//	BYTE rgbGreen; 
//	BYTE rgbRed; 
//	BYTE rgbReserved;
//} RGBQUAD;

//typedef struct BmpFileInfo
//{
//	unsigned short int type; 
//	unsigned int file_size; 
//	unsigned short int reserved1; 
//	unsigned short int reserved2; 
//	unsigned int offset;
//}bmpFileInfo;

//typedef struct BmpImageInfo
//{
//	unsigned int header_size; 
//	int width, height; 
//	unsigned short int planes; 
//	unsigned short int bits; 
//	unsigned int compression; 
//	unsigned int imagesize; 
//	int hresolution, vresolution; 
//	unsigned int ncolors, importantcolors; 
//}bmpImageInfo;

inline unsigned char clip(int value, int min, int max);
unsigned char clip(int value, int min, int max)
{
    return(value > max? max : value < min? min : value);
}

int main(int argc, char** argv) {
	FILE* fp; 
	RGBQUAD palrgb[256];
	
	BITMAPFILEHEADER bmpFileInfo;
	BITMAPINFOHEADER bmpImageInfo;

	char input[128], output[128];
	
	int i, j, size; 
	float srcX, srcY;
	int index; 
	float r,g,b,gray;
	int graysize; 
	int index2;
	
	unsigned char *grayimg, *inimg, *outimg;
	
	/* usage a.out in.bmp out.bmp */
	strcpy(input, argv[1]); 
	strcpy(output, argv[2]);
	
	
	if((fp=fopen(input, "rb")) == NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	fread(&bmpFileInfo, sizeof(BITMAPFILEHEADER), 1, fp); 
	
	fread(&bmpImageInfo, sizeof(BITMAPINFOHEADER), 1, fp); 
	
	size=widthbytes(bmpImageInfo.biBitCount * bmpImageInfo.biWidth); 
	graysize = widthbytes(8 * bmpImageInfo.biWidth);
	
	if(!bmpImageInfo.SizeImage) 
		bmpImageInfo.SizeImage=bmpImageInfo.biHeight*size;
	
	inimg=(BYTE*)malloc(sizeof(BYTE)*bmpImageInfo.SizeImage); 
	outimg=(BYTE*)malloc(sizeof(BYTE)*bmpImageInfo.SizeImage); 
	fread(inimg, sizeof(BYTE), bmpImageInfo.SizeImage, fp); 
	
	fclose(fp);
	
	for(i=0; i<bmpImageInfo.biHeight*3; i+=3) { 
		for(j=0; j<bmpImageInfo.biWidth*3; j+=3) {
			int b = inimg[j+(i*bmpImageInfo.biWidth+0)]; 
			int g = inimg[j+(i*bmpImageInfo.biWidth+1)]; 
			int r = inimg[j+(i*bmpImageInfo.biWidth+2)]; 
			
			b += 50;
			g += 50;
			r += 50; 
			
			outimg[j+bmpImageInfo.biWidth*i+0]= clip(b, 0, 255);
			outimg[j+bmpImageInfo.biWidth*i+1]= clip(g, 0, 255);
			outimg[j+bmpImageInfo.biWidth*i+2]= clip(r, 0, 255);
		};
	};
	 
	size=widthbytes(bmpImageInfo.biBitCount*bmpImageInfo.biWidth); 
	bmpImageInfo.SizeImage=bmpImageInfo.biHeight*size; 
	bmpFileInfo.bfOffBits+=256*sizeof(RGBQUAD);
	
	if((fp=fopen(output, "wb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	fwrite(&bmpFileInfo, sizeof(BITMAPFILEHEADER), 1, fp); 
	fwrite(&bmpImageInfo, sizeof(BITMAPINFOHEADER), 1, fp); 
	fwrite(palrgb, sizeof(unsigned int), 256, fp); 
	
	fwrite(outimg, sizeof(unsigned char), bmpImageInfo.SizeImage, fp);
	
	free(inimg); 
	free(outimg);
	
	fclose(fp); 
	
	return 0;
}
