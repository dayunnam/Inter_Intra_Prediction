#include "inter_intra_header.h"

//������ ��⸦ 0~ 255 �����ȿ� ������ clipping
unsigned char clipping(int temp) {
	unsigned char img;
	if (temp < 0)
		img = 0;
	else if (temp > 255)
		img = 255;
	else
		img = temp;
	return img;
}