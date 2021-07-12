#include "inter_intra_header.h"


//unsinged char ���� ���� �б�
unsigned char* readFile(char* filename, int size_row, int size_col) {
	//FILE* input_img = fopen(in_file, "rb");
	FILE* input_img;
	unsigned char* org_img = NULL;
	fopen_s(&input_img, filename, "rb");

	org_img = (unsigned char*)malloc(sizeof(unsigned char)*(size_row*size_col));
	if (input_img == NULL) {
		puts("input ���� ���� ���� ");
		return NULL;
	}
	else {
		fseek(input_img, 0, SEEK_SET);
		fread((void*)org_img, sizebyte, size_row * size_col, input_img);
	}
	return org_img;
	fclose(input_img);
	free(org_img);
}

//int ���� ���� �б�
int* ReadFile_int(char* filename, int size_row, int size_col) {
	//FILE* input_img = fopen(in_file, "rb");
	FILE* input;
	int* org = NULL;

	fopen_s(&input, filename, "rb");
	org = (int*)malloc(sizeof(int)*(org_row*org_col));
	if (input == NULL) {
		puts("input ���� ���� ���� ");
		return NULL;
	}
	else {
		fseek(input, 0, SEEK_SET);
		fread((void*)org, sizeof(int), org_row*org_col, input);
	}
	fclose(input);
	return org;
	free(org);
}

//unsigned char �� ���� ����
unsigned char* WriteFile_U(unsigned char* out_img, char* filename, int size_row, int size_col) {
	//FILE* output_img = fopen(out_file, "wb");
	FILE* output_img;
	fopen_s(&output_img, filename, "wb");
	if (output_img == NULL) {
		puts("output ���� ���� ����");
		return NULL;
	}
	else {
		fseek(output_img, 0, SEEK_SET);
		fwrite((void*)out_img, sizebyte, size_row*size_col, output_img);
	}
	fclose(output_img);
	return out_img;

}

// int �� ���� ����
int* WriteFile_I(int* out_img, char* filename, int size_row, int size_col) {
	FILE* output_img;
	fopen_s(&output_img, filename, "wb");
	if (output_img == NULL) {
		puts("output ���� ���� ����");
		return NULL;
	}
	else {
		fseek(output_img, 0, SEEK_SET);
		fwrite((void*)out_img, sizeof(int), size_row*size_col, output_img);
	}
	fclose(output_img);
	return out_img;
}

//double�� ���� ���� + ������� �̹��� �����
double* WriteFile_D( double* double_img, char* filename, int size_row, int size_col) {
	unsigned char* uchar_img = NULL;
	FILE* output_img;
	fopen_s(&output_img, filename, "wb");
	int i, j;
	uchar_img = (unsigned char*)malloc(sizeof(unsigned char)*(size_row*size_col));
	for (i = 0; i< size_row; i++) {
		for (j = 0; j < size_col; j++) {
			double temp;
			temp = (*(double_img + j*size_row + i))*scale;
			if (temp < 0)
				*(uchar_img + j*size_row + i) = 0;
			else if (temp > 255)
				*(uchar_img + j*size_row + i) = 255;
			else
				*(uchar_img + j*size_row + i) = (unsigned char)temp;
		}
	}

	if (output_img == NULL) {
		puts("output ���� ���� ����");
		return NULL;
	}
	else {
		fseek(output_img, 0, SEEK_SET);
		fwrite((void*)uchar_img, sizebyte, size_row*size_col, output_img);
	}
	return double_img;
	fclose(output_img);
}