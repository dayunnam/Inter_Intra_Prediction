#include<stdio.h>
#include<math.h>
#include <stdlib.h>
#include <time.h>
#include<string.h>
# define org_row  720
# define org_col  480
# define sizebyte 1
# define scale 10 
# define pi 3.1415926535897
# define in_file_bf "football_bf.y" //mse 를 구하기 위한 용도
# define in_file_af "football_af.y" //mse 를 구하기 위한 용도
# define reconstruct_file_bf_encoder "0_before_encoding.y"//mismatch 를 확인하기 위한 용도
# define reconstruct_file_bf_decoder "0_before_decoding.y" //decoding 의 결과

# define label_file "1_label_before"
# define error_file "2_error_before"
# define label_file_af "3_label_after"
# define MV_x_file "4_motion_vector_x.y"
# define MV_y_file "5_motion_vector_y.y"
# define Quantization_file "6_몫.y"
# define inv_Quantization_file "6_inv_몫.y"
# define flag_MB_file "7_flag_MB.y"

# define current_encoding_file "0_current_encodiong.y"//mismatch 를 확인하기 위한 용도
# define current_decoding_file "0_current_decodiong.y"//decoding 의 결과

# define N_intra 4// 공간예측 block
# define N_tempo 16// 시간예측 block
# define use_psnr 1//(0 : psnr 측정 x   , 1 : psnr 측정 o ) 
# define FALSE 0
# define TRUE 1
# define sample 1.75 // 양자화 계수
# define pre_N 9 // 예측 방향 (4 or 9)
# define DCT_N_intra 4//DCT 주파수 개수 (2,4,8,16.32.64.128.256)
# define DCT_N_inter 16//DCT 주파수 개수 (2,4,8,16.32.64.128.256)



//파일  unsinged char 형 으로 읽기(s : 파일 이름) 
unsigned char* readFile(char* filename, int size_row, int size_col);

//파일 int 형으로 읽기
int* ReadFile_int(char* filename, int size_row, int size_col);

//파일 unsigned char 형으로 쓰기
unsigned char* WriteFile_U(unsigned char* out_img, char* filename, int size_row, int size_col);


//DCT : 원본 영상에서  DCT 를 한 결과를 double 형으로 출력 (8x8 블록의 forward DCT)
double* DCT_U(unsigned char* org_img, int size_row, int size_cul,int DCT_N);
double* DCT_I(int* org_img, int size_row, int size_cul, int DCT_N);

//IDCT : double 형의 DCT 영상을 다시 원본영상에 가깝게 unsinged char형으로 복원한다.
unsigned char* IDCT_U(double* DCT_img, int size_row, int size_cul, int DCT_N);
int* IDCT_I(double* DCT_img, int size_row, int size_cul, int DCT_N);

// type = 0 이면 양자화 해서 몫만 출력, type = 1  이면 양자화된 몫을 역양자화한 값을 출력
int* sampling_error(int* error, int type, int size_row, int size_col, int DCT_N);

//MSE & PSNR **입력은 uchar 형**
double MSE_f(unsigned char* sp_img, char* s);

//mismatch 확인
int ismismatch(unsigned char* en_img, unsigned char* de_img);

//영상의 밝기를 0~ 255 범위안에 오도록 clipping
unsigned char clipping(int temp);

//시간 예측======================================================================================================================================
//motion vector 정보를 받으면, 예측 블록 출력
unsigned char* pre_block_tempo(unsigned char* before_dec_img, int current_cul, int current_row, int mv_x, int mv_y);

//시간예측 decoder
unsigned char* decoding_tempo(unsigned char* before_dec_img, int* mv_x, int* mv_y, int* Q_diff);


//공간예측=======================================================================================================================================
//현재 블록 위치의 이웃한 13개(또는 8개) 의 이웃 픽셀을 뽑아냄
unsigned char* neighbor_pixels(unsigned char* org_img, int current_row, int current_cul, int type);

//label 값(type) 에 따른 공간 예측 블록 출력 
unsigned char* pre_block_intra(unsigned char* neighbor_pix, int type, int current_cul, int current_row);

//양자화된 error와 label 을 이용하여, decoding 하기
unsigned char* decoding_intra(unsigned char* Label_arr, int* Error);

//시험 용 (label 분포확인)====================
void sort_Label(unsigned char* Label_arr);

//test 용(difference 값 분포확인)====================
void sort_Error(int* Error);

//intra_inter_decoding 
unsigned char* inter_intra_decoding(unsigned char* before_recon_img, unsigned char* flag_MB, unsigned char* Label_arr, int* mv_x, int* mv_y, int* Q_diff);
