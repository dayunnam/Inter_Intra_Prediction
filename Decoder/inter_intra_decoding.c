#include "inter_intra_header.h"

unsigned char* inter_intra_decoding(unsigned char* before_recon_img, unsigned char* flag_MB, unsigned char* Label_arr, int* mv_x, int* mv_y, int* Q_diff) {
	int i, j;
	int u, v;
	int a, b;
	unsigned char label;
	int n = 0;
	unsigned char* inter_16x16 = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));
	int* IQ_diff_16x16 = (int*)malloc(sizeof(int)* (N_tempo*N_tempo));
	int* Q_diff_16x16 = (int*)malloc(sizeof(int)* (N_tempo*N_tempo));
	unsigned char* reconstruct_img2 = (unsigned char*)malloc(sizeof(unsigned char)*(org_row * org_col));
	unsigned char* intra_4x4 = (unsigned char*)malloc(sizeof(unsigned char)*(N_intra*N_intra));
	int* Q_diff_4x4 = (int*)malloc(sizeof(int)* (N_intra*N_intra));
	int* IQ_diff_4x4 = (int*)malloc(sizeof(int)* (N_intra*N_intra));
	unsigned char* n_pix = (unsigned char*)malloc(sizeof(unsigned char)*(N_intra + pre_N));
	

	for (i = 0; i < org_col; i += N_tempo) {
		for (j = 0; j < org_row; j += N_tempo) {
			
			//°ø°£¿¹Ãø¿¡¼­ º¹¿ø
			if (*(flag_MB + (i / N_tempo)*(org_row / N_tempo) + j / N_tempo) == 0) {
				n++;
				for (a = 0; a < N_tempo; a += N_intra) {
					for (b = 0; b < N_tempo; b += N_intra) {
						n_pix = neighbor_pixels(reconstruct_img2, j + b, i + a, 1);
						intra_4x4 = pre_block_intra(n_pix, *(Label_arr + ((i + a) / N_intra)*(org_row / N_intra) + (j + b) / N_intra), i + a, j + b);
						for (u = 0; u < N_intra; u++) {
							for (v = 0; v < N_intra; v++) {
								*(Q_diff_4x4 + u*N_intra + v) = *(Q_diff + (i + a + u)*org_row + (j + b + v));
							}
						}
						IQ_diff_4x4 = sampling_error(Q_diff_4x4, 1, N_intra, N_intra, N_intra);
						for (u = 0; u < N_intra; u++) {
							for (v = 0; v < N_intra; v++) {
								*(reconstruct_img2 + (i + a + u)*org_row + j + b + v) = clipping((int)(*(intra_4x4 + u*N_intra + v)) + *(IQ_diff_4x4 + u*N_intra + v));
							}
						}
					}
				}
			}

			else if (*(flag_MB + (i / N_tempo)*(org_row / N_tempo) + j / N_tempo) == 1) {
				for (u = 0; u < N_tempo; u++) {
					for (v = 0; v < N_tempo; v++) {
						*(Q_diff_16x16 + u*N_tempo + v) = *(Q_diff + (i + u)*org_row + (j + v)); 
					}
				}
				IQ_diff_16x16 = sampling_error(Q_diff_16x16, 1, N_tempo, N_tempo, N_tempo);
				inter_16x16 = pre_block_tempo(before_recon_img, i, j, *(mv_x + i / N_tempo*(org_row / N_tempo) + j / N_tempo), *(mv_y + i / N_tempo*(org_row / N_tempo) + j / N_tempo));
				for (u = 0; u < N_tempo; u++) {
					for (v = 0; v < N_tempo; v++) {
						*(reconstruct_img2 + (i + u)*org_row + (j + v)) = clipping((int)(*(inter_16x16 + u*N_tempo + v)) + *(IQ_diff_16x16 + u*N_tempo + v));
					}
				}
			}
			
		}
	}

	printf("n = %d\n", n);
	free(inter_16x16);
	return reconstruct_img2;
	free(reconstruct_img2);
}


