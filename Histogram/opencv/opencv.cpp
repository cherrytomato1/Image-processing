#include <opencv\cv.h>
#include <opencv\highgui.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>



#define uchar unsigned char 

int histogram[256], cdOfHisto[256], histogramEqual[256];

uchar** uc_alloc(int size_x, int size_y)
{
	uchar** m;
	int i;

	if ((m = (uchar * *)calloc(size_y, sizeof(uchar*))) == NULL)
	{
		printf("uc_alloc error 1\7\n");
		exit(0);
	}

	for (i = 0; i < size_y; i++)
		if ((m[i] = (uchar*)calloc(size_x, sizeof(uchar))) == NULL)
		{
			printf("uc_alloc error 2\7\n");
			exit(0);
		}	
	return m;
}

void read_ucmatrix(int size_x, int size_y, uchar** ucmatrix, char* filename)
{
	int i;
	FILE* f;

	if ((fopen_s(&f, filename, "rb")) != NULL)
	{
		printf("%s File open error!\n", filename);
		exit(0);

	}
	for (i = 0; i < size_y; i++)
	{
		if (fread(ucmatrix[i], sizeof(uchar), size_x, f) != size_x)
		{
			printf("Data Read Error!\n");
			exit(0);

		}
	}
	fclose(f);
}

void write_ucmatrix(int size_x, int size_y, uchar** ucmatrix, char* filename)
{
	int i;
	FILE* f;

	if ((fopen_s(&f, filename, "wb")) != NULL)
	{
		printf("%s File open Error! \n", filename);
		exit(0);

	}
	for (i = 0; i < size_y; i++)
		if (fwrite(ucmatrix[i], sizeof(uchar), size_x, f) != size_x)
		{
			printf("Data Write Error!\n");
			exit(0);
		}
	fclose(f);
}

void get_hist1(uchar** img, int X_size, int Y_size)
{
	int i, j, tmp;
	double tmp1;
	int t, tp, range, hrange;

	CvSize histoSize, cdfSize;
	IplImage* imgHisto, * cdfImgHisto;

	histoSize.width = 256;
	histoSize.height = 256;

	cdfSize.width = 256;
	sdfSize.height = 256;

	imgHisto = cvCreateImage(histoSize, 8, 1);
	cdfimgHisto = cvCreateImage(cdfSize, 8, 1);

	for(i=0;i<histoSize.height;i++)
		for (j = 0; j < histoSize.width; j++)
		{
			((uchar*)(imgHisto->imageData + imgHisto->widthStep * i))[j] = 0;
		}
	for(i=0;i<cdfSize.height;i++)
		for (j = 0; j < cdfSize.width; j++)
		{
			((uchar*)(cdfImgHisto->imageData + cdfImgHisto->widthStep * i))[j] = 0;
		}

	tp = X_Size * Y_Size;

	for (i = 0; i < 256; i++)
		histogram[i] = 0;

	for (i = 0; i < Y_Size; i++)
		for (j = 0; j < X_Size; i++)
			histogram[img[i][j]]++;

	tmp1 = 0;

	for (i = 0; i < 256; ++i)
	{
		tmp1 = tmp > histogram[i] ? tmp1 : histogram[i];
	}

	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (histogram[i] / tmp1);
		cvLine(imgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);

	}

	cvShowImage("Histo Equal", imgHisto);

	cdfOfHisto[0] = histogram[0];

	for (i = 1; i < 256; i++)
	{
		cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i];
	}

	tmp1 = (double)cdfOfHisto[255];

	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (cdfOfHisto[i] / tmp1);
		cvLine(cdfImgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);

	}
	cvShowImage("CDF of Histogram", cdfImgHisto);

	range = cdfOfHisto[255] - cdfOfHisto[0];

	histogramEqual[0] = 0;

	for (i = 1; i < 256; ++i)
	{
		t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range);
		histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t;
	}

	cvReleaseImage(&imgHisto);
	cvReleaseImage(&cdfImgHisto);

	for (i = 0; i < Y_Size; ++i)
		for (j = 0; j < X_Size; ++j)
			img[i][j] = histogramEqual[img[i][j]];
}

void get_hist(uchar** img, int X_Size, int Y_Size)
{
	int i, j, tmp;
	double tmp1;
	int t, tp, range, hrange;

	CvSize histoSize, cdfSize;
	IplImage* imgHisto, * cdfImgHisto;

	histoSize.width = 256;
	histoSize.height = 256;

	cdfSize.width = 256;
	sdfSize.height = 256;

	imgHisto = cvCreateImage(histoSize, 8, 1);
	cdfimgHisto = cvCreateImage(cdfSize, 8, 1);

	for (i = 0; i < histoSize.height; i++)
		for (j = 0; j < histoSize.width; j++)
		{
			((uchar*)(imgHisto->imageData + imgHisto->widthStep * i))[j] = 0;
		}
	for (i = 0; i < cdfSize.height; i++)
		for (j = 0; j < cdfSize.width; j++)
		{
			((uchar*)(cdfImgHisto->imageData + cdfImgHisto->widthStep * i))[j] = 0;
		}

	tp = X_Size * Y_Size;

	for (i = 0; i < 256; i++)
		histogram[i] = 0;

	for (i = 0; i < Y_Size; i++)
		for (j = 0; j < X_Size; i++)
			histogram[img[i][j]]++;

	tmp1 = 0;

	for (i = 0; i < 256; ++i)
	{
		tmp1 = tmp > histogram[i] ? tmp1 : histogram[i];
	}

	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (histogram[i] / tmp1);
		cvLine(imgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);

	}

	cvShowImage("Histo Equal", imgHisto);

	cdfOfHisto[0] = histogram[0];

	for (i = 1; i < 256; i++)
	{
		cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i];
	}

	tmp1 = (double)cdfOfHisto[255];

	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (cdfOfHisto[i] / tmp1);
		cvLine(cdfImgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);

	}
	cvShowImage("CDF of Histogram", cdfImgHisto);

	range = cdfOfHisto[255] - cdfOfHisto[0];

	histogramEqual[0] = 0;

	for (i = 1; i < 256; ++i)
	{
		t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range);
		histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t;
	}

	cvReleaseImage(&imgHisto);

	for (i = 0; i < Y_Size; ++i)
		for (j = 0; j < X_Size; ++j)
			img[i][j] = histogramEqual[img[i][j]];

}

int main(int argc, char* argv[])
{
	int i, j;
	IplImage* cvImg;
	CvSize imgSize;
	uchar** img;

	if (argc != 4)
	{
		printf("Exe imgData X_size y_size\n");
		exit(0);
	}

	imgSize.width = atoi(argv[2]);
	imgSize.height = atoi(argv[3]);

	img = uc_alloc(imgSize.width, imgSize.height);
	read_ucmatrix(imgSize.width, imgSize.height, img, argv[1]);

	cvImg = cvCreateImage(imgSize, 8, 1);

	for (i = 0; i < imgSize.height; i++)
		for (j = 0; j < imgSize.width; j++)
		{
			((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];
		}
	cvNamedWindow(argv[1], 1);
	cvShowImage(argv[1], cvImg);

	get_hist(img, imgSize.width, imgSize.height);

	for (i = 0; i < imgSize.height; i++)
		for (j = 0; j < imgSize.width; j++)
			((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];

	cvShowImage("Histogram Equalizer .. . .. ", cvImg);

	get_hist1(img, imgSize.width, imgSize.height);


	cvWaitKey(0);

	cvDestroyWindow(argv[1]);
	cvReleaseImage(&cvImg);


	return 0;
}


// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
