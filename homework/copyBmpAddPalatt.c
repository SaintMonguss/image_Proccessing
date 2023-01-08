/**************************************************************
* BMP ����� �̿��ؼ� BMP ������ �����ϴ� ���α׷��� �ۼ�
* �ȼ� ���� ����ϴ� ���α׷��� �ۼ��Ͻÿ�
* *************************************************************/

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     // USHRT_MAX ����� ���ؼ� ����Ѵ�. 

#include "bmpHeader.h"

#define widthbytes(bits) (((bits)+31)/32*4)

typedef unsigned char ubyte;
ubyte *inimg, *copyimg, *palette;

int main(int argc, char** argv)
{
	FILE* fp, *wf;
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;
	RGBQUAD palrgb;


	int sizeOfColumn, imgSize;

	if (argc != 3)
	{
		fprintf(stderr, "���� ���� : %s input.bmp, out.bmp\n", argv[0]);
		return -1;
	}

	if ((fp = fopen(argv[1], "rb")) == NULL)
	{
		fprintf(stderr, "���� : ������ ���µ��� �����߽��ϴ�...\n");
		return -1;
	}

	if ((wf = fopen(argv[2], "wb")) == NULL)
	{
		fprintf(stderr, "���� : ������ ���µ��� �����߽��ϴ�...\n");
		return -1;
	}

	//bmp ������ ��� ���� �о����
	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);


	if ((inimg =(ubyte*)malloc(sizeof(ubyte) * bmpInfoHeader.SizeImage)) == NULL)
	{
		fprintf(stderr, "���� : �̹��� ������ �޾ƿ� �޸� �Ҵ翡 ���� �߽��ϴ�...\n");
		return 0;
	}

	if ((copyimg = (ubyte*)malloc(sizeof(ubyte) * bmpInfoHeader.SizeImage)) == NULL)
	{
		fprintf(stderr, "���� : �̹��� ������ �޾ƿ� �޸� �Ҵ翡 ���� �߽��ϴ�...\n");
		return 0;
	}

	if ((palette = (ubyte*)malloc(sizeof(RGBQUAD) * 256)) == NULL)
	{
		fprintf(stderr, "���� : �̹��� ������ �޾ƿ� �޸� �Ҵ翡 ���� �߽��ϴ�...\n");
		return 0;
	}

	
	//������ ��ġ�� ����
	fseek(fp, bmpHeader.bfOffBits, SEEK_SET);

	fread(inimg, sizeof(ubyte), bmpInfoHeader.SizeImage, fp);
	fclose(fp);

	for (int i = 0; i < bmpInfoHeader.SizeImage; i++)
	{
		copyimg[i] = inimg[i];
	}


	bmpHeader.bfSize = bmpInfoHeader.SizeImage + 14 + 40 + (sizeof(RGBQUAD) * 256);
	bmpHeader.bfOffBits = 54 + (sizeof(RGBQUAD) * 256);
	bmpInfoHeader.biClrUsed = 256;


	/* BITMAPFILEHEADER ����ü�� ������ */
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, wf);

	/* BITMAPINFOHEADER ����ü�� ������ */
fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, wf);
	/* �ȷ�Ʈ ���� Ȯ��*/ 
	fwrite(&palette, sizeof(RGBQUAD), 256, wf);
	fwrite(copyimg, sizeof(ubyte), bmpInfoHeader.SizeImage, wf);


	fclose(wf);


	return 0;
}
