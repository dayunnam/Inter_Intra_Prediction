#include "inter_intra_header.h"


//시간예측
// motion vector 정보를 받으면, 예측 블록 출력
unsigned char* pre_block_tempo(unsigned char* before_dec_img, int i, int j, int mv_x, int mv_y) {
	unsigned char* block_pre = NULL;
	int u, v;
	block_pre = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));

	for (u = 0; u < N_tempo; u++) {
		for (v = 0; v < N_tempo; v++) {
			*(block_pre + u*N_tempo + v) = *(before_dec_img + ((i + mv_y) + u)*org_row + ((j + mv_x) + v));
		}
	}

	return block_pre;
	free(block_pre);
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

	invQ_difference = sampling_error(Q_diff, 1, org_row, org_col,N_tempo); //역양자화

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


