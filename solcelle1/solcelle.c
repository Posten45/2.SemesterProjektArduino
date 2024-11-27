/*
 * solcelle.c
 *
 * Created: 18-11-2024 10:40:40
 *  Author: frederikpost
 */ 
#include "solcelle.h"
#include "uart.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>


#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#define VOLTAGE_PIN 5 
#define CURRENT_PIN 4  

float R1 = 13000.0;  
float R2 = 5100.0;   
float sensitivity = 0.185;  
float offset = 2.5;   

extern volatile char uartReceivedChar;

void setup_sensors() {
	InitUART(9600,8,1);  // Initialiserer UART med den rette BAUD
	// initialiserer ADC
	ADMUX = (1 << REFS0);  // S�tter referencesp�ndingen for ADC til AVcc
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // T�nder ADC og s�tter prescaler til 128
}

int analogRead(uint8_t channel) {
	// V�lger ADC-kanal og nulstiller de 3 sidste bits
	ADMUX = (ADMUX & 0xF8) | (channel & 0x07);
	// Starter conversion
	ADCSRA |= (1 << ADSC);
	// venter for conversion er f�rdig
	while (ADCSRA & (1 << ADSC));
	// Returnerer ADC value
	return ADCW;
}

void measure_and_print() {
	char buffer[100];

	int sensorValue = analogRead(VOLTAGE_PIN);
	

	// Konverterer den r� value til en sp�nding (0-5V)
	float vOut = sensorValue * (5.0 / 1023.0);
	float inputVoltage = vOut * ((R1 + R2) / R2);

	// Print voltage value step by step


	int voltage_whole = (int)vOut;
	int voltage_fraction = (int)((vOut - voltage_whole) * 100);
	snprintf(buffer, sizeof(buffer), "Sp�nding m�lt af sp�ndingsdeler: %d.%02d V\n", voltage_whole, voltage_fraction);
	SendString(buffer);
	
	
	   // L�s str�m fra str�msensor (A0)
	   int currentValue = analogRead(CURRENT_PIN);

	   // Konverter r� value til sp�nding(0-5V)
	   float vShunt = currentValue * (5.0 / 1023.0);

	   // Omregn til str�m i ampere
		float current = (vShunt - offset) / sensitivity;

	   // Print str�m
	  int current_whole = (int)current;
	  int current_fraction = (int)((current - current_whole) * 100);
	  snprintf(buffer, sizeof(buffer), "Str�m m�lt af str�msensor: %d.%02d A\n", current_whole, current_fraction);
	  SendString(buffer);

	   // Beregn effekt
	   float power = vOut * current;

	   // Print effekt
	  int power_whole = (int)power;
	  int power_fraction = (int)((power - power_whole) * 100);
	  snprintf(buffer, sizeof(buffer), "Effekt: %d.%02d W\n", power_whole,power_fraction);
	  SendString(buffer);
	  
	  
   
_delay_ms(2000);
	   
   }