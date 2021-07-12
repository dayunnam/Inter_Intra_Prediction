#include "inter_intra_header.h"

/*
inter_intra_decoder
1. reconstruct before image (<= intra prediction) <= label, Quantinized difference
2. reconstruct after image (<= intee of intra prediction)  <= label, motion vector (x,y),  Quantinized difference, flag_MB
*/



//(미확인)main 함수
int main(void) {
	unsigned char* before_renconstruct_img = NULL;
	unsigned char* current_renconstruct_img = NULL;
	unsigned char* encoding_img = NULL;
	unsigned char* decoding_img = NULL;

	unsigned char*flag_MB = NULL;
	
	unsigned char* Label_arr = NULL;//before 영상 label
	unsigned char*label_arr_after = NULL;
	int* Error = NULL; // before 영상 differencr


	int * mv_x = NULL;
	int * mv_y = NULL;
	int * difference = NULL;

	
	int mismatch;
	clock_t before1, after1, before2, after2, before, after;
	double Time1, Time2, total_time;
	flag_MB = (unsigned char*)malloc(sizeof(unsigned char)*(org_row * org_col / (N_tempo*N_tempo)));
	before_renconstruct_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row*org_col));
	current_renconstruct_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row*org_col));
	encoding_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row*org_col));
	decoding_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row*org_col));
	Label_arr = (unsigned char*)malloc(sizeof(unsigned char)*(org_col*org_row / (N_intra*N_intra)));
	Error = (int*)malloc(sizeof(int)*(org_col*org_row));
	label_arr_after = (unsigned char*)malloc(sizeof(unsigned char)*(org_col*org_row / (N_intra*N_intra)));
	mv_x = (int*)malloc(sizeof(int)*(org_row * org_col / (N_tempo*N_tempo)));
	mv_y = (int*)malloc(sizeof(int)*(org_row * org_col / (N_tempo*N_tempo)));
	difference = (int*)malloc(sizeof(int)*(org_row * org_col));

	before = clock();

	flag_MB = readFile(flag_MB_file, org_row / N_tempo, org_col / N_tempo);
	before1 = clock();
	Label_arr = readFile(label_file, org_row / N_intra, org_col / N_intra);
	Error = ReadFile_int(error_file, org_row, org_col);
	sort_Label(Label_arr); 

	before_renconstruct_img = decoding_intra(Label_arr, Error); // decoding : error 와 label 을 받아 복구 영상 출력
	after1 = clock();

	before2 = clock();
	label_arr_after = readFile(label_file_af, org_row / N_intra, org_col / N_intra);
	mv_x = ReadFile_int(MV_x_file, org_row / N_tempo, org_col / N_tempo);
	mv_y = ReadFile_int(MV_y_file, org_row / N_tempo, org_col / N_tempo);
	difference = ReadFile_int(Quantization_file, org_row, org_col);
	
	sort_Label(label_arr_after);
	
	//current_renconstruct_img = decoding_tempo(before_renconstruct_img, mv_x, mv_y, difference);
	current_renconstruct_img = inter_intra_decoding(before_renconstruct_img, flag_MB, label_arr_after, mv_x, mv_y, difference);

	after2 = clock();
	after = clock();
	Time1 = (double)(after1 - before1);
	Time2 = (double)(after2 - before2);
	total_time = (double)(after - before);
	WriteFile_U(before_renconstruct_img, reconstruct_file_bf_decoder, org_row, org_col); //복원 영상 출력
	WriteFile_U(current_renconstruct_img, current_decoding_file, org_row, org_col); //복원 영상 출력

																  
	encoding_img = readFile(reconstruct_file_bf_encoder, org_row, org_col);
	decoding_img = readFile(reconstruct_file_bf_decoder, org_row, org_col);


	if (use_psnr == 1) {
	printf("===========before image intra prediction===========");
	MSE_f(before_renconstruct_img, in_file_bf); // PSNR, MSE 계산, in_file과 비교
	printf("===========after image temporal prediction===========");
	MSE_f(current_renconstruct_img, in_file_af);
	}

	printf("===========before image intra prediction===========");
	mismatch = ismismatch(encoding_img, decoding_img);
	if (mismatch == FALSE) {
	printf("\nmismatch 없음\n");
	}
	else
	printf("\nmismatch 발생\n");
	printf("===========after image temporal prediction===========");

	encoding_img = readFile(current_encoding_file, org_row, org_col);
	decoding_img = readFile(current_decoding_file, org_row, org_col);
	mismatch = ismismatch(encoding_img, decoding_img);
	if (mismatch == FALSE) {
		printf("\nmismatch 없음\n");
	}
	else
		printf("\nmismatch 발생\n");

																  

	printf("\ntime : %.4f (msec)\n", Time1);
	printf("\ntime : %.4f (msec)\n", Time2);
	printf("\ntime : %.4f (msec)\n", total_time);
	free(mv_x);
	free(mv_y);
	free(difference);
	free(before_renconstruct_img);
	free(current_renconstruct_img);
	free(encoding_img);
	free(decoding_img);
	free(Label_arr);
	free(Error);
	return 0;
}
