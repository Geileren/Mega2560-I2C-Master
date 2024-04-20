/*
 * I2CMasterDriver.cpp
 *
 * Created: 20-04-2024 14:50:09
 *  Author: Geileren
 */ 

#include <avr/io.h>

using namespace std;
/*
enum status{
	};
	*/

enum mode{
	TRANSMITTER_MODE = 0,
	RECIEVER_MODE = 1
	};

enum data{
	LAST_DATA = 1,
	NOT_LAST = 0
	};

class I2CMaster{
public:
	void InitI2C(); //setup for the I2C. Sets scl speed to 333.33 kHz.
	
	void SendStart(uint8_t slaveAddress, uint8_t masterMode); //used to start I2C communication
	void SendStop(); //used to end I2C communication
	
	uint8_t ReadByte(uint8_t lastData); //reads a byte from the active slave
	void SendByte(uint8_t byteToSend); //sends a byte to the active slave
	
	uint8_t GetStatus(); //Reads the status code
	
private:
	uint8_t status;

	void GenerateStartMSG();
	void WaitForTWINT();
	void setStatus();
	
};