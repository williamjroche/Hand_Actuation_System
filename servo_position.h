#ifndef servo_position_h
#define servo_position_h

#include <stddef.h> //for NULL

//Custom delay function (for function usage)
void delay_ms(int ms) {
	for(int i = 0; i < ms; i++) {
		_delay_ms(1);
	}
}

//Set servo position directly (no animation)
void set_servo_pos(int servo_number, int pulse) {
	switch(servo_number) {
		case 1:
		OCR1A = pulse * 2;
		break;
		case 2:
		OCR1B = pulse * 2;
		break;
		case 3:
		OCR2A = pulse / 16;
		break;
		case 4:
		OCR2B = pulse / 16;
		break;
		case 5:
		OCR0A = pulse / 16;
		break;
		case 6:
		OCR0B = pulse / 16;
		break;
	}
}

//Structure to hold servo movement data
typedef struct {
	int servo_num;      //servo (1-6)
	int start_pos;      //Starting pulse width (starting angle)
	int end_pos;        //Ending pulse width (end angle)
} ServoMove;


//Usage: move_servos_together(delay, array_of_moves, number_of_servos)
void move_servos_together(int delay, ServoMove* servos, int count) {
	if(count == 0 || servos == NULL) return;
	
	//current positions and completion status
	int current_pos[count];
	bool done[count];
	
	//Init
	for(int i = 0; i < count; i++) {
		current_pos[i] = servos[i].start_pos;
		done[i] = false;
	}
	
	bool all_done = false;
	
	while(!all_done) {
		all_done = true;
		
		for(int i = 0; i < count; i++) {
			if(!done[i]) {
				all_done = false;
				
				//increment servos (or decrement)
				if(current_pos[i] < servos[i].end_pos) {
					current_pos[i] += 10;
					if(current_pos[i] >= servos[i].end_pos) {
						current_pos[i] = servos[i].end_pos;
						done[i] = true;
					}
					} else if(current_pos[i] > servos[i].end_pos) {
					current_pos[i] -= 10;
					if(current_pos[i] <= servos[i].end_pos) {
						current_pos[i] = servos[i].end_pos;
						done[i] = true;
					}
					} else {
					done[i] = true;
				}
				
				//servo position
				set_servo_pos(servos[i].servo_num, current_pos[i]);
			}
		}
		
		delay_ms(delay);
	}
}

//Original single servo function (for individual movement)
void move_servo(int servo_number, int delay, int pulse, int pulse_end, bool to_pos){
	if(to_pos == true){
		for(; pulse <= pulse_end; pulse+=10){
			set_servo_pos(servo_number, pulse);
			delay_ms(delay);
		}
	}
	else{
		for(; pulse >= pulse_end; pulse-=10){
			set_servo_pos(servo_number, pulse);
			delay_ms(delay);
		}
	}
}

#endif