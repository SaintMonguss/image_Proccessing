/**************************************************************
* BMP 헤더만 이용해서 BMP 파일을 복사하는 프로그램을 작성
* 픽셀 값을 출력하는 프로그램을 작성하시오
* *************************************************************/

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     // USHRT_MAX 상수를 위해서 사용한다. 

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
		fprintf(stderr, "실행 형태 : %s input.bmp, out.bmp\n", argv[0]);
		return -1;
	}

	if ((fp = fopen(argv[1], "rb")) == NULL)
	{
		fprintf(stderr, "에러 : 파일을 여는데에 실패했습니다...\n");
		return -1;
	}

	if ((wf = fopen(argv[2], "wb")) == NULL)
	{
		fprintf(stderr, "에러 : 파일을 여는데에 실패했습니다...\n");
		return -1;
	}

	//bmp 파일의 헤더 정보 읽어오기
	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);


	if ((inimg =(ubyte*)malloc(sizeof(ubyte) * bmpInfoHeader.SizeImage)) == NULL)
	{
		fprintf(stderr, "에러 : 이미지 파일을 받아올 메모리 할당에 실패 했습니다...\n");
		return 0;
	}

	if ((copyimg = (ubyte*)malloc(sizeof(ubyte) * bmpInfoHeader.SizeImage)) == NULL)
	{
		fprintf(stderr, "에러 : 이미지 파일을 받아올 메모리 할당에 실패 했습니다...\n");
		return 0;
	}

	if ((palette = (ubyte*)malloc(sizeof(RGBQUAD) * 256)) == NULL)
	{
		fprintf(stderr, "에러 : 이미지 파일을 받아올 메모리 할당에 실패 했습니다...\n");
		return 0;
	}

	
	//오프셋 위치로 지정
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


	/* BITMAPFILEHEADER 구조체의 데이터 */
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, wf);

	/* BITMAPINFOHEADER 구조체의 데이터 */
fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, wf);
	/* 팔레트 공간 확보*/ 
	fwrite(&palette, sizeof(RGBQUAD), 256, wf);
	fwrite(copyimg, sizeof(ubyte), bmpInfoHeader.SizeImage, wf);


	fclose(wf);


	return 0;
}
