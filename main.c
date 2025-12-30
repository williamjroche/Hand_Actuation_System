#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>
//custom header files
#include "servo.h"
#include "servo_position.h"
#include "uart.h"

//servo1 -- index finger (and thumb1)
//servo2 -- middle finger
//servo3 -- ring finger
//servo4 -- pinky finger
//servo5 -- thumb2

//buffer for receive commands
volatile char rx_buffer[20];
volatile uint8_t rx_index = 0;
volatile bool new_command = false;

//to track current finger position
volatile int index_curpos = 2500;
volatile int middle_curpos = 2500;
volatile int ring_curpos = 0;
volatile int pinky_curpos = 0;
volatile int thumb2_curpos = 2200;

// UART Receive Interrupt
ISR(USART_RX_vect) {
	char received = UDR0;
	
	if (received == '\n') {
		rx_buffer[rx_index] = '\0'; // Null terminate
		new_command = true;
		rx_index = 0;
		} else if (rx_index < 19) {
		rx_buffer[rx_index++] = received;
	}
}

void process_gesture(char* gesture){
	uart_print("Received: ");
	uart_print(gesture);
	uart_print("\n");
	
	if (strcmp(gesture, "OPEN") == 0) {
		//open hand
		ServoMove all_servos_open[] = {
			{1, index_curpos, 2500},  //servo 1: index open
			{2, middle_curpos, 2500},  //servo 2: middle open
			{3, ring_curpos, 0},     //servo 3: ring open
			{4, pinky_curpos, 0},     //servo 4: pinky open
			{5, thumb2_curpos, 2200}   //servo 5: thumb open
		};
		move_servos_together(10, all_servos_open, 5);
		index_curpos = 2500;
		middle_curpos = 2500;
		ring_curpos = 0;
		pinky_curpos = 0;
		thumb2_curpos = 2200;
	} 
	else if (strcmp(gesture, "CLOSED") == 0) {
		//closed fist
		ServoMove all_servos_close[] = {
			{1, index_curpos, 1500},  //servo 1: index closed
			{2, middle_curpos, 1200},  //servo 2: middle closed
			{3, ring_curpos, 1700},     //servo 3: ring closed
			{4, pinky_curpos, 1700},     //servo 4: pinky closed
			{5, thumb2_curpos, 1500}   //servo 5: thumb closed
		};
		move_servos_together(10, all_servos_close, 5);
		index_curpos = 1500;
		middle_curpos = 1200;
		ring_curpos = 1700;
		pinky_curpos = 1700;
		thumb2_curpos = 1500;
	} 
	else if (strcmp(gesture, "POINT") == 0) {
		//point (index only)
		ServoMove point[] = {
			{1, index_curpos, 2500},  //servo 1: index open
			{2, middle_curpos, 1200},  //servo 2: middle closed
			{3, ring_curpos, 1700},     //servo 3: ring closed
			{4, pinky_curpos, 1700},     //servo 4: pinky closed
			{5, thumb2_curpos, 1500}   //servo 5: thumb closed
		};
		move_servos_together(10, point, 5);
		index_curpos = 2500;
		middle_curpos = 1200;
		ring_curpos = 1700;
		pinky_curpos = 1700;
		thumb2_curpos = 1500;
	} 
	else if (strcmp(gesture, "PINCH") == 0) {
		//pinch (thumb and index only)
		ServoMove pinch[] = {
			{1, index_curpos, 1500},  //servo 1: index partial
			{2, middle_curpos, 2500},  //servo 2: middle closed
			{3, ring_curpos, 0},     //servo 3: ring closed
			{4, pinky_curpos, 0},     //servo 4: pinky closed
			{5, thumb2_curpos, 1500}   //servo 5: thumb partial
		};
		move_servos_together(10, pinch, 5);
		index_curpos = 1500;
		middle_curpos = 2500;
		ring_curpos = 0;
		pinky_curpos = 0;
		thumb2_curpos = 1500;
	} 
	else if (strcmp(gesture, "PEACE") == 0) {
		//peace
		ServoMove peace[] = {
			{1, index_curpos, 2500},  //servo 1: index open
			{2, middle_curpos, 2500},  //servo 2: middle open
			{3, ring_curpos, 1700},     //servo 3: ring closed
			{4, pinky_curpos, 1700},     //servo 4: pinky closed
			{5, thumb2_curpos, 1500}   //servo 5: thumb closed
		};
		move_servos_together(10, peace, 5);
		index_curpos = 2500;
		middle_curpos = 2500;
		ring_curpos = 1700;
		pinky_curpos = 1700;
		thumb2_curpos = 1500;
	} 
	else if (strcmp(gesture, "ROCK") == 0) {
		//rock n roll (thumb extended)
		ServoMove rock[] = {
			{1, index_curpos, 2500},  //servo 1: index open
			{2, middle_curpos, 1200},  //servo 2: middle closed
			{3, ring_curpos, 1700},     //servo 3: ring closed
			{4, pinky_curpos, 0},     //servo 4: pinky open
			{5, thumb2_curpos, 2200}   //servo 5: thumb open
		};
		move_servos_together(10, rock, 5);
		index_curpos = 2500;
		middle_curpos = 1200;
		ring_curpos = 1700;
		pinky_curpos = 0;
		thumb2_curpos = 2200;
	} 
	else if(strcmp(gesture, "F.U.") == 0){
		//middle finger
		ServoMove fu[] = {
			{1, index_curpos, 1500}, //servo 1: index closed
			{2, middle_curpos, 2500}, //servo 2: middle open
			{3, ring_curpos, 1700},  //servo 3: ring closed
			{4, pinky_curpos, 1700}, //servo 4: pinky closed
			{5, thumb2_curpos, 1500} //servo 5: thumb closed
		};
		move_servos_together(10, fu, 5);
		index_curpos = 1500;
		middle_curpos = 2500;
		ring_curpos = 1700;
		pinky_curpos = 1700;
		thumb2_curpos = 1500;
	} 
	else if (strcmp(gesture, "None") == 0) {
		//do nothing
	} 
	else {
		uart_print("Unknown gesture\n");
	}
}

int main(void){
	servo_setup();
	uart_init(9600);
	sei(); // Enable global interrupts
	
	uart_print("Robot Ready!\n");
	
	// Set initial position (open hand)
	ServoMove initial[] = {
		{1, index_curpos, 2500},
		{2, middle_curpos, 2500},
		{3, ring_curpos, 0},
		{4, pinky_curpos, 0},
		{5, thumb2_curpos, 2200}
	};
	move_servos_together(10, initial, 5);
	
	while(1){
		DDRC &= ~(1 << PORTC0);
		DDRC &= ~(1 << PORTC1);
		
		// Check for new UART command
		if (new_command) {
			char cmd_copy[20];
			strcpy(cmd_copy, (char*)rx_buffer);
			new_command = false;
			process_gesture(cmd_copy);
		}
		
		if((PINC & (1 << 0)) != 0){
			_delay_ms(500);
			ServoMove all_servos_open[] = {
				{1, 2500, 1500},  //servo 1: 180 to 90
				{2, 2500, 1200},  //servo 2: 180 to 90
				{3, 0, 1700},  //servo 3: 180 to 90
				{4, 0, 1700},  //servo 4: 180 to 90
				{5, 2200, 1500}   //servo 5: 180 to 90
			};
			move_servos_together(10, all_servos_open, 5);
		}
		if((PINC & (1 << 1)) != 0){
			_delay_ms(500);
			ServoMove all_servos_close[] = {
				{1, 1500, 2500},  //servo 1: 90 to 180
				{2, 1200, 2500},  //servo 2: 90 to 180
				{3, 1700, 0},  //servo 3: 90 to 180
				{4, 1700, 0},  //servo 4: 90 to 180
				{5, 1500, 2200}   //servo 5: 90 to 180
			};
			move_servos_together(10, all_servos_close, 5);
		}
		
		
	}
	
	return 0;
}


