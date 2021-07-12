#include "inter_intra_header.h"


//(미완)before 공간예측 영상과 after 원본 영상을 입력으로 주어, encoder 에서 복원한 영상 출력 
unsigned char* inter_intra_encodinng(unsigned char* current_img, unsigned char* before_recon_img) {
	int i, j, w; //꼭지점 좌표
	int u, v; //블록 안 좌표
	int a, b;
	int energy_intra, energy_inter;
	int num_of_intra, num_of_inter;
	int* buff = NULL;
	unsigned char* block = NULL;//after 영상의 원본블록
	
	unsigned char* block_pre_inter = NULL; //시간 예측 블록 (16x16)
	unsigned char* block_pre_intra = NULL; //공간 예측 블록 (16x16)
	unsigned char* block_pre_intra_4x4 = NULL; //공간 예측 블룍 (4x4)
	unsigned char * reconstruct_img = NULL;
	unsigned char * pre_img = NULL;//test 용===========
								   //시간예측 해당
	int * mv_x = NULL;
	int * mv_y = NULL;
	int * a_mv = NULL;
	int * difference_16x16 = NULL;
	int * Q_difference_16x16 = NULL;
	int * IQ_difference_16x16 = NULL;
	//공간예측 해당
	int label;
	unsigned char* neighbor_pix = NULL;
	unsigned char * label_array = NULL; //전체 label
	int * error_4x4 = NULL;
	int * Q_error_4x4 = NULL;
	int * IQ_error_4x4 = NULL;
	//시간, 공간 모두 해당
	int * Quantization_difference = NULL;
	int * inv_Quantization_difference = NULL;
	int * difference = NULL;
	int * inv_difference = NULL;//test 용===============


	//MB 종류
	unsigned char* flag_MB = NULL;

	error_4x4 = (int*)malloc(sizeof(int)*(N_intra*N_intra));
	Q_error_4x4 = (int*)malloc(sizeof(int)*(N_intra*N_intra));
	IQ_error_4x4 = (int*)malloc(sizeof(int)*(N_intra*N_intra));
	buff = (int*)malloc(sizeof(int)*(N_intra*N_intra + 1));
	block = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));

	block_pre_inter = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));
	block_pre_intra = (unsigned char*)malloc(sizeof(unsigned char)*(N_tempo*N_tempo));
	block_pre_intra_4x4 = (unsigned char*)malloc(sizeof(unsigned char)*(N_intra*N_intra));
	reconstruct_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row * org_col));
	a_mv = (int*)malloc(sizeof(int) * 2);
	pre_img = (unsigned char*)malloc(sizeof(unsigned char)*(org_row * org_col));//test 용=============
	mv_x = (int*)malloc(sizeof(int)*(org_row * org_col / (N_tempo*N_tempo)));// size 수정할 필요 있음
	mv_y = (int*)malloc(sizeof(int)*(org_row * org_col / (N_tempo*N_tempo)));// size 수정할 필요 있음
	label_array = (unsigned char*)malloc(sizeof(unsigned char)*(org_col*org_row / (N_intra*N_intra))); //size 수정할 필요 있음

	Quantization_difference = (int*)malloc(sizeof(int)*(N_tempo*N_tempo));
	inv_Quantization_difference = (int*)malloc(sizeof(int)*(N_tempo*N_tempo));
	difference = (int*)malloc(sizeof(int)*(org_row * org_col));
	inv_difference = (int*)malloc(sizeof(int)*(org_row * org_col));//test 용================
	difference_16x16 = (int*)malloc(sizeof(int)*(N_tempo * N_tempo));
	Q_difference_16x16 = (int*)malloc(sizeof(int)*(N_tempo * N_tempo));
	IQ_difference_16x16 = (int*)malloc(sizeof(int)*(N_tempo * N_tempo));
	flag_MB = (unsigned char*)malloc(sizeof(unsigned char)*(org_row * org_col / (N_tempo*N_tempo)));
	neighbor_pix = (unsigned char*)malloc(sizeof(unsigned char)*(N_intra + pre_N));

	num_of_intra = 0;//intra 예측 블록의 개수
	num_of_inter = 0;//inter 예측 블록의 개수

					 //복원영상을 먼저 원본 영상으로 초기화
	for (i = 0; i < org_col; i++) {
		for (j = 0; j < org_row; j++) {
			*(reconstruct_img + i*org_row + j) = *(current_img + i*org_row + j);
		}
	}

	//공간과 시간 예측 블록 찾기
	for (i = 0; i < org_col; i += N_tempo) {
		for (j = 0; j < org_row; j += N_tempo) {
			energy_intra = 0;
			energy_inter = 0;
			//원본영상의 현재위치의 블록 만들기 
			for (u = 0; u < N_tempo; u++) {
				for (v = 0; v < N_tempo; v++) {
					*(block + u*N_tempo + v) = *(current_img + (i + u)*org_row + (j + v));
				}
			}
			//공간예측의 블록 만들기
			for (a = 0; a < N_tempo; a += N_intra) {
				for (b = 0; b < N_tempo; b += N_intra) {
					//label, error 각각 저장
					buff = label_finder(reconstruct_img, i + a, j + b, 1);
					label = *(buff);
					for (w = 0; w < N_intra*N_intra; w++) {
						*(error_4x4 + w) = *(buff + w + 1);
					}
					Q_error_4x4 = sampling_error(error_4x4, 0, N_intra, N_intra, N_intra);//error 를 양자화 
					IQ_error_4x4 = sampling_error(Q_error_4x4, 1, N_intra, N_intra, N_intra);//양자화된 error 복구

					neighbor_pix = neighbor_pixels(reconstruct_img, j + b, i + a, 1);//두번째 인수 : 행, 세번째 인수 : 열
					block_pre_intra_4x4 = pre_block_intra(neighbor_pix, label, i + a, j + b);

					for (v = 0; v < N_intra; v++) {
						for (u = 0; u < N_intra; u++) {
							*(reconstruct_img + (i + a + v)*org_row + (j + b + u)) = clipping((int)(*(block_pre_intra_4x4 + N_intra*v + u)) + *(IQ_error_4x4 + N_intra*v + u));
							//*(reconstruct_img + (i + a + v)*org_row + (j + b + u)) = (int)(*(block_pre_intra_4x4 + N_intra*v + u)) + *(IQ_error_4x4 + N_intra*v + u);
							*(block_pre_intra + (a + v)*N_tempo + (b + u)) = *(block_pre_intra_4x4 + N_intra*v + u);
							*(difference + (i + a + v)*org_row + (j + b + u)) = *(Q_error_4x4 + N_intra*v + u);
							*(inv_difference + (i + a + v)*org_row + (j + b + u)) = *(IQ_error_4x4 + N_intra*v + u);//test 용=================
							//printf("Q_difference = %d, difference = %d\n", *(Q_error_4x4 + N_intra*v + u), *(difference + (i + a + v)*org_row + (j + b + u)));
						}
					}
					*(label_array + ((i + a) / N_intra)*(org_row / N_intra) + (j + b) / N_intra) = (unsigned char)label;
				}
			}

			//시간예측의 블록 만들기
			a_mv = mv_finder(current_img, before_recon_img, i, j);
			*(mv_x + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = *(a_mv);
			*(mv_y + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = *(a_mv + 1);
			block_pre_inter = pre_block_tempo(before_recon_img, i, j, *(a_mv), *(a_mv + 1));

			//시간예측 블록과 공간예측 블록의 에너지 구하기
			energy_inter = energy_f(block, block_pre_inter, N_tempo, N_tempo);
			energy_intra = energy_f(block, block_pre_intra, N_tempo, N_tempo);
			//printf("energy_inter = %d, energy_intra = %d\n", energy_inter, energy_intra);

			if (energy_inter >= energy_intra) {
				num_of_intra++;//test 용==================
				*(flag_MB + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = 0; //현재 블록의 예측블록은 공간 예측블록으로 결정할 경우
				*(mv_x + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = 0;
				*(mv_y + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = 0;


				//test 용==============================================================
				for (a = 0; a < N_tempo; a += N_intra) {
					for (b = 0; b < N_tempo; b += N_intra) {
						//printf("label = %d\n", *(label_array + ((i + a)/ N_intra)*(org_row / N_intra) + (j + b)/ N_intra));
						for (v = 0; v < N_intra; v++) {
							for (u = 0; u < N_intra; u++) {
								//printf("reconstruct_img = %d\n", *(reconstruct_img + (i + a + v)*org_row + j + b + u));
								//printf("block_pre_intra = %d\n", *(block_pre_intra + (a + v)*N_tempo + (b + u)));
								//printf("Q_difference_16x16  = %d, invQ_difference_16x16  = %d\n", v,u, *(Q_error_4x4 + N_intra*v + u), *(IQ_error_4x4 + N_intra*v + u));
								//printf("%d 열 %d 행  difference = %d\n", (i + a + v), (j + b + u), *(difference + (i + a + v)*org_row + (j + b + u)));
							}
						}
					}
				}
				//=========================================================
				
			}
			else {

				num_of_inter++;//test 용==================
				*(flag_MB + (i / N_tempo)*(org_row / N_tempo) + (j / N_tempo)) = 1; //현재 블록의 예측블록을 시간 예측블록으로 결정할 경우
				for (u = 0; u < N_tempo; u++) {
					for (v = 0; v < N_tempo; v++) {
						//잔차 블록에 값 저장
						*(difference_16x16 + u*N_tempo + v) = *(block + u*N_tempo + v) - *(block_pre_inter + u*N_tempo + v);
					}
				}

				Q_difference_16x16 = sampling_error(difference_16x16, 0, N_tempo, N_tempo, N_tempo);//error 를 양자화 
				IQ_difference_16x16 = sampling_error(Q_difference_16x16, 1, N_tempo, N_tempo, N_tempo);//양자화된 error 복구

				for (v = 0; v < N_tempo; v++) {
					for (u = 0; u < N_tempo; u++) {
						*(reconstruct_img + (i + v)*org_row + (j + u)) = clipping( (int)(*(block_pre_inter + N_tempo*v + u)) + *(IQ_difference_16x16 + +N_tempo*v + u));
						//*(reconstruct_img + (i + v)*org_row + (j + u)) = (int)(*(block_pre_inter + N_tempo*v + u)) + *(IQ_difference_16x16 + +N_tempo*v + u);
						*(difference + (i + v)*org_row + (j + u)) = *(Q_difference_16x16 + N_tempo*v + u);
						*(inv_difference + (i + v)*org_row + (j + u)) = *(IQ_difference_16x16 + N_tempo*v + u);
					}
				}

				for (a = 0; a < N_tempo; a += N_intra) {
					for (b = 0; b < N_tempo; b += N_intra) {
						*(label_array + ((i + a) / N_intra)*(org_row / N_intra) + (j + b) / N_intra) = 10;
					}
				}


			}
		}
	}

	printf("\nafter 영상의 시간_공간 예측 결과=======================");
	sort_Label(label_array);
	//sort_Error(sampling_error(difference, 1, org_row, org_col));
	printf("\nafter 영상의 mv 절댓값의 평균 : %.4f\n", average_of_mv(mv_x, mv_y, num_of_inter));
	printf("\n공간예측 %d 개, 시간예측 %d 개\n", num_of_intra, num_of_inter);

	WriteFile_U(flag_MB, flag_MB_file, org_row / N_tempo, org_col / N_tempo);
	WriteFile_U(label_array, label_file_af, org_row / N_intra, org_col / N_intra);
	WriteFile_I(mv_x, MV_x_file, org_row / N_tempo, org_col / N_tempo);//motion vector 의 x값만 저장하기
	WriteFile_I(mv_y, MV_y_file, org_row / N_tempo, org_col / N_tempo);//motion vector 의 y값만 저장하기
	WriteFile_I(difference, Quantization_file, org_row, org_col);//differency block 저장하기
	WriteFile_I(inv_difference, inv_Quantization_file, org_row, org_col);//differency block 저장하기
	WriteFile_U(pre_img, prediction_defore, org_row, org_col);
	free(mv_x);
	free(mv_y);
	free(a_mv);
	free(flag_MB);
	free(difference);
	free(difference_16x16);
	free(Quantization_difference);
	free(inv_Quantization_difference);
	free(block);

	free(block_pre_inter);
	free(block_pre_intra);
	free(block_pre_intra_4x4);
	free(pre_img);//test 용=================
	free(label_array);
	return reconstruct_img;
	free(reconstruct_img);

}

