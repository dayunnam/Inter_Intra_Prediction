#include "inter_intra_header.h"
/*
inter_intra_encoder
1. intra prediction of before image
2. inter or intra prediction of after image
3. transmit array of [1. label, 2, Quantinized error] about before image and [1. label, 2. Quantinized error about intra, 3. motion vector_x,  4. motion_vector_y,  5. Quantinized error about inter]   
*/




//main 함수 
int main(void) {
	unsigned char* buff_img = NULL;
	unsigned char* encoding_current = NULL;
	unsigned char* recontruct_before = NULL;
	int * mv_x = NULL;
	int * mv_y = NULL;
	double sumofmv;
	clock_t before1, after1, before2, after2, before, after;
	double Time1, Time2, total_Time;
	buff_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row*org_col));
	encoding_current = (unsigned char*)malloc(sizeof(unsigned char)*(org_row*org_col));
	mv_x = (int*)malloc(sizeof(int)*(org_row * org_col / (N_tempo*N_tempo)));
	mv_y = (int*)malloc(sizeof(int)*(org_row * org_col / (N_tempo*N_tempo)));
	recontruct_before = (unsigned char*)malloc(sizeof(unsigned char)*(org_col*org_row));

	before = clock();
	before1 = clock();
	buff_img = readFile(in_file_bf, org_row, org_col);
	recontruct_before = encoding_intra(buff_img); // encoding : 원본 영상을 받아 error 와 label 출력

	after1 = clock();
	Time1 = (double)(after1 - before1);

	before2 = clock();
	buff_img = readFile(in_file_af, org_row, org_col);
	//encoding_current = encoding_tempo(buff_img, recontruct_before);
	encoding_current = inter_intra_encodinng(buff_img, recontruct_before);
	after2 = clock();
	after = clock();
	Time2 = (double)(after2 - before2);
	total_Time = (double)(after - before);
	//encoding_current = readFile(reconstruct_file_bf, org_row, org_col);

	WriteFile_U(encoding_current, current_encoding_file, org_row, org_col);

	if (use_psnr == 1) {
		printf("===========before image intra prediction===========");
		MSE_f(recontruct_before, in_file_bf); // PSNR, MSE 계산, in_file과 비교
		printf("===========after image temporal prediction===========");
		MSE_f(encoding_current, in_file_af); // PSNR, MSE 계산, in_file과 비교

	}

	//mv_x = ReadFile_int(MV_x_file, org_row / N_tempo, org_col / N_tempo);
	//mv_y = ReadFile_int(MV_y_file, org_row / N_tempo, org_col / N_tempo);
	//sumofmv = sum_of_mv(mv_x, mv_y);

	//printf("MV들의 절대값의 sum : %f", sumofmv);

	printf("\ndefore image encoding time : %.4f (msec)\n", Time1);
	printf("\ncurrent image encoding time : %.4f (msec)\n", Time2);
	printf("\n encoding time : %.4f (msec)\n", total_Time);
	free(buff_img);
	free(mv_x);
	free(mv_y);
	free(encoding_current);
	free(recontruct_before);
	return 0;
}