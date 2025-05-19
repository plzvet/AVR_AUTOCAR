/*
 * ultrasonic.c
 *
 * Created: 2025-03-12 오후 2:49:18
 *  Author: microsoft
 */ 

#include "ultrasonic.h"

void init_ultrasonic(void);
void trigger_ultrasonic(void);
void distance_ultrasonic(void);

volatile int ultrasonic_dis_left = 0;
volatile int ultrasonic_dis_forward = 0;
volatile int ultrasonic_dis_right = 0;

volatile char scm_left[50];
volatile char scm_forward[50];
volatile char scm_right[50];

volatile uint8_t flag=0;
volatile uint8_t ovflag=0;
extern volatile int ultrasonic_check_timer;
extern volatile int direction_flag;
// 상승, 하강 엣지 둘 다 에서 들어옴!
ISR(TIMER2_OVF_vect)  // Timer2는 8bit 타이머 -> 255까지 간 다음에 오버플로우에 대한 처리가 없으면 카운터가 초기화가 돼.
{
	ovflag = 1; // ovflow가 1이라는건 overflow가 발생했다는걸 알려주는 flag
}

// 40cm정도 떨어져있고, 255 -> 35cm
// 35cm까지 재고 초기화돼서 5cm라고 나옴.
// ovflow가 있는지 없는지

ISR(INT4_vect)
{
	// 1. rising edge -> 지금 echo는 HIGH
	if(ECHO_PIN & 1 << ECHO_LEFT)
	{
		TCNT2 = 0; // TIMER2 8bit counter 초기화		
		ovflag = 0;	
	}
	// 2. falling edge -> 지금 echo는 LOW
	else
	{
		// ECHO핀에 들어온 펄스 개수를 us로 환산한다
		if(ovflag)
			ultrasonic_dis_left = 255; // 얘처럼 값을 고정안해주면 잘못된 값이 나옴.
			//ultrasonic_dis_left = 1000000.0 * 255 * 128 / F_CPU; // overflow발생시 35cm로 고정
		else
			ultrasonic_dis_left = TCNT2;
			//ultrasonic_dis_left = 1000000.0 * TCNT2 * 128 / F_CPU / 58;
		
		// ex) TCNT1 == 10 일 때
		// 15.625KHz의 1주기는 64us
		// 즉,,, 지금은 640us가 지난시점 == 0.00064sec가 지남!!!
		// 초음파가 1cm에 58us가 걸리니까,,,
		// 640us / 58us ==> 11cm	
	}
	//sprintf(scm_left, "dis left : %dcm",ultrasonic_dis_left); // 화면에 출력하지 말고 scm에 넣기
}

ISR(INT5_vect)
{
	// 1. rising edge -> 지금 echo는 HIGH
	if(ECHO_PIN & 1 << ECHO_FORWARD)
	{
		TCNT2 = 0; // TIMER2 8bit counter 초기화
		ovflag = 0;
	}
	// 2. falling edge -> 지금 echo는 LOW
	else
	{
		// ECHO핀에 들어온 펄스 개수를 us로 환산한다
		if(ovflag)
			ultrasonic_dis_forward = 255;
			//ultrasonic_dis_forward = 1000000.0 * 255 * 128 / F_CPU; // overflow발생시 35cm로 고정
		else
			ultrasonic_dis_forward = TCNT2;
			//ultrasonic_dis_forward = 1000000.0 * TCNT2 * 128 / F_CPU / 58;

		// ex) TCNT1 == 10 일 때
		// 15.625KHz의 1주기는 64us
		// 즉,,, 지금은 640us가 지난시점 == 0.00064sec가 지남!!!
		// 초음파가 1cm에 58us가 걸리니까,,,
		// 640us / 58us ==> 11cm
	}
	//sprintf(scm_forward, "dis forward : %dcm",ultrasonic_dis_forward); // 화면에 출력하지 말고 scm에 넣기
}

ISR(INT6_vect)
{
	// 1. rising edge -> 지금 echo는 HIGH
	if(ECHO_PIN & 1 << ECHO_RIGHT)
	{
		TCNT2 = 0; // TIMER2 8bit counter 초기화
		ovflag = 0;
	}
	// 2. falling edge -> 지금 echo는 LOW
	else
	{
		// ECHO핀에 들어온 펄스 개수를 us로 환산한다		
		if(ovflag)
			ultrasonic_dis_right = 255;
			//ultrasonic_dis_right = 1000000.0 * 255 * 128 / F_CPU; // overflow발생시 35cm로 고정
		else
			ultrasonic_dis_right = TCNT2;
			//ultrasonic_dis_right = 1000000.0 * TCNT2 * 128 / F_CPU / 58;
		
		// ex) TCNT1 == 10 일 때
		// 15.625KHz의 1주기는 64us
		// 즉,,, 지금은 640us가 지난시점 == 0.00064sec가 지남!!!
		// 초음파가 1cm에 58us가 걸리니까,,,
		// 640us / 58us ==> 11cm
	}
	//sprintf(scm_right, "dis right : %dcm",ultrasonic_dis_right); // 화면에 출력하지 말고 scm에 넣기
}

void init_ultrasonic(void)
{
	// ddr
	TRIG_DDR |= 1 << TRIG_LEFT | 1 << TRIG_FORWARD | 1 << TRIG_RIGHT;		// output mode
	ECHO_DDR &= ~(1 << ECHO_LEFT | 1 << ECHO_FORWARD | 1 << ECHO_RIGHT);	// input mode로 설정 
	// interrupt 설정 (echo, timer)
	
	// rising edge, falling edge 둘 다 인터럽트 받음 == ISCn0 를 1로 설정
	EICRB |= 0 << ISC41 | 1 << ISC40 | 0 << ISC51 | 1 << ISC50 | 0 << ISC61 | 1 << ISC60;
	// INT4 : 상승/하강 에지 감지
	// INT5 : 상승/하강 에지 감지
	// INT6 : 상승/하강 에지 감지
	
	// 16MHz를 1024로 분주 16000000Hz / 1024 = 15625Hz == 15.625KHz
	// 1주기 T = 1 / 15625 = 0.000064s = 0.064ms = 64us
	
	// 분주비 128
	TCCR2 |= 0 << CS22 | 1 << CS21 | 1 << CS20;
	
	// local interrupt 마스크
	EIMSK |= 1 << INT4 | 1 << INT5 | 1 << INT6; // EXTERNAL interrupt 4,5,6 활성화
	TIMSK |= 1 << TOIE2; 	
}

void trigger_ultrasonic(void)
{
	// TRIG 보내기 전에 일정시간 LOW로 둔 다음에 보내야함!
	// HIGH 보낸 다음에도 LOW로 다시 돌려두기
	
	TRIG_PORT &= ~(1 << TRIG_LEFT | 1 << TRIG_FORWARD | 1 << TRIG_RIGHT); // 0으로 초기화
	
	// 약간의 딜레이
	_delay_us(1);
	
	// HIGH 쏘기!!
	TRIG_PORT |= 1 << TRIG_LEFT | 1 << TRIG_FORWARD | 1 << TRIG_RIGHT;
	
	// 10us동안 보내기 -> 약간 여유있게 15로 했당
	_delay_us(15);
	
	// 다시 LOW로 돌리기
	TRIG_PORT &= ~(1 << TRIG_LEFT | 1 << TRIG_FORWARD | 1 << TRIG_RIGHT);
}

void distance_ultrasonic(void)
{	
	// 200msec
	if (ultrasonic_check_timer >= 70) // 70ms, 최소가 60ms 이상
	{
		ultrasonic_check_timer = 0;
		//printf("left : %d\n forward : %d\n right : %d\n", ultrasonic_dis_left,ultrasonic_dis_forward,ultrasonic_dis_right); // 저장해둔 거리text 출력
		/*
		if(ultrasonic_dis_forward <= 10)
			direction_flag = FORWARD_FLAG;
		if(ultrasonic_dis_left <= 10 && ultrasonic_dis_forward <= 10)
			direction_flag = LEFT_FLAG;
		if(ultrasonic_dis_right <= 10 && ultrasonic_dis_forward <= 10)
			direction_flag = RIGHT_FLAG;	
		*/	
		trigger_ultrasonic(); // 출력하고 트리거 또 보내기!
	}
}