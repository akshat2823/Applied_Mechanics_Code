/*
 * ExternalADC.h
 *
 *  Created on: Nov 30, 2024
 *      Author: rnd14
 */

#ifndef INC_EXTERNALADC_H_
#define INC_EXTERNALADC_H_

#include "main.h"


#define CLK_REG_CMD_CH0		0x20
#define CLK_REG_CMD_CH1		0x21

#define	SETUP_REG_CMD_CH0	0x10
#define	SETUP_REG_CMD_CH1	0x11




//---CLK=1/2/3/4 Mhz
#define CLK_REG_DATA_20		0x18	//CLK=1/2/3/4 Mhz
#define CLK_REG_DATA_25		0x19	//CLK=1/2/3/4 Mhz
#define CLK_REG_DATA_100	0x1A	//CLK=1/2/3/4 Mhz
#define CLK_REG_DATA_200	0x1B	//CLK=1/2/3/4 Mhz //working

//---CLK=2.4576 or 4.9152 Mhz
#define CLK_REG_DATA_50		0x1C	//CLK=2.4576 or 4.9152 Mhz
#define CLK_REG_DATA_500	0x1F	//CLK=2.4576 or 4.9152 Mhz


//#define	SETUP_REG_DATA		0x44       //0x44 //0x4C(gain 1 Vref=1.22(65,000 count)  ----- 0x44(gain 2 Vref=2.55(65,000 count)
#define	SETUP_REG_DATA		0x44

//#define	SETUP_REG_DATA		0x46	//working
//#define	SETUP_REG_DATA		0x84
//#define	SETUP_REG_DATA		0x86









#define CH0_BSY_CHK			(uint8_t)0x08
#define CH1_BSY_CHK			(uint8_t)0x09

#define CURR_BSY_CHK		CH1_BSY_CHK
#define VOLT_BSY_CHK		CH0_BSY_CHK

#define CH0_DATA_READ		0x38	// for voltage reading	//but according to schematic and AD7705 register setting this channel-0(AIN1+) is for current reading
#define CH1_DATA_READ		0x39	// for current reading	//but according to schematic and AD7705 register setting this channel-1(AIN2+) is for voltage reading

#define CURR_DATA_READ		CH1_DATA_READ
#define VOLT_DATA_READ		CH0_DATA_READ




#define SETUP_REG  0x44
#define ADC_WAIT_TIME()	  __NOP();//(Delay_us(150))//150




//Inline functions
#define Set_ADC_SCLK_HIGH()	HAL_GPIO_WritePin(ADC_DAC_SCK_GPIO_Port, ADC_DAC_SCK_Pin, SET)
#define Set_ADC_SCLK_LOW()	HAL_GPIO_WritePin(ADC_DAC_SCK_GPIO_Port, ADC_DAC_SCK_Pin, RESET)

#define Set_ADC_CS_HIGH()	HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, SET)
#define Set_ADC_CS_LOW()	HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, RESET)

#define Set_ADC_DIN_HIGH()	HAL_GPIO_WritePin(ADC_DAC_DIN_GPIO_Port, ADC_DAC_DIN_Pin, SET)
#define Set_ADC_DIN_LOW()	HAL_GPIO_WritePin(ADC_DAC_DIN_GPIO_Port, ADC_DAC_DIN_Pin, RESET)


#define VOLT_BUFF_SIZE    20
extern float FloatVoltBuff[VOLT_BUFF_SIZE];
extern float FloatCurrBuff[VOLT_BUFF_SIZE];



extern unsigned int  voltage;
extern unsigned int  current;

extern UART_HandleTypeDef huart1;

extern uint16_t ADC_Scan_Delay;
extern uint8_t flag;





extern volatile unsigned char adc_state;

#define NUM_SAMPLES 100  // Number of ADC samples for averaging

extern float volt_samples[NUM_SAMPLES];
extern float curr_samples[NUM_SAMPLES];
extern float voltage_sum;
extern float avg_voltage;
extern float avg_current;



//************************* SAGAR **************************

#define COMM_REG       (0x00 << 4)
#define SETUPP_REG      (0x01 << 4)
#define CLOCK_REG      (0x02 << 4)
#define DATA_REG       (0x03 << 4)
#define TEST_REG       (0x04 << 4)
#define OFFSET_REG     (0x06 << 4)
#define GAIN_REG       (0x07 << 4)

#define CURR_CHANNEL   0x00
#define VOLT_CHANNEL   0x01

#define ADC_WRITE      (0x00 << 3)
#define ADC_READ       (0x01 << 3)

// Setup Register Fields
#define NORMAL_MODE    (0x00 << 6)
#define SELF_CALIB     (0x01 << 6)
#define ZERO_SCALE_CAL (0x02 << 6)
#define FULL_SCALE_CAL (0x03 << 6)

#define GAIN_1         (0x00 << 3)
#define GAIN_2         (0x01 << 3)
#define GAIN_4         (0x02 << 3)
#define GAIN_8         (0x03 << 3)
#define GAIN_16        (0x04 << 3)
#define GAIN_32        (0x05 << 3)
#define GAIN_64        (0x06 << 3)
#define GAIN_128       (0x07 << 3)

#define UNIPOLAR       (0x01 << 2)
#define BIPOLAR        (0x00 << 2)

#define BUFFER_ENABLE  (0x01 << 1)
#define BUFFER_DISABLE (0x00 << 1)

#define FSYNC_ENABLE   (0x01 << 0)
#define FSYNC_DISABLE  (0x00 << 0)

// Clock Register Fields
#define CLKDIS_ENABLE  (0x01 << 4)
#define CLKDIS_DISABLE (0x00 << 4)

#define CLKDIV_1       (0x01 << 3)
#define CLKDIV_0       (0x00 << 3)

#define CLK_HIGH_FREQ  (0x01 << 2)
#define CLK_LOW_FREQ   (0x00 << 2)

#define FS_20HZ        (0x00 << 0)
#define FS_25HZ        (0x01 << 0)
#define FS_100HZ       (0x02 << 0)
#define FS_200HZ       (0x03 << 0)

#define FS_50HZ        (0x00 << 0)
#define FS_60HZ        (0x01 << 0)
#define FS_250HZ       (0x02 << 0)
#define FS_500HZ       (0x03 << 0)

//************************* SAGAR **************************

//uint16_t data_reg = 0;
//uint8_t comm_reg = 0;
//
//
//char status = 0;

//void Delay_us( uint8_t time_us );
void adc_write(uint8_t cmd);
void ADCSend(uint8_t Register, uint8_t Channel, uint8_t NextOperation);

//uint8_t adc_read_comm_reg();
uint8_t adc_read_comm_reg(uint8_t cmd);

uint8_t drdy_read();

//uint16_t adc_read_data_reg();
uint16_t adc_read_data_reg(uint8_t cmd);

//uint8_t ADC_CH0_Is_Busy();
//uint8_t ADC_CH1_Is_Busy();
void adc_init();

void Write_ADC_Data(unsigned char data);
unsigned char Read_ADC_Data(void);
unsigned int Read_ADC_Output(void);
unsigned char ADC_CH0_Is_Busy(void);
unsigned char ADC_CH1_Is_Busy(void);
//void scan_adc(void);
void ADC_Config(void);
void ExtADCScan(void);




bool DataReady(uint8_t Channel);
void ADC_Write(uint8_t cmd);
uint8_t Read8BitData();
uint16_t Read16BitData();
uint32_t Read24BitData();
void SetNextOperation( uint8_t NextOperation, uint8_t Channel, uint8_t Register);
void ConfigureSetupRegister(uint8_t mode, uint8_t gain, uint8_t bipolar, uint8_t buffer, uint8_t fsync);
void ConfigureClockRegister(uint8_t clkdis, uint8_t clkdiv, uint8_t clk, uint8_t fs);
void ADC_Reset();
void ADC_Init();
void scan_adc();

#endif /* INC_EXTERNALADC_H_ */
