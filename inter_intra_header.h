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
# define in_file_bf "football_bf.y" //mse �� ���ϱ� ���� �뵵
# define in_file_af "football_af.y" //mse �� ���ϱ� ���� �뵵
# define reconstruct_file_bf_encoder "0_before_encoding.y"//mismatch �� Ȯ���ϱ� ���� �뵵
# define reconstruct_file_bf_decoder "0_before_decoding.y" //decoding �� ���

# define label_file "1_label_before"
# define error_file "2_error_before"
# define label_file_af "3_label_after"
# define MV_x_file "4_motion_vector_x.y"
# define MV_y_file "5_motion_vector_y.y"
# define Quantization_file "6_��.y"
# define inv_Quantization_file "6_inv_��.y"
# define flag_MB_file "7_flag_MB.y"

# define current_encoding_file "0_current_encodiong.y"//mismatch �� Ȯ���ϱ� ���� �뵵
# define current_decoding_file "0_current_decodiong.y"//decoding �� ���

# define N_intra 4// �������� block
# define N_tempo 16// �ð����� block
# define use_psnr 1//(0 : psnr ���� x   , 1 : psnr ���� o ) 
# define FALSE 0
# define TRUE 1
# define sample 1.75 // ����ȭ ���
# define pre_N 9 // ���� ���� (4 or 9)
# define DCT_N_intra 4//DCT ���ļ� ���� (2,4,8,16.32.64.128.256)
# define DCT_N_inter 16//DCT ���ļ� ���� (2,4,8,16.32.64.128.256)



//����  unsinged char �� ���� �б�(s : ���� �̸�) 
unsigned char* readFile(char* filename, int size_row, int size_col);

//���� int ������ �б�
int* ReadFile_int(char* filename, int size_row, int size_col);

//���� unsigned char ������ ����
unsigned char* WriteFile_U(unsigned char* out_img, char* filename, int size_row, int size_col);


//DCT : ���� ���󿡼�  DCT �� �� ����� double ������ ��� (8x8 ����� forward DCT)
double* DCT_U(unsigned char* org_img, int size_row, int size_cul,int DCT_N);
double* DCT_I(int* org_img, int size_row, int size_cul, int DCT_N);

//IDCT : double ���� DCT ������ �ٽ� �������� ������ unsinged char������ �����Ѵ�.
unsigned char* IDCT_U(double* DCT_img, int size_row, int size_cul, int DCT_N);
int* IDCT_I(double* DCT_img, int size_row, int size_cul, int DCT_N);

// type = 0 �̸� ����ȭ �ؼ� �� ���, type = 1  �̸� ����ȭ�� ���� ������ȭ�� ���� ���
int* sampling_error(int* error, int type, int size_row, int size_col, int DCT_N);

//MSE & PSNR **�Է��� uchar ��**
double MSE_f(unsigned char* sp_img, char* s);

//mismatch Ȯ��
int ismismatch(unsigned char* en_img, unsigned char* de_img);

//������ ��⸦ 0~ 255 �����ȿ� ������ clipping
unsigned char clipping(int temp);

//�ð� ����======================================================================================================================================
//motion vector ������ ������, ���� ��� ���
unsigned char* pre_block_tempo(unsigned char* before_dec_img, int current_cul, int current_row, int mv_x, int mv_y);

//�ð����� decoder
unsigned char* decoding_tempo(unsigned char* before_dec_img, int* mv_x, int* mv_y, int* Q_diff);


//��������=======================================================================================================================================
//���� ��� ��ġ�� �̿��� 13��(�Ǵ� 8��) �� �̿� �ȼ��� �̾Ƴ�
unsigned char* neighbor_pixels(unsigned char* org_img, int current_row, int current_cul, int type);

//label ��(type) �� ���� ���� ���� ��� ��� 
unsigned char* pre_block_intra(unsigned char* neighbor_pix, int type, int current_cul, int current_row);

//����ȭ�� error�� label �� �̿��Ͽ�, decoding �ϱ�
unsigned char* decoding_intra(unsigned char* Label_arr, int* Error);

//���� �� (label ����Ȯ��)====================
void sort_Label(unsigned char* Label_arr);

//test ��(difference �� ����Ȯ��)====================
void sort_Error(int* Error);

//intra_inter_decoding 
unsigned char* inter_intra_decoding(unsigned char* before_recon_img, unsigned char* flag_MB, unsigned char* Label_arr, int* mv_x, int* mv_y, int* Q_diff);