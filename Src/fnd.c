/*
 * fnd.c
 *
 * Created: 2025-03-06 오후 12:23:41
 *  Author: microsoft
 */ 
#include "fnd.h"
#include "def.h"
#include "extern.h"
void init_fnd(void);
void fnd_display(void);

volatile uint8_t last_action = STOP;  // 초기값은 정지 상태

										 // 0     1     2     3
										 // W     A     S     H
volatile uint8_t fnd_font_tesla[]  =	{0xd5, 0x88, 0x92, 0x89,
										 // 4     5     6     7
										 // R     N     E     D
										0x8f, 0xab, 0x86, 0xa1,
									  // 8     9     10    11
									  // R     Y     O     T
										0x8f, 0x99, 0xc0, 0x87,
									  //  12     13    14     15    16   17   18
									  //  C       U     P     I     L     F   b
										0x92, 0xc1, 0x8c, 0xcf, 0xc7, 0x8e, 0x83};

void fnd_display(void)
{
	static uint8_t digit_select = 0;

	switch (digit_select)
	{
		case 0:  // FND d1
			FND_DIGIT_PORT = 0x10;
			if (func_index == MANUAL_MODE)
				FND_DATA_PORT = fnd_font_tesla[3]; // 'H'
			else if (func_index == AUTO_MODE)
				FND_DATA_PORT = fnd_font_tesla[1]; // 'A'
			break;
		
		case 1://FND D2
			FND_DIGIT_PORT = 0x20;
			FND_DATA_PORT = 0xff;
			break;
		
		case 2://FND D3
			FND_DIGIT_PORT=0x40;
			FND_DATA_PORT = 0xff;
			break;
		
		case 3:  // FND d4
			FND_DIGIT_PORT = 0x80;
			if(last_action == STOP)
				FND_DATA_PORT = fnd_font_tesla[2]; // 'S'
			else if(last_action == RIGHT)
				FND_DATA_PORT = fnd_font_tesla[4]; // 'R' 
			else if(last_action == LEFT)
				FND_DATA_PORT = fnd_font_tesla[16]; // 'L'
			else if(last_action == FORWARD)
				FND_DATA_PORT = fnd_font_tesla[17]; // 'F'
			else if(last_action == BACKWARD)
				FND_DATA_PORT = fnd_font_tesla[18]; // 'B'
				 
			break;
	}
	digit_select++;
	digit_select %= 4;   // 다음 표시할 자리수 선택
}

void init_fnd(void)
{
	FND_DATA_DDR = 0xff;  // 출력 모드로 설정
	// FND_DIGIT_DDR |= 0xf0;   // 자릿수 선택 7654 
	FND_DIGIT_DDR |= 1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3 
					 | 1 << FND_DIGIT_D4; 
	// fnd를 all off 
	FND_DATA_PORT = ~0x00;   // fnd를 all off  0xff;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             