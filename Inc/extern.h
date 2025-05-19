/*
 * extern.h
 *
 * Created: 2025-03-20 오전 11:29:14
 *  Author: microsoft
 */ 


#ifndef EXTERN_H_
#define EXTERN_H_

#define NO_FLAG 0
#define LEFT_FLAG 1
#define FORWARD_FLAG 2
#define RIGHT_FLAG 3

extern volatile int msec_count;
extern volatile int func_index;
extern volatile int button_count;
extern volatile uint8_t last_action;
extern void init_button(void);
extern int get_button(int button_num, int button_pin);
extern void auto_mode_check(void);
extern void init_led(void);
extern void i2c_lcd_print(void);
extern void init_timer1(void);
extern void init_L298N(void);
extern void stop(void);
extern void forward(int speed);
extern void backward(int speed);
extern void turn_left(int speed);
extern void turn_right(int speed);

extern void init_fnd(void);
extern void fnd_display(void);
extern volatile int lcd_flag;

extern void init_uart0(void);
extern void UART0_transmit(uint8_t data);
extern void pc_command_processing(void);
extern void init_uart1(void);
extern volatile uint8_t bt_data;
extern volatile int func_index;

extern volatile int ultrasonic_dis_left;
extern volatile int ultrasonic_dis_forward;
extern volatile int ultrasonic_dis_right;
extern volatile int lcd_count;

extern void I2C_LCD_init(void);
extern void Siren(void);
extern void Beep(void);
extern void RRR(void);
extern void play_song(int func_index);
extern void init_ultrasonic(void);
extern void distance_ultrasonic(void);
extern void trigger_ultrasonic(void);
extern void init_speaker(void);
extern void Butterfly(void);

extern void LCD_AUTO(void);
extern void LCD_MANUAL(void);
extern void LCD_EMERGENCY(void);
#endif /* EXTERN_H_ */