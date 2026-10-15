/*
 * ExternalADC.c
 *
 *  Created on: Nov 30, 2024
 *      Author: rnd14
 */

#include "ExternalADC.h"
#include "math.h"
#include "usart.h"


#define ADCclk_20Hz  0x08
#define ADCclk_25Hz  0x09
#define ADCclk_100Hz 0x0A
#define ADCclk_200Hz 0x0B
#define ADCclk_50Hz  0x0C
#define ADCclk_60Hz  0x0D
#define ADCclk_250Hz 0x0E
#define ADCclk_500Hz 0x0F

unsigned char cs_dly = 75;
//extern volatile unsigned char adc_state;
//extern int setADclk;

unsigned int  voltage;
unsigned int  current;

uint16_t ADC_Scan_Delay = 0;
uint8_t flag = 0;


float FloatVoltBuff[VOLT_BUFF_SIZE];
float FloatCurrBuff[VOLT_BUFF_SIZE];
double NumSum = 0.0;
//float average;




float volt_samples[NUM_SAMPLES] = {0};
float curr_samples[NUM_SAMPLES] = {0};
uint8_t sample_count = 0;



//float volt_samples[NUM_SAMPLES] = {0};
//float curr_samples[NUM_SAMPLES] = {0};
uint8_t sample_index = 0;
float voltage_sum = 0, current_sum = 0;
double prev_voltage = 0.0, prev_current = 0.0;


#define ALPHA 0.1  // Filter Strength (0.0 - No Filtering, 1.0 - Full Update)

float avg_voltage;
float avg_current;

volatile unsigned char adc_state;



uint8_t   CommReg_ChCurr;
uint8_t   SetupReg_ChCurr;
uint8_t   ClockReg_ChCurr;
uint16_t  DataReg_ChCurr;
uint8_t   TestReg_ChCurr;
uint16_t  OffsetReg_ChCurr;
uint16_t  GainReg_ChCurr;

uint8_t   CommReg_ChVolt;
uint8_t   SetupReg_ChVolt;
uint8_t   ClockReg_ChVolt;
uint16_t  DataReg_ChVolt;
uint8_t   TestReg_ChVolt;
uint16_t  OffsetReg_ChVolt;
uint16_t  GainReg_ChVolt;

uint8_t setup_data = 0;
uint8_t clock_data = 0;

uint16_t ReadVoltExtADCCount;
uint16_t ReadCurrExtADCCount;


void adc_write(uint8_t cmd)
{
	ADC_CS_GPIO_Port->BSRR = (uint32_t)ADC_CS_Pin << 16u;	//HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, RESET);
	ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin;	//HAL_GPIO_WritePin(ADC_DAC_SCK_GPIO_Port, ADC_DAC_SCK_Pin, SET);
	ADC_DAC_DIN_GPIO_Port->BSRR = ADC_DAC_DIN_Pin;

	for(uint8_t i = 0; i < 8; i++)
	{
		ADC_DAC_SCK_GPIO_Port->BSRR = (uint32_t)ADC_DAC_SCK_Pin << 16u;	//HAL_GPIO_WritePin(ADC_DAC_SCK_GPIO_Port, ADC_DAC_SCK_Pin, RESET);
//		Delay_us(150);
		//HAL_GPIO_WritePin(ADC_DAC_DIN_GPIO_Port, ADC_DAC_DIN_Pin, cmd & (0x80 >> i));
		if(cmd & (0x80 >> i)) // here ANDING with 0x80 bcoz MSB is first sent in the ADC comm register
		{
			ADC_DAC_DIN_GPIO_Port->BSRR = ADC_DAC_DIN_Pin;	//HAL_GPIO_WritePin(ADC_DAC_DIN_GPIO_Port, ADC_DAC_DIN_Pin, SET));
		}
		else
		{
			ADC_DAC_DIN_GPIO_Port->BSRR = (uint32_t)ADC_DAC_DIN_Pin << 16u;	//HAL_GPIO_WritePin(ADC_DAC_DIN_GPIO_Port, ADC_DAC_DIN_Pin, RESET);
		}

		for(int DCount = 0;DCount <= 100; DCount++){
			 __NOP();
		}
		ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin;	//HAL_GPIO_WritePin(ADC_DAC_SCK_GPIO_Port, ADC_DAC_SCK_Pin, SET);
	}

	ADC_CS_GPIO_Port->BSRR = ADC_CS_Pin;	//HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, SET);
	ADC_DAC_DIN_GPIO_Port->BSRR = (uint32_t)ADC_DAC_DIN_Pin << 16;
}



uint16_t adc_read_data_reg(uint8_t cmd)
{
	uint16_t adc_data = 0x0000;

	adc_write(cmd);


	ADC_CS_GPIO_Port->BSRR = (uint32_t)ADC_CS_Pin <<16u;		//HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, RESET);
	ADC_DAC_DIN_GPIO_Port->BSRR = ADC_DAC_DIN_Pin;
	ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin;	//HAL_GPIO_WritePin(ADC_DAC_SCK_GPIO_Port, ADC_DAC_SCK_Pin, SET);

	for(uint8_t i = 0; i < 16; i++)
	{
		ADC_DAC_SCK_GPIO_Port->BSRR = (uint32_t)ADC_DAC_SCK_Pin <<16u;	//HAL_GPIO_WritePin(ADC_DAC_SCK_GPIO_Port, ADC_DAC_SCK_Pin, RESET);
//		Delay_us(150);
		adc_data = adc_data << 1;

		if ((ADC_DOUT_GPIO_Port->IDR & ADC_DOUT_Pin) != (uint32_t)GPIO_PIN_RESET)	//(HAL_GPIO_ReadPin(ADC_DOUT_GPIO_Port, ADC_DOUT_Pin))
		{
			adc_data |= 1;
		}
		else
		{
			adc_data |= 0;
		}
		for(int DCount = 0;DCount <= 100; DCount++){
			 __NOP();
		}
//		Delay_us(150);
//			adc_data = adc_data << 1;
			ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin;	//HAL_GPIO_WritePin(ADC_DAC_SCK_GPIO_Port, ADC_DAC_SCK_Pin, SET);
	}

	ADC_CS_GPIO_Port->BSRR = ADC_CS_Pin;	//HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, SET);
	ADC_DAC_DIN_GPIO_Port->BSRR = (uint32_t)ADC_DAC_DIN_Pin << 16;
//	HAL_Delay(5);  // rohit ne chalu kia
	return adc_data;
}


uint8_t adc_read_comm_reg(uint8_t cmd)
{
	uint8_t adc_data = 0x00;
	uint8_t i = 0;
//	uint8_t read_data = 0;
//	char adc_buff[30] = {0};

	adc_write(cmd);

	ADC_CS_GPIO_Port->BSRR = (uint32_t)ADC_CS_Pin <<16u;		//HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, RESET);
	ADC_DAC_DIN_GPIO_Port->BSRR = ADC_DAC_DIN_Pin;
	ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin;	//HAL_GPIO_WritePin(ADC_DAC_SCK_GPIO_Port, ADC_DAC_SCK_Pin, SET);

	for(i = 0; i < 8; i++)
	{
		adc_data = adc_data << 1;
/*CLK LOW */
		ADC_DAC_SCK_GPIO_Port->BSRR = (uint32_t)ADC_DAC_SCK_Pin <<16u;	//HAL_GPIO_WritePin(ADC_DAC_SCK_GPIO_Port, ADC_DAC_SCK_Pin, RESET);
//		Delay_us(150);

		if ((ADC_DOUT_GPIO_Port->IDR & ADC_DOUT_Pin) != (uint32_t)GPIO_PIN_RESET)	//(HAL_GPIO_ReadPin(ADC_DOUT_GPIO_Port, ADC_DOUT_Pin))
		{
			adc_data |= 1;
		}
		else
		{
			adc_data |= 0;
		}

//		sprintf(adc_buff, "READ => %.3d\r\n",  adc_data);
//		HAL_UART_Transmit(&huart1, (uint8_t*)adc_buff, strlen("READ => 000\r\n"), HAL_MAX_DELAY);
//		HAL_Delay(1000);

//		if((i == 0) && (adc_data == 0x00))
//		{
//			ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin;
//			goto end1;
//		}
/*CLK HIGH */
		ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin;	//HAL_GPIO_WritePin(ADC_DAC_SCK_GPIO_Port, ADC_DAC_SCK_Pin, SET);
	}

	ADC_DAC_DIN_GPIO_Port->BSRR = (uint32_t)ADC_DAC_DIN_Pin << 16;
	ADC_CS_GPIO_Port->BSRR = ADC_CS_Pin;	//HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, SET);

	return adc_data;
}


void adc_init()
{
	HAL_UART_Transmit(&huart1, (uint8_t*)"INITIALISING ADC\r\n", strlen("INITIALISING ADC\r\n"), HAL_MAX_DELAY);
	HAL_Delay(1000);

	adc_write(0xff);
	adc_write(0xff);
	adc_write(0xff);
	adc_write(0xff);

	char adc_buff[30] = {0};
	uint8_t read_data = 0xFF; //any value greater than 0 can be taken

	HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, SET);

//CH0 Configuration//////////////////////
	adc_write(CLK_REG_CMD_CH0);	//passing 0x20 command to write to clock register

	adc_write(CLK_REG_DATA_200);
//	adc_write(0x1F);
	HAL_Delay(100);
//	while((adc_read_comm_reg(CH0_BSY_CHK) & 0x80) != 0x00);

	adc_write(SETUP_REG_CMD_CH0);	//passing 0x10 command to write to setup register
	adc_write(SETUP_REG_DATA);	//passing 0x44 data to setup register

	while((read_data & 0x80) != 0x00)
	{
		read_data = adc_read_comm_reg(CH0_BSY_CHK);

		sprintf((char*)adc_buff, "READ_ADC_CH0_BUSY => %.3d\r\n",  read_data);
		HAL_UART_Transmit(&huart1, (uint8_t*)adc_buff, strlen("READ_ADC_CH0_BUSY => 000\r\n"), HAL_MAX_DELAY);
	}
//	HAL_Delay(500);

	adc_write(0xff);
	adc_write(0xff);
	adc_write(0xff);
	adc_write(0xff);

//CH1 Configuration/////////////////////
	adc_write(CLK_REG_CMD_CH1);	//passing 0x21 command to write to clock register

	adc_write(CLK_REG_DATA_200);
//	adc_write(0x1F);
//	while((adc_read_comm_reg(CH1_BSY_CHK) & 0x80) != 0x00);

	adc_write(SETUP_REG_CMD_CH1);	//passing 0x11 command to write to setup register
	adc_write(SETUP_REG_DATA);	//passing 0x44 data to setup register

//	HAL_Delay(500);

	read_data = 0xFF;

	while((read_data & 0x80) != 0x00)
	{
		read_data = adc_read_comm_reg(CH1_BSY_CHK);
		sprintf((char*)adc_buff, "READ_ADC_CH1_BUSY => %.3d\r\n",  read_data);
		HAL_UART_Transmit(&huart1, (uint8_t*)adc_buff, strlen("READ_ADC_CH1_BUSY => 000\r\n"), HAL_MAX_DELAY);
	}

	HAL_Delay(2000);

	HAL_UART_Transmit(&huart1, (uint8_t*)"ADC INITIALISED SUCCESSFULLY\r\n", strlen("ADC INITIALISED SUCCESSFULLY\r\n"), HAL_MAX_DELAY);
	HAL_Delay(500);
}



void Write_ADC_Data(unsigned char data)
{
	//Local variables
	unsigned char pos=128;
	unsigned char counter;

	//Make CS line low to begin transmission
	Set_ADC_CS_LOW();
	//Set the SCLK line low to set rising edge
	Set_ADC_DIN_HIGH();
	//Send 8 bits serial
	for(counter=0;counter<8;counter++)
	{

		Set_ADC_SCLK_LOW();
		Delay_us(cs_dly);//chalu tha
		//Check for Bit
		if(data & pos)
		{
				Set_ADC_DIN_HIGH();
				//Delay_us(cs_dly);//chalu tha
		}
		else
		{
				Set_ADC_DIN_LOW();
				//Delay_us(cs_dly);//chalu tha
			 //Set_ADC_DIN_LOW();
		}


		Set_ADC_SCLK_HIGH();
		pos=pos>>1;
		//Delay_ms(cs_dly);

	}
	Set_ADC_DIN_LOW();
	//Delay_us(cs_dly);//chalu tha
	//Delay_us(cs_dly);//chalu tha
	//Delay_ms(cs_dly);
	//Make CS line high to end transmission
	Set_ADC_CS_HIGH();
	//Delay_us(cs_dly);

}

unsigned char Read_ADC_Data(void)
{
	unsigned char data=0;

	unsigned char counter;

	//Make CS line low to begin transmission
	Set_ADC_CS_LOW();

	Set_ADC_DIN_HIGH();

	for(counter=0;counter<8;counter++)
	{
		data=data<<1;
		//Set the SCLK line low to set rising edge
		Set_ADC_SCLK_LOW();

		if(HAL_GPIO_ReadPin(ADC_DOUT_GPIO_Port, ADC_DOUT_Pin))
		{
			data |= 1;
		}
		else
		{
			data |= 0;
		}
		//Make rising edge
		Set_ADC_SCLK_HIGH();

	}

	Set_ADC_DIN_LOW();
	//Set_ADC_SCLK_HIGH();
	//Delay_us(cs_dly); //chalu tha
	Delay_us(cs_dly);
	Delay_us(cs_dly);//chalu tha

	//Make CS line high to end transmission
	Set_ADC_CS_HIGH();
	//Delay_us(cs_dly);

	return data;
}


unsigned int Read_ADC_Output(void)
{

	unsigned int data=0;

	unsigned char counter;

	//Make CS line low to begin transmission
	Set_ADC_CS_LOW();
  Set_ADC_DIN_HIGH();

	for(counter=0;counter<16;counter++)
	{
		data=data<<1;
		//Set the SCLK line low to set rising edge
		Set_ADC_SCLK_LOW();
		if(HAL_GPIO_ReadPin(ADC_DOUT_GPIO_Port, ADC_DOUT_Pin))
		{
			data |= 1;
		}
		else
		{
			data |= 0;
		}

		ADC_WAIT_TIME();
		Set_ADC_SCLK_HIGH();
	}

	Set_ADC_DIN_LOW();
	Delay_us(cs_dly);//chalu tha
	Delay_us(cs_dly);//chalu tha
	//Delay_ms(cs_dly);
	//Make CS line high to end transmission
	Set_ADC_CS_HIGH();
	//Delay_us(cs_dly);
	//data = 15000;
	return data;
}


unsigned char ADC_CH0_Is_Busy(void)
{
	unsigned char data;

	//mater clock enabled
	Write_ADC_Data(0x08);

	data = Read_ADC_Data();
	//sprintf(voltage_buf,"rr %d\r\n",data);
	//USART_Write(voltage_buf);
	if((data&0x80)==0x00)
	{
		return 0;
	}
	else
	{
		return 1;
	}

}

unsigned char ADC_CH1_Is_Busy(void)
{
	unsigned char data;

	//mater clock enabled
	Write_ADC_Data(0x09);

	data = Read_ADC_Data();

	if((data&0x80)==0x00)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

//void scan_adc(void)
//{
//
//		switch(adc_state)
//				{
//
//					case 0:
//									//Channel AIN1
//									Write_ADC_Data(0x10);
//									//Mode Self calibration
//									//Unipolar
//									Write_ADC_Data(SETUP_REG);
//									adc_state=1;
//
//					break;
//					case 1:
//									if(ADC_CH0_Is_Busy() == 0)
//									{
//										Write_ADC_Data(0x38);
//										current = Read_ADC_Output();
//										current &= 0xFFFF;
//										//voltage = Read_ADC_Output();
//										//voltage &= 0xFFFF;
//										adc_state=2;
//									}
//
//					break;
//					case 2:
//									//Channel AIN1
//									Write_ADC_Data(0x11);
//									//Mode Self calibration
//									//Unipolar
//									Write_ADC_Data(SETUP_REG);
//									adc_state=3;
//
//					break;
//					case 3:
//									if(ADC_CH1_Is_Busy() == 0)
//									{
//										Write_ADC_Data(0x39);
//										voltage = Read_ADC_Output();
//										voltage &= 0xFFFF;
//										//current = Read_ADC_Output();
//										//current &= 0xFFFF;
//										adc_state=0;
//									}
//					break;
//
//
//				}
//
//}



void ADC_Config(void)
{

	//unsigned char temp;

	//set CS PIN
	Set_ADC_CS_HIGH();


	Write_ADC_Data(0xFF);
	Write_ADC_Data(0xFF);
	Write_ADC_Data(0xFF);
	Write_ADC_Data(0xFF);

	//Configure ADC
  //Next reg clock, write
  //Channel AIN1
  Write_ADC_Data(0x20);
	//Delay_ms(50);

	// Master clock enabled, 2.000MHz Clock, set output rate to 50Hz
	Write_ADC_Data(0x0c);//0x08->20Hz,0x09->25Hz,0x0A->100Hz,0x0B->200Hz,0x0C->50Hz,0x0D->60Hz,0x0E->250Hz,0x0F->500Hz
	//Delay_ms(50);

	// Active Channel is Ain1(+)/Ain1(-), next operation as write to the setup register
	Write_ADC_Data(0x10);
	//Delay_ms(50);

	// Gain = 1, unipolar mode, buffer off, clear FSYNC and perform a Self Calibration
	Write_ADC_Data(SETUP_REG);//44
	//Delay_ms(50);

	//Active Channel is Ain1(+)/Ain1(-), next operation as write to the clock register
	Write_ADC_Data(0x21);
	//Delay_ms(50);

	// Master clock enabled, 2.000MHz Clock, set output rate to 50Hz
	Write_ADC_Data(0x0c);//0c 09
	//Delay_ms(50);

	// Active Channel is Ain1(+)/Ain1(-), next operation as write to the setup register
	Write_ADC_Data(0x11);
	//Delay_ms(50);

	// Gain = 1, unipolar mode, buffer off, clear FSYNC and perform a Self Calibration
	Write_ADC_Data(SETUP_REG);//44
	//Delay_ms(50);

	//Write_ADC_Data(0x28);
	//Write_ADC_Data(0x18);
	//Write_ADC_Data(0x29);
	//Write_ADC_Data(0x19);

}


void ExtADCScan(void)
{





}










































//This function return status of DRDY pin or MSB bit of communication resistor as it is
//Example DRDY bit is 0 i will return 0.
//if DRDY is 1 it will return 1.
bool DataReady(uint8_t Channel)
{
	uint8_t ADC_RecData;
	SetNextOperation(ADC_READ, Channel, COMM_REG);
	ADC_RecData = Read8BitData();
	return (ADC_RecData & 0x80) != 0x00;          							//return either 0 or 1 if data is ready return 0
}

void ADC_Write(uint8_t cmd)
{
    ADC_CS_GPIO_Port->BSRR = (uint32_t)ADC_CS_Pin << 16u;      				// Pull CS low to start communication
    ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin;       					// Ensure SCK is high before communication starts

    for(uint8_t i = 0; i < 8; i++)
    {
        ADC_DAC_SCK_GPIO_Port->BSRR = (uint32_t)ADC_DAC_SCK_Pin << 16u; 	// Pull clock low
        if(cmd & (0x80 >> i))
        {
            ADC_DAC_DIN_GPIO_Port->BSRR = ADC_DAC_DIN_Pin; 					// Set or clear data bit before clock rises
        }
        else
        {
            ADC_DAC_DIN_GPIO_Port->BSRR = (uint32_t)ADC_DAC_DIN_Pin << 16u;
        }
//		for(int DCount = 0;DCount <= 100; DCount++)//100
//		{         				// Small delay for data setup time
//			 __NOP();
//		}

        ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin; 						// Pull clock high (data sampled on rising edge)
//		for(int DCount = 0;DCount <= 100; DCount++){   						// Small delay for clock high time
//			 __NOP();
//		}
    }

    ADC_CS_GPIO_Port->BSRR = ADC_CS_Pin;  									// Pull CS high to end communication
}

uint8_t Read8BitData()
{
    uint8_t adc_data = 0;

    ADC_CS_GPIO_Port->BSRR = (uint32_t)ADC_CS_Pin << 16u; 			       			// Pull CS low to start reading
    ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin; 				           			// Ensure SCK is high before reading starts
    ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin;

    for (uint8_t i = 0; i < 8; i++)
    {
        ADC_DAC_SCK_GPIO_Port->BSRR = (uint32_t)ADC_DAC_SCK_Pin << 16u;    			// Pull clock low to prepare for reading

//		for(int DCount = 0;DCount <= 100; DCount++){                       			// Small delay to stabilize the signal
//			 __NOP();
//		}

        if ((ADC_DOUT_GPIO_Port->IDR & ADC_DOUT_Pin) != (uint32_t)GPIO_PIN_RESET)   // Read data bit from ADC DOUT
        {
            adc_data = (adc_data << 1) | 1; 									  // Set LSB to 1
        }
        else
        {
            adc_data = (adc_data << 1); 										  // Set LSB to 0
        }

//		for(int DCount = 0;DCount <= 100; DCount++){ 							  // Small delay before clocking high
//			 __NOP();
//		}

        ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin; 							  // Pull clock high (data is stable for ADC)
    }

    ADC_DAC_DIN_GPIO_Port->BSRR = (uint32_t)ADC_DAC_DIN_Pin << 16;
    ADC_CS_GPIO_Port->BSRR = ADC_CS_Pin;   										  // Pull CS high to end communication

    return adc_data;
}

uint16_t Read16BitData()
{
    uint16_t adc_data = 0;

    ADC_CS_GPIO_Port->BSRR = (uint32_t)ADC_CS_Pin << 16u; 			       			// Pull CS low to start reading
    ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin; 				           			// Ensure SCK is high before reading starts
    ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin;

    for (uint8_t i = 0; i < 16; i++)
    {
        ADC_DAC_SCK_GPIO_Port->BSRR = (uint32_t)ADC_DAC_SCK_Pin << 16u;    			// Pull clock low to prepare for reading

//		for(int DCount = 0;DCount <= 100; DCount++){                       			// Small delay to stabilize the signal
//			 __NOP();
//		}

        if ((ADC_DOUT_GPIO_Port->IDR & ADC_DOUT_Pin) != (uint32_t)GPIO_PIN_RESET)   // Read data bit from ADC DOUT
        {
            adc_data = (adc_data << 1) | 1; 									  // Set LSB to 1
        }
        else
        {
            adc_data = (adc_data << 1); 										  // Set LSB to 0
        }

//		for(int DCount = 0;DCount <= 100; DCount++){ 							  // Small delay before clocking high
//			 __NOP();
//		}

        ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin; 							  // Pull clock high (data is stable for ADC)
    }

    ADC_DAC_DIN_GPIO_Port->BSRR = (uint32_t)ADC_DAC_DIN_Pin << 16;
    ADC_CS_GPIO_Port->BSRR = ADC_CS_Pin;   										  // Pull CS high to end communication

    return adc_data;
}

uint32_t Read24BitData()
{
    uint32_t adc_data = 0;

    ADC_CS_GPIO_Port->BSRR = (uint32_t)ADC_CS_Pin << 16u; 			       			// Pull CS low to start reading
    ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin; 				           			// Ensure SCK is high before reading starts

    for (uint8_t i = 0; i < 24; i++)
    {
        ADC_DAC_SCK_GPIO_Port->BSRR = (uint32_t)ADC_DAC_SCK_Pin << 16u;    			// Pull clock low to prepare for reading

		for(int DCount = 0;DCount <= 100; DCount++){                       			// Small delay to stabilize the signal
			 __NOP();
		}

        if ((ADC_DOUT_GPIO_Port->IDR & ADC_DOUT_Pin) != (uint32_t)GPIO_PIN_RESET)   // Read data bit from ADC DOUT
        {
            adc_data = (adc_data << 1) | 1; 									  // Set LSB to 1
        }
        else
        {
            adc_data = (adc_data << 1); 										  // Set LSB to 0
        }

		for(int DCount = 0;DCount <= 100; DCount++){ 							  // Small delay before clocking high
			 __NOP();
		}

        ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin; 							  // Pull clock high (data is stable for ADC)
    }
    ADC_CS_GPIO_Port->BSRR = ADC_CS_Pin;   										  // Pull CS high to end communication

    return adc_data & 0xFFFFFF;
}

void SetNextOperation(uint8_t NextOperation, uint8_t Channel, uint8_t Register)
{
	uint8_t Command = 0;

    Command = (Register & 0x70) |   				// Mask RS2-RS0 bits (bits 6-4)
              (NextOperation & 0x08) |  			// Mask R/W bit (bit 3)
              (Channel & 0x03);  					// Mask CH1-CH0 bits (bits 1-0)

    ADC_Write(Command);
}

// Function to configure the Setup Register
void ConfigureSetupRegister(uint8_t mode, uint8_t gain, uint8_t bipolar, uint8_t buffer, uint8_t fsync)
{
     setup_data = 0;

    setup_data |= (mode & 0xC0);   					// Mode setting (bits 7-6)
    setup_data |= (gain & 0x38);   					// Gain setting (3 bits, bits 5-3)
    setup_data |= (bipolar & 0x04); 				// Bipolar/Unipolar mode (bit 2)
    setup_data |= (buffer & 0x02);  				// Buffer enable/disable (bit 1)
    setup_data |= (fsync & 0x01);   				// FSYNC enable/disable (bit 0)

    ADC_Write(setup_data);  						// Write setup configuration
}

// Function to configure the Clock Register
void ConfigureClockRegister(uint8_t clkdis, uint8_t clkdiv, uint8_t clk, uint8_t fs)
{
    clock_data = 0;

    clock_data |= (clkdis & 0x10);  				// Master Clock Disable Bit (bit 3)
    clock_data |= (clkdiv & 0x08);  				// Clock Divider Bit (bit 2)
    clock_data |= (clk & 0x04);     				// Clock Bit (bit 1)
    clock_data |= (fs & 0x03);      				// Filter Selection Bits (bit 0)

    ADC_Write(clock_data); 							// Write clock configuration
}

void ADC_Reset()
{
    ADC_CS_GPIO_Port->BSRR = (uint32_t)ADC_CS_Pin << 16u; 					// Pull CS low to initiate the reset sequence
    ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin; 							// Ensure SCK starts high before toggling
    ADC_DAC_DIN_GPIO_Port->BSRR = ADC_DAC_DIN_Pin; 							// Set DIN high (some ADCs require this for reset sequence)

    for (uint8_t i = 0; i < 32; i++)  										// Toggle the clock 32 times to reset the ADC
    {
        ADC_DAC_SCK_GPIO_Port->BSRR = (uint32_t)ADC_DAC_SCK_Pin << 16u; 	// Pull clock low

        for (int DCount = 0; DCount <= 100; DCount++)  						// Small delay to meet ADC timing requirements
        {
            __NOP();
        }

        ADC_DAC_SCK_GPIO_Port->BSRR = ADC_DAC_SCK_Pin; 						// Pull clock high
    }

    ADC_CS_GPIO_Port->BSRR = ADC_CS_Pin;  									// Pull CS high to complete the reset sequence
    ADC_DAC_DIN_GPIO_Port->BSRR = (uint32_t)ADC_DAC_DIN_Pin << 16u; 		// Reset DIN to low (default state after reset)
}


void ADC_Init()
{
	HAL_Delay(1000);
	HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, SET);
	// ********** ADC Channel 0 (CH0) Configuration **********
	// *******************************************************

	SetNextOperation(ADC_WRITE, VOLT_CHANNEL, CLOCK_REG);
	ConfigureClockRegister(CLKDIS_DISABLE, CLKDIV_1, CLK_LOW_FREQ, FS_20HZ); //YOLO ADC

	SetNextOperation(ADC_WRITE, VOLT_CHANNEL, SETUPP_REG);
	ConfigureSetupRegister(SELF_CALIB, GAIN_1, UNIPOLAR, BUFFER_DISABLE, FSYNC_DISABLE);


    while (DataReady(VOLT_CHANNEL) == 1)       				//wait krna hai jb tk ki status 0 na ho, jb 0 hoga mtlb channel ready ho gyi hai
    {
    	 __NOP();
    }
	// ********** ADC Channel 1 (CH1) Configuration **********
	// *******************************************************

	SetNextOperation(ADC_WRITE, CURR_CHANNEL, CLOCK_REG);
	ConfigureClockRegister(CLKDIS_DISABLE, CLKDIV_1, CLK_LOW_FREQ, FS_20HZ);  //YOLO ADC FS_20HZ

	SetNextOperation(ADC_WRITE, CURR_CHANNEL, SETUPP_REG);
	ConfigureSetupRegister(SELF_CALIB, GAIN_1, UNIPOLAR, BUFFER_DISABLE, FSYNC_DISABLE);

    while (DataReady(CURR_CHANNEL) == 1)
    {
    	 __NOP();
    }


	SetNextOperation(ADC_READ, VOLT_CHANNEL, CLOCK_REG);
	ClockReg_ChVolt = Read8BitData();
	SetNextOperation(ADC_READ, VOLT_CHANNEL, SETUPP_REG);
	SetupReg_ChVolt = Read8BitData();

	SetNextOperation(ADC_READ, CURR_CHANNEL, CLOCK_REG);
	ClockReg_ChCurr = Read8BitData();
	SetNextOperation(ADC_READ, CURR_CHANNEL, SETUPP_REG);
	SetupReg_ChCurr = Read8BitData();

}


void scan_adc()
{
//	char adc_buff[10];
	ADC_Scan_Delay++;
//	if(ADC_Scan_Delay > 5)
	{
		switch(adc_state)
		{

		case 0:
			//Channel AIN1
			SetNextOperation(ADC_WRITE, VOLT_CHANNEL, SETUPP_REG);
			ConfigureSetupRegister(SELF_CALIB, GAIN_1, UNIPOLAR, BUFFER_DISABLE, FSYNC_DISABLE);

			adc_state=1;

			break;
		case 1:
//			if((adc_read_comm_reg(CURR_BSY_CHK) & 0x80) == 0x00)
			if(DataReady(VOLT_CHANNEL) == 0)
			{
				ReadVoltExtADCCount = adc_read_data_reg(VOLT_DATA_READ);

				ReadVoltage = ((float)ReadVoltExtADCCount * 80.00f) / (65535.0f);
//				ReadVoltage = ((float)ReadVoltExtADCCount * 10.00f) / (65535.0f);
//				ReadVolt =   (float)(ReadVoltExtADCCount)/VDacExtRatio;
#ifdef FIVE_POINT
				//FIVE POINT

				if (ReadVoltExtADCCount>45)
				{
					if (ReadVolt <= VRead_X2)
					{
						Vread_cal_data = CalculateGain (VRead_X1, VRead_Y1, VRead_X2, VRead_Y2);
						ReadVolt = (float)((Vread_cal_data.gain * ReadVolt)+(Vread_cal_data.offset));
					}
					else if ( (ReadVolt > VRead_X2 ) && (ReadVolt <= VRead_X3) )
					{
						Vread_cal_data = CalculateGain (VRead_X2, VRead_Y2, VRead_X3, VRead_Y3);
						ReadVolt = (float)((Vread_cal_data.gain*ReadVolt)+(Vread_cal_data.offset));
					}
					else if ( (ReadVolt > VRead_X3) && (ReadVolt <= VRead_X4) )
					{
						Vread_cal_data = CalculateGain (VRead_X3, VRead_Y3, VRead_X4, VRead_Y4);
						ReadVolt = (float)((Vread_cal_data.gain*ReadVolt)+(Vread_cal_data.offset));
					}
					else if (ReadVolt > VRead_X4)
					{
						Vread_cal_data = CalculateGain (VRead_X4, VRead_Y4, VRead_X5, VRead_Y5);
						ReadVolt = (float)((Vread_cal_data.gain*ReadVolt)+(Vread_cal_data.offset));
					}
				}
				else ReadVolt = 0;
#endif
//				if (ReadVolt < 0) ReadVolt = 0;
//				if (ReadVolt>65535) ReadVolt = 65535;
				adc_state=2;
			}
			break;
		case 2:
			//Channel AIN1
			SetNextOperation(ADC_WRITE, CURR_CHANNEL, SETUPP_REG);
			ConfigureSetupRegister(SELF_CALIB, GAIN_1, UNIPOLAR, BUFFER_DISABLE, FSYNC_DISABLE);

			adc_state=3;
			break;
		case 3:
//			if((adc_read_comm_reg(VOLT_BSY_CHK) & 0x80) == 0x00)
			if(DataReady(CURR_CHANNEL) == 0)
			{
				ReadCurrExtADCCount = adc_read_data_reg(CURR_DATA_READ);
				ReadCurrent = ((float)ReadCurrExtADCCount * 200.00f) / (65535.0f);
				//ReadCurrent = ((float)ReadCurrExtADCCount * 2.5f) / (65536.0f);
//				ReadCurr =  (float)(ReadCurrExtADCCount)/IDacExtRatio;

				#ifdef FIVE_POINT
					//FIVE POINT
				//master calibration

				if (ReadCurrExtADCCount >0)
				{
				ReadCurr = (float)(ReadCurr * 1.18323);

					if (ReadCurr <= IRead_X2)
					{
						Iread_cal_data = CalculateGain (IRead_X1, IRead_Y1, IRead_X2, IRead_Y2);
						ReadCurr = (float)((Iread_cal_data.gain * ReadCurr)+(Iread_cal_data.offset));
					}
					else if ( (ReadCurr > IRead_X2 ) && (ReadCurr <= IRead_X3) )
					{
						Iread_cal_data = CalculateGain (IRead_X2, IRead_Y2, IRead_X3, IRead_Y3);
						ReadCurr = (float)((Iread_cal_data.gain*ReadCurr)+(Iread_cal_data.offset));
					}
					else if ( (ReadCurr > IRead_X3) && (ReadCurr <= IRead_X4) )
					{
						Iread_cal_data = CalculateGain (IRead_X3, IRead_Y3, IRead_X4, IRead_Y4);
						ReadCurr = (float)((Iread_cal_data.gain*ReadCurr)+(Iread_cal_data.offset));
					}
					else if (ReadCurr > IRead_X4)
					{
						Iread_cal_data = CalculateGain (IRead_X4, IRead_Y4, IRead_X5, IRead_Y5);
						ReadCurr = (float)((Iread_cal_data.gain*ReadCurr)+(Iread_cal_data.offset));
					}
				}
				else ReadCurr  = 0;
		#endif
//				if (ReadCurr < 0) ReadCurr = 0;
//				if (ReadCurr>65535) ReadCurr = 65535;
				adc_state=0;
			}

			break;


		}



	}
}





