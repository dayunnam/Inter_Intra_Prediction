#include<stdio.h>
#include<math.h>
#include <stdlib.h>
#include <time.h>
#include<string.h>
//#include "encoder_intra.h"
//#include "file_read_or_write.h"
# define org_row  720
# define org_col  480
# define sizebyte 1
//# define in_file_bf "lena.img"
# define scale 10 
# define pi 3.1415926535897
# define in_file_bf "football_bf.y"
# define in_file_af "football_af.y"
# define dct_file "dct_outfile.y"//test ��=====================
# define idct_file "idct_outfile.y"//test ��=====================
# define reconstruct_file_bf "0_before_encoding.y"
# define label_file "1_label_before"
# define error_file "2_error_before"
# define label_file_af "3_label_after"
# define MV_x_file "4_motion_vector_x.y"
# define MV_y_file "5_motion_vector_y.y"
# define Quantization_file "6_��.y"
# define inv_Quantization_file "6_inv_��.y"
# define flag_MB_file "7_flag_MB.y"
# define current_encoding_file "0_current_encodiong.y"
# define prediction_defore "0_prediction.y"
# define N_intra 4// �������� block
# define N_tempo 16// �ð����� block
# define use_psnr 1// (0 : psnr ���� x   , 1 : psnr ���� o ) 
# define FALSE 0
# define TRUE 1
# define sample 1.75 // ����ȭ ���
# define pre_N 9 // ���� ���� (4 or 9)
//====option================
# define SR 4// search range => 4x4, 8x8, 16x16
# define DCT_N_intra 4//DCT ���ļ� ���� (2,4,8,16.32.64.128.256)
# define DCT_N_inter 16//DCT ���ļ� ���� (2,4,8,16.32.64.128.256)

//����  unsinged char �� ���� �б�(s : ���� �̸�) 
unsigned char* readFile(char* filename, int size_row, int size_col);

//���� int ������ �б�
int* ReadFile_int(char* filename, int size_row, int size_col);

//���� unsigned char ������ ����
unsigned char* WriteFile_U(unsigned char* out_img, char* filename, int size_row, int size_col);

//���� int ������ ����
int* WriteFile_I(int* out_img, char* filename, int size_row, int size_col);

//���� double������ ����(������� �̹��� �����)
double* WriteFile_D(double* double_img, char* filename, int size_row, int size_col);

//DCT : ���� ���󿡼�  DCT �� �� ����� double ������ ��� (8x8 ����� forward DCT)
double* DCT_U(unsigned char* org_img, int size_row, int size_cul, int DCT_N);
double* DCT_I(int* org_img, int size_row, int size_cul, int DCT_N);

//IDCT : double ���� DCT ������ �ٽ� �������� ������ unsinged char������ �����Ѵ�.
unsigned char* IDCT_U(double* DCT_img, int size_row, int size_cul, int DCT_N);
int* IDCT_I(double* DCT_img, int size_row, int size_cul, int DCT_N);

// type = 0 �̸� ����ȭ �ؼ� �� ���, type = 1  �̸� ����ȭ�� ���� ������ȭ�� ���� ���
int* sampling_error(int* error, int type, int size_row, int size_col,int DCT_N);

//MSE & PSNR **�Է��� uchar ��**
double MSE_f(unsigned char* sp_img, char* s);

//mismatch Ȯ��
int ismismatch(unsigned char* en_img, unsigned char* de_img);

//��������� ���� ������ �Է����� ������, energy �� ������.
int energy_f(unsigned char* org_img, unsigned char* pre_img, int size_row, int size_cul);

//������ ��⸦ 0~ 255 �����ȿ� ������ clipping
unsigned char clipping(int temp);

//�ð� ����======================================================================================================================================
//motion vector ������ ������, ���� ��� ���
unsigned char* pre_block_tempo(unsigned char* before_dec_img, int current_cul, int current_row, int mv_x, int mv_y);

//����� ���� ��ġ�� �ָ�, search range�� ���ϰ�, �׾ȿ� ���� ������ ����� ���ϴ� motion vector ���ϱ�
int* mv_finder(unsigned char* current_img, unsigned char* before_dec_img, int current_cul, int current_row);

//�ð����� decoder
unsigned char* decoding_tempo(unsigned char* before_dec_img, int* mv_x, int* mv_y, int* Q_diff);

//�ð����� encoder 
unsigned char* encoding_tempo(unsigned char* current_img, unsigned char* before_dec_img);

// mv �� �� 
double sum_of_mv(int* mv_x, int* mv_y);

//mv ������ ���
double average_of_mv(int* mv_x, int* mv_y, int num_inter);

//��������=======================================================================================================================================
//���� ��� ��ġ�� �̿��� 13��(�Ǵ� 8��) �� �̿� �ȼ��� �̾Ƴ�
unsigned char* neighbor_pixels(unsigned char* org_img, int current_row, int current_cul, int type);

//label ��(type) �� ���� ���� ���� ��� ��� 
unsigned char* pre_block_intra(unsigned char* neighbor_pix, int type, int current_cul, int current_row);

//�� block�� label �� ����
int* label_finder(unsigned char* org_img, int current_cul, int current_row, int prediction_type);

//���� �� (label ����Ȯ��)====================
void sort_Label(unsigned char* Label_arr);

//test ��(difference �� ����Ȯ��)====================
void sort_Error(int* Error);

//intra prediction encoding 
unsigned char* encoding_intra(unsigned char* org_img);

//�ð� of ���� ����==========================
//before �������� ����� after ���� ������ �Է����� �־�, encoder ���� ������ ���� ��� 
unsigned char* inter_intra_encodinng(unsigned char* current_img, unsigned char* before_recon_img);