#include "inter_intra_header.h" 

//(미완)before 공간예측 영상과 after 원본 영상을 입력으로 주어, encoder 에서 복원한 영상 출력 
unsigned char* inter_intra_encodinng(unsigned char* current_img, unsigned char* before_recon_img) {
	int i, j, a, b, u, v, w;//for 문 변수
	int energy_intra, energy_inter; //inter 와 intra 의 energy
	int num_of_intra = 0;
	int num_of_inter = 0; //inter 와 intra 블록 개수


	unsigned char* flag_MB = (unsigned char*)malloc(sizeof(unsigned char)*(org_row * org_col)/(N_tempo*N_tempo));
	unsigned char* reconstruct_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row * org_col));
	
	int*  buff = (int*)malloc(sizeof(int)*(N_intra*N_intra + 1));
	int* difference_4x4 = (int*)malloc(sizeof(int)* (N_intra*N_intra));
	int* Q_difference_4x4 = (int*)malloc(sizeof(int)* (N_intra*N_intra));
	int* IQ_difference_4x4 = (int*)malloc(sizeof(int)* (N_intra*N_intra));
	unsigned char* neighbor_pix = (unsigned char*)malloc(sizeof(unsigned char)*(N_intra + pre_N));
	unsigned char* label_arr = (unsigned char*)malloc(sizeof(unsigned char)*(org_col*org_row)/(N_intra*N_intra));
	unsigned char* pre_intra_4x4 = (unsigned char*)malloc(sizeof(unsigned char)*(N_intra*N_intra));
	unsigned char* pre_intra_16x16 = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));
	unsigned char* pre_inter_16x16 = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));
	unsigned char* block = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));
	int* difference = (int*)malloc(sizeof(int)* (org_row*org_col));
	int* mv_x = (int*)malloc(sizeof(int)*(org_row * org_col / (N_tempo*N_tempo)));
	int* mv_y = (int*)malloc(sizeof(int)*(org_row * org_col / (N_tempo*N_tempo)));
	int* a_mv = (int*)malloc(sizeof(int) * 2);
	int* difference_16x16 = (int*)malloc(sizeof(int)* (N_tempo*N_tempo));
	int* Q_difference_16x16 = (int*)malloc(sizeof(int)* (N_tempo*N_tempo));
	int* IQ_difference_16x16 = (int*)malloc(sizeof(int)* (N_tempo*N_tempo));
	int* Q_difference_16x16_intra = (int*)malloc(sizeof(int)* (N_tempo*N_tempo));





	//원본영상 옮기기
	for (i = 0; i < org_col; i++) {
		for (j = 0; j < org_row; j++) {
			*(reconstruct_img + i*org_row + j) = *(current_img + i*org_row + j);
		}
	}

	for (i = 0; i < org_col; i += N_tempo) {
		for (j = 0; j < org_row; j += N_tempo) {

			//현재 원본 블록 만들기
			for (u = 0; u < N_tempo; u++) {
				for (v = 0; v < N_tempo; v++) {
					*(block + u*N_tempo + v) = *(current_img + (i + u)*org_row + (j + v));  
				}
			}
			//공간예측 
			for (a = 0; a < N_tempo; a += N_intra) {
				for (b = 0; b < N_tempo; b += N_intra) {
					buff = label_finder(reconstruct_img, i + a, j + b, 1); //label + difference 값 전달
					for (w = 0; w < N_intra*N_intra; w++) {
						*(difference_4x4 + w) = *(buff + w + 1); // difference block 만들기
					}
					Q_difference_4x4 = sampling_error(difference_4x4, 0, N_intra, N_intra, N_intra);//difference 블록 양자화
					IQ_difference_4x4 = sampling_error(Q_difference_4x4, 1, N_intra, N_intra, N_intra);//Q_difference 블록 역양자화
					neighbor_pix = neighbor_pixels(reconstruct_img, j + b, i + a, 1); 
					pre_intra_4x4 = pre_block_intra(neighbor_pix, *buff, i + a, j + b);
					for (u = 0; u < N_intra; u++) {
						for (v = 0; v < N_intra; v++) {
							*(pre_intra_16x16 + (a + u)*N_tempo + (b + v)) = *(pre_intra_4x4 + u*N_intra + v); //test 용================================
							*(reconstruct_img + (i + a + u)*org_row + (j + b + v)) = clipping(*(pre_intra_4x4 + u*N_intra + v) + *(IQ_difference_4x4 + u*N_intra + v));
							*(difference + (i + a + u)*org_row + (j + b + v)) = *(Q_difference_4x4 + u*N_intra + v);
						}
					}
					*(label_arr + ((i + a)/N_intra)*(org_row/N_intra) + ((j + b)/N_intra)) = *buff;
				}
			}

	
			
		//시간예측의 블록 만들기
			a_mv = mv_finder(current_img, before_recon_img, i, j);
			*(mv_x + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = *(a_mv);
			*(mv_y + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = *(a_mv + 1);
			pre_inter_16x16 = pre_block_tempo(before_recon_img, i, j, *(a_mv), *(a_mv + 1));

			energy_inter = energy_f(block, pre_inter_16x16, N_tempo, N_tempo);
			energy_intra = energy_f(block, pre_intra_16x16, N_tempo, N_tempo);

			
			if (energy_inter >= energy_intra) {
				num_of_intra++;
				*(flag_MB + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = 0;
				*(mv_x + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = 0;
				*(mv_y + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = 0;

			}
			else {
				num_of_inter++;
				*(flag_MB + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = 1;
				for (u = 0; u < N_tempo; u++) {
					for (v = 0; v < N_tempo; v++) {
						*(difference_16x16 + u*N_tempo + v) = *(block + u*N_tempo + v) - *(pre_inter_16x16 + u*N_tempo + v);
					}
				}

				Q_difference_16x16 = sampling_error(difference_16x16, 0, N_tempo, N_tempo, N_tempo);
				IQ_difference_16x16 = sampling_error(Q_difference_16x16, 1, N_tempo, N_tempo, N_tempo);
			
				//printf("sdgv\n");//test 용================

				for (u = 0; u < N_tempo; u++) {
					for (v = 0; v < N_tempo; v++) {						
						*(reconstruct_img + (i + u)*org_row + (j + v)) = clipping(*(pre_inter_16x16 + u*N_tempo + v) + *(IQ_difference_16x16 + u*N_tempo + v));
						*(difference + (i + u)*org_row + (j + v)) = *(Q_difference_16x16 + u*N_tempo + v);
					}
				}
				for (a = 0; a < N_tempo; a += N_intra) {
					for (b = 0; b < N_tempo; b += N_intra) {
						*(label_arr + ((i + a) / N_intra)*(org_row / N_intra) + (j + b) / N_intra) = 10;
					}
				}
			}
		}
	}


	
	
	printf("\nafter 영상의 시간_공간 예측 결과=======================");
	sort_Label(label_arr);
	printf("\nafter 영상의 mv 절댓값의 평균 : %.4f\n", average_of_mv(mv_x, mv_y, num_of_inter));
	printf("\n공간예측 %d 개, 시간예측 %d 개\n", num_of_intra, num_of_inter);

	WriteFile_U(flag_MB, flag_MB_file, org_row / N_tempo, org_col / N_tempo);
	WriteFile_U(label_arr, label_file_af, org_row / N_intra, org_col / N_intra);
	WriteFile_I(mv_x, MV_x_file, org_row / N_tempo, org_col / N_tempo);//motion vector 의 x값만 저장하기
	WriteFile_I(mv_y, MV_y_file, org_row / N_tempo, org_col / N_tempo);//motion vector 의 y값만 저장하기
	WriteFile_I(difference, Quantization_file, org_row, org_col);//differency block 저장하기
	

	free(mv_x);
	free(mv_y);
	free(a_mv);
	free(difference_4x4);
	free(Q_difference_4x4);
	free(IQ_difference_4x4);
	free(difference_16x16);
	free(Q_difference_16x16);
	free(IQ_difference_16x16);

	return reconstruct_img;
	
	free(reconstruct_img);

}