#include "inter_intra_header.h"


//MSE & PSNR **입력은 uchar 형**
double MSE_f(unsigned char* sp_img, char* s)
{
	double MSE;
	double PSNR;
	double sum = 0.0;
	int i, j;
	unsigned char* org_img2 = NULL;
	FILE* in2;

	org_img2 = (unsigned char*)malloc(sizeof(unsigned char)*(org_row*org_col));

	fopen_s(&in2, s, "rb");
	
	fseek(in2, 0, SEEK_SET);
	//printf("sfv");//test 용=========
	fread((void*)org_img2, sizebyte, org_row*org_col, in2);
	
	for (i = 0; i < org_col; i++)
	{
		for (j = 0; j< org_row; j++) {
			double temp = (double)(*(org_img2 + i*org_row + j)) - (double)(*(sp_img + i*org_row + j));
			sum += pow(temp, 2); //sum += temp*temp
		}
	}
	
	MSE = (long double)sum / ((double)org_row*(double)org_col);
	PSNR = 10 * log10(255.0*255.0 / MSE);
	fseek(in2, 0, SEEK_SET);
	printf("\n\nMSE = %f\n\n", MSE);
	printf("\n\nPSNR = %f\n\n", PSNR);
	fclose(in2);
	free(org_img2);
	return MSE;
}

//mismatch 확인
int ismismatch(unsigned char* en_img, unsigned char* de_img) {
	int i, j;
	for (i = 0; i < org_col; i++)
	{
		for (j = 0; j< org_row; j++) {
			if (*(en_img + i*org_row + j) != *(de_img + i*org_row + j)) {
				printf("\n%d 열 %d 행 : mismatch %d \n", i, j, *(en_img + i*org_row + j) - *(de_img + i*org_row + j));//test 용===========
				return TRUE;
			}
		}
	}

	return FALSE;
}


int* sampling_error(int* error, int type, int size_row, int size_col, int DCT_N) {
	int n;
	double* error_dct = NULL;
	int* error_idct = NULL;
	int* error_d = NULL;
	double* error_double = NULL;
	error_dct = (double*)malloc(sizeof(double)*(size_row*size_col));
	error_idct = (int*)malloc(sizeof(int)*(size_row*size_col));
	error_d = (int*)malloc(sizeof(int)*(size_row*size_col));
	error_double = (double*)malloc(sizeof(double)*(size_row*size_col));

	//error_double = DCT_I(error, size_row, size_col);
	//error = IDCT_I(error_double, size_row, size_col);

	//type 이 0이면 error 를 양자화만 함
	if (type == 0) {
		error_dct = DCT_I(error, size_row, size_col, DCT_N);//double 형
		for (n = 0; n < size_row*size_col; n++) {
			if ((*(error_dct + n)) > 0)
				*(error_d + n) = (int)((*(error_dct + n)) / sample + 0.5);
			else
				*(error_d + n) = (int)((*(error_dct + n)) / sample - 0.5);
		}
		return error_d;

	}

	//type = 1 양자화된 에러를 다시 복구함 
	else if (type == 1) {
		for (n = 0; n < size_row*size_col; n++) {
			*(error_double + n) = ((double)(*(error + n))*sample);
		}
		error_idct = IDCT_I(error_double, size_row, size_col,  DCT_N);
		return error_idct;
	}
	return NULL;
}
