﻿/*
10월 14일 히스토그램, equalization program
EX) 256 * 256 픽셀 수의 이미지인경우
영상은 다양한 화소 값(0~255)으로 이루어져있으며,
영상에 걸친 화소 값 분포는 영상의 중요한 특징을 구성(0의 개수, 1의 개수, ~ 255의 개수) -> 화소의 개수
예를 들어, 1채널 그레이레벨 영상에서 각 화소는 0~255 사이의 값을 갖는다.
영상 내용에 따라 영상 내부에 펼처진 그레이 음영의 양이 저마다 다르므로 여기서 히스토그램은 영상에서
특정 값을 갖는 화소 개수를 제공하게되는 단순한 테이블.

- 분포를 표현하는 histogram (PDF)
1. original Image인 경우 골고루 분포돼있는 형태
2. dark Image인 경우 0의 값 부분이 가장크며 그 후 떨어져 200대는 값이 없는 형태
3. Bright Image인 경우 0은 값이 없고 중간값 부터 계속 증가하는 형태

- 누적 분포를 표현하는 histogram (CDF)
 0의 값 + 1의 값 식으로 누적인형태를 보여줌
 256 256 크기의 이미지인 경우 256부분에서 누적 값이 1인 형태

- histogram equalization(평활화)
모여있는 히스토그램을 평활화 시켜서 전체를 고르게 만드는 방법
Input 값을 역함수 취해 평평한 형태로 만둠.
*/

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define unsigned char uchar

// 정수형 크기 설정 히스토그램(확률분포함수), CDF(누적분포함수), Equalization histogram
int histogram[256], cdfOfHisto[256], histogramEqual[256];

// 문자형 포인터 생성
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

void read_unmatrix(int size_x, int size_y, uchar** ucmatrix, char* filename)
{
	int i;
	FILE* f;

	if ((fopen_s(&f, filename, "rb")) != NULL)
	{
		printf("%s File open Error!\n", filename);
		exit(0);
	}

	for (i = 0; i < size_y; i++)
		if (fread(ucmatrix[i], sizeof(uchar), size_x, f) != size_x)
		{
			printf("Data Read Error!\n");
			exit(0);
		}
	fclose(f);
}

void write_ucmatrix(int size_x, int size_y, uchar** ucmatrix, char* filename)
{
	int i;
	FILE* f;

	if ((fopen_s(&f, filename, "wb")) != NULL)
	{
		printf("%s File open Error\n", filename);
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

// 히스토그램 이미지 출력(opencv 이미지로 출력)
void get_hist1(uchar** img, int X_Size, int Y_Size)
{
	int i, j, tmp;
	double tmp1;
	int t, tp, range, hrange;
	CvSize histoSize, cdfSize;
	IplImage* imgHisto, * cdfImgHisto;

	// 생성할 이미지 사이즈(원본 이미지, 누적분포 후의 이미지)
	histoSize.width = 256;
	histoSize.height = 256;
	cdfSize.width = 256;
	cdfSize.height = 256;

	imgHisto = cvCreateImage(histoSize, 8, 1);
	cdfImgHisto = cvCreateImage(cdfSize, 8, 1);
														//이미지 초기화
	for (i = 0; i < histoSize.height; i++)
		for (j = 0; j < histoSize.width; j++)
		{
			((uchar*)(imgHisto->imageData + imgHisto->widthStep * i))[j] = 0;
		}

	for (i = 0; i < cdfSize.height; i++)
		for (j = 0; j < histoSize.width; j++)
		{
			((uchar*)(cdfImgHisto->imageData + cdfImgHisto->widthStep * i))[j] = 0;
		}

	tp = X_Size * Y_Size;

	for (i = 0; i < 256; i++)				//히스토그램 값 초기화
		histogram[i] = 0;

	for (i = 0; i < Y_Size; i++)			//히스토그램 값 입력
		for (j = 0; j < X_Size; j++)
			histogram[img[i][j]]++;

	// Find the maximum histogram value 히히스토그램 최댓값 찾기 가장큰 값을 찾아냄
	tmp1 = 0;
	for (i = 0; i < 256; ++i)
	{
		tmp1 = tmp1 > histogram[i] ? tmp1 : histogram[i];
	}
	//최대 값을 1로 하고 나머지 값들을 최대 값에 비례해서 크기를 만듬 0~1
	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (histogram[i] / tmp1);

		cvLine(imgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
	}

	cvShowImage("Histo Equal ", imgHisto);         // 원본 히스토그램 출력


	//모든 히스토그램을 더하여 다음 히스토그램
	cdfOfHisto[0] = histogram[0];
	for (i = 1; i < 256; i++)
	{
		cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i];
	}
	// Draw the CDF of Histogram
	//최대 히스토그램 값을(중첩된 마지막 값) t,p로 지정
	tmp1 = (double)cdfOfHisto[255];
	//그와 비례하게 나누어 값 입력(0~1)
	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (cdfOfHisto[i] / tmp1);

		cvLine(cdfImgHisto, cvPoint(i, 255), cvPoint(i, 256 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
	}

	cvShowImage("CDF of Histogram ", cdfImgHisto); //누적 히스토그램

	range = cdfOfHisto[255] - cdfOfHisto[0];
	// printf("%d" %d \n", tp, range);

	//range == 히스토그램의 범위 
	histogramEqual[0] = 0;
	for (i = 0; i < 256; ++i) {
		t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range);
		histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t;
		
		//0~255사이로 값을 고정ㄴ
	}
	//히스토그램의 평활화ㄹㄴ 작업
	//ceil : 올림함수, 매개변수 실수형 1개

	cvReleaseImage(&imgHisto);						//메모리 해제
	cvReleaseImage(&cdfImgHisto);					//메모리 해제
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
	cdfSize.height = 256;

	imgHisto = cvCreateImage(histoSize, 8, 1);
	cdfImgHisto = cvCreateImage(cdfSize, 8, 1);

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
		for (j = 0; j < X_Size; j++)
			histogram[img[i][j]]++;
	
	tmp1 = 0;

	for (i = 0; i < 256; ++i)
	{
		tmp1 = tmp1 > histogram[i] ? tmp1 : histogram[i];
	}
	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (histogram[i] / tmp1);
		cvLine(imgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
	}
	cvShowImage("Histo Line", imgHisto);         // 히스토그램 equasl
	cdfOfHisto[0] = histogram[0];
	for (i = 0; i < 256; i++)
		cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i];

	//Draw the CDF of Histogram
	tmp1 = (double)cdfOfHisto[255];
	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (cdfOfHisto[i] / tmp1);

		cvLine(cdfImgHisto, cvPoint(i, 255), cvPoint(i, 256 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
	}
	cvShowImage("Original CDF of Histogram", cdfImgHisto);

	range = cdfOfHisto[255] - cdfOfHisto[0];
	histogramEqual[0] = 0;
	for (i = 1; i < 256; ++i) {
		t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range);
		histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t;
	}
	cvReleaseImage(&imgHisto);
	for (i = 0; i < Y_Size; ++i)
		for (j = 0; j < X_Size; ++j)
			img[i][j] = histogramEqual[img[i][j]];
}


void get_hist2(uchar** img, int X_Size, int Y_Size,int *tempHisto)
{
	int i, j, tmp;
	double tmp1;
	int t, tp, range, hrange;
	CvSize histoSize, cdfSize;
	IplImage* imgHisto, * cdfImgHisto;

	histoSize.width = 256;
	histoSize.height = 256;

	cdfSize.width = 256;
	cdfSize.height = 256;

	imgHisto = cvCreateImage(histoSize, 8, 1);
	cdfImgHisto = cvCreateImage(cdfSize, 8, 1);

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
		for (j = 0; j < X_Size; j++)
			histogram[img[i][j]]++;

	tmp1 = 0;

	for (i = 0; i < 256; ++i)
	{
		tmp1 = tmp1 > histogram[i] ? tmp1 : histogram[i];
	}
	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (histogram[i] / tmp1);
		cvLine(imgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
	}
	cvShowImage("TestTest Histo Line", imgHisto);         // 히스토그램 equasl
	cdfOfHisto[0] = histogram[0];
	for (i = 0; i < 256; i++)
		cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i];

	for (i = 0; i < 256; i++)
		tempHisto[i] = cdfOfHisto[i];
		//tempHisto[i] = histogram[i];

	//Draw the CDF of Histogram
	tmp1 = (double)cdfOfHisto[255];
	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (cdfOfHisto[i] / tmp1);

		cvLine(cdfImgHisto, cvPoint(i, 255), cvPoint(i, 256 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
	}
	cvShowImage("TESt CDF of Histogram", cdfImgHisto);

	range = cdfOfHisto[255] - cdfOfHisto[0];
	histogramEqual[0] = 0;
	for (i = 1; i < 256; ++i) {
		t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range);
		histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t;
	}
	cvReleaseImage(&imgHisto);
}

/*

 원하는 히스토그램을 입력해서 원본 이미지와 입력 히스토그램과 같게 만듬.



*/
void get_Match(uchar** img, int X_Size, int Y_Size, int histogramSpec[256], int histogramMatch[256])
{
	int i, j, tmp, matchz = 0;
	float diff;

	printf("Start HistoGram Specification \n");

	for (i = 0; i < 256; i++)
	{
		histogramMatch[i] = 0;
		for (j = 0; j < 256; j++)
		{
			if ((i - histogramSpec[j]) > 0)
				histogramMatch[i] = j;
		}
	}
	/*
	for (i = 0; i < 256; ++i)
	{
		diff = abs(i - histogramSpec[0]);
		matchz = 0;
		for (j = 0; j < 256; ++j)
		{
			if (abs(i - histogramSpec[j]) < diff)
			{
				diff = abs(i - histogramSpec[j]);
				matchz = j;
			}
		}
		histogramMatch[i] = (uchar)matchz;
	}
	*/
	for (i = 0; i < Y_Size; ++i)
		for (j = 0; j < X_Size; ++j)
			img[i][j] = histogramMatch[img[i][j]];

}


int main(int argc, char* argv[])
{
	int i, j, tmp;

	double tmp1;

	int tempHisto[256],tempHisto2[256],resHisto[256];

	IplImage* cvImg,*testimg;
	CvSize imgSize,imgSize2, histoSize;
	uchar** img,**img2;
	if (argc != 7)
	{
		printf("arg error \n");
		exit(0);
	}
	imgSize.width = atoi(argv[2]);
	imgSize.height = atoi(argv[3]);

	imgSize2.width = atoi(argv[5]);
	imgSize2.height = atoi(argv[6]);



	img = uc_alloc(imgSize.width, imgSize.height);

	img2 = uc_alloc(imgSize2.width, imgSize2.height);

	read_unmatrix(imgSize.width, imgSize.height, img, argv[1]);


	read_unmatrix(imgSize2.width, imgSize2.height, img2, argv[4]);


	cvImg = cvCreateImage(imgSize, 8, 1);
	for (i = 0; i < imgSize.height; i++)
		for (j = 0; j < imgSize.width; j++)
		{
			((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];
		}
	cvNamedWindow(argv[1], 1);

	cvShowImage(argv[1], cvImg);
	/*
	get_hist(img, imgSize.width, imgSize.height);
	for (i = 0; i < imgSize.height; i++)
		for (j = 0; j < imgSize.width; j++)
		{
			((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];
		}
	cvShowImage("Histogram Equalizer...", cvImg);

	*/
	//get_hist1(img, imgSize.width, imgSize.height);

	//get_hist1(img2, imgSize2.width, imgSize2.height);

	//get_hist2(img, imgSize.width, imgSize.height, tempHisto);

	get_hist2(img2, imgSize2.width, imgSize2.height, tempHisto2);

	histoSize.width = 256;
	histoSize.height = 256;

	testimg = cvCreateImage(histoSize, 8, 1);

	tmp1 = (double)cdfOfHisto[255];
	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (cdfOfHisto[i] / tmp1);
		cvLine(testimg, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
	}
	cvShowImage("TNEOEOE", testimg);         // 히스토그램 equasl
	
	get_Match(img, imgSize.width, imgSize.height, histogramEqual, tempHisto2);


	for (i = 0; i < imgSize.height; i++)
		for (j = 0; j < imgSize.width; j++)
		{
			((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];
		}
		
	cvShowImage("TestTest...", cvImg);

	get_hist1(img, imgSize.width, imgSize.height);
	
	


	cvWaitKey(0);
	cvDestroyWindow(argv[1]);
	cvReleaseImage(&cvImg);
	return 0;
}
