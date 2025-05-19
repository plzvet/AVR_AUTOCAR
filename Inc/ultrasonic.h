/*
 * ultrasonic.h
 *
 * Created: 2025-03-12 오후 2:49:06
 *  Author: microsoft
 */ 


#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define  F_CPU 16000000UL  // 16MHZ
#include <avr/io.h>
#include <util/delay.h>  // _delay_ms _delay_us
#include <avr/interrupt.h>
#include <stdio.h>

#define TRIG_LEFT 0  // LEFT echo pin 번호
#define TRIG_FORWARD 1  // FORWARD echo pin 번호
#define TRIG_RIGHT 2  // RIGHT echo pin 번호

#define TRIG_PORT PORTA
#define TRIG_DDR DDRA

#define ECHO_LEFT 4 // echo pin 번호
#define ECHO_FORWARD 5 // echo pin 번호
#define ECHO_RIGHT 6 // echo pin 번호

#define ECHO_PIN PINE // External INT 4
#define ECHO_DDR DDRE

#define NO_FLAG 0
#define LEFT_FLAG 1
#define FORWARD_FLAG 2
#define RIGHT_FLAG 3

#endif /* ULTRASONIC_H_ */