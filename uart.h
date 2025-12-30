#ifndef UART_H
#define UART_H

#include <avr/io.h>

void uart_init(unsigned int baud){
    unsigned int ubrr = F_CPU/16/baud-1;
	
	//set baud rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)(ubrr);
	
	//enable transmit, receive, receive interrupt
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
	
	//8 bit frame size and 1 stop bit
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
}

void uart_transmit(char data){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

void uart_print(const char* str){
	while(*str){
		uart_transmit(*str++);
	}
}

#endif