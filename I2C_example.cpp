/*
 * I2C_example.cpp
 * Using the I2C Master driver in conjunction with a LM75 temp-sensor,
 * to write the temp with UART to a PC
 * Created: 20-04-2024 14:33:49
 * Author : Geileren
 */ 

//CPU frequency
#define F_CPU 16000000

//The Address of the LM75 sensor
#define SensorAddress 0b1001000

//includes:
#include <avr/io.h>
#include <util/delay.h>
#include "I2CMasterDriver.h"

//UART-functions
#include <stdlib.h> //uses itoa()
void InitUART(unsigned long BaudRate, unsigned char DataBit, unsigned char Rx_Int);
void SendChar(char Tegn);
void SendString(char* Streng);
void SendInteger(int Tal);

int main(void)
{
	uint8_t data1, data2;
	uint16_t temp;
	
	//enables the I2C master
	I2CMaster master;
	master.InitI2C();
	
	//Enables UART
	InitUART(9600, 8, 0);
	SendString("Start of program \r\n");
	
	/*Example of sending data to slave on the I2C-bus:
	* in this case changing the Over-temperature of an LM75
	* this feature is not really used in this application
	* but works as a great example of how to send data to a slave*/
	
	master.SendStart(SensorAddress, TRANSMITTER_MODE); //starting the communication
	master.SendByte(3); //selects the LM75's OS-register (over-temperature alarm)
	master.SendByte(0b00011001);//sets the OS-temp to 25 C
	master.SendByte(0);
	master.SendStop(); //ends the communication
	
	//reselecting the temperature-register
	master.SendStart(SensorAddress, TRANSMITTER_MODE);
	master.SendByte(0);
	master.SendStop();
	
	while (1) 
    {
		/*Example of reading data from slave on the I2C-bus
		* in this case reading the temperature from a LM75.*/
		
		master.SendStart(SensorAddress, RECIEVER_MODE); //starting the communication
		data1 = master.ReadByte(NOT_LAST); //reading first byte
		data2 = master.ReadByte(LAST_DATA); //reading second byte
		master.SendStop(); //ends the communication
		
		//Processing the received data
		temp = data1;
		temp = temp << 8;
		temp += data2;
		temp = (temp >> 7) / 2;
		
		//Writing the temperature to UART
		SendString("Temp: ");
		SendInteger(temp);
		SendString("\r\n");
		
		//Waiting for sensor to make new measurement
		_delay_ms(500);
		
    }
}

//The following function are just used to communicate with a PC over UART. They are not needed for the I2C driver.

void InitUART(unsigned long BaudRate, unsigned char DataBit, unsigned char RX_Int)
{

	if ((BaudRate >= 300) && (BaudRate <= 112500) && (DataBit >= 5) && (DataBit <=8)){
		UCSR0B = 0b00011000; //no interrupts, receiver and transmitter enable
		UCSR0C = (DataBit - 5) << 1;
		// baud = f_cpu/(16*(UBRRN+1))
		// UBRRN = (f_CPU/(16*baud)) - 1
		UBRR0 = F_CPU/(16*BaudRate) - 1;
	}
	
	if (RX_Int){
		UCSR0B |= 0b10000000; //Enables Receiver interrupt
	}
}

void SendChar(char character)
{
	while((UCSR0A & 0b00100000) == 0){	}
	UDR0 = character;
}

void SendString(char* text)
{
	while(*text != 0){
		SendChar(*text);
		text++;
	}
}

void SendInteger(int Tal)
{
	char charArray[6]; //1 space for every possible character
	itoa(Tal, charArray, 10);
	SendString(charArray);
}
