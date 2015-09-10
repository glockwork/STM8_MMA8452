#ifndef __MMA8452__H
#define __MMA8452__H
//=============UserDefine========================
#define   uchar unsigned char
#define   uint unsigned int

#define	SlaveAddress   0x38	//定义器件在IIC总线中的从地址,根据SA0地址引脚不同修改
//=============RegisterAddress========================
#define CTRL_REG1        0x2A //系统控制1寄存器
#define CTRL_REG3        0x2c
#define CTRL_REG4        0x2D //系统控制4寄存器
#define CTRL_REG5        0x2E //系统控制5寄存器
#define INT_SOURCE       0x0C 
#define FF_MT_CFG        0x15
#define FF_MT_THS        0X17
#define FF_MT_SRC        0X16
#define FF_MT_COUNT      0X18
#define STATUS 0x00

#define STATUS_ZYXDR 0X08
#define INT_SOURCE_SRC_DRDY 0x01
#define INT_SOURCE_SRC_FF_MT 0x04
#define CTRL_REG1_ACTIVE 0X01
#define CTRL_REG1_FREAD 0X02
#define CTRL_REG4_INT_EN_DRDY 0x01
#define CTRL_REG4_INT_EN_FF_MT 0X04
#define CTRL_REG5_INT_CFG_DRDY 0x01
#define CTRL_REG5_INT_CFG_FF_MT 0x04
#define Output_Data_Rates_800 0x00
#define Output_Data_Rates_400 0x08
#define Output_Data_Rates_200 0x10
#define Output_Data_Rates_100 0x18
#define Output_Data_Rates_50 0x20
#define Output_Data_Rates_12_5 0x28
#define Output_Data_Rates_6_25 0x30
#define Output_Data_Rates_1_563 0x38
#define FF_MT_CFG_EFE           0X80
#define FF_MT_CFG_OAE           0X40
#define FF_MT_CFG_ZEFE          0X20
#define FF_MT_CFG_YEFE          0X10
#define FF_MT_CFG_XEFE          0X08



#define setOutput_8Bit setOutputBit(0)
#define setOutput_12Bit setOutputBit(1)
//================FunctionsDefine================
void setOutputBit(unsigned char state);
void writeI2C(unsigned char slaveAddress,unsigned char regAddress,unsigned char *point,unsigned char len);
void readI2C(unsigned char slaveAddress,unsigned char regAddress,unsigned char *point,unsigned char len);
void MMA845x_Output_Data_Rates(unsigned char rates);
void MMA8451_GPIO_Config(void);
unsigned char Single_Read_MMA8452(uchar REG_Address);
void Single_Write_MMA8452(uchar REG_Address,uchar REG_data);
void Multiple_Read_MMA8452(void);
void Init_MMA8452(void);
int Get_x(void);
int Get_y(void);
int Get_z(void);
void MMA845x_Standby(void);
void MMA845x_Active(void);




#endif
