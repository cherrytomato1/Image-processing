/*
	--  19�� 9��~ ����ó�� �ǽ� ���α׷�

	--  ROW, COLUM
		row -> ��(����) // colum -> ��(����) 
		row count == �� ���� (���� ����) ==  y ���� ��
		colum count == �� ���� (���� ����) == x ���� ��

	--  ���α׷� �Ű����� 
		myImgEdit.exe / (���) /  (�׸��̸�) /  (������ �̸�) / (row) / (col) / (�Ű����� 1) /

	--  ���							-- �Ű�����
		0	:	���� (negative)				x									0->255, 255->0
		1	:	��ü������ũ (mosaic)		�� ũ��(2�� �������� �ƴ� ��ħ)	�ش� �� ��� ������ �� �� �ȼ� ����
		2	:	���̳ʸ� �����				x									��� �̻�-> 255, ��� �̸�->0
		3	:	���̳ʸ� ����� 2			���̳ʸ��� ������ ���� ����(0~255)	�Ű�����-50 ~ �Ű�����+50�� �����ϰ� ��� 0����
		4	:	���� ����					������(default 1.0,0~10)			������ ���� �̹��� ��ü �� ����
		5	:	��� ���� ����				x									��ü �̹����� ��հ��� 128(�߰���)�� �� �� �ֵ��� ���� ����
		6	:	��Ʈ �̹��� ���			��°����ϰ� �� ��Ʈ ��(0~7)		�ȼ����� 0~255���� �̹����� 2������ ��ȯ�Ͽ� �ش� ��Ʈ��
																				1�� ��쿡�� �� �ȼ��� ���� ��� 255�� ����� �ƴ� ��� 0���� ���
		7	:	��Ʈ �̹��� ���2			���� ��Ʈ ��(0~7)					������Ʈ���� ���� �Է��� ��Ʈ������ ��Ʈ �̹����� ������
																				������ ��Ʈ�� �ռ��� ��Ʈ �̹����� ����ϰ� �Ѵ�.
																				0x01�� mask�� ���ϴ� ��Ʈ���� ��Ʈ�̵��Ͽ� & ��������
																				���ϴ� ��Ʈ�� ���ܳ��� �������� �����Ѵ�
		8	:	�� �׸���					�׷��� ���� ��(0~255)				���ϴ� ��ǥ, �������� �Է¹޾Ƽ� �ش� ��ǥ���� ��������ŭ
																				������ �ִ� ��� �ȼ��� �Ű������� �Է¹��� ������ ĥ�Ѵ�.
		9	:	���� ������ũ				������ũ �κ� ����(0,1)				������ũ ������, ��ǥ, ������ �Է¹޾� ��ǥ�κ��� �Է¹���
																				�������� ���� ���� �׷� �� �ȼ��� ������ũ �ϰų� �ش� �ȼ���
																				������ ��ü �κ��� ������ũ ó����.
		10	:	����þ� �����߻� �̹���										���� ������ ����þ� ������ �� �̹����� ���, ����
		11	:	����þ� ���� ���� ����		���� ������ ��						����þ� ������ �߻� ��Ų ������ ���ϴ� ������ ��ŭ ���Ѵ�.
																				���� ���Ҽ��� ����þ� ����� �پ�� ������ ��µȴ�.

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "wfilter.h"

#define _USE_MATH_DEFINES
#include <math.h>


#define uchar unsigned char
#define MAXrange 256
#define MAX 255


static int dct_buffer[8][8] = {
	{4096,4096,4096,4096,4096,4096,4096,4096},
	{5681,4816,3218,1130,-1130,-3218,-4816,-5681},
	{5352,2217,-2217,-5352,-5352,-2217,2217,5352},
	{4816,-1130,-5681,-3218,3218,5681,1130,-4816},
	{4096,-4096,-4096,4096,4096,-4096,-4096,4096},
	{3218,-5681,1130,4816,-4816,-1130,5681,-3218},
	{2217,-5352,5352,-2217,-2217,5352,-5352,2217},
	{1130,-3218,4816,-5681,5681,-4816,3218,-1130}
};


uchar** uc_alloc(int size_x, int size_y)
{  
	uchar** m;
	int i;

	/*malloc�� calloc�� ������ -->>>>> 

	��κ� �����Ϳ� �Ҵ��� �����Ƿ� ����ϴ� �����Ϳ� ���� ���� �°� �� ��ȯ ���־�� �մϴ�
		void malloc(size_t size)				->>>			1���� ���ڸ��� �ް� ���� ���ڸ�ŭ�� �޸𸮸� �Ҵ�/�ʱ�ȭ ����
			### pt= malloc(15*sizeof(int);
				
		void calloc(size_t num, size_t bytes)	->>				2���� ���ڸ� �ް� num * size ��ŭ�� �޸𸮸� �Ҵ�/ �ش� �� �޸𸮸� 0���� �ʱ�ȭ
			### pt= calloc(15,sizeof(int);

		### calloc�� �޸� �Ҵ�� 2���� ���ڸ� �ް� overflow�� �˻�!!! �����÷ο� �߻��� NULL �������ݴϴ�.
		### malloc�� ����ϸ鼭 �޸𸮸� �ʱ�ȭ �Ϸ��� 
		### memset(void *ptr, int value, size_t num);
		### -->> void * ���� ��� ��(������������)�� �Ű������� �޾ƿ� �� �ֽ��ϴ�. 
		###int value�� �ʱ�ȭ ��, size_t num�� �ʱ�ȭ�� ���ϴ� ����(�޸�)

	*/

		
	/*
		-	�����Ϳ� ���Ͽ�

		&p	=>	�ش� �������� �ּڰ�
		p	=>	�����ͱ� �����ϰ� �ִ� ���� �ּҰ�
		*p  =>  �����Ͱ� �����ϰ� �ִ� �ּҰ��� ������

		**�� => *���� �ּҰ� ���� ( *������ *�� ����x)
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

double** d_alloc(int size_x, int size_y)
{
	double** m;
	int i;
	if ((m = (double**)calloc(size_y, sizeof(double*))) == NULL)
	{
		printf("d_alloc error 1\7\n");
		exit(0);
	}

	for (i = 0; i < size_y; i++)
		if ((m[i] = (double*)calloc(size_x, sizeof(double))) == NULL)
		{
			printf("d_alloc error 2\7\n");
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

	//ucmatrix : �̹����� �޸𸮿� �ø��� ���� ����, unsigned character matrix
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
				//row�� ���� ������ŭ �ݺ��ϵ� blockũ�⸸ŭ�� �ѹ���
				//col�� block ũ�⿡ �ѹ��� �˻�
	/*
				tmp		:	������ �� ������ �ִ� ���� ����� ���� ���� �հ谪
				count	:	������ �� ������ �ִ� ���� ����� ���� ���� ���� ī��Ʈ ��

				�� ���� ��� �ȼ��� �˻��ϱ� ���� x�� * y�� ��ŭ �ݺ�
				�ȼ��� �˻��ϸ� �ش� �ȼ��� ���� tmp�� �հ��ϰ� count�� 1��ŭ �ø�

				�ִ� row�� col �ʰ��� �����ϱ� ���� ���ǹ��� �ɾ� �ʰ��� break
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
				tmp(�հ�)�� count(�ȼ� ����)�� ������ �� �� �ȼ��� ��� ���� ����
				���� ������ ���� �� �� ��� �ȼ��� �Է��� �� �� ��� �ȼ��� ���� ���� ������.
				�ִ� row�� col �ʰ��� break.
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

			// tmp = �ش� �ȼ��� ���� 255�� ������ (��� 0~1), ^(1/�Ű�����)�� ����
			// ������ ���� �¸�ŭ �����ϹǷ� ������ ���� ���� ���� tmp �� �� (0.0~ 1.0)^ (1/gamma)
			

			if (tmp * 255 > 255)	tmp = 1;
			else if (tmp * 255 < 0)	tmp = 0;

			//tmp ���� 1 �ʰ��̰ų� 0 �̸��� ��� ���� 0,1 �� �ʱ�ȭ ������
			
			res[i][j] = tmp * 255;

			//�������� 255 ���� �ٽ� ���Ͽ� ��
			
		}
	printf("gamma changed by %lf ", gamma);
	return;
}
double powAvgImg(uchar** img, uchar** res, int row, int col)
{
	int i, j;
	double tmp,k;

	/*
		k= ���� , ������ 0���� 10���� �����ؼ� 
		��µǴ� �̹����� ����� 127.5�ʰ� 128.5 �̸��� �� ��
		�ݺ��� ������ ����. ��µǴ� �̹����� ��� �̹����� �ڵ����� �Է�
		������ ��ȯ
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
		��Ʈ�̵� - mask��(0000 0001)�� �Է��ϴ� �Ű�������ŭ (1~7)*/


	for(i=0; i< row; i++)
		for (j = 0; j < col; j++)
		{
			if ((mask & img[i][j]) == mask) // =pow(2, position) ����
				res[i][j] = 255;
			else
				res[i][j] = 0;
		}
	//�Ű�������ŭ ��Ʈ �̵���Ų ����ũ ���� & ������ ���� ���� ������ �Ǵ��Ͽ�
	//��� ���� ���� 0 Ȥ�� 255�� ��ȯ
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
		//�ش� �ڵ� ������ ����ũ ���˻縦 ���ؼ� �� ��Ʈ �˻� �� �ش� ��Ʈ�� �´� �� 
		//8(��°��Ʈ,pos7)(1000 0000) 7(0100 0000) �̷������� �������� �����
		//1000 0000 + 0100 0000 + 0010 0000 + 0001 0000 + 0000 1000 ,,,, �� ����� ��µȴ�.
		//�׷���,,, 8��Ʈ �������� ���� ���� ��������� (pos 7) 128�� ��(1000 0000)

		//�� ��Ʈ ������ ���� ������ ���� �� ���ϸ� ������ ���̴�������-->> ������ ������ ��µǴ� ��,,,
		//res8(��Ʈ,pos7)/(2^0)+ res7/(2^1) + res6/(2^2) + res5/(2^3) +res4/(2^4) + res3/(2^5) .. . .. 
		//0111 1111 + 0011 1111 + 0001 1111 + 0000 1111 + 0000 0111 ... �� ����� ��µȴ�
		//8��Ʈ �������� ���� ���� ������� (pos 7) 127(0111 1111)
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
		{													//abs	== ���밪
			ySquare = (abs(locY - i)) * (abs(locY - i));	//ySquare == (abs(-256~255))^2 ->> 0~255^2 (row,col�� 512��)
																//row/2 --> y�� �߾� ��
			xSquare = (abs(locX - j)) * (abs(locX - j));	//xSquare == (abs(-256~255))^2 ->> 0~255^2 
			//������ �����ε� ���밪�� �ִ� ����??
			
			//������ �ȼ��� �߾����κ��� �󸶳� �־��� �ִ����� �����ϰ�����
			//�߾� ������  ysquare/xsquare row�� col�� �߾� �Ÿ� ���
			//��Ÿ����� ���� = a^2 +b^2 = c^2

			tmp = sqrt(ySquare + xSquare); 
			
			if (diameter<tmp + 1 && diameter>tmp - 1)
				res[i][j] = mod;
			else
				res[i][j] = img[i][j];
			// tmp == ������ �ȼ��� �Ѱ���� �߾� �����κ��� ���ݵǾ� �ִ� ����.
			//������ ���� = (a^2+b^2)�� ������ = tmp = (ysquare +xsquare) ������
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

	//i = �� ���� row, j = �� ���� col

	for (i = 0; i < row; i += block)
		for (j = 0; j < col; j += block)
		{
			sum = 0;
			count = 0;

			for (y = 0; y < block && i + y < row - 1; y++)
				for (x = 0; x < block && j + x < col - 1; x++, count++)
					sum += img[i + y][j + x];

			/*
				������ũ�� ���� �۾�
				tmp(�հ�)�� count(�ȼ� ����)�� ������ �� �� �ȼ��� ��� ���� ����
				���� ������ ���� �� �� ��� �ȼ��� �Է��� �� �� ��� �ȼ��� ���� ���� ������.
				�ִ� row�� col �ʰ��� break.
			*/
			sum /= count;
			for (y = 0; y < block && i + y < row - 1; y++)
				for (x = 0; x < block && j + x < col - 1; x++, count++)
				{
					ySquare = (abs(locY - (i + y))) * (abs(locY - (i + y)));
					xSquare = (abs(locX - (j + x))) * (abs(locX - (j + x)));

					tmp = sqrt(ySquare + xSquare);

					/*
						mod�� ���� ������ũ �۾��� �ȼ��� ����� ��
						���� �ȼ��� �״�� �۾��� �� ��� ���
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
			//abs	== ���밪
				//xSquare == (abs(-256~255))^2 ->> 0~255^2 
			//������ �����ε� ���밪�� �ִ� ����??

			//������ �ȼ��� �߾����κ��� �󸶳� �־��� �ִ����� �����ϰ�����
			//�߾� ������  ysquare/xsquare row�� col�� �߾� �Ÿ� ���
			//��Ÿ����� ���� = a^2 +b^2 = c^2

			// tmp == ������ �ȼ��� �Ѱ���� �߾� �����κ��� ���ݵǾ� �ִ� ����.
			//������ ���� = (a^2+b^2)�� ������ = tmp = (ysquare +xsquare) ������
}

double uniform()
{
	return((double)(rand() % RAND_MAX) / RAND_MAX - 0.5);
	//����þ� ���� �߻��� ���� �� 
}

//����þ� ���� �߻�
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

//��� �̹����� ����þ� ����(-1~1 * 50 )�� ���� �߰���( ���� �� ����÷� ������ ���� ������ ���������� ��� ����)
double noisedImage(uchar** img, int** res, int row, int col)
{
	double sum = 0;
	int i, j,temp;

	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
		{
			//����þ� ���� �ռ�, ����þ� ���� ���� �����Ƿ� 50���Ͽ� ����
			temp = img[i][j] + ((int)gaussian() * 50);
			
			//Ŭ���� ����
			if (temp > 255)
				temp = 255;
			else if (temp < 0)
				temp = 0;
			res[i][j] = temp;
		}
	return 0;
}
//���ϴ� ������ ��ŭ ����þ� ���� �߻�
void noisedImage2(uchar** img, uchar** res, int row, int col, int page)
{
	int** tmp;
	int** tmp2;
	int i, j, k;

	
	tmp = i_alloc(row, col);
	tmp2 = i_alloc(row, col);

	//���ϴ� ������ŭ ���� ���Ե� �̹����� ���Ͽ� ����
	for (k = 0; k < page; k++)
	{
		noisedImage(img, tmp2, row, col);
		for (i = 0; i < row; i++)
			for (j = 0; j < col; j++)
				tmp[i][j] += tmp2[i][j];
	}
	//������ ���� ������ ����� ����
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
			res[i][j] = (tmp[i][j]/page);

}




void convolution(double** h, int F_length, int size_x, int size_y, uchar** img, uchar** res,int val)
{
	int i, j, x, y;

	int margin, indexX, indexY;
	double sum, coeff;
	//F_LENGTH = ���� ������
	//���� ����, ���� �߾����� ���� ������ �ִ� ��.
	margin = (int)(F_length / 2);	

	for (i = 0; i < size_y; i++)
		for (j = 0; j < size_x; j++)
		{
			sum = 0;
			// X, Y = ���� ���ο��� �����ϴ� �ݺ���
			// ���� �����ŭ �ݺ��Ͽ� �� �ȼ����� �ش� �ȼ����� F_LENGTH^2 ��ŭ �ݺ��Ͽ�
			// ���͸� �����Ų��.
			for (y = 0; y < F_length; y++)
			{
				//x,y = ������ �߾Ӱ��� ��ġ�� �ȼ�
				//indexX, Y = �ش� ���͸� ����ϴ� ù��° �ε���
				indexY = i - margin + y;

				//�ش� �ε����� ������ �ܰ��� ����� ��� ó��
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
						indexX = (2 * size_x - indexX - 1);			//�ܰ�ó��( ��Ī��� )
					/*���͸� ����� ������ ���Ѵ�. (�� 9���� ��)*/
					sum += h[y][x] * (double)img[indexY][indexX];

				}
			}
			//val = ���� �� �ռ��� ���� backgroud �� ����, ���ö� ���Ϳ� ���� ��쿡��
			//���� ���� �߰��Ͽ� ���͸��� ����+ val ���� ���
			sum+=val;

			//Ŭ���� ����
			if (sum < 0)
				sum = 0;
			else if (sum > 255)
				sum = 255.;
			res[i][j] = (uchar)sum;
		}
}

int make_Mask(int mSize, double** mask, int flag)
{

	int i, j,sum=0,mod=0;
	//sum == ��׶���� ������ ��

	if (flag >= 100)
	{
		flag -= 100;
		mod = 1;
	}

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
	double embosMask[3][3] =	{ -1.,  0.,  0.,
								   0.,  0.,  0.,
								   0.,  0.,  1. };

	switch (flag)
	{
		//����ũ�� ����Ű�� �ּҸ� ���ϴ� ����ũ�� ����
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
		break;

	case 9:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = laplace8Mask[i][j];
		break;
	case 10:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = embosMask[i][j];
		sum = 128;
		break;
	case 23:
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = sobelXMask[i][j]+sobelYMask[i][j];
		break;
	case 99 :
		for (i = 0; i < mSize; i++)
			for (j = 0; j < mSize; j++)
				mask[i][j] = laplace8Mask[i][j];
	//	mask[1][1] += 1;
		break;
	default:
		printf("mask number wrong...");
		exit(1);
	}
	if (mod == 1)
	{
		mask[1][1] += 1;
	}
	for (i = 0; i < mSize; i++)
	{
		printf("{");
		for (j = 0; j < mSize; j++)
			printf(" %2.0lf ", mask[i][j]);
		printf(" }\n");
	}
	
	return sum;

}

void Filtering(uchar** img, uchar** res, int x_size, int y_size, int flag)
{
	int block_size = 3, i;
	int bg = 0;
	double **mask;

	//����� ������ �޸� �����Ҵ�, ���ͻ�����^2�� ������ �Ҵ��Ѵ�.
	mask = (double **)calloc(block_size, sizeof(double*));		
	for (i = 0; i < block_size; i++)
		mask[i] = (double *)calloc(block_size, sizeof(double));

	//�Է��� flag�� ���� �ش��ϴ� ����ũ�� ������
	//backGround �׷��� ����, �ռ��ϴ� flag �Է½� ���� ��ȯ��
	bg=make_Mask(block_size, mask, flag);

	//���͸� �����Ű�� ����, ����ũ, ����ũ ������, �̹����� �Ű������� �Ѵ�.
	convolution(mask, block_size, x_size, y_size, img, res, bg);
}


//medianfilter�� �� ������ ���� ���� ����
void Bubble_sort(uchar* Sort, uchar* median_value, int Mode, int filterSize)
{
	int i, x;
	uchar temp, swap;
	//���ͻ����� ^2 ��ŭ �ݺ�( ���Ϳ� ���� �� ���� ��)
	for (x = 0; x < filterSize*filterSize; x++)
	{
		//�񱳸� ���� �� ����
		temp = Sort[x];
		for (i = x; i < filterSize * filterSize- 1; i++)
		{
			//���� ���� ����
			if (temp >= Sort[i + 1])
			{
				swap = temp;
				temp = Sort[i + 1];
				Sort[i + 1] = swap;
			}
		}
		Sort[x] = temp;
	}
	if (Mode == -1)
		*median_value = (uchar)Sort[0]; // median filter�� �߾Ӱ��� ���ͳ��� �ּڰ����� ����
	else if (Mode == 0)
		*median_value = (uchar)Sort[filterSize * filterSize / 2]; // median filter�� �߾Ӱ��� ���ͳ��� �߰������� ����
	else if (Mode == 1)
		*median_value = (uchar)Sort[filterSize * filterSize - 1]; // median filter�� �߾Ӱ��� ���ͳ��� �ִ밪���� ����
	//printf("%d  ,", *median_value);

}

void median(uchar** inImg, uchar** outImg, int ROW, int COL, int Mode, int filterSize) // Median Filtering
{
	int i, j, x, y, z, count = 0;
	uchar median_value; // ������ �߾Ӱ�
	uchar* Sort;
	// ������ ����ũ���� ������ ������ ������ �迭 �����Ҵ�
	Sort = (uchar*)malloc(filterSize * filterSize * sizeof(uchar)); 


	for (i = 0; i < ROW; i++)
		for (j = 0; j < COL; j++)
			outImg[i][j] = inImg[i][j];

	//��� �ȼ��� ���Ͽ� ������ �ܰ��� �ʰ����� �ʴ� ���� ������ ���͸�
	for (i = 0; i < ROW - filterSize; i++)
		for (j = 0; j < COL - filterSize; j++)
		{
			//���ͳ��� ��� ���� sort �迭�� �Է�
			for (x = 0; x < filterSize; x++)
				for (y = 0; y < filterSize; y++)	
					Sort[filterSize * x + y] = inImg[i + x][j + y];
			//�Էµ� �迭�� ����
			Bubble_sort(Sort, &median_value, 0, filterSize);
			//������ �߾ӿ� ��ġ�� �ȼ��� median value�� �Է�
			outImg[i + 1][j + 1] = median_value;
		}
	free(Sort);
}

void median2(uchar** inImg, uchar** outImg, int ROW, int COL, int Mode, int filterSize) // Median Filtering
{
	int i, j, x, y, z, count = 0;
	uchar median_value; // ������ �߾Ӱ�
	uchar* Sort;

	Sort = (uchar*)malloc(filterSize * sizeof(uchar)); // ������ ����ũ���� ������ ������ ������ �迭 �����Ҵ�

	printf("malloc done\n");

	for (i = 0; i < ROW; i++)
		for (j = 0; j < COL; j++)
			outImg[i][j] = inImg[i][j];

	for (i = 0; i < ROW; i++)
		for (j = 0; j < COL - filterSize; j++)
		{
			//printf("debug1 \n");
			for (y = 0; y < filterSize; y++)
			{
				Sort[y] = inImg[i][j + y];
				//printf("%d ", Sort[y]);
			}
			//printf("\n");
			
			Bubble_sort(Sort, &median_value, 0, filterSize);
			outImg[i][j + (filterSize/2)] = median_value;
			//outImg[i][j + (filterSize / 2)] = 128;
			//printf("%d ", j + filterSize/2);
			//printf("%d, ", median_value);
		}
	
	//free(Sort);
	printf("done\n");
}









int rearrange(double* X, int N)
{
	int i, j, * power_of_2, * pos, stage, num_of_stages = 0;
	double temp;

	for (i = N; i > 1; i >>= 1, num_of_stages++);

	if ((power_of_2 = (int*)malloc(sizeof(int) * num_of_stages)) == NULL)
		return -1;
	if ((pos = (int*)malloc(sizeof(int) * N)) == NULL)
		return -1;

	power_of_2[0] = 1;

	for (stage = 1; stage < num_of_stages; stage++)
		power_of_2[stage] = power_of_2[stage - 1] << 1;

	for (i = 1; i < N - 1; i++)
		pos[i] = 0;
	for (i = 1; i < N - 1; i++)
	{
		if (!pos[i])
		{
			for (j = 0; j < num_of_stages; j++)
			{
				if (i & power_of_2[j])
					pos[i] += power_of_2[num_of_stages - 1 - j];
			}

			temp = X[i];
			X[i] = X[pos[i]];
			X[pos[i]] = temp;
			pos[pos[i]] = 1;
		}
	}
	free(power_of_2);
	free(pos);

	return 0;

}

void fft(double* X_re, double* X_im, int N)
{
	double X_temp_re, X_temp_im;
	double phase;

	int num_of_stages = 0, num_of_elements, num_of_sections, size_of_butterfly;
	int i, j, stage, m1, m2;

	for (i = N; i > 1; i >>= 1, num_of_stages++);

	num_of_elements = N;
	num_of_sections = 1;
	size_of_butterfly = N >> 1;

	for (stage = 0; stage < num_of_stages; stage++)
	{
		m1 = 0;
		m2 = size_of_butterfly;

		for (i = 0; i < num_of_sections; i++)
		{
			for (j = 0; j < size_of_butterfly; j++, m1++, m2++)
			{
				X_temp_re = X_re[m1] - X_re[m2];
				X_temp_im = X_im[m1] - X_im[m2];

				X_re[m1] = X_re[m1] + X_re[m2];
				X_im[m1] = X_im[m1] + X_im[m2];

				phase = -2.0 * M_PI * j / num_of_elements;

				X_re[m2] = X_temp_re * cos(phase) - X_temp_im * sin(phase);
				X_im[m2] = X_temp_re * sin(phase) + X_temp_im * cos(phase);
			}

			m1 += size_of_butterfly;
			m2 += size_of_butterfly;

		}
		num_of_elements >>= 1;
		num_of_sections <<= 1;
		size_of_butterfly >>= 1;

	}

	rearrange(X_re, N);
	rearrange(X_im, N);

}

int fft_2d(double** X_re, double** X_im, int N, int Mode)
{
	int i, j;
	double* temp_re, *temp_im;

	//temp �޸� �Ҵ�
	if ((temp_re = (double*)malloc(sizeof(double) * N)) == NULL)
		return -1;
	if ((temp_im = (double*)malloc(sizeof(double) * N)) == NULL)
		return -1;

	if (Mode == 0)
	{
		//row��ŭ Ƚ����  fft ����
		for (i = 0; i < N; i++)
			fft(X_re[i], X_im[i], N);

		for (j = 0; j < N; j++)
		{
			for (i = 0; i < N; i++)
			{
				temp_re[i] = X_re[i][j];
				temp_im[i] = X_im[i][j];
			}
			fft(temp_re, temp_im, N);

			for (i = 0; i < N; i++)
			{
				X_re[i][j] = temp_re[i] / N;
				X_im[i][j] = temp_im[i] / N;
			}
		}
	}

	else if (Mode == 1)
	{

		for (i = 0; i < N; i++)
			fft(X_re[i], X_im[i], N);

		for (j = 0; j < N; j++)
		{
			for (i = 0; i < N; i++)
			{
				temp_re[i] = X_re[i][j];
				temp_im[i] = X_im[i][j];
			}
			fft(temp_re, temp_im, N);

			for (i = 0; i < N; i++)
			{
				X_re[i][j] = temp_re[i] / N;
				X_im[i][j] = temp_im[i] / N;
			}
		}
	}
	else if (Mode == 2)
	{
		for (i = 0; i < N; i++)
		{
			for (j = 0; j < N; j++)
			{
				temp_re[j] = X_re[i][j] * pow(-1, j);
				temp_im[j] = X_im[i][j] * pow(-1, j);
			}

			fft(temp_re, temp_im, N);

			for (j = 0; j < N; j++)
			{
				X_re[i][j] = temp_re[j];
				X_im[i][j] = temp_im[j];
			}

		}
		for (j = 0; j < N; j++)
		{
			for (i = 0; i < N; i++)
			{
				temp_re[i] = X_re[i][j] * pow(-1, i);
				temp_im[i] = X_im[i][j] * pow(-1, i);
			}

			fft(temp_re, temp_im, N);

			for (i = 0; i < N; i++)
			{
				X_re[i][j] = temp_re[i] / N;
				X_im[i][j] = temp_im[i] / N;
			}
		}
	}

	free(temp_re);
	free(temp_im);

	return 0;

}
//fft �����Լ�, mode�� �Ű������� ����
void fftInit(uchar** img, uchar** res,int row, int col, int Mode)
{
	int i,j;
	double** t_img, ** t_tmp;
	t_img = d_alloc(row, col);
	t_tmp = d_alloc(row, col);

	//��� �̹��� �� �Է� �̹����� double�� �Ҵ�, ������
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
		{
			t_img[i][j] = (double)img[i][j];
			//printf(" %lf, ", t_img[i][j]);
		}

	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
		{
			t_tmp[i][j] = 0.;
			//printf(" %lf ,", t_tmp[i][j]);
		}

	//2���� fft ����, row�� �Ű������� �ѱ�
	fft_2d(t_img, t_tmp, row, Mode);

	//Ŭ���ε���
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
		{
			if (t_img[i][j] > 255.)
				t_img[i][j] = 255.;
			else if (t_img[i][j] < 0.)
				t_img[i][j] = 0.;

			//��� ���� ����� ��� ���� ����
			res[i][j] = (uchar)t_img[i][j];
			//printf(" %d, ", res[i][j]);
		}
}

/*

void fftInit(uchar** img, uchar** res, int row, int col, int Mode)
{
	int i,j, trow, tcol;
	double max = -10E30, min = 10E30;

	double** f_img, **i_img;

	double diameter;

	f_img = (double **)d_alloc(col, row);
	i_img = (double **)d_alloc(col, row);

	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
			f_img[i][j] = (double)img[i][j];

	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
			i_img[i][j] = 0.;

	fft_2d(f_img, i_img, row, Mode);

	for(i=0;i<row;i++)
		for (j = 0; j < col; j++)
		{
			diameter = sqrt((row / 2. - i) * (row / 2. - i) + (col / 2. - j) * (col / 2. - j));

			if (fabs(diameter) > 50)
			{
				f_img[i][j] = 0;
				i_img[i][j] = 0;
			}
			else
				i_img[i][j] = -i_img[i][j];
		}
	fft_2d(f_img, i_img, row, Mode - 1);

	for (i = 0; i < row; i++)
		for (j = 0; col < j; j++)
			f_img[i][j] = f_img[i][j] * pow(-1, i + j);

	for(i=0; i<row ; i++)
		for (j = 0; j < col; j++)
		{
			if (max < f_img[i][j])
				max = f_img[i][j];
			if (min > f_img[i][j])
				max = f_img[i][j];
		}

}

*/


void Fdct(int** PEL, int** Coeff) {
	int i, j, k;
	long dd;
	int t[8][8];

	for (i = 0; i < 8; i++) 
	{
		for (j = 0; j < 8; j++) 
		{
			dd = 0;
			for (k = 0; k < 8; k++)
				dd += (long)PEL[i][k] * dct_buffer[j][k];

			t[i][j] = ((dd + 2048) >> 12);
		}
	}


	for (i = 0; i < 8; i++) 
	{
		for (j = 0; j < 8; j++) 
		{
			dd = 0;
			for (k = 0; k < 8; k++)
				dd += (long)t[k][i] * dct_buffer[j][k];
			Coeff[j][i] = ((dd + 16384) >> 15);

			//printf("coeff[%d][%d] = %d \n", j, i, Coeff[j][i]);
		}
	}
}

void Idct(int** Coeff, int** PEL) {
	int i, j, k;
	long dd;
	int t[8][8];

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			dd = 0;
			for (k = 0; k < 8; k++) {
				dd += (long)Coeff[k][i] * dct_buffer[k][j];
			}
			t[i][j] = ((dd + 2048) / 4096);
		}
	}

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			dd = 0;
			for (k = 0; k < 8; k++) 
				dd += (long)t[k][i] * dct_buffer[k][j];
		
			PEL[i][j] = ((dd + 16384) / 32768);

			//printf("PEL[%d][%d] = %d \n", j, i, PEL[j][i]);
		}
	}
}

//fdct ���� �Ű����� �Է¹ڽ�, ��¹ڽ�
void myFdct(double** PEL, double** Coeff)
{
	int u, v, x, y;
	//��ȯ ��ϻ�����
	int blockSize = 8;

	//C(u), C(v) ����
	double cU = 0, cV = 0, temp,value,N;
	
	//���� N �ʱ�ȭ (x,y �� ������� �ϳ��� ����)
	N = (double)blockSize;
			
	//��� ����� F(u,v) ������ ���� �ݺ��� (�⺻ 8*8)
	for (v = 0; v < N; v++)
	{
		//C(v) �� �ʱ�ȭ
		if (v == 0)
			cV = 1. / sqrt(2);
		else
			cV = 1.;

		for (u = 0; u < N; u++)
		{
			//�� F(u,v)�� ���Ͽ� �ΰ��� �ñ׸� ������ ���� �����ϴ� ���� value �ʱ�ȭ
			value = 0.;

			//C(u) �ʱ�ȭ
			if (u == 0)	
				cU = 1. / sqrt(2);
			else
				cU = 1.;

			//�ñ׸� ���� ���� �������� C(u),C(v)�� ���ѽ� ����
			temp =(cU * cV * 4.) / (N * 2);

			//�ñ׸��� ���� �ݺ���
			for (y = 0; y < N; y++)
			{
				for (x = 0; x < N; x++)
				{
					//���� ���� ����
					value += PEL[x][y] * cos((((2. * (double)x) + 1) * ((double)u * M_PI)) / (2. * N))
						* cos((((2. * (double)y) + 1.) * (v * M_PI)) / (2. * N))  ;
				}
			}
			//printf("value = %0.5lf \n ", value);

			//�ñ׸� �ջ����� Cu,Cv �� ��
			Coeff[u][v] = (double)(value * temp);

		//	printf("coeff[%d][%d] = %d \n", u, v, Coeff[u][v]);

		}
	}


}

//idct ���� �Ű����� �Է¹ڽ� ��¹ڽ�
void myIdct(double** Coeff, double** PEL) {
	int u, v, x, y;
	
	//�ڽ� ������
	int blockSize = 8;

	//cu, cv ����
	double cU = 0, cV = 0, temp, value,N;

	//���� N ���� (�ڽ� ������)
	N = (double)blockSize;

	//��� ����� f(x,y) ������ ���� �ݺ��� (�⺻ 8*8)
	for (y = 0; y < N; y++)
	{		
		//cV = 1./sqrt(2);
		//printf("cV = .0%lf \n", cV);
		for (x = 0; x < N; x++)
		{
			//�� f(x,y)�� ���Ͽ� �ΰ��� �ñ׸� ������ ���� �����ϴ� ���� value �ʱ�ȭ
			value = 0.;

			//cU = 1. / sqrt(2);
			//printf("cU = .0%lf \n", cU);

			//�ñ׸� ���� ���� �������� �� ����
			//�ݺ����� ���Ǵ� ������ �����Ƿ� �ݺ��� �ܺη� �� �� ����
			temp = 4. / (N * 2);


			//printf("temp = %lf \n ", temp);

			for (v = 0; v < N; v++)
			{
				//C(v) �ʱ�ȭ
				if (v == 0)
					cV = 1. / sqrt(2);
				else
					cV = 1.;

				for (u = 0; u < N; u++)
				{
					//C(u) �ʱ�ȭ
					if (u == 0)
						cU = 1. / sqrt(2);
					else
						cU = 1.;

						//�ջ� ��
					value += cU * cV * Coeff[u][v] * cos( ( ( (2 * (double)x) + 1 ) * ((double)u * M_PI ) ) / (2 * N) )
						* cos( ( ( ( 2 * (double)y ) + 1 ) * ((double)v * M_PI ) ) / (2 * N ) ) ;
				}
			}
			//printf("value = %0.5lf \n ", value);
			
			//��� ����
			PEL[x][y] = value * temp;

			//printf("PEL[%d][%d] = %d \n", x, y, PEL[x][y]);

		}
	}
}

/*	
	dct ��ȯ�� ���� �����Լ� - dct ��ȯ �� ����ȯ ����
	�Ű����� : �̹��� ������, ��� �̹��� ������, ROW, COL, DCT ��ȯ ��� (integer ����, ����)

*/
void dctInit(uchar** img, uchar** res, int row, int col, int Mode)
{
	int i, j, block = 8,x,y;
	//	�Է� box(��ȯũ�� /�⺻�� 8), �߰���� box, ��� ��� box
	double** inBox, ** outBox, **outBox2;

	// box �޸� �Ҵ�
	inBox = d_alloc(block,block);
	outBox = d_alloc(block, block);
	outBox2 = d_alloc(block, block);


	//box ���� �ݺ�
	for (i = 0; i < row; i += block)
		for (j = 0; j < col; j += block)
		{
			/*
				�� ���� ��ϸ��� box�� �� ������ �״�� ���� (��� ��ǥ + �ش� �ȼ���ǥ ����)

				
			*/
			for (y = 0; y < block && i + y < row - 1; y++)
				for (x = 0; x < block && j + x < col - 1; x++)
				{
					inBox[y][x] = (double)img[i + y][j + x];

					//���ʿ� �۾�( ������ ���)
					//outBox[y][x] = (double)img[i + y][j + x];
					//outBox2[y][x] = (double)img[i + y][j + x];
				}

			// � ��ȯ�� ����� ������ �Ű������� ���� ����
			
			switch (Mode)
			{
				//case 1 : integer ���� ���
			case 1 :
	//			printf("IDCT ...outbox[%d][%d] = %d\n", i + y, j + x, inBox[y][x]);
				Fdct(inBox, outBox);

				Idct(outBox, outBox2);
				break;

				//case 2 : ������ �ڵ�� �ۼ��� ��ȯ ���, ��ȯ �� ����ȯ

			case 2:

				//				printf("FDCT ...inbox[%d][%d] = %d\n",i+y,j+x,inBox[y][x]);
				//printf("my Fdct, Idct\n");
				myFdct(inBox, outBox);

				//Idct(outBox, outBox);

				myIdct(outBox, outBox2); 


				break;
			default :
				printf("invalid mode input\n");
				exit(1);
			}

			
			//�ڽ������� ��ȯ�� �̹����� ����� ���
	
			for (y = 0; y < block && i + y < row - 1; y++)
				for (x = 0; x < block && j + x < col - 1; x++)
					res[i + y][j + x] = (uchar)outBox2[y][x];


			/*
			for (y = 0; y < block && i + y < row - 1; y++)
				for (x = 0; x < block && j + x < col - 1; x++)
				{
					if (outBox[y][x] > 255.)
						outBox[y][x] = 255.;
					else if (outBox[y][x] < 0.)
						outBox[y][x] = 0.;
					res[i + y][j + x] = (uchar)outBox[y][x];
					

				}
			*/
		}
}

void row_analysis(double* h, int F_length, int size_x, int size_y, double** image1, double** image2)
{
	int i, j, k, margin, index, size_x2;
	double sum, coeff;
	double** temp;

	size_x2 = size_x/2;
	temp = d_alloc(size_x, size_y);

	for (i = 0, coeff = 0.; i < F_length; i++) coeff += h[i];
	printf("coeff = %lf\n", coeff);

	margin = (int)(F_length / 2);
	for (i = 0; i < size_y; i++)
		for (j = 0; j < size_x; j++)
		{
			for (k = 0, sum = 0.; k < F_length; k++)
			{
				index = j - margin + k;
				if (index < 0) index = -index;
				else if (index >= size_x) index = (2 * size_x - index - 2);
				sum += h[k] * image1[i][index];
			}
			if (coeff > 1.) sum /= coeff;
			/*	if (sum < 0) sum = 0.;
				else if (sum > 255) sum = 255.;
			*/
			temp[i][j] = sum;
			//			printf("sum = %lf\n", sum);
		}

	for (i = 0; i < size_y; i++)
		for (j = 0, k = 0; j < size_x; j += 2, k++)
		{
			image2[i][k] = temp[i][j];
			//printf("row  image2[%d][%d] = %lf\n", i, k, image2[i][k]);
		}

	//free(temp); // d_free(size_x, size_y, temp);
}

void column_analysis(double* h, int F_length, int size_x, int size_y, double** image1, double** image2)
{
	int i, j, k, margin, index, size_x2, size_y2;
	double sum, coeff;
	double** temp;

	size_x2 = size_x/2;
	size_y2 = size_y/2;

	temp = d_alloc(size_x2, size_y);

	for (i = 0, coeff = 0.; i < F_length; i++) coeff += h[i];
	printf("coeff = %lf\n", coeff);

	margin = (int)(F_length / 2);
	for (i = 0; i < size_x2; i++)
		for (j = 0; j < size_y; j++)
		{
			for (k = 0, sum = 0.; k < F_length; k++)
			{
				index = j - margin + k;
				if (index < 0) index = -index;
				else if (index >= size_y) index = (2 * size_y - index - 2);
				sum += h[k] * image1[index][i];
			}
			if (coeff > 1.) sum /= coeff;
			if (sum < 0) sum = 0.;
			else if (sum > 255) sum = 255.;

			temp[j][i] = sum;
			//printf("sum = %lf\n", sum);

		}
	//printf("col-----last\n");
	for (i = 0; i < size_x2; i++)
		for (j = 0, k = 0; j < size_y; j += 2, k++)
		{
			image2[k][i] = temp[j][i];
			//printf("col  image2[%d][%d] = %lf\n", k, i, image2[k][i]);

		}
	//printf("col ==== end===\n");
	//free(temp); // d_free(size_x2, size_y, temp);
}


void analysis(uchar** img, uchar** res, int row, int col, int level)
{
	int i, j,size_x,size_y, mod =0;
	double **dImg, **dRes,**dTmp;
	//���̺�, �����ϸ� ��ȯ �Լ����� � ���� ����� ������ �����ϴ� ��� �� �Է� 0~3
	printf("insert mode");
	scanf_s("%d", &mod);

	//����� �̹��� �޸� �Ҵ�
	dImg = d_alloc(row, col);
	dTmp = d_alloc(row, col);
	dRes = d_alloc(row, col);


	size_x = row;
	size_y = col;

	//�Է� ���� double�� �Ҵ�
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
			dImg[i][j] = (double)img[i][j];

	//level ��ŭ �����ϸ� �� ���̺� �Լ� �ݺ�
	for (i = 0; i < level; i++)
	{
		//���ϴ� ��忡 ���� 4���� ���ػ� ����ȯ ������ ��ħ

		switch (mod)
		{
		case 0 :
			//row�� col �� ����� ���̸鼭 ������ ���� 
			//����� ���ҵ� ������ ���� ��� �����ϸ�, �����ϸ�
			row_analysis(A97L1, A97L1_l, size_x, size_y, dImg, dTmp);
			column_analysis(A97L1, A97L1_l, size_x, size_y, dTmp, dRes);
			break;

		case 1:
			//row�� col �� ����� ���̰� ���� ���� ����
			//�����ϸ�, ���̺�
			
			row_analysis(A97L1, A97L1_l, size_x, size_y, dRes, dTmp);
			column_analysis(A97H1, A97H1_l, size_x, size_y, dTmp, dRes);
			break;

		case 2:
			//row�� col �� ����� ���̰� ���� ���� ����
			//���̺�, �����ϸ�
			row_analysis(A97H1, A97H1_l, size_x, size_y, dRes, dTmp);
			column_analysis(A97L1, A97L1_l, size_x, size_y, dTmp, dRes);
			break;

		case 3 :
			//row�� col �� ����� ���̰� �밢�� ���� ����
			//���̺� ���̺�
			row_analysis(A97H1, A97H1_l, size_x, size_y, dRes, dTmp);
			column_analysis(A97H1, A97H1_l, size_x, size_y, dTmp, dRes);
			break;
		}
		//�� �۾��� ���� dimg�� ��� ������ �־���
		dImg = dRes;
	}
	
//	column_analysis(A97L1, A97L1_l, size_x, size_y, dTmp, dRes);
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
		{
			//dRes[i][j] += 128;

			if (dRes[i][j] < 0)
				dRes[i][j] = 0;
			else if (dRes[i][j] > 255)
				dRes[i][j] = 255;
			res[i][j] = (uchar)(dRes[i][j]);
		}
}
/*
void binErosion(uchar** img, uchar** res, int block,int **mask)
{
	int i, j, count;
	int indexi, indexj;
	int maski, maskj;

	printf("binEros init\n");
	for (i = ; i < block - row; i++)
	{
		for (j = -col; j < block - col; j++)
		{
			printf(" -row= %d , -col= %d, i= %d , j=%d\n", -row, -col, i, j);
			count = 0;
			for (maski = 0; maski <= 2 * row; maski++)
			{
				for (maskj = 0; maskj <= 2 * col; maskj++)
				{
					if (mask[maski][maskj] != 0)
					{
						indexi = i + maski;
						indexj = j + maskj;

						if (img[indexi][indexj] == 255) 
						{
							count++;
						}
					}
				}
			}
			if (count == 5)
				res[i + row][j + col] = 255;
			else
				res[i + row][j + col] = 0;

		}
	}
}
*/

//
void binErosion(uchar** Data_In, uchar** Data_Out, int Row, int Col, int Size) 
{
	int i, j, count;
	int indexi, indexj;
	int maski, maskj;
	int N_MASK[3][3] = { 0, 1, 0, 
						 1, 1, 1,
						 0, 1, 0 };

	//row , col ��ŭ �ݺ�
	for (i = -Size; i < Row - Size; i++)
		for (j = -Size; j < Col - Size; j++)
		{
			count = 0;
			for (maski = 0; maski < Size; maski++)
				for (maskj = 0; maskj < Size; maskj++)
				{
					if (N_MASK[maski][maskj] != 0)
					{
						//�ֺ� ����ũ �� �ռ�
						indexi = i + maski;
						indexj = j + maskj;

						//�ܰ� ó��
						if (indexi < 0) 
							indexi = -indexi;
						else if (indexi >= Row) 
							indexi = 2 * Row - indexi - 1;
						if (indexj < 0) 
							indexj = -indexj;
						else if (indexj >= Col) 
							indexj = 2 * Col - indexj - 1;

						if (Data_In[indexi][indexj] == 255) 
							count++;
					}
				}
			//�ֺ� 1 ���� 5 �̻��� ��쿡�� ħ�ĵ��� ����
			if (count == 5)
				Data_Out[i + Size][j + Size] = 255;
			else
				Data_Out[i + Size][j + Size] = 0;
		}

}

void binDilation(uchar** Data_In, uchar** Data_Out, int Row, int Col, int Size) // Size : kernel Size
{
	int i, j, count;
	int indexi, indexj;
	int maski, maskj;
	int N_MASK[3][3] = { 0, 1, 0,
						 1, 1, 1,
						 0, 1, 0 };

	for (i = -Size; i < Row - Size; i++)
		for (j = -Size; j < Col - Size; j++)
		{
			count = 0;
			for (maski = 0; maski < Size; maski++)
				for (maskj = 0; maskj < Size; maskj++)
				{
					if (N_MASK[maski][maskj] != 0)
					{
						indexi = i + maski;
						indexj = j + maskj;

						//�ܰ� ó��
						if (indexi < 0) indexi = -indexi;
						else if (indexi >= Row) indexi = 2 * Row - indexi - 1;
						if (indexj < 0) indexj = -indexj;
						else if (indexj >= Col) indexj = 2 * Col - indexj - 1;

						if (Data_In[indexi][indexj] == 255) count++;
					}
				}
			//���� ��� ī���Ͱ� 1�̶� �����ϸ� ��â����
			if (count)
				Data_Out[i + Size][j + Size] = 255;
			else
				Data_Out[i + Size][j + Size] = 0;
		}
}

void binOpening(uchar** Data_In, uchar** Data_Out, int Row, int Col, int Size)
{
	uchar** TempBlk;
	TempBlk = uc_alloc(Row, Col);
	binErosion(Data_In, TempBlk, Row, Col, Size);
	binDilation(TempBlk, Data_Out, Row, Col, Size);

}

void binClosing(uchar** Data_In, uchar** Data_Out, int Row, int Col, int Size)
{
	uchar** TempBlk;
	TempBlk = uc_alloc(Row, Col);
	binDilation(Data_In, TempBlk, Row, Col, Size);
	binErosion(TempBlk, Data_Out, Row, Col, Size);

}

void mopology(uchar** img, uchar** res,int row, int col, int mode)
{
	int block = 3,i,j;
	int mod;


	switch (mode)
	{
	case 0:
		binErosion(img, res, row, col, block);
		break;
	case 1:
		binDilation(img, res, row, col, block);
		break;
	case 2:
		binOpening(img, res, row, col, block);
		break;
	case 3:
		binClosing(img, res, row, col, block);
		break;
	}
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

	mode = atoi(argv[1]);			//atoi == �ƽ�Ű -> ���� ����ȯ (��ȯ�� ����)
	
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

		case 13 :

			median(img, res, row, col, arg0,5);
			break;

		case 14:

			median2(img, res, row, col, 0,arg0);
			break;

		case 15: 

			fftInit(img, res, row , col, arg0);
			break;

		case 16:
			dctInit(img, res, row, col, arg0);
			break;

		case 17:
			analysis(img, res, row, col, arg0);
			break;

		case 18 :
			mopology(img, res,row,col, arg0);
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


// ���α׷� ����: <Ctrl+F5> �Ǵ� [�����] > [��������� �ʰ� ����] �޴�
// ���α׷� �����: <F5> Ű �Ǵ� [�����] > [����� ����] �޴�

// ������ ���� ��: 
//   1. [�ַ�� Ž����] â�� ����Ͽ� ������ �߰�/�����մϴ�.
//   2. [�� Ž����] â�� ����Ͽ� �ҽ� ��� �����մϴ�.
//   3. [���] â�� ����Ͽ� ���� ��� �� ��Ÿ �޽����� Ȯ���մϴ�.
//   4. [���� ���] â�� ����Ͽ� ������ ���ϴ�.
//   5. [������Ʈ] > [�� �׸� �߰�]�� �̵��Ͽ� �� �ڵ� ������ ����ų�, [������Ʈ] > [���� �׸� �߰�]�� �̵��Ͽ� ���� �ڵ� ������ ������Ʈ�� �߰��մϴ�.
//   6. ���߿� �� ������Ʈ�� �ٽ� ������ [����] > [����] > [������Ʈ]�� �̵��ϰ� .sln ������ �����մϴ�.
