/*
	--  19년 9월~ 영상처리 실습 프로그램

	--  ROW, COLUM
		row -> 행(가로) // colum -> 열(세로) 
		row count == 행 개수 (가로 개수) ==  y 축의 값
		colum count == 열 개수 (세로 개수) == x 축의 값

	--  프로그램 매개변수 
		myImgEdit.exe / (모드) /  (그림이름) /  (저장할 이름) / (row) / (col) / (매개변수 1) /

	--  모드							-- 매개변수
		0	:	반전 (negative)				x									0->255, 255->0
		1	:	전체모자이크 (mosaic)		블럭 크기(2의 지수승이 아님 고침)	해당 블럭 평균 값으로 블럭 내 픽셀 통일
		2	:	바이너리 만들기				x									평균 이상-> 255, 평균 미만->0
		3	:	바이너리 만들기 2			바이너리로 만들지 않을 범위(0~255)	매개변수-50 ~ 매개변수+50을 제외하고 모두 0으로
		4	:	감마 조절					감마값(default 1.0,0~10)			감마에 따라 이미지 전체 값 조절
		5	:	평균 감마 조절				x									전체 이미지의 평균값이 128(중간값)이 될 수 있도록 감마 조절
		6	:	비트 이미지 출력			출력가능하게 할 비트 값(0~7)		픽셀별로 0~255값의 이미지를 2진수로 변환하여 해당 비트가
																				1인 경우에만 그 픽셀의 값을 모두 255로 만들고 아닐 경우 0으로 출력
		7	:	비트 이미지 출력2			버릴 비트 값(0~7)					하위비트부터 내가 입력한 비트까지의 비트 이미지를 버리고
																				나머지 비트를 합성한 비트 이미지를 출력하게 한다.
																				0x01의 mask를 원하는 비트마다 비트이동하여 & 연산으로
																				원하는 비트만 남겨놓고 나머지는 제거한다
		8	:	원 그리기					그려질 원의 색(0~255)				원하는 좌표, 반지름을 입력받아서 해당 좌표에서 반지름만큼
																				떨어져 있는 모든 픽셀을 매개변수로 입력받은 색으로 칠한다.
		9	:	원형 모자이크				모자이크 부분 선택(0,1)				모자이크 사이즈, 좌표, 반지름 입력받아 좌표로부터 입력받은
																				반지름을 갖는 원을 그려 그 픽셀을 모자이크 하거나 해당 픽셀을
																				제외한 전체 부분을 모자이크 처리함.
		10	:	가우시안 난수발생 이미지										원래 영상을 가우시안 난수가 들어간 이미지로 출력, 저장
		11	:	가우시안 난수 영상 덧셈		더할 페이지 수						가우시안 난수를 발생 시킨 영상을 원하는 페이지 만큼 더한다.
																				많이 더할수록 가우시안 노이즈가 줄어든 영상이 출력된다.

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define uchar unsigned char
#define MAXrange 256
#define MAX 255



uchar** uc_alloc(int size_x, int size_y)
{  
	uchar** m;
	int i;

	/*malloc과 calloc의 차이점 -->>>>> 

	대부분 포인터에 할당을 받으므로 사용하는 포인터에 대한 값에 맞게 형 변환 해주어야 합니다
		void malloc(size_t size)				->>>			1개의 인자만을 받고 받은 인자만큼의 메모리를 할당/초기화 없음
			### pt= malloc(15*sizeof(int);
				
		void calloc(size_t num, size_t bytes)	->>				2개의 인자를 받고 num * size 만큼의 메모리를 할당/ 해당 힙 메모리를 0으로 초기화
			### pt= calloc(15,sizeof(int);

		### calloc은 메모리 할당시 2개의 인자를 받고 overflow를 검사!!! 오버플로우 발생시 NULL 리턴해줍니다.
		### malloc을 사용하면서 메모리를 초기화 하려면 
		### memset(void *ptr, int value, size_t num);
		### -->> void * 형은 어떠한 값(포인터형으로)도 매개변수로 받아올 수 있습니다. 
		###int value에 초기화 값, size_t num에 초기화를 원하는 영역(메모리)

	*/

		
	/*
		-	포인터에 대하여

		&p	=>	해당 포인터의 주솟값
		p	=>	포인터기 저장하고 있는 실제 주소값
		*p  =>  포인터가 저장하고 있는 주소값의 실제값

		**형 => *형의 주소값 저장 ( *형으로 *형 저장x)
		*/
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

int** i_alloc(int size_x, int size_y)
{
	int** m;
	int i;
	if ((m = (int* *)calloc(size_y, sizeof(int*))) == NULL)
	{
		printf("i_alloc error 1\7\n");
		exit(0);
	}

	for (i = 0; i < size_y; i++)
		if ((m[i] = (int*)calloc(size_x, sizeof(int))) == NULL)
		{
			printf("i_alloc error 2\7\n");
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

	//ucmatrix : 이미지를 메모리에 올리기 위한 공간, unsigned character matrix
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

void Negative(int row, int col, uchar** img, uchar** res)
{
	int i, j;
	for (i = 0; i < col; i++)
		for (j = 0; j < row; j++)
			res[i][j] = 255 - img[i][j];

	printf("Negative done...");
	return;
}

void Mosaic(int row, int col, uchar** img, uchar** res, int block)
{
	int i, j, x, y, tmp, count;
				//row의 제한 개수만큼 반복하되 block크기만큼에 한번만
				//col도 block 크기에 한번만 검사
	/*
				tmp		:	지정한 블럭 공간에 있는 값의 평균을 내기 위한 합계값
				count	:	지정한 블럭 공간에 있는 값의 평균을 내기 위한 갯수 카운트 값

				블럭 내의 모든 픽셀을 검사하기 위해 x블럭 * y블럭 만큼 반복
				픽셀을 검사하며 해당 픽셀의 값을 tmp에 합계하고 count를 1만큼 올림

				최대 row와 col 초과를 방지하기 위한 조건문을 걸어 초과시 break
	*/
	for (i = 0; i < row; i += block)																	
		for (j = 0; j < col; j += block)
		{
			tmp = 0;
			count = 0;
			
			for (y = 0; y < block && i + y < row - 1; y++)
				for (x = 0; x < block && j+x<col-1; x++, count++)
					tmp += img[i + y][j + x];		

			/*
				tmp(합계)를 count(픽셀 개수)로 나누어 블럭 내 픽셀의 평균 값을 구함
				이후 구해진 값을 블럭 내 모든 픽셀에 입력해 블럭 내 모든 픽셀이 같은 값을 갖게함.
				최대 row와 col 초과시 break.
			*/
			tmp /= count;
			for (y = 0; y < block && i + y < row - 1; y++)
				for (x = 0; x < block && j + x < col - 1; x++, count++)
					res[i + y][j + x] = tmp;	
		}
	printf("Mosaic block %d done... ",block);
	return;
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

double average2(uchar** img, int row, int col)
{
	double avg=0, size;
	int numDot[MAXrange] = { 0 };
	int i, j, k;

	size = row * col;

	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
			for (k = 0; k < MAXrange; k++)
				if (k == img[i][j])
				{
					numDot[k]++;
					break;
				}

	for (k = 0; k < MAXrange; k++)
 	{
		printf("%d = %d, ", k, numDot[k]);
		printf("%d = %lf, ", k, (double)numDot[k] / size);
		avg += (double)numDot[k]*(double)k / size;
	}
	printf("avg= %lf", avg);
	/*for (k = 0; k < MAXrange; k++)
	{
		((k*k)-(avg*avg))*numDot[k]
	}
	*/
	return avg;
}

void makeBinary(uchar** img, uchar** res, int row, int col, double avg)
{
	int i, j;

	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
		{
			if (img[i][j] > avg) res[i][j] = 255;
			else res[i][j]=0;
		}
	printf("Binary %lf done...",avg);
	return; 
}

void adaptiveBinary(uchar** img, uchar** res, int row, int col, double val)
{
	int i, j;

	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
		{
			if (img[i][j] > (val - 50) && img[i][j] < (val + 50)) res[i][j] = img[i][j];
			else res[i][j] = 0;
		}
	printf("adaptive binary +-%lf done...", val);
}

void powImg(uchar** img, uchar** res, int row, int col, double gamma)
{
	int i, j;
	double tmp;

	for(i=0;i<row;i++)
		for (j = 0; j < col; j++)
		{
			tmp = pow(img[i][j] / 255., 1 / gamma);

			// tmp = 해당 픽셀의 값을 255로 나눈후 (결과 0~1), ^(1/매개변수)의 연산
			// 감마의 역수 승만큼 제곱하므로 감마가 작을 수록 높은 tmp 의 값 (0.0~ 1.0)^ (1/gamma)
			

			if (tmp * 255 > 255)	tmp = 1;
			else if (tmp * 255 < 0)	tmp = 0;

			//tmp 값이 1 초과이거나 0 미만일 경우 각각 0,1 로 초기화 시켜줌
			
			res[i][j] = tmp * 255;

			//나누어진 255 값을 다시 곱하여 줌
			
		}
	printf("gamma changed by %lf ", gamma);
	return;
}
double powAvgImg(uchar** img, uchar** res, int row, int col)
{
	int i, j;
	double tmp,k;

	/*
		k= 감마 , 감마를 0부터 10까지 대입해서 
		출력되는 이미지의 평균이 127.5초과 128.5 미만이 될 때
		반복문 동작을 멈춤. 출력되는 이미지는 결과 이미지에 자동으로 입력
		감마값 반환
	*/

	for (k = 0.0; k < 10; k += 0.001)		
	{
		for (i = 0; i < row; i++)
			for (j = 0; j < col; j++)
			{
				tmp = pow(img[i][j] / 255., 1 / k);

				if (tmp * 255 > 255)	tmp = 1;
				else if (tmp * 255 < 0)	tmp = 0;

				res[i][j] = tmp * 255;
			}
		if (average(res, row, col) > 127.5 && average(res, row, col) < 128.5)
			break;
	}
	printf("auto gamma changed by %lf ", k);
	return k;
}


void bitSlicing(uchar** img, uchar** res, int row, int col, int position)
{
	int i, j;
	uchar mask = 0x01;
	mask <<= position;
	/*
		비트연산 - mask값(0000 0001)을 입력하는 매개변수만큼 (1~7)*/


	for(i=0; i< row; i++)
		for (j = 0; j < col; j++)
		{
			if ((mask & img[i][j]) == mask) // =pow(2, position) 제곱
				res[i][j] = 255;
			else
				res[i][j] = 0;
		}
	printf("bit sliced %d...",position);
	return;
}


void bitSlicing2(uchar** img, uchar** res, int row, int col, int count)
{
	int i, j, pos,temp;
	uchar mask = 0x01;

	//for (i = 0; i < row; i++)
		//for (j = 0; j < col; j++)
			//res[i][j] = 0;
	
	for (pos = 7; pos >= count; mask = 0x01, pos--)
	{
		mask <<= pos; // mask == mask << pos;
		printf("pos= %d, ", pos);
		printf("mask=%d \n", mask);

		for (i = 0; i < row; i++)
			for (j = 0; j < col; j++)
				if ((mask & img[i][j]) == mask)
					res[i][j] += mask;
		//해당 코드 구현은 마스크 값검사를 통해서 각 비트 검사 후 해당 비트에 맞는 값 
		//8(번째비트,pos7)(1000 0000) 7(0100 0000) 이런식으로 구해져서 결과가
		//1000 0000 + 0100 0000 + 0010 0000 + 0001 0000 + 0000 1000 ,,,, 의 결과가 출력된다.
		//그래서,,, 8비트 페이지만 남긴 최종 결과물에는 (pos 7) 128의 값(1000 0000)

		//각 비트 페이지 별로 반으로 나눈 걸 더하면 다음과 같이더해진다-->> 반으로 나누어 출력되는 값,,,
		//res8(비트,pos7)/(2^0)+ res7/(2^1) + res6/(2^2) + res5/(2^3) +res4/(2^4) + res3/(2^5) .. . .. 
		//0111 1111 + 0011 1111 + 0001 1111 + 0000 1111 + 0000 0111 ... 의 결과가 출력된다
		//8비트 페이지만 남긴 최종 결과물은 (pos 7) 127(0111 1111)
	}  
	printf("bit sliced2");
	return;
}

void circle(uchar** img, uchar** res,int row, int col, int mod)
{
	int i, j,locY,locX;
	double tmp, xSquare, ySquare,diameter=0;

	locY = row / 2;
	locX = col / 2;

	printf("Insert prameter (diameter Xlocation Ylocation) : ");
	scanf_s("%lf", &diameter);
	scanf_s("%d", &locX);
	scanf_s("%d", &locY);

	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
		{													//abs	== 절대값
			ySquare = (abs(locY - i)) * (abs(locY - i));	//ySquare == (abs(-256~255))^2 ->> 0~255^2 (row,col이 512일)
																//row/2 --> y축 중앙 점
			xSquare = (abs(locX - j)) * (abs(locX - j));	//xSquare == (abs(-256~255))^2 ->> 0~255^2 
			//제곱의 과정인데 절대값을 넣는 이유??
			
			//각각의 픽셀이 중앙으로부터 얼마나 멀어져 있는지를 측정하게위한
			//중앙 점에서  ysquare/xsquare row와 col의 중앙 거리 계산
			//피타고라스의 정리 = a^2 +b^2 = c^2

			tmp = sqrt(ySquare + xSquare); 
			
			if (diameter<tmp + 1 && diameter>tmp - 1)
				res[i][j] = mod;
			else
				res[i][j] = img[i][j];
			// tmp == 각각의 픽셀이 한가운데의 중앙 점으로부터 유격되어 있는 정도.
			//빗변의 길이 = (a^2+b^2)의 제곱근 = tmp = (ysquare +xsquare) 제곱근
		}

}

void circleMosaic(uchar** img, uchar** res, int row, int col, int mod)
{
	int i, j,sum,count,x,y,block = 16,locX= 0,locY= 0;
	double tmp, xSquare, ySquare, diameter = 0;

	locY = row / 2;
	locX = col / 2;

	printf("Insert prameter (blocksize diameter Xlocation Ylocation) : ");
	scanf_s("%d", &block);
	scanf_s("%lf", &diameter);
	scanf_s("%d", &locX);
	scanf_s("%d", &locY);

	//i = 블럭 단위 row, j = 블럭 단위 col

	for (i = 0; i < row; i += block)
		for (j = 0; j < col; j += block)
		{
			sum = 0;
			count = 0;

			for (y = 0; y < block && i + y < row - 1; y++)
				for (x = 0; x < block && j + x < col - 1; x++, count++)
					sum += img[i + y][j + x];

			/*
				모자이크를 위한 작업
				tmp(합계)를 count(픽셀 개수)로 나누어 블럭 내 픽셀의 평균 값을 구함
				이후 구해진 값을 블럭 내 모든 픽셀에 입력해 블럭 내 모든 픽셀이 같은 값을 갖게함.
				최대 row와 col 초과시 break.
			*/
			sum /= count;
			for (y = 0; y < block && i + y < row - 1; y++)
				for (x = 0; x < block && j + x < col - 1; x++, count++)
				{
					ySquare = (abs(locY - (i + y))) * (abs(locY - (i + y)));
					xSquare = (abs(locX - (j + x))) * (abs(locX - (j + x)));

					tmp = sqrt(ySquare + xSquare);

					/*
						mod에 따라 모자이크 작업된 픽셀을 사용할 지
						원래 픽셀을 그대로 작업할 지 결과 출력
					*/

					if (mod)
					{
						if (tmp > diameter)
							res[i + y][j + x] = sum;
						else
							res[i + y][j + x] = img[i + y][j + x];
					}
					else
					{
						if (tmp < diameter)
							res[i + y][j + x] = sum;
						else
							res[i + y][j + x] = img[i + y][j + x];
					}
				}
		}
	printf("Mosaic block %d done... ", block);
	return;
			//abs	== 절대값
				//xSquare == (abs(-256~255))^2 ->> 0~255^2 
			//제곱의 과정인데 절대값을 넣는 이유??

			//각각의 픽셀이 중앙으로부터 얼마나 멀어져 있는지를 측정하게위한
			//중앙 점에서  ysquare/xsquare row와 col의 중앙 거리 계산
			//피타고라스의 정리 = a^2 +b^2 = c^2

			

			// tmp == 각각의 픽셀이 한가운데의 중앙 점으로부터 유격되어 있는 정도.
			//빗변의 길이 = (a^2+b^2)의 제곱근 = tmp = (ysquare +xsquare) 제곱근
}

double uniform()
{
	return((double)(rand() % RAND_MAX) / RAND_MAX - 0.5);
	//가우시안 난수 발생을 위한 값 
}

//가우시안 난수 발생
double gaussian()
{

	static int ready = 0;
	static double gstore;
	double v1=0, v2=0, r, fac, gaus;
	double uniform();

	//printf("%lf\n", uniform());
	if (ready == 0) {

		do {
			
			v1 = 2. * uniform();
			v2 = 2. * uniform();
			r = v1 * v1 + v2 * v2;
			//printf(" r= %lf", r);
		} while (r > 1.0);

		fac = sqrt(-2. * log(r) / r);
		gstore = v1 * fac;
		gaus = v2 * fac;
		ready = 1;

	}
	else {
		ready = 0;
		gaus = gstore;
	}
	//printf("%lf ", gaus);
	return (gaus);
}

//모든 이미지에 가우시안 난수(-1~1 * 50 )의 값을 추가함( 오버 및 언더플로 방지를 위한 정수형 더블포인터 결과 받음)
double noisedImage(uchar** img, int** res, int row, int col)
{
	double sum = 0;
	int i, j;

	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
			res[i][j] = img[i][j] + ((int)gaussian()*50);


	return 0;
}
//원하는 페이지 만큼 가우시안 난수 발생
void noisedImage2(uchar** img, uchar** res, int row, int col, int page)
{
	int** tmp;
	int** tmp2;
	int i, j, k;

	
	tmp = i_alloc(row, col);
	tmp2 = i_alloc(row, col);

	for (k = 0; k < page; k++)
	{
		noisedImage(img, tmp2, row, col);
		for (i = 0; i < row; i++)
			for (j = 0; j < col; j++)
				tmp[i][j] += tmp2[i][j];
	}
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
			res[i][j] = (tmp[i][j]/page);

}




void convolution(double** h, int F_length, int size_x, int size_y, uchar** img, uchar** res)
{
	int i, j, x, y;

	int margin, indexX, indexY;
	double sum, coeff;

	//여백 공간
	margin = (int)(F_length / 2);	

	for (i = 0; i < size_y; i++)
		for (j = 0; j < size_x; j++)
		{
			sum = 0;
			for (y = 0; y < F_length; y++)
			{
				indexY = i - margin + y;
				if (indexY < 0)
					indexY = -indexY;
				else if (indexY >= size_y)
					indexY = (2 * size_y - indexY - 1);
				for (x = 0; x < F_length; x++)
				{
					indexX = j - margin + x;
					if (indexX < 0)
						indexX = -indexX;
					else if (indexX >= size_x)
						indexX = (2 * size_x - indexX - 1);

					sum += h[y][x] * (double)img[indexY][indexX];

				}
			}

			//sum+=128;

			if (sum < 0)
				sum = 0;
			else if (sum > 255)
				sum = 255.;
			res[i][j] = (uchar)sum;
		}
}

void make_Mask(int mSize, double** mask, int flag)
{

	int i, j;

	double gausMask[3][3] =		{ 1 / 16., 2 / 16., 1 / 16.,
								  2 / 16., 4 / 16., 2 / 16.,
								  1 / 16., 2 / 16., 1 / 16. };
	double aveMask[3][3] =		{ 1 / 9., 1 / 9., 1 / 9.,
								 1 / 9., 1 / 9., 1 / 9.,
								 1 / 9., 1 / 9., 1 / 9. };
	double sobelXMask[3][3] =	{ -1., -2., -1.,
								   0.,  0.,  0.,
								   1.,  2.,  1. };
	double sobelYMask[3][3] =	{  1.,  0., -1.,
								   2.,  0., -2.,
								   1.,  0., -1. };
	double prewittXMask[3][3] = { -1., -1., -1.,
								   0.,  0.,  0.,
								   1.,  1.,  1. };
	double prewittYMask[3][3] = { -1.,  0.,  1.,
								  -1.,  0.,  1.,
								  -1.,  0.,  1. };
	double robertsXMask[3][3] = { -1.,  0.,  0.,
								   0.,  1.,  0.,
								   0.,  0.,  0. };
	double robertsYMask[3][3] = {  0.,  0., -1.,
								   0.,  1.,  0.,
								   0.,  0.,  0. };
	double laplace4Mask[3][3] = {  0., -1.,  0.,
								  -1.,  4., -1.,
								   0., -1.,  0. };
	double laplace8Mask[3][3] = { -1., -1., -1.,
								  -1.,  8., -1.,
								  -1., -1., -1. };

	switch (flag)
	{
		//마스크가 가르키는 주소를 원하는 마스크로 대입
	case 0:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = gausMask[i][j];
		break;	
	case 1:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = aveMask[i][j];
		break;
	case 2:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = sobelXMask[i][j];
		break;
	case 3:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = sobelYMask[i][j];
		break;

	case 4:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = prewittXMask[i][j];
		break;

	case 5:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = prewittYMask[i][j];
		break;

	case 6:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = robertsXMask[i][j];
		break;
	
	case 7:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = robertsYMask[i][j];
		break;

	case 8:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = laplace4Mask[i][j];

	case 9:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = laplace8Mask[i][j];
		break;
	case 10:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = sobelXMask[i][j]+sobelYMask[i][j];
		break;
	default:
		printf("mask number wrong...");
		exit(1);
	}
	for (i = 0; i < mSize; i++)
	{
		printf("{");
		for (j = 0; j < mSize; j++)
			printf(" %2.0lf ", mask[i][j]);
		printf(" }\n");
	}

}


void Filtering(uchar** img, uchar** res, int x_size, int y_size, int flag)
{
	int block_size = 3, i;

	double **mask;

	mask = (double **)calloc(block_size, sizeof(double*));
	for (i = 0; i < block_size; i++)
		mask[i] = (double *)calloc(block_size, sizeof(double));

	make_Mask(block_size, mask, flag);

	convolution(mask, block_size, x_size, y_size, img, res);
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	int row, col,mode,arg0,i;
	
	//IplImage* cvImg;
	//CvSize imgSize;

	uchar** img, ** res;

	if (argc != 7)
	{
		printf("argv error1\n");
		exit(0);
	}

	mode = atoi(argv[1]);			//atoi == 아스키 -> 정수 형변환 (반환값 정수)
	
	row = atoi(argv[4]);
	col = atoi(argv[5]);
	arg0 = atoi(argv[6]);

	img = uc_alloc(row, col);

	res = uc_alloc(row, col);
	read_ucmatrix(row, col, img, argv[2]);
	switch (mode)
	{
		case 0:	
			Negative(row, col, img, res);
			break;
		case 1:
			Mosaic(row, col, img, res, arg0);
			break;
		case 2:
			makeBinary(img, res, row, col, average(img, row, col));
			break;
		case 3:
			adaptiveBinary(img, res, row, col, arg0);
				break;
		case 4: 
			
			powImg(img, res, row, col, (double)atof(argv[6]));
			average(res, row, col);
			average(img, row, col);
			break;
		case 5:

			powAvgImg(img, res, row, col);
			average(res, row, col);
			average(img, row, col);
			break;

		case 6:

			bitSlicing(img, res, row, col, arg0);
			average(res, row, col);
			average(img, row, col);
			break; 

		case 7:

			bitSlicing2(img, res, row, col, arg0);
			average(res, row, col);
			average(img, row, col);
			break;

		case 8:

			circle(img, res, row, col, arg0);
			break;

		case 9:

			circleMosaic(img, res, row, col, arg0);
			break;

		case 10:
			noisedImage(img, res, row, col);
			break;

		case 11:
			noisedImage2(img, res, row, col,arg0);
			break;

		case 12 :
			
			Filtering(img, res, row, col, arg0);
			break;

		default : 
			printf("argv error2\n");
			exit(0);
	}
	
	write_ucmatrix(row, col, res, argv[3]);
	printf("%s is saved\n",argv[3]);
	free(img);
	free(res);


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
