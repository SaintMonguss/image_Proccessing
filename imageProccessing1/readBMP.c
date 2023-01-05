#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */


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
		fprintf(stderr, "실행 형태 : %s input.bmp\n", argv[0]);
		return -1;
	}

	if ((fp = fopen(argv[1], "rb")) == NULL)
	{
		fprintf(stderr, "에러 : 파일을 여는데에 실패했습니다...\n");
		return -1;
	}

	//bmp 파일의 헤더 정보 읽어오기
	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	/*
	sizeOfColumn = widthbytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth);
	imgSize = sizeOfColumn * bmpInfoHeader.biHeight;
	*/
	
	
	if ((inimg =(ubyte*)malloc(sizeof(ubyte) * bmpInfoHeader.SizeImage)) == NULL)
	{
		fprintf(stderr, "에러 : 이미지 파일을 받아올 메모리 할당에 실패 했습니다...\n");
		return 0;
	}  

	if (bmpInfoHeader.biBitCount == 8 && bmpInfoHeader.biClrImportant == 0)
		bmpInfoHeader.biClrImportant = 256;

	if ((palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD) * bmpInfoHeader.bmpInfoHeader.biClrImportant)) == NULL)
	{
		fprintf(stderr, "에러 : 팔레트를 받아올 메모리 할당에 실패했습니다...\n");
		return 0;
	}

	//오프셋 위치로 지정
	fseek(fp, bmpHeader.bfOffBits, SEEK_SET);
	
	fread(inimg, sizeof(ubyte), bmpInfoHeader.SizeImage, fp);

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
	printf("픽셀당 비트 수 : 1 bit\n");
	printf("지원색상 : 흑백\n\n");
	printf("픽셀 정보 :\n");
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
	printf("픽셀당 비트 수 : %d bit\n", bmpInfoHeader->biBitCount);
	printf("지원색상 : 팔레트 컬러 \n\n");
	printf("픽셀 정보 :\n");

}

void printPixcel_24(BITMAPFILEHEADER* bmpHeader, BITMAPINFOHEADER* bmpInfoHeader)
{
	printf("픽셀당 비트 수 : 24 bit\n");
	printf("지원색상 : 트루컬러\n\n");
	printf("픽셀 정보 :\n");
	
	int bgr[3];
	for (int i = 0; i < bmpInfoHeader->SizeImage; i += 3)
	{
		for (int j = 0; j < 3; j++)
		{
			bgr[j] = inimg[i + j]
		}
		printf("(%d, %d , %d) ", bgr[0], bgr[1], bgr[2]);
		return 0;
	}
}