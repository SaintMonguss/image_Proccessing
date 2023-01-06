/**************************************************************
* BMP ����� �̿��ؼ� BMP ������ �а�
* �ȼ� ���� ����ϴ� ���α׷��� �ۼ��Ͻÿ�
* *************************************************************/


#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     // USHRT_MAX ����� ���ؼ� ����Ѵ�. 

#include "bmpHeader.h"

#define widthbytes(bits) (((bits)+31)/32*4)

typedef unsigned char ubyte;
ubyte* inimg;
RGBQUAD* palrgb;

void printPixcelBin(BITMAPFILEHEADER*, BITMAPINFOHEADER*);
void printPixcel_2_4_8(BITMAPFILEHEADER*, BITMAPINFOHEADER*);
void printPixcel_24(BITMAPFILEHEADER*, BITMAPINFOHEADER*);

int main(int argc, char** argv)
{
	FILE* fp;
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;
	

	int sizeOfColumn, imgSize;
	
	if (argc != 2)
	{
		fprintf(stderr, "���� ���� : %s input.bmp\n", argv[0]);
		return -1;
	}

	if ((fp = fopen(argv[1], "rb")) == NULL)
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

	if (bmpInfoHeader.biBitCount == 8 && bmpInfoHeader.biClrImportant == 0)
		bmpInfoHeader.biClrImportant = 256;

	if ((palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD) * bmpInfoHeader.biClrImportant)) == NULL)
	{
		fprintf(stderr, "���� : �ȷ�Ʈ�� �޾ƿ� �޸� �Ҵ翡 �����߽��ϴ�...\n");
		return 0;
	}
	// 2,4,8��Ʈ ������ bmp ������ �ȷ�Ʈ �޾ƿ���
	if (bmpInfoHeader.biBitCount == 2 || bmpInfoHeader.biBitCount == 4 || bmpInfoHeader.biBitCount == 8)
	{
		fseek(fp, 54, SEEK_SET);
		fread(palrgb, sizeof(RGBQUAD), bmpInfoHeader.biClrImportant, fp);
	}
	//������ ��ġ�� ����
	fseek(fp, bmpHeader.bfOffBits, SEEK_SET);
	
	fread(inimg, sizeof(ubyte), bmpInfoHeader.SizeImage, fp);

	fclose(fp);
	
	

	switch (bmpInfoHeader.biBitCount)
	{
	case 1:
		printPixcelBin(&bmpHeader, &bmpInfoHeader);
		break;
	case 24:
		printPixcel_24(&bmpHeader, &bmpInfoHeader);
		break;
	default:
		printPixcel_2_4_8(&bmpHeader, &bmpInfoHeader);
		break;
	}
	return 0;
}

void printPixcelBin(BITMAPFILEHEADER *bmpHeader, BITMAPINFOHEADER *bmpInfoHeader)
{
	printf("�ȼ��� ��Ʈ �� : 1 bit\n");
	printf("�������� : ���\n\n");
	printf("�ȼ� ���� :\n");
	for (int i = 0; i < bmpInfoHeader->SizeImage; i++)
	{
		for (int j = 7; j > 0; j--)
		{
			int n = 0;
			n = (inimg[i] >> j) & 1;
			printf("%d ", n);
		}
	}
}

void printPixcel_2_4_8(BITMAPFILEHEADER* bmpHeader, BITMAPINFOHEADER* bmpInfoHeader)
{
	printf("�ȼ��� ��Ʈ �� : %d bit\n", bmpInfoHeader->biBitCount);
	printf("�������� : �ȷ�Ʈ �÷� \n\n");
	printf("�ȼ� ���� :\n");

	switch (bmpInfoHeader->biBitCount)
	{
	case 2:
		for (int i = 0; i < bmpInfoHeader->SizeImage; i++)
		{
			for (int j = 6; j > 0; j -= 2)
			{
				int n = 0;
				n = (inimg[i] >> j) & 0b11;
				printf("(%d, %d , %d) ", palrgb[n].rgbBlue, palrgb[n].rgbGreen, palrgb[n].rgbRed);
			}
		}
		break;
	case 4:
		for (int i = 0; i < bmpInfoHeader->SizeImage; i++)
		{
			for (int j = 4; j > 0; j -= 4)
			{
				int n = 0;
				n = (inimg[i] >> j) & 0b1111;
				printf("(%d, %d , %d) ", palrgb[n].rgbBlue, palrgb[n].rgbGreen, palrgb[n].rgbRed);
			}
		}
		
		break;
	case 8:
		for (int i = 0; i < bmpInfoHeader->SizeImage; i++)
		{
			int n = inimg[i];
			printf("(%d, %d , %d) ", palrgb[n].rgbBlue, palrgb[n].rgbGreen, palrgb[n].rgbRed);

		}
		break;
	}
	return 0;
}

void printPixcel_24(BITMAPFILEHEADER* bmpHeader, BITMAPINFOHEADER* bmpInfoHeader)
{
	printf("�ȼ��� ��Ʈ �� : 24 bit\n");
	printf("�������� : Ʈ���÷�\n\n");
	printf("�ȼ� ���� :\n");
	
	int bgr[3];
	for (int i = 0; i < bmpInfoHeader->SizeImage; i += 3)
	{
		for (int j = 0; j < 3; j++)
		{
			bgr[j] = inimg[i + j];
		}
		printf("(%d, %d , %d) ", bgr[0], bgr[1], bgr[2]);
		
	}
	return 0;
}