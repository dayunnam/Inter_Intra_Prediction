#include "inter_intra_header.h"

//motion vector 정보를 받으면, 예측 블록 출력
unsigned char* pre_block_tempo(unsigned char* before_dec_img, int current_cul, int current_row, int mv_x, int mv_y) {
	unsigned char* block_pre = NULL;
	int u, v;
	block_pre = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));

	for (u = 0; u < N_tempo; u++) {
		for (v = 0; v < N_tempo; v++) {
			*(block_pre + u*N_tempo + v) = *(before_dec_img + ((current_cul + mv_y) + u)*org_row + ((current_row + mv_x) + v));
		}
	}

	return block_pre;
	free(block_pre);
}

//블록의 현재 위치를 주면, search range를 구하고, 그안에 가장 적절한 블록을 향하는 motion vector 구하기
int* mv_finder(unsigned char* current_img, unsigned char* before_dec_img, int current_cul, int current_row) {
	int u, v;
	int n, m;
	int min;
	int left_row = 0;
	int right_row = 0;
	int top_cul = 0;
	int bottom_cul = 0;
	//int part_sum;
	int* motion_vector = NULL;
	int* energy = NULL;
	unsigned char* block_pre = NULL;
	unsigned char* block = NULL;

	block_pre = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));
	block = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));
	motion_vector = (int*)malloc(sizeof(int) * 2);



	for (n = 0; n < N_tempo; n++) {
		for (m = 0; m < N_tempo; m++) {
			*(block + n*N_tempo + m) = *(current_img + (current_cul + n)*org_row + current_row + m);
		}
	}

	if (current_row - SR < 0)
		left_row = (-1)*current_row;
	else
		left_row = (-1)*SR;
	if ((current_row + N_tempo - 1) + SR >= org_row)
		right_row = (org_row - 1) - (current_row + N_tempo - 1);
	else
		right_row = SR;
	if (current_cul - SR < 0)
		top_cul = (-1)*current_cul;
	else
		top_cul = (-1)*SR;
	if ((current_cul + N_tempo - 1) + SR >= org_col)
		bottom_cul = (org_col - 1) - (current_cul + N_tempo - 1);
	else
		bottom_cul = SR;
	//printf("left row :%d\n", left_row);//test 용===============
	//printf("right row :%d\n", right_row);//test 용===============
	//printf("top cul :%d\n", top_cul);//test 용===============
	//printf("bottom cul :%d\n", bottom_cul);//test 용===============

	energy = (int*)malloc(sizeof(int) * (bottom_cul - top_cul + 1)*(right_row - left_row + 1));
	//search_range 안에 하나의 16 by 16 블록을 만들어(만약 영상의 가장자리를 넘어간다면, -1 저장) 복원영상과의 energy 저장
	for (u = top_cul; u <= bottom_cul; u++) {
		for (v = left_row; v <= right_row; v++) {
			*(energy + (u - top_cul)*(right_row - left_row + 1) + (v - left_row)) = 0;

			//printf("%d\n",(u - top_cul)*(right_row - left_row + 1) + (v-left_row)) ;//test 용=====================

			block_pre = pre_block_tempo(before_dec_img, current_cul, current_row, v, u); //v = mv_x, u = mv_y
			*(energy + (u - top_cul)*(right_row - left_row + 1) + (v - left_row)) = energy_f(block, block_pre, N_tempo, N_tempo);
		}
	}

	min = *(energy);
	*(motion_vector) = left_row; //x 좌표
	*(motion_vector + 1) = top_cul; //y 좌표
	for (u = top_cul; u <= bottom_cul; u++) {
		for (v = left_row; v <= right_row; v++) {
			if (min >= *(energy + (u - top_cul)*(right_row - left_row + 1) + (v - left_row))) {
				min = *(energy + (u - top_cul)*(right_row - left_row + 1) + (v - left_row));
				*(motion_vector) = v;
				*(motion_vector + 1) = u;
				//printf("x = %d, y = %d\n", *(motion_vector), *(motion_vector + 1));//test 용======================
			}
		}
	}
	//printf("x = %d, y = %d\n", *(motion_vector), *(motion_vector + 1));//test 용======================
	// 가장 energy 가 작은 블록을 찾아, 그 블록에서 현재 블록의 위치 까지의 모션벡터를 구한다.
	//printf("0번째 에너지 = %d, 1번째 에너지 = %d \n", *(energy), *(energy + 1));
	//printf("dv\n");//test 용====================
	free(energy);
	free(block_pre);
	free(block);
	return  motion_vector;
	free(motion_vector);

}

//decoding
unsigned char* decoding_tempo(unsigned char* before_dec_img, int* mv_x, int* mv_y, int* Q_diff) {
	int i, j;
	int u, v;
	int * invQ_difference = NULL;
	unsigned char * reconstruct_img = NULL;
	unsigned char * block_pre = NULL;
	invQ_difference = (int*)malloc(sizeof(int)*(org_row * org_col));
	reconstruct_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row * org_col));
	block_pre = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));

	invQ_difference = sampling_error(Q_diff, 1, org_row, org_col, N_tempo); //역양자화

	for (i = 0; i < org_col; i += N_tempo) {
		for (j = 0; j < org_row; j += N_tempo) {
			//printf("i =  %d, j = %d\n",i, j);//test 용===========
			block_pre = pre_block_tempo(before_dec_img, i, j, *(mv_x + i / N_tempo*(org_row / N_tempo) + j / N_tempo), *(mv_y + i / N_tempo*(org_row / N_tempo) + j / N_tempo));
			for (u = 0; u < N_tempo; u++) {
				for (v = 0; v < N_tempo; v++) {
					*(reconstruct_img + (i + u)*org_row + (j + v)) = clipping((int)(*(block_pre + u*N_tempo + v)) + *(invQ_difference + (i + u)*org_row + (j + v)));
				
				}
			}
		}
	}

	free(block_pre);
	free(invQ_difference);
	return reconstruct_img;


	free(reconstruct_img);
}

//encoder 
unsigned char* encoding_tempo(unsigned char* current_img, unsigned char* before_dec_img) {
	int i, j;
	int u, v;
	unsigned char* block_pre = NULL;
	unsigned char * reconstruct_img = NULL;
	unsigned char * pre_img = NULL;//test 용===========
	int * mv_x = NULL;
	int * mv_y = NULL;
	int * a_mv = NULL;
	int * difference = NULL;

	block_pre = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));
	reconstruct_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row * org_col));
	pre_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row * org_col));//test 용=============
	mv_x = (int*)malloc(sizeof(int)*(org_row * org_col / (N_tempo*N_tempo)));
	mv_y = (int*)malloc(sizeof(int)*(org_row * org_col / (N_tempo*N_tempo)));
	a_mv = (int*)malloc(sizeof(int) * 2);
	difference = (int*)malloc(sizeof(int)*(org_row * org_col));


	for (i = 0; i < org_col; i += N_tempo) {
		for (j = 0; j < org_row; j += N_tempo) {
			//printf("111\n");//test 용====================
			a_mv = mv_finder(current_img, before_dec_img, i, j);
			*(mv_x + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = *(a_mv);
			*(mv_y + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = *(a_mv + 1);
			block_pre = pre_block_tempo(before_dec_img, i, j, *(a_mv), *(a_mv + 1));
			//printf("222\n");//test 용====================
			for (u = 0; u < N_tempo; u++) {
				for (v = 0; v < N_tempo; v++) {
					*(difference + (i + u)*org_row + (j + v)) = *(current_img + (i + u)*org_row + (j + v)) - *(block_pre + u*N_tempo + v);
					*(pre_img + (i + u)*org_row + (j + v)) = *(block_pre + u*N_tempo + v);//test 용===============
				}
			}
		}
	}
	//printf("333\n");//test 용====================
	difference = sampling_error(difference, 0, org_row, org_col,N_tempo);
	//printf("444\n");//test 용====================
	reconstruct_img = decoding_tempo(before_dec_img, mv_x, mv_y, difference);
	//printf("555\n");//test 용====================
	WriteFile_I(mv_x, MV_x_file, org_row / N_tempo, org_col / N_tempo);//motion vector 의 x값만 저장하기
	WriteFile_I(mv_y, MV_y_file, org_row / N_tempo, org_col / N_tempo);//motion vector 의 y값만 저장하기
	WriteFile_I(difference, Quantization_file, org_row, org_col);//differency block 저장하기
	WriteFile_U(pre_img, prediction_defore, org_row, org_col);
	free(mv_x);
	free(mv_y);
	free(a_mv);
	free(difference);
	free(block_pre);
	free(pre_img);//test 용=================
	return reconstruct_img;
	free(reconstruct_img);
}

//mv 절댓값의 총합
double sum_of_mv(int* mv_x, int* mv_y) {
	double sum = 0;
	int x_2;
	int y_2;
	int i, j;
	for (i = 0; i < org_col; i += N_tempo) {
		for (j = 0; j < org_row; j += N_tempo) {
			x_2 = (*(mv_x + i / N_tempo*(org_row / N_tempo) + j / N_tempo))*(*(mv_x + i / N_tempo*(org_row / N_tempo) + j / N_tempo));
			y_2 = (*(mv_y + i / N_tempo*(org_row / N_tempo) + j / N_tempo))*(*(mv_y + i / N_tempo*(org_row / N_tempo) + j / N_tempo));
			sum += sqrt(x_2 + y_2);
		}
	}
	return sum;
}

//mv 절댓값의 평균
double average_of_mv(int* mv_x, int* mv_y, int num_inter) {
	double sum = 0;
	double avg = 0;
	int x_2;
	int y_2;
	int i, j;
	for (i = 0; i < org_col; i += N_tempo) {
		for (j = 0; j < org_row; j += N_tempo) {
			x_2 = (*(mv_x + i / N_tempo*(org_row / N_tempo) + j / N_tempo))*(*(mv_x + i / N_tempo*(org_row / N_tempo) + j / N_tempo));
			y_2 = (*(mv_y + i / N_tempo*(org_row / N_tempo) + j / N_tempo))*(*(mv_y + i / N_tempo*(org_row / N_tempo) + j / N_tempo));
			sum += sqrt(x_2 + y_2);
		}
	}
	avg = sum / (double)num_inter;
	return avg;
}