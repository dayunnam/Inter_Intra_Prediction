#include "inter_intra_header.h"

//공간예측

//블록의 이웃한 픽셀 축력하기, type = 1 인 경우는 시간 공간 예측인 경우, type = 0 은 공간예측만 할 경우
unsigned char* neighbor_pixels(unsigned char* org_img, int current_row, int current_cul, int type) { // current_row = 행, current_cul = 열
	int u, v;
	int w;
	unsigned char* neighbor_pix = NULL;
	neighbor_pix = (unsigned char*)malloc(sizeof(unsigned char)*(N_intra + pre_N)); //8 or 13
	if (org_img == NULL) {
		for (w = 0; w < pre_N; w++) {
			*(neighbor_pix + w) = 128;
		}
	}

	else {
		//A,B,C,D
		for (u = 0; u < N_intra; u++) {
			if ((current_cul - 1) >= 0)
				*(neighbor_pix + u) = *(org_img + (current_cul - 1)*org_row + current_row + u);
			else
				*(neighbor_pix + u) = 128;
		}

		//I,J,K,L
		for (v = 0; v < N_intra; v++) {
			if ((current_row - 1) >= 0)
				*(neighbor_pix + N_intra + v) = *(org_img + (current_cul + v)*org_row + current_row - 1);
			else
				*(neighbor_pix + N_intra + v) = 128;
		}
		if (pre_N == 9) {
			//E,F,G,H
			if (type == 1) {
				if ((current_row + N_intra + 3 <= org_row - 1) && (current_row % N_tempo == 0 || current_cul % N_tempo == 0)) {

					for (u = 0; u < N_intra; u++) {
						if ((current_cul - 1) >= 0)
							*(neighbor_pix + 2 * N_intra + u) = *(org_img + (current_cul - 1)*org_row + current_row + u + N_intra);
						else
							*(neighbor_pix + 2 * N_intra + u) = 128;
					}

				}
				else {
					for (u = 0; u < N_intra; u++) {
						if ((current_cul - 1) >= 0)
							*(neighbor_pix + 2 * N_intra + u) = *(org_img + (current_cul - 1)*org_row + current_row + 3);
						else
							*(neighbor_pix + 2 * N_intra + u) = 128;
					}
				}
			}


			else if (type == 0) {
				if ((current_row + N_intra + 3 <= org_row - 1)) {

					for (u = 0; u < N_intra; u++) {
						if ((current_cul - 1) >= 0)
							*(neighbor_pix + 2 * N_intra + u) = *(org_img + (current_cul - 1)*org_row + current_row + u + N_intra);
						else
							*(neighbor_pix + 2 * N_intra + u) = 128;
					}

				}
				else {
					for (u = 0; u < N_intra; u++) {
						if ((current_cul - 1) >= 0)
							*(neighbor_pix + 2 * N_intra + u) = *(org_img + (current_cul - 1)*org_row + current_row + 3);
						else
							*(neighbor_pix + 2 * N_intra + u) = 128;
					}
				}
			}

			//Q
			if ((current_cul - 1) >= 0 && (current_row - 1) >= 0)
				*(neighbor_pix + 3 * N_intra) = *(org_img + (current_cul - 1)*org_row + current_row - 1);
			else
				*(neighbor_pix + 3 * N_intra) = 128;
		}
	}
	return  neighbor_pix;
	free(neighbor_pix);
}

//예측 블록 출력 
unsigned char* pre_block_intra(unsigned char* neighbor_pix, int type, int current_cul, int current_row) {
	unsigned char* block = NULL;

	int i, u, v;
	unsigned char m;
	//double sum = 0.0;
	int sum = 0;

	int A = *(neighbor_pix);
	int B = *(neighbor_pix + 1);
	int C = *(neighbor_pix + 2);
	int D = *(neighbor_pix + 3);
	int I = *(neighbor_pix + 4);
	int J = *(neighbor_pix + 5);
	int K = *(neighbor_pix + 6);
	int L = *(neighbor_pix + 7);
	int E, F, G, H, Q;
	if (pre_N == 9) {
		E = *(neighbor_pix + 8);
		F = *(neighbor_pix + 9);
		G = *(neighbor_pix + 10);
		H = *(neighbor_pix + 11);
		Q = *(neighbor_pix + 12);
	}

	block = (unsigned char*)malloc(sizeof(unsigned char)*(N_intra*N_intra));

	if (type == 0) {
		for (v = 0; v < N_intra; v++) {
			for (u = 0; u < N_intra; u++) {
				*(block + v*N_intra + u) = *(neighbor_pix + u);
			}
		}
	}
	else if (type == 1) {
		for (v = 0; v < N_intra; v++) {
			for (u = 0; u < N_intra; u++) {
				*(block + v*N_intra + u) = *(neighbor_pix + v + N_intra);
			}
		}
	}

	else if (type == 2) {
		sum = 0;
		if (current_cul - 1 >= 0 && current_row - 1 >= 0) {
			for (i = 0; i < 2 * N_intra; i++) {

				sum += *(neighbor_pix + i);
			}
			m = (sum + 4) >> 3;

		}
		else if (current_cul - 1 < 0 && current_row - 1 >= 0) { // 세로 막대 = 128
			for (i = 0; i < N_intra; i++) {
				sum += *(neighbor_pix + i);
			}
			m = (unsigned char)(sum / ((double)N_intra) + 0.5);
		}
		else if (current_cul - 1 >= 0 && current_row - 1 < 0) { // 가로 막대 = 128
			for (i = 0; i < N_intra; i++) {
				sum += *(neighbor_pix + N_intra + i);
			}
			m = (unsigned char)(sum / ((double)N_intra) + 0.5);
		}
		else
			m = 128;

		for (v = 0; v < N_intra; v++) {
			for (u = 0; u < N_intra; u++) {
				*(block + v*N_intra + u) = m;
			}
		}
	}

	else if (type == 3) {
		if (pre_N == 4) {
			for (i = 0; i < N_intra*N_intra; i++) {
				if (i == 0 || i == 5 || i == 10 || i == 15)
					*(block + i) = (A + I + 1) >> 1;
				else if (i == 1 || i == 6 || i == 11)
					*(block + i) = (A + B + 1) >> 1;
				else if (i == 2 || i == 7)
					*(block + i) = (B + C + 1) >> 1;
				else if (i == 3)
					*(block + i) = (C + D + 1) >> 1;
				else if (i == 4 || i == 9 || i == 14)
					*(block + i) = (I + J + 1) >> 1;
				else if (i == 8 || i == 13)
					*(block + i) = (J + K + 1) >> 1;
				else if (i == 12)
					*(block + i) = (K + L + 1) >> 1;
			}
		}
		else if (pre_N == 9) {
			for (i = 0; i < N_intra*N_intra; i++) {
				if (i == 0)
					*(block + i) = (A + 2 * B + C + 2) >> 2;
				else if (i == 1 || i == 4)
					*(block + i) = (B + 2 * C + D + 2) >> 2;
				else if (i == 2 || i == 5 || i == 8)
					*(block + i) = (C + 2 * D + E + 2) >> 2;
				else if (i == 3 || i == 6 || i == 9 || i == 12)
					*(block + i) = (D + 2 * E + F + 2) >> 2;
				else if (i == 7 || i == 10 || i == 13)
					*(block + i) = (E + 2 * F + G + 2) >> 2;
				else if (i == 11 || i == 14)
					*(block + i) = (F + 2 * G + H + 2) >> 2;
				else if (i == 15)
					*(block + i) = (G + 3 * H + 2) >> 2;
			}
		}
	}
	if (pre_N == 9) {
		if (type == 4) {
			for (i = 0; i < N_intra*N_intra; i++) {
				if (i == 0 || i == 5 || i == 10 || i == 15)
					*(block + i) = (A + 2 * Q + I + 2) >> 2;
				else if (i == 1 || i == 6 || i == 11)
					*(block + i) = (Q + 2 * A + B + 2) >> 2;
				else if (i == 2 || i == 7)
					*(block + i) = (A + 2 * B + C + 2) >> 2;
				else if (i == 3)
					*(block + i) = (B + 2 * C + D + 2) >> 2;
				else if (i == 4 || i == 9 || i == 14)
					*(block + i) = (Q + 2 * I + J + 2) >> 2;
				else if (i == 8 || i == 13)
					*(block + i) = (I + 2 * J + K + 2) >> 2;
				else if (i == 12)
					*(block + i) = (J + 2 * K + L + 2) >> 2;
			}
		}
		else if (type == 5) {
			for (i = 0; i < N_intra*N_intra; i++) {
				if (i == 0)
					*(block + i) = (A + B + 1) >> 1;
				else if (i == 1 || i == 8)
					*(block + i) = (B + C + 1) >> 1;
				else if (i == 2 || i == 9)
					*(block + i) = (C + D + 1) >> 1;
				else if (i == 3 || i == 10)
					*(block + i) = (D + E + 1) >> 1;
				else if (i == 11)
					*(block + i) = (E + F + 1) >> 1;
				else if (i == 4)
					*(block + i) = (A + 2 * B + C + 2) >> 2;
				else if (i == 5 || i == 12)
					*(block + i) = (B + 2 * C + D + 2) >> 2;
				else if (i == 6 || i == 13)
					*(block + i) = (C + 2 * D + E + 2) >> 2;
				else if (i == 7 || i == 14)
					*(block + i) = (D + 2 * E + F + 2) >> 2;
				else if (i == 15)
					*(block + i) = (E + 2 * F + G + 2) >> 2;
			}
		}
		else if (type == 6) {
			for (i = 0; i < N_intra*N_intra; i++) {
				if (i == 0 || i == 6)
					*(block + i) = (Q + I + 1) >> 1;
				else if (i == 1 || i == 7)
					*(block + i) = (I + 2 * Q + A + 2) >> 2;
				else if (i == 2)
					*(block + i) = (Q + 2 * A + B + 2) >> 2;
				else if (i == 3)
					*(block + i) = (A + 2 * B + C + 2) >> 2;
				else if (i == 4 || i == 10)
					*(block + i) = (I + J + 1) >> 1;
				else if (i == 5 || i == 11)
					*(block + i) = (Q + 2 * I + J + 2) >> 2;
				else if (i == 8 || i == 14)
					*(block + i) = (J + K + 1) >> 1;
				else if (i == 9 || i == 15)
					*(block + i) = (I + 2 * J + K + 2) >> 2;
				else if (i == 12)
					*(block + i) = (K + L + 1) >> 1;
				else if (i == 13)
					*(block + i) = (J + 2 * K + L + 2) >> 2;
			}
		}
		else if (type == 7) {
			for (i = 0; i < N_intra*N_intra; i++) {
				if (i == 0 || i == 9)
					*(block + i) = (Q + A + 1) >> 1;
				else if (i == 1 || i == 10)
					*(block + i) = (A + B + 1) >> 1;
				else if (i == 2 || i == 11)
					*(block + i) = (B + C + 1) >> 1;
				else if (i == 3)
					*(block + i) = (C + D + 1) >> 1;
				else if (i == 4 || i == 13)
					*(block + i) = (I + 2 * Q + A + 2) >> 2; //****************************
				else if (i == 5 || i == 14)
					*(block + i) = (Q + 2 * A + B + 2) >> 2;
				else if (i == 6 || i == 15)
					*(block + i) = (A + 2 * B + C + 2) >> 2;
				else if (i == 7)
					*(block + i) = (B + 2 * C + D + 2) >> 2;
				else if (i == 8)
					*(block + i) = (Q + 2 * I + J + 2) >> 2;
				else if (i == 12)
					*(block + i) = (I + 2 * J + K + 2) >> 2;
			}
		}
		else if (type == 8) {
			for (i = 0; i < N_intra*N_intra; i++) {
				if (i == 0)
					*(block + i) = (I + J + 1) >> 1;
				else if (i == 1)
					*(block + i) = (I + 2 * J + K + 2) >> 2;
				else if (i == 2 || i == 4)
					*(block + i) = (J + K + 1) >> 1;
				else if (i == 3 || i == 5)
					*(block + i) = (J + 2 * K + L + 2) >> 2;
				else if (i == 6 || i == 8)
					*(block + i) = (K + L + 1) >> 1;
				else if (i == 7 || i == 9)
					*(block + i) = (K + 3 * L + 2) >> 2;
				else if (i >= 10)
					*(block + i) = L;
			}
		}
	}
	return block;
	free(block);
}


//test 용====================
void sort_Label(unsigned char* Label_arr) {

	int* label_N = NULL;
	int i, j, w;

	label_N = (int*)malloc(sizeof(int)*(pre_N));
	for (w = 0; w < pre_N; w++) {
		*(label_N + w) = 0;
	}

	for (i = 0; i < org_col / N_intra; i++) {
		for (j = 0; j < org_row / N_intra; j++) {
			for (w = 0; w < pre_N; w++) {
				if (*(Label_arr + i*(org_row / N_intra) + j) == w) {
					*(label_N + w) += 1;
				}
			}
		}
	}
	printf("\n");
	for (w = 0; w < pre_N; w++) {
		printf("number of %d label :  %d\n", w, *(label_N + w));
	}

	free(label_N);
}

//test 용====================
void sort_Error(int* Error) {

	int* error_N = NULL;
	int i, j, w;

	error_N = (int*)malloc(sizeof(int)*(256 * 2));
	for (w = 0; w < 256 * 2; w++) {
		*(error_N + w) = 0;
	}

	for (i = 0; i < org_col; i++) {
		for (j = 0; j < org_row; j++) {
			for (w = 0; w < 2 * 256; w++) {
				if (*(Error + i*(org_row / N_intra) + j) == w - 255) {
					*(error_N + w) += 1;
				}
			}
		}
	}
	printf("\n");
	for (w = -15 + 255; w < 15 + 255; w++) {
		printf("number of %d error :  %d\n", w - 255, *(error_N + w));
	}

	free(error_N);
}


//양자화된 error와 label 을 이용하여, decoding 하기
unsigned char* decoding_intra(unsigned char* Label_arr, int* Error) {
	unsigned char* res_img = NULL; //복구 영상
	unsigned char* neighbor_pix = NULL;
	unsigned char* block_pre = NULL;

	int i, j;
	int u, v;
	unsigned char label;


	res_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_col*org_row));
	neighbor_pix = (unsigned char*)malloc(sizeof(unsigned char)*(N_intra + pre_N)); //8 or 13
	block_pre = (unsigned char*)malloc(sizeof(unsigned char)*(N_intra*N_intra));

	//sort_Error(Error); //test 용==========================

	Error = sampling_error(Error, 1, org_row, org_col, N_intra); //양자화된 error 복구
														//sort_Error(Error); //test 용==========================
	for (i = 0; i < org_col; i += N_intra) {
		for (j = 0; j < org_row; j += N_intra) {
			neighbor_pix = neighbor_pixels(res_img, j, i, 0);
			label = *(Label_arr + (i / N_intra)*(org_row / N_intra) + j / N_intra);
			block_pre = pre_block_intra(neighbor_pix, label, i, j);
			for (v = 0; v < N_intra; v++) {
				for (u = 0; u < N_intra; u++) {
					*(res_img + (i + v)*org_row + j + u) = clipping((int)(*(block_pre + v*N_intra + u)) + *(Error + (i + v)*org_row + j + u));
				}
			}
		}
	}
	//예측 영상 찾기
	return(res_img);
	free(res_img);
	free(neighbor_pix);
	free(block_pre);
}