/*

	1. cdf를 이미지에 대응하면서 동시에 역변환을 수행함..
	2. 첫 역변환 수행시 이미지는 평활화된 cdf를 갖음
	3. 평활화된 이미지에 원하는 영상의 cdf를 적용하면 그영상의 cdf를 갖음
		3-1. cdf를 적용시 역변환한 cdf를 적용해야 하므로 지정영상의 cdf의 역변환을 평활화된 영상에 대입함.
*/



/*
히스토그램 매칭 과정
1. 입력영상의 히스토그램
2. 입력영상의 누적분포함수(CDF)
3. 타겟영상의 히스토그램
4. 타겟영상의 CDF
5. 타겟영상 CDF의 역변환
6. 입력영상 CDF 평활화
7. 평활화된 입력영상에 타겟영상 CDF 역변환 적용


*/

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define unsigned char uchar

// 정수형 크기 설정 히스토그램(확률분포함수), CDF(누적분포함수), 역변환된(CDF)
int histogram[256], cdfOfHisto[256], histogramEqual[256], tmpCDF[256], val=0,mytemp;

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
void get_hist1(uchar** img, int X_Size, int Y_Size, int mod)
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
			((uchar*)(imgHisto->imageData + imgHisto->widthStep * i))[j] = 0;

	for (i = 0; i < cdfSize.height; i++)
		for (j = 0; j < cdfSize.width; j++)
			((uchar*)(cdfImgHisto->imageData + cdfImgHisto->widthStep * i))[j] = 0;


	tp = X_Size * Y_Size;

	for (i = 0; i < 256; i++)				//히스토그램 배열 초기화
		histogram[i] = 0;

	//img[i][j]가 갖고 있는 그레이 레벨에 해당하는 히스토그램 값을 ++함
	for (i = 0; i < Y_Size; i++)			
		for (j = 0; j < X_Size; j++)
			histogram[img[i][j]]++;



	// Find the maximum histogram value 히히스토그램 최댓값 찾기(정규화를 위해)
	tmp1 = 0;
	for (i = 0; i < 256; ++i)
	{
		tmp1 = tmp1 > histogram[i] ? tmp1 : histogram[i];
	}


	//최대 값에 비례해서 정규화 (0~255)
	printf("\n\nHisto %d \n\n\n", mod);
	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (histogram[i] / tmp1);

		cvLine(imgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
	}

	switch (mod)
	{
	case 0:
		cvShowImage("Original Histogram", imgHisto);
		break;
	case 1:
		cvShowImage("Target Histogram", imgHisto);
		break;
	case 2:
		cvShowImage("Result Histogram", imgHisto);
		break;
	}


	//모든 히스토그램을 더하여 CDF 만들기
	cdfOfHisto[0] = histogram[0];
	printf("\n\ncdfHisto %d \n\n\n", mod);
	for (i = 1; i < 256; i++)
	{
		cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i];
		printf("%d, ", cdfOfHisto[i]);
	}
	// Draw the CDF of Histogram
	//최대 히스토그램 값을(중첩된 마지막 값) tmp로 지정
	tmp1 = (double)cdfOfHisto[255];

	//히스토그램 정규화(0~255) 및 출력
	printf("\n\ncdfHisto Temp%d \n\n\n", mod);
	for (i = 0; i < 256; ++i)
	{
		
		tmp = (int)255 * (cdfOfHisto[i] / tmp1);
		printf("%d ,", tmp);
		cvLine(cdfImgHisto, cvPoint(i, 255), cvPoint(i, 256 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
		tmpCDF[i] = tmp;
	}
	switch (mod)
	{
	case 0 :
		cvShowImage("Original CDF of Histogram", cdfImgHisto);
		break;
	case 1 :
		cvShowImage("Target CDF of Histogram", cdfImgHisto);
		break;
	case 2 :
		cvShowImage("Result CDF of Histogram", cdfImgHisto);
		break;
	}
	
	range = cdfOfHisto[255] - cdfOfHisto[0];
	// printf("%d" %d \n", tp, range);

	//range == 히스토그램의 범위 

	printf("\n\nHistoEqual %d \n\n\n", mod);
	histogramEqual[0] = 0;
	for (i = 0; i < 256; ++i) {
		t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range);
		histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t;
		printf("%d, ", histogramEqual[i]);
		//정규화.
	}
	
	for (i = 0; i < 256; i++)
	{
		if (histogramEqual[i] > 127)
		{
			val = i;
			break;
		}
	}
	//히스토그램의 평활화ㄹㄴ 작업

	

	//if(mod==0) cvShowImage("역변환", rvsHisto);

	cvReleaseImage(&imgHisto);						//메모리 해제
	cvReleaseImage(&cdfImgHisto);					//메모리 해제

	//mod 0일 때 cdf를 역변환하여 대입 평활화 수행(오리지날 이미지)
	//대입하는 것 자체가 역변환 하는 동작을 수행함
	if (mod == 0)									
		for (i = 0; i < Y_Size; ++i)				
			for (j = 0; j < X_Size; ++j)
				img[i][j] = histogramEqual[img[i][j]];
	
}



void get_Match(uchar** img, int X_Size, int Y_Size, int histogramSpec[256])
{
	int i, j, tmp, matchz = 0;
	int histogramMatch[256];
	float diff;

	CvSize matchSize;
	IplImage* matchImg;

	matchSize.width = 256;
	matchSize.height = 256;


	
	matchImg = cvCreateImage(matchSize, 8, 1);
	//이미지 초기화
	for (i = 0; i < matchSize.height; i++)
		for (j = 0; j < matchSize.width; j++)
			((uchar*)(matchImg->imageData + matchImg->widthStep * i))[j] = 0;


printf("Start HistoGram Specification \n");

//지정영상의 CDF를 다시 역변환하는 과정
for (i = 0; i < 256; i++)
{
	histogramMatch[i] = 0;
	for (j = 0; j < 256; j++)
		if ((i - histogramSpec[j]) > 0)
			histogramMatch[i] = j;
}

for (i = 0; i < 256; ++i)
	cvLine(matchImg, cvPoint(i, 255), cvPoint(i, 255 - histogramMatch[i]), CV_RGB(255, 255, 255), 1, 8, 0);

cvShowImage("역변환된 히스토그램", matchImg);

//역변환하며 이미지 대응
for (i = 0; i < Y_Size; ++i)
	for (j = 0; j < X_Size; ++j)
		img[i][j] = histogramMatch[img[i][j]];


//역변환하며 이미지 대응
/*for (i = 0; i < Y_Size; ++i)
	for (j = 0; j < X_Size; ++j)
		img[i][j] = histogramSpec[img[i][j]];
		*/

}

void constrastStreching(uchar** img, uchar** outImg, int X_Size, int Y_Size)
{
	int i, j, chistogram[256];
	int min = 255, max = 0;
	uchar LUT[256];
	double scaleFactor, tmp;
	for (i = 0; i < 256; i++)
		chistogram[i] = 0;

	for (i = 0; i < Y_Size; i++)
		for (j = 0; j < X_Size; j++)
			chistogram[img[i][j]]++;

	for (i = 0; i < 256; i++)
		if (chistogram[i])
		{
			min = i;
			break;
		}

	for (i = 255; i = 0; i--)
		if (chistogram[i])
		{
			max = i;
			break;
		}

	printf("Low Threshold is %d High Threshold is %d \n", min, max);
	for (i = 0; i < min; i++)
		LUT[i] = 0;
	for (i = 255; i >= max; i--)
		LUT[i] = 255;

	scaleFactor = 255.0 / (double)(max - min);

	for (i = min; i < max; i++)
	{
		tmp = (i - min) * scaleFactor;

		if (tmp < 0) tmp = 0;
		if (tmp > 255) tmp = 255;

		LUT[i] = (uchar)tmp;
	}

	for (i = 0; i < Y_Size; i++)
		for (j = 0; j < X_Size; j++)
			outImg[i][j] = LUT[img[i][j]];
}

double average(uchar** img, int row, int col)
{
	double sum = 0, avg;
	int i, j;

	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
			sum += img[i][j];
	}

	avg = sum / ((double)row * col);

	printf("avg =%lf\n", avg);

	return avg;
}

int main(int argc, char* argv[])
{
	int i, j;
	IplImage* cvImg, * cvImg2;
	CvSize imgSize, imgSize2;
	uchar** img, ** img2, ** outimg;
	if (argc != 7)
	{
		printf("arg error \n");
		exit(0);
	}
	imgSize.width = atoi(argv[2]);
	imgSize.height = atoi(argv[3]);
	img = uc_alloc(imgSize.width, imgSize.height);
	read_unmatrix(imgSize.width, imgSize.height, img, argv[1]);

	if (atoi(argv[4]) == -2)			//중간시험
	{
		get_hist1(img, imgSize.width, imgSize.height, 1);

		cvImg = cvCreateImage(imgSize, 8, 1);

		printf("\n\nHistoEQUAL == %d, average == % lf\n",val, average(img,imgSize.width,imgSize.height));

		for(i=0;i< imgSize.width;i++)
			for (j = 0; j < imgSize.height; j++)
			{
				if (img[i][j] < histogramEqual[128])
					img[i][j] = 0;
				else
					img[i][j] = 255;
			}
		cvNamedWindow(argv[1], 1);				//윈도우 열기

		cvShowImage(argv[1], cvImg);			//이미지 열기

		for (i = 0; i < imgSize.height; i++)
			for (j = 0; j < imgSize.width; j++)
				((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];

		cvShowImage("go", cvImg);			//이미지 열기
	}

	else if(atoi(argv[4])==-1)
	{
		printf("equal\n		");
		cvImg = cvCreateImage(imgSize, 8, 1);
		for (i = 0; i < imgSize.height; i++)
			for (j = 0; j < imgSize.width; j++)
				((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];

		// 원본 히스토그램 출력
		get_hist1(img, imgSize.width, imgSize.height, 0);			//원본 히스토그램 출력 및 평활화

		cvNamedWindow(argv[1], 1);				//윈도우 열기

		cvShowImage(argv[1], cvImg);			//이미지 열기

		for (i = 0; i < imgSize.height; i++)
			for (j = 0; j < imgSize.width; j++)
				((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];

		cvShowImage("histoequals...", cvImg);			//이미지 열기

		get_hist1(img, imgSize.width, imgSize.height, 2);
	}

	else 
	{
		printf("else...\n");
		imgSize2.width = atoi(argv[5]);
		imgSize2.height = atoi(argv[6]);
		img2 = uc_alloc(imgSize2.width, imgSize2.height);
		read_unmatrix(imgSize2.width, imgSize2.height, img2, argv[4]);

		outimg = uc_alloc(imgSize.width, imgSize.height);

		cvImg = cvCreateImage(imgSize, 8, 1);

		//	constrastStreching(img, outimg, imgSize.width, imgSize.height);


		for (i = 0; i < imgSize.height; i++)
			for (j = 0; j < imgSize.width; j++)
				((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];

		// 원본 히스토그램 출력
		get_hist1(img, imgSize.width, imgSize.height, 0);			//원본 히스토그램 출력 및 평활화

		cvNamedWindow(argv[1], 1);				//윈도우 열기

		cvShowImage(argv[1], cvImg);			//이미지 열기

		cvImg2 = cvCreateImage(imgSize2, 8, 1);	//이미지 생성2

		//타겟 이미지 입력
		for (i = 0; i < imgSize2.height; i++)
			for (j = 0; j < imgSize2.width; j++)
				((uchar*)(cvImg2->imageData + cvImg2->widthStep * i))[j] = img2[i][j];
		//타겟 이미지 출력
		cvShowImage(argv[4], cvImg2);


		//타겟 히스토그램 출력
		get_hist1(img2, imgSize2.width, imgSize2.height, 1);

		//역변환된 타겟 히스토그램 cdf를 평활화된 원본 이미지에 대입
//		get_Match(img, imgSize.width, imgSize.height, histogramEqual);
		get_Match(img, imgSize.width, imgSize.height, tmpCDF);

		for (i = 0; i < imgSize.height; i++)
			for (j = 0; j < imgSize.width; j++)
				((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];

		cvShowImage("Histogram match...", cvImg);
		get_hist1(img, imgSize.width, imgSize.height, 2);
	}
	cvWaitKey(0);
	cvDestroyWindow(argv[1]);
	cvReleaseImage(&cvImg);
	return 0;
}
