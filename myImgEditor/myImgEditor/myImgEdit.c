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

#define uchar unsigned char
#define MAXrange 256
#define MAX 255



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
		��Ʈ���� - mask��(0000 0001)�� �Է��ϴ� �Ű�������ŭ (1~7)*/


	for(i=0; i< row; i++)
		for (j = 0; j < col; j++)
		{
			if ((mask & img[i][j]) == mask) // =pow(2, position) ����
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
	int i, j;

	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
			res[i][j] = img[i][j] + ((int)gaussian()*50);


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

	//���� ����
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
