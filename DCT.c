#include "inter_intra_header.h"

//DCT : ���� ���󿡼� ���� DCT �� �� ����� double ������ ��� (8x8 ����� forward DCT)
double* DCT_U(unsigned char* org_img, int size_row, int size_cul, int DCT_N) {
	int i, j; //��ȣ ���� ��ǥ��
	int m, n; //block���� ��ǥ��;
	int u, v; //����, ���� ���ļ�
	double* DCT_img = NULL;

	DCT_img = (double*)malloc(sizeof(double)*(size_row*size_cul));

	for (i = 0; i < size_row; i = i + DCT_N) {
		for (j = 0; j< size_cul; j = j + DCT_N) {
			for (v = 0; v < DCT_N; v++) {
				for (u = 0; u < DCT_N; u++) {
					double temp = 0.0;
					double C_u = 1 / sqrt((double)DCT_N / 2.0);
					double C_v = 1 / sqrt((double)DCT_N / 2.0);
					if (u == 0)
						C_u = 1 / sqrt((double)DCT_N);
					if (v == 0)
						C_v = 1 / sqrt((double)DCT_N);
					for (m = 0; m < DCT_N; m++) {//f(n,m)
						for (n = 0; n < DCT_N; n++) {
							temp += cos(((2 * n + 1)*(double)u*pi) / (2.0*DCT_N)) * cos(((2 * m + 1)*(double)v*pi) / (2.0*DCT_N)) * (*(org_img + (j + m)*size_row + (i + n)));
						}
					}
					*(DCT_img + (j + v)*size_row + i + u) = temp*(C_u*C_v);//****
				}
			}
		}
	}
	return DCT_img;
	free(DCT_img);
}

double* DCT_I(int* org_img, int size_row, int size_cul, int DCT_N) {
	int i, j; //��ȣ ���� ��ǥ��
	int m, n; //block���� ��ǥ��
	int u, v; //����, ���� ���ļ�
	double* DCT_img = NULL;

	DCT_img = (double*)malloc(sizeof(double)*(size_row*size_cul));

	for (i = 0; i < size_row; i = i + DCT_N) {
		for (j = 0; j< size_cul; j = j + DCT_N) {
			for (v = 0; v < DCT_N; v++) {
				for (u = 0; u < DCT_N; u++) {
					double temp = 0.0;
					double C_u = 1.0 / sqrt((double)DCT_N / 2.0);
					double C_v = 1.0 / sqrt((double)DCT_N / 2.0);
					if (u == 0)
						C_u = 1.0 / sqrt((double)DCT_N);
					if (v == 0)
						C_v = 1.0 / sqrt((double)DCT_N);
					for (m = 0; m < DCT_N; m++) {//f(n,m)
						for (n = 0; n < DCT_N; n++) {
							temp += cos(((2 * n + 1)*(double)u*pi) / (2.0*DCT_N)) * cos(((2 * m + 1)*(double)v*pi) / (2.0*DCT_N)) * (double)(*(org_img + (j + m)*size_row + (i + n)));
						}
					}
					*(DCT_img + (j + v)*size_row + i + u) = temp*(C_u*C_v);//****
				}
			}
		}
	}
	return DCT_img;
	free(DCT_img);
}


//IDCT : double ���� DCT ������ �ٽ� �������� ������ unsinged char������ �����Ѵ�.
unsigned char* IDCT_U(double* DCT_img, int size_row, int size_cul, int DCT_N) {
	int i, j;
	int m, n; //block���� ��ǥ��;
	int u, v; //����, ���� ���ļ�
	double* IDCT_double = NULL;
	unsigned char* IDCT_img = NULL;

	IDCT_double = (double*)malloc(sizeof(double)*(size_row*size_cul));
	IDCT_img = (unsigned char*)malloc(sizeof(unsigned char)*(size_row*size_cul));

	//unsigned char �� ����ȯ �ϱ� �� 0.5 �����ֱ�.
	for (i = 0; i < size_row; i = i + DCT_N) {
		for (j = 0; j< size_cul; j = j + DCT_N) {
			for (n = 0; n < DCT_N; n++) {
				for (m = 0; m < DCT_N; m++) {
					double temp = 0.0;
					for (u = 0; u < DCT_N; u++) {//F(u,v)
						for (v = 0; v < DCT_N; v++) {
							double C_u = 1 / sqrt((double)DCT_N / 2.0);
							double C_v = 1 / sqrt((double)DCT_N / 2.0);
							if (u == 0)
								C_u = 1 / sqrt((double)DCT_N);
							if (v == 0)
								C_v = 1 / sqrt((double)DCT_N);
							temp += (C_u*C_v)*cos(((2 * n + 1)*(double)u*pi) / (2.0*DCT_N)) * cos(((2 * m + 1)*(double)v*pi) / (2.0*DCT_N)) * (*(DCT_img + (j + v)*size_row + i + u)); //***
						}
					}
					*(IDCT_img + (j + m)*size_row + i + n) = (unsigned char)(temp + 0.5);//****
				}
			}
		}
	}
	//uchar ������ ����ȯ���ֱ�

	return IDCT_img;
	free(IDCT_double);
	free(IDCT_img);
}

int* IDCT_I(double* DCT_Quantization_img, int size_row, int size_cul, int DCT_N) {
	int i, j;
	int m, n; //block���� ��ǥ��;
	int u, v; //����, ���� ���ļ�
	double* IDCT_double = NULL;
	int* IDCT_img = NULL;

	IDCT_double = (double*)malloc(sizeof(double)*(size_row*size_cul));
	IDCT_img = (int*)malloc(sizeof(int)*(size_row*size_cul));


	for (i = 0; i < size_row; i = i + DCT_N) {
		for (j = 0; j< size_cul; j = j + DCT_N) {
			for (n = 0; n < DCT_N; n++) {
				for (m = 0; m < DCT_N; m++) {
					double temp = 0.0;
					for (u = 0; u < DCT_N; u++) {//F(u,v)
						for (v = 0; v < DCT_N; v++) {
							double C_u = 1.0 / sqrt((double)DCT_N / 2.0);
							double C_v = 1.0 / sqrt((double)DCT_N / 2.0);
							if (u == 0)
								C_u = 1.0 / sqrt((double)DCT_N);
							if (v == 0)
								C_v = 1.0 / sqrt((double)DCT_N);
							temp += (C_u*C_v)*cos(((2 * n + 1)*(double)u*pi) / (2.0*DCT_N)) * cos(((2 * m + 1)*(double)v*pi) / (2.0*DCT_N)) * (*(DCT_Quantization_img + (j + v)*size_row + i + u)); //***
						}
					}
					if (temp >= 0)
						*(IDCT_img + (j + m)*size_row + i + n) = (int)(temp + 0.5);//****
					else
						*(IDCT_img + (j + m)*size_row + i + n) = (int)(temp - 0.5);
				}
			}
		}
	}

	return IDCT_img;
	free(IDCT_double);
	free(IDCT_img);
}
