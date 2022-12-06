#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>

#include "bmpHeader.h"

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

unsigned char clip(float value, int min, int max)
{   
    return(value > max? max : value < min? min : value);
}

int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
    RGBQUAD *palrgb;
    ubyte *inimg, *outimg, *tmpimg;
    int x, y, z, imageSize, paddingSize;

    if(argc != 3) {
        fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
        return -1;
    }
    
    /***** read bmp *****/ 
    if((fp=fopen(argv[1], "rb")) == NULL) { 
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    /* 트루 컬러를 지원하면 변환할 수 없다. */
    if(bmpInfoHeader.biBitCount != 24) {
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp);
        return -1;
    }

    int elemSize = bmpInfoHeader.biBitCount/8;
    int size = bmpInfoHeader.biWidth*elemSize;
    imageSize = size * bmpInfoHeader.biHeight; 
	// 패딩 사이즈 = (기존의 너비 2개 + 높이 2개 + 네 모서리 1픽셀) * 24/8(3바이트) 
	paddingSize = imageSize+(((bmpInfoHeader.biWidth+bmpInfoHeader.biHeight)*2+4)*elemSize);


    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */
    printf("Image Size : %d\n", imageSize);
    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize); 
	tmpimg = (ubyte*)malloc(sizeof(ubyte)*paddingSize);
    outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);
	fread(inimg, sizeof(ubyte), imageSize, fp); 
	
    fclose(fp);


	/*for(int i = 0 ; i < paddingSize ; i ++)
	{
		tmpimg[i] = 0;
	}*/



	for(int j = 0 ; j < bmpInfoHeader.biHeight; j++)
		for(int i = 0; i < size ; i++)
		{
			//if(j == 0 || j == bmpInfoHeader.biHeight)
				
			tmpimg[(1*elemSize)+ i + ((bmpInfoHeader.biWidth+2)*elemSize)*(j+1)] = 
				inimg[i + size*j];
		}

    // define the kernel
    float kernel[3][3] = {{-1.0, -1.0, -1.0},
                           {-1.0, 9.0, -1.0},
                           {-1.0, -1.0, -1.0}};
    memset(outimg, 0, sizeof(ubyte)*imageSize);
    for(y = 1; y <= bmpInfoHeader.biHeight ; y++) { 
        for(x = 1*elemSize; x <= bmpInfoHeader.biWidth * elemSize; x+=elemSize) {
            for(z = 0; z < elemSize; z++) {
				float tmp = 0;
                float sum = 0.0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) { 
						tmp = kernel[i+1][j+1]*tmpimg[(x+i*elemSize)+(y+j)*(bmpInfoHeader.biWidth+2)*elemSize+z];
					sum += tmp;
                    }
                }
                outimg[(x-3)+(y-1)*size+z] = clip(sum, 0, 255);
            }
        }
    }              
    /***** write bmp *****/ 
    if((fp=fopen(argv[2], "wb"))==NULL) { 
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    //fwrite(inimg, sizeof(ubyte), imageSize, fp); 
    fwrite(outimg, sizeof(ubyte), imageSize, fp);

    fclose(fp); 
    
    free(inimg); 
    free(outimg);
    
    return 0;
}
