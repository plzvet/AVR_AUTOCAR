/*
 * 01.LED_CONTROL.c
 *
 * Created: 2025-03-04 오후 4:25:34
 * Author : microsoft
 */ 

/*
Timer 0 : 8bit timer : interrupt
Timer 2 : 8bit timer 초음파 -> echo : counter
Timer 1 : 16bit timer piezo buzzer, pwm : pwm 제어용
Timer 3 : 16bit timer pwm제어, 모터제어 : pwm 제어용
*/

#include "button.h"
#include "uart0.h"
#include "extern.h"
#include "def.h"
#define OK 1
#define ERROR 0
#define threshhold 240
// 선언 ----------------------------------------------------
void init_timer0(void);

void manual_mode(void);
void auto_mode(void);

volatile int direction_flag = 0;
volatile int msec_count = 0;
volatile int button_count = 0;
volatile int ultrasonic_check_timer = 0;
volatile int fnd_update = 0;
volatile int lcd_count = 0;
volatile int lcd_flag = 0;
FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

volatile int func_index = MANUAL_MODE;
void (*pfunc[])() =
{
	manual_mode,		// 수동모드
	auto_mode,			// 자율주행
	auto_mode,			// 자율주행
	auto_mode,			// 자율주행
};

// interrupt service routine은 반드시 main함수 이전에 정의한다.
ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6; // 6 ~ 256으로 돌리기 위해
	msec_count++;
	ultrasonic_check_timer++;
	button_count++;
	fnd_update++;
	lcd_count++;
}



int main(void)
{
	init_timer0();
	init_uart0();
	init_button();
	init_L298N();
	init_uart1();
	init_timer1();
	init_ultrasonic();
	init_fnd();
	init_speaker();
	I2C_LCD_init();
	
	stdout = &OUTPUT;	
	sei();				
	

	while(1)
	{
		if(fnd_update > 1)
		{
			fnd_update = 0;
			fnd_display();
		}
		if(get_button(BUTTON0, BUTTON0PIN))
		{
			func_index++;
			func_index %= 4;
			lcd_flag = 1;
		}

		if(lcd_flag)
		{
			lcd_flag = 0;
			i2c_lcd_print();
		}
		else
		{
			play_song(func_index);
			pfunc[func_index]();
			distance_ultrasonic();
		}
		
	}

	return 0;
}



// AVR의 8bit counter timer0를 초기화한다.
// 임베디드/FPGA에서 가장 중요한건 초기화 -> init함수는 특히 신경쓰기!!
void init_timer0(void)
{
	// 분주 (divider / prescale)
	// 16MHz / 64 down!!
	// 16000000Hz / 64 = 250,000 Hz 로 만든다!
	// T(주기) = 1clock에 걸리는 시간 = 1 / 250000 = 0.000004sec = 0.004ms (4us)
	// 8bit timer overflow 발생 시 소요시간 = 0.004ms * 256 = 1.024ms = 0.001024sec
	//	-> interrupt를 정확하게 1ms마다 오게 하고싶으면, 
	//    사용자 지정으로 256이 아니라 250클록마다 인터럽트가 생기도록
	
	// TCMT0 초기값 설정
	TCNT0 = 6; // 정확히 1ms를 세기 위해 초기값을 0이 아니라 6으로 해준다. (250 펄스 이후에 인터럽트)
	
	// 분주비 설정 (64)
	TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00;
	
	// timer overflow interrupt 허용 (TIMER0 OVF)
	TIMSK |= 1 << TOIE0;	// 지역 인터럽트 허용
}

void manual_mode(void)		// 수동 mode
{
	switch (bt_data)
	{
		case 'F' :
		case 'f' :
			forward(700);	 // 4us x 500 = 0.002sec (2ms)
			last_action = FORWARD;
			break;
		case 'B' :
		case 'b' :
			backward(500);	 // 4us x 500 = 0.002sec (2ms)
			last_action = BACKWARD;
			break;
		case 'L' :
		case 'l' :
			turn_left(700);  // 4us x 700 = 0.0028sec (2.8ms)
			last_action = LEFT;
			break;
		case 'R' :
		case 'r' :
			turn_right(700); // 4us x 700 = 0.0028sec (2.8ms)
			last_action = RIGHT;
			break;
		case 'S' :
		case 's' :
			stop();
			break;
		default:
			break;	
	}
}

void auto_mode(void)		// 자율주행
{
	
	//printf("L : %d\n F : %d\n R : %d\n", ultrasonic_dis_left,ultrasonic_dis_forward,ultrasonic_dis_right); // 저장해둔 거리text 출력
	
	// TCNT2 10당 1.37cm
	// TCNT2 100당 13.7cm
	// TCNT2 145 --> 대략 20cm  
	// TCNT2 200당 27.4cm
	
	if(ultrasonic_dis_forward > threshhold) // 전방주행가능 (장애물 X)
	{
		if(ultrasonic_dis_left > threshhold && ultrasonic_dis_right > threshhold) // 모든 방향 장애물 X
		{	
			// 직진
			forward(1000);
			last_action = FORWARD;
		}
		else if(ultrasonic_dis_left <= threshhold && ultrasonic_dis_right > threshhold) // 좌측 장애물존재
		{
			// 우회전
			turn_right(800);
			last_action = RIGHT;
		}	
		else if(ultrasonic_dis_left > threshhold && ultrasonic_dis_right <= threshhold) // 우측 장애물존재
		{
			// 좌회전
			turn_left(800);
			last_action = LEFT;
		}
		else // 양쪽모두 장애물 존재
		{
			// 후진
			backward(600);
			last_action = BACKWARD;
		}
	}
	else // 전방주행불가능 (장애물존재)
	{
		if(ultrasonic_dis_left > threshhold && ultrasonic_dis_right > threshhold)
		{
			// 좌우 장애물 X 좌회전으로 길찾기
			backward(600);
			last_action = BACKWARD;
		}
		else if(ultrasonic_dis_left <= threshhold && ultrasonic_dis_right > threshhold)
		{
			// 좌측 장애물 -> 우회전
			turn_right(800);
			last_action = RIGHT;
		}
		else if(ultrasonic_dis_left > threshhold && ultrasonic_dis_right <= threshhold)
		{
			// 우측 장애물 -> 좌회전
			turn_left(800);
			last_action = LEFT;
		}
		else
		{
			// 모든 방향 장애물 -> 후진
			backward(600);
			last_action = BACKWARD;
		}
	}	
}
 
/*
ultrasonic_dis_left
ultrasonic_dis_forward
ultrasonic_dis_right
*/
