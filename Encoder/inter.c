#include "inter_intra_header.h"

//motion vector ������ ������, ���� ��� ���
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

//����� ���� ��ġ�� �ָ�, search range�� ���ϰ�, �׾ȿ� ���� ������ ����� ���ϴ� motion vector ���ϱ�
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
	//printf("left row :%d\n", left_row);//test ��===============
	//printf("right row :%d\n", right_row);//test ��===============
	//printf("top cul :%d\n", top_cul);//test ��===============
	//printf("bottom cul :%d\n", bottom_cul);//test ��===============

	energy = (int*)malloc(sizeof(int) * (bottom_cul - top_cul + 1)*(right_row - left_row + 1));
	//search_range �ȿ� �ϳ��� 16 by 16 ����� �����(���� ������ �����ڸ��� �Ѿ�ٸ�, -1 ����) ����������� energy ����
	for (u = top_cul; u <= bottom_cul; u++) {
		for (v = left_row; v <= right_row; v++) {
			*(energy + (u - top_cul)*(right_row - left_row + 1) + (v - left_row)) = 0;

			//printf("%d\n",(u - top_cul)*(right_row - left_row + 1) + (v-left_row)) ;//test ��=====================

			block_pre = pre_block_tempo(before_dec_img, current_cul, current_row, v, u); //v = mv_x, u = mv_y
			*(energy + (u - top_cul)*(right_row - left_row + 1) + (v - left_row)) = energy_f(block, block_pre, N_tempo, N_tempo);
		}
	}

	min = *(energy);
	*(motion_vector) = left_row; //x ��ǥ
	*(motion_vector + 1) = top_cul; //y ��ǥ
	for (u = top_cul; u <= bottom_cul; u++) {
		for (v = left_row; v <= right_row; v++) {
			if (min >= *(energy + (u - top_cul)*(right_row - left_row + 1) + (v - left_row))) {
				min = *(energy + (u - top_cul)*(right_row - left_row + 1) + (v - left_row));
				*(motion_vector) = v;
				*(motion_vector + 1) = u;
				//printf("x = %d, y = %d\n", *(motion_vector), *(motion_vector + 1));//test ��======================
			}
		}
	}
	//printf("x = %d, y = %d\n", *(motion_vector), *(motion_vector + 1));//test ��======================
	// ���� energy �� ���� ����� ã��, �� ��Ͽ��� ���� ����� ��ġ ������ ��Ǻ��͸� ���Ѵ�.
	//printf("0��° ������ = %d, 1��° ������ = %d \n", *(energy), *(energy + 1));
	//printf("dv\n");//test ��====================
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

	invQ_difference = sampling_error(Q_diff, 1, org_row, org_col, N_tempo); //������ȭ

	for (i = 0; i < org_col; i += N_tempo) {
		for (j = 0; j < org_row; j += N_tempo) {
			//printf("i =  %d, j = %d\n",i, j);//test ��===========
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
	unsigned char * pre_img = NULL;//test ��===========
	int * mv_x = NULL;
	int * mv_y = NULL;
	int * a_mv = NULL;
	int * difference = NULL;

	block_pre = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));
	reconstruct_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row * org_col));
	pre_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row * org_col));//test ��=============
	mv_x = (int*)malloc(sizeof(int)*(org_row * org_col / (N_tempo*N_tempo)));
	mv_y = (int*)malloc(sizeof(int)*(org_row * org_col / (N_tempo*N_tempo)));
	a_mv = (int*)malloc(sizeof(int) * 2);
	difference = (int*)malloc(sizeof(int)*(org_row * org_col));


	for (i = 0; i < org_col; i += N_tempo) {
		for (j = 0; j < org_row; j += N_tempo) {
			//printf("111\n");//test ��====================
			a_mv = mv_finder(current_img, before_dec_img, i, j);
			*(mv_x + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = *(a_mv);
			*(mv_y + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = *(a_mv + 1);
			block_pre = pre_block_tempo(before_dec_img, i, j, *(a_mv), *(a_mv + 1));
			//printf("222\n");//test ��====================
			for (u = 0; u < N_tempo; u++) {
				for (v = 0; v < N_tempo; v++) {
					*(difference + (i + u)*org_row + (j + v)) = *(current_img + (i + u)*org_row + (j + v)) - *(block_pre + u*N_tempo + v);
					*(pre_img + (i + u)*org_row + (j + v)) = *(block_pre + u*N_tempo + v);//test ��===============
				}
			}
		}
	}
	//printf("333\n");//test ��====================
	difference = sampling_error(difference, 0, org_row, org_col,N_tempo);
	//printf("444\n");//test ��====================
	reconstruct_img = decoding_tempo(before_dec_img, mv_x, mv_y, difference);
	//printf("555\n");//test ��====================
	WriteFile_I(mv_x, MV_x_file, org_row / N_tempo, org_col / N_tempo);//motion vector �� x���� �����ϱ�
	WriteFile_I(mv_y, MV_y_file, org_row / N_tempo, org_col / N_tempo);//motion vector �� y���� �����ϱ�
	WriteFile_I(difference, Quantization_file, org_row, org_col);//differency block �����ϱ�
	WriteFile_U(pre_img, prediction_defore, org_row, org_col);
	free(mv_x);
	free(mv_y);
	free(a_mv);
	free(difference);
	free(block_pre);
	free(pre_img);//test ��=================
	return reconstruct_img;
	free(reconstruct_img);
}

//mv ������ ����
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

//mv ������ ���
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