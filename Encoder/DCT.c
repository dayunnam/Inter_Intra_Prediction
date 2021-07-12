#include "inter_intra_header.h"

//DCT : 원본 영상에서 가로 DCT 를 한 결과를 double 형으로 출력 (8x8 블록의 forward DCT)
double* DCT_U(unsigned char* org_img, int size_row, int size_cul,int DCT_N) {
	int i, j; //신호 공간 좌표값
	int m, n; //block안의 좌표값;
	int u, v; //가로, 세로 주파수
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
	int i, j; //신호 공간 좌표값
	int m, n; //block안의 좌표값
	int u, v; //가로, 세로 주파수
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


//IDCT : double 형의 DCT 영상을 다시 원본영상에 가깝게 unsinged char형으로 복원한다.
unsigned char* IDCT_U(double* DCT_img, int size_row, int size_cul, int DCT_N) {
	int i, j;
	int m, n; //block안의 좌표값;
	int u, v; //가로, 세로 주파수
	double* IDCT_double = NULL;
	unsigned char* IDCT_img = NULL;

	IDCT_double = (double*)malloc(sizeof(double)*(size_row*size_cul));
	IDCT_img = (unsigned char*)malloc(sizeof(unsigned char)*(size_row*size_cul));

	//unsigned char 로 형변환 하기 전 0.5 더해주기.
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
	//uchar 형으로 형변환해주기

	return IDCT_img;
	free(IDCT_double);
	free(IDCT_img);
}

int* IDCT_I(double* DCT_Quantization_img, int size_row, int size_cul, int DCT_N) {
	int i, j;
	int m, n; //block안의 좌표값;
	int u, v; //가로, 세로 주파수
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

/*
//main 함수
int main(void) {
	int size_row = org_row;
	int size_col = org_col;
	int i, j;
	int* buff_img_I = NULL;
	int* buff_img_I2 = NULL;
	double* buff_img_D = NULL;
	int mismatch;
	buff_img_I = (int*)malloc(sizeof(int)*(size_row*size_col));
	buff_img_I2 = (int*)malloc(sizeof(int)*(size_row*size_col));
	buff_img_D = (double*)malloc(sizeof(double)*(size_row*size_col));

	buff_img_I = ReadFile_int(error_file, org_row, org_col); //1. lena 영상 불러오기

	buff_img_D = DCT_I(buff_img_I, org_row, org_col);  //DCT
	buff_img_I2 = IDCT_I(buff_img_D, org_row, org_col);//IDCT

	//test 용=============================
	for (i = 0; i < org_col; i += N_tempo) {
		for (j = 0; j < org_row; j += N_tempo) {
			printf("image1 = %d, image2 = %d\n", *(buff_img_I + i*org_row + j), *(buff_img_I2 + i*org_row + j));
		}

	}
	//=====================================
	mismatch = ismismatch(buff_img_I, buff_img_I2);
	if (mismatch == FALSE) {
		printf("\nmismatch 없음\n");
	}
	else
		printf("\nmismatch 발생\n");

	free(buff_img_I);
	free(buff_img_D);
	free(buff_img_I2);
	return 0;
}*/