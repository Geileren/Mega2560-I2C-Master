/*
 * I2CMasterDriver.cpp
 *
 * Created: 20-04-2024 15:13:39
 *  Author: Geileren
 */ 

#include "I2CMasterDriver.h"

void I2CMaster::SendStart(uint8_t slaveAddress, uint8_t mode)
{
	GenerateStartMSG();
	//Loading the address and mode into TWDR ready to be sent on the bus
	TWDR = (slaveAddress << 1) | mode;
	
	//Sending the address and mode
	TWCR = 0b10000100;
	
	//Waiting for TWINT to go high indicating a new status code
	WaitForTWINT();
	
}

void I2CMaster::SendStop()
{
	//Sends stop msg on the bus, ending all communication.
	TWCR = 0b10010100;
}

uint8_t I2CMaster::ReadByte(uint8_t lastData)
{
	if (lastData)
	{
		//Data will be received and NACK returned on the bus, indicating the communication should end
		TWCR = 0b10000100;
	} 
	else
	{
		//Data will be received and ACK returned on the bus, indicating the communication should continue
		TWCR = 0b11000100;
	}
	
	//Waiting for new status (aka. ACK or NACK)
	WaitForTWINT();
	
	//returns the received byte
	return TWDR;
}

void I2CMaster::SendByte(uint8_t byteToSend)
{
	//Loads the byte into TWDR to send
	TWDR = byteToSend;
	
	//Data will be send and ACK or NACK received
	TWCR = 0b10000100;
	
	//Waiting for new status (aka. ACK or NACK)
	WaitForTWINT();
}

void I2CMaster::InitI2C()
{
	TWSR = 0; //Setting the TWI prescaler to 1
	
	TWBR = 16; //Setting SCL frequency to 333.33 kHz. Should add functionality to change this
}

uint8_t I2CMaster::GetStatus()
{
	return status;
}

void I2CMaster::GenerateStartMSG()
{
	//TWI-Control-register: TWINT is reset, TWEN and TWSTA is set to enable TWI and send start.
	TWCR = 0b10100100;
	
	//Waits for TWINT to go high indicating the start has been sent
	WaitForTWINT();
}

void I2CMaster::WaitForTWINT()
{
	//Waits until TWI - interrupt flag in  TWCR goes high
	while ((TWCR & (0b10000000)) == 0){}
	setStatus();
}

void I2CMaster::setStatus()
{
	//Reads the status register. The meaning of the status codes can be found in the datasheet
	status = TWSR;
	
}
