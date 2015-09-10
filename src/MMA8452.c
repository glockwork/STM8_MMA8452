#include "MMA8452.h"
#include <iostm8s003f3.h>
#include <stdio.h>
//==============一些变量的定义=========================
char BUF[3];        //接收数据缓存区 
//=========================================
//功能：用于IIC协议单字节写入
//输入：无
//输出：无
//返回：无
//=========================================
void Single_Write_MMA8452(uchar REG_Address,uchar REG_data)
{	
    unsigned char temp;
    
    while(I2C_SR3_BUSY);          //检测总线是否空闲
    I2C_CR2_START = 1;            //产生起始信号
    while(!I2C_SR1_SB);           //查询起始位是否已发送
    
    I2C_DR = SlaveAddress;         //发送写地址,写DR将清除I2C_SR1_SB待验证
    while(!I2C_SR1_ADDR);		  //查询地址是否发送	
    temp = I2C_SR1;
    temp = I2C_SR3;               //清除I2C_SR1_ADDR
    
    I2C_DR = REG_Address;         //写数据
    while(!(I2C_SR1&0x84));       //判断位移寄存器是否为空与字节发送完成
    
    I2C_DR = REG_data;		      //写数据
    while(!(I2C_SR1&0x84));       //判断位移寄存器是否为空与字节发送完成
    
    I2C_CR2_STOP = 1;             //待验证
}
//=========================================
//功能：用于IIC协议单字节读取
//输入：无
//输出：无
//返回：无
//=========================================
unsigned char Single_Read_MMA8452(uchar REG_Address)
{	
    unsigned char temp;
    
    while(I2C_SR3_BUSY);          //检测总线是否空闲
    I2C_CR2_START = 1;            //产生起始信号
    while(!I2C_SR1_SB);           //查询起始位是否已发送
    
    I2C_DR = SlaveAddress;         //发送写地址,写DR将清除I2C_SR1_SB待验证
    while(!I2C_SR1_ADDR);		  //查询地址是否发送	
    temp = I2C_SR1;
    temp = I2C_SR3;               //清除I2C_SR1_ADDR
    
    I2C_DR = REG_Address;         //写数据
    while(!(I2C_SR1&0x84));       //判断位移寄存器是否为空与字节发送完成
    
    I2C_CR2_START = 1;            //产生起始信号
    while(!I2C_SR1_SB);
    
    I2C_DR = SlaveAddress+1;        //发送写地址,写DR将清除I2C_SR1_SB待验证
    while(!I2C_SR1_ADDR);		   //查询地址是否发送	
    temp = I2C_SR1;
    temp = I2C_SR3;                //清除I2C_SR1_ADDR
    
    I2C_CR2_ACK = 0;
    
    I2C_CR2_STOP = 1;
    I2C_CR2_POS = 0;
    
    while(!(I2C_SR1&0x40));
    temp =I2C_DR;          
    
    I2C_CR2_ACK = 1;  //开启应答
    
    return temp;
}
//=========================================
//功能：连续读出MMA8452内部加速度数据，地址范围0x01~0x06
//输入：无
//输出：无
//返回：无
//=========================================
void Multiple_Read_MMA8452(void)
{   
    /*uchar i;
    
    while(I2C_SR3_BUSY);          //检测总线是否空闲
    I2C_CR2_START = 1;            //产生起始信号
    while(!I2C_SR1_SB);           //查询起始位是否已发送
    
    I2C_DR = SlaveAddress;         //发送写地址,写DR将清除I2C_SR1_SB待验证
    while(!I2C_SR1_ADDR);		  //查询地址是否发送	
    i = I2C_SR1;
    i = I2C_SR3;               //清除I2C_SR1_ADDR
    
    I2C_DR = 0x01;                //写数据
    while(!(I2C_SR1&0x84)); 
    
    I2C_CR2_START = 1;            //产生起始信号
    while(!I2C_SR1_SB);
    
    I2C_DR = SlaveAddress+1;        //发送写地址,写DR将清除I2C_SR1_SB待验证
    while(!I2C_SR1_ADDR);		   //查询地址是否发送	
    i = I2C_SR1;
    i = I2C_SR3;                //清除I2C_SR1_ADDR
    
    for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
    if (i == 5){ //设置关闭应答
    I2C_CR2_ACK = 0;
    I2C_CR2_STOP = 1;
}
    while(!(I2C_SR1&0x40));
    BUF[i] =I2C_DR;          
}
    
    I2C_CR2_ACK = 1;  //开启应答*/
    
    readI2C(SlaveAddress,0x01,BUF,6);
    
}
void InterruptTestDome(void){
    Single_Write_MMA8452(0x2B,0x02);   //
    Single_Write_MMA8452(CTRL_REG1,(Single_Read_MMA8452(CTRL_REG1)& ~CTRL_REG1_FREAD));
    MMA845x_Output_Data_Rates(Output_Data_Rates_1_563);
    Single_Write_MMA8452(CTRL_REG4, CTRL_REG4_INT_EN_DRDY);
    Single_Write_MMA8452(CTRL_REG5, CTRL_REG5_INT_CFG_DRDY);
}
//=========================================
//功能：初始化MMA8451
//输入：无
//输出：无
//返回：无
//=========================================
void Init_MMA8452(void)
{
    unsigned char temp;
    
    //设置休眠
    MMA845x_Standby();
    
    /* 设置采样频率为100Hz */
    MMA845x_Output_Data_Rates(Output_Data_Rates_400);
    
    setOutput_8Bit;
    
    readI2C(SlaveAddress,CTRL_REG4,&temp,1);
    temp |= CTRL_REG4_INT_EN_DRDY;
    writeI2C(SlaveAddress,CTRL_REG4,&temp,1);
    
    readI2C(SlaveAddress,CTRL_REG5,&temp,1);
    temp |= CTRL_REG5_INT_CFG_DRDY;
    writeI2C(SlaveAddress,CTRL_REG5,&temp,1);
    
    /* 将设备切换到主动模式 */
    MMA845x_Active();
}
////========得到X轴的数据========================================
//int Get_x(void)
//{   
//    dis_data = (BUF[0]<<8)|BUF[1];  //合成数据   
//    dis_data >>= 4;					//数据合成，OUT_X_MSB :[7-0],OUT_X_LSB:[7-2],故合成后的数据需要右移两位
//    /*	if(dis_data&0x800){
//    dis_data |= 0xf800;
//}
//    x = (float)dis_data;
//    x /= 1024;
//    x *= 9.8;
//    dis_data = x;*/
//    
//    return dis_data;
//}
//
////========得到Y轴的数据====================================
//int Get_y(void)
//{   
//    dis_data = (BUF[2]<<8)|BUF[3];  //合成数据   
//    dis_data >>= 4;					//数据合成，OUT_X_MSB :[7-0],OUT_X_LSB:[7-2],故合成后的数据需要右移两位
//    /*	if(dis_data&0x800){
//    dis_data |= 0xf800;
//}
//    x = (float)dis_data;
//    x /= 1024;
//    x *= 9.8;
//    //dis_data = x;*/
//    
//    return dis_data;
//}
////========得到Z轴的数据====================================
//int Get_z(void)
//{  
//    dis_data = (BUF[4]<<8)|BUF[5];  //合成数据   
//    dis_data >>= 4;					//数据合成，OUT_X_MSB :[7-0],OUT_X_LSB:[7-2],故合成后的数据需要右移两位
//    if(dis_data&0x800){
//        dis_data |= 0xf800;
//    }
//    /*x = (float)dis_data;
//    x /= 1024;
//    x *= 9.8;
//    //dis_data = x;*/
//    
//    return dis_data;
//}
//==============END OF FILE=================================
void MMA845x_Standby(void){
    unsigned char temp;
    temp = Single_Read_MMA8452(CTRL_REG1)&~CTRL_REG1_ACTIVE;
    writeI2C(SlaveAddress,CTRL_REG1,&temp,1);
    //Single_Write_MMA8452(CTRL_REG1,temp&~CTRL_REG1_ACTIVE);
}
void MMA845x_Active(void){
    unsigned char temp;
    temp = Single_Read_MMA8452(CTRL_REG1);
    Single_Write_MMA8452(CTRL_REG1,temp|CTRL_REG1_ACTIVE);
}
void MMA845x_Output_Data_Rates(unsigned char rates){
    unsigned char temp = Single_Read_MMA8452(CTRL_REG1)&0xc3;
    temp |= rates;
    Single_Write_MMA8452(CTRL_REG1,rates);
}
void writeI2C(unsigned char slaveAddress,unsigned char regAddress,unsigned char *point,unsigned char len){
    unsigned char temp;
    
    /* 检测总线是否空闲 */
    while(I2C_SR3_BUSY);         
    
    /* 产生起始信号 */
    I2C_CR2_START = 1; 
    
    /* 查询起始位是否已发送 */           
    while(!I2C_SR1_SB);
    
    /* 发送写地址,写DR将清除I2C_SR1_SB */
    I2C_DR = slaveAddress; 
    
    /* 查询地址是否发送 */
    while(!I2C_SR1_ADDR);
    
    /* 作用清除I2C_SR1_ADDR */		 	
    temp = I2C_SR1;
    temp = I2C_SR3;               
    
    /*写数据 */
    I2C_DR = regAddress;
    
    /* 判断位移寄存器是否为空与字节发送完成 */
    while(!(I2C_SR1&0x84));      
    
    for(;len>0;len--){
        /*写数据 */
        I2C_DR = *point;	
        point ++;
        
        /* 判断位移寄存器是否为空与字节发送完成 */		      	
        while(!(I2C_SR1&0x84));       //
    }
    
    /* 发送结束位 */
    I2C_CR2_STOP = 1;
}
void readI2C(unsigned char slaveAddress,unsigned char regAddress,unsigned char *point,unsigned char len)
{   
    unsigned char i;
    
    /* 检测总线是否空闲 */
    while(I2C_SR3_BUSY);   
    
    /* 产生起始信号 */
    I2C_CR2_START = 1;            
    
    /* 查询起始位是否已发送 */
    while(!I2C_SR1_SB);           
    
    /* 发送写地址,写DR将清除I2C_SR1_SB */
    I2C_DR = slaveAddress;
    
    /* 查询地址是否发送 */
    while(!I2C_SR1_ADDR);
    
    /* 作用清除I2C_SR1_ADDR */
    i = I2C_SR1;
    i = I2C_SR3;               
    
    /* 写数据 */
    I2C_DR = regAddress;   
    
    /* 判断位移寄存器是否为空与字节发送完成 */
    while(!(I2C_SR1&0x84)); 
    
    /* 产生起始信号 */
    I2C_CR2_START = 1;
    
    /* 查询地址是否发送 */
    while(!I2C_SR1_SB);
    
    /* 发送写地址,写DR将清除I2C_SR1_SB */
    I2C_DR = slaveAddress+1;        
    
    /* 查询地址是否发送 */
    while(!I2C_SR1_ADDR);
    
    /* 作用清除I2C_SR1_ADDR */
    i = I2C_SR1;
    i = I2C_SR3;
    
    
    for (; len > 0; len --){
        /* 查询是否为组后一个 */
        if (len == 1){ 
            /* 设置无应答 */
            I2C_CR2_ACK = 0;
            /* 发送停止位 */
            I2C_CR2_STOP = 1;
        }
        
        /* 等待接收完成 */
        while(!(I2C_SR1&0x40));
        
        /* 保存接收数据 */
        *point =I2C_DR;
        point ++;
    }
    /* 开启应答 */
    I2C_CR2_ACK = 1;
}
void setOutputBit(unsigned char state){
    unsigned char temp;
    readI2C(SlaveAddress,CTRL_REG1,&temp,1);
    if(state)
        temp &= ~CTRL_REG1_FREAD; 
    else
        temp |= CTRL_REG1_FREAD;
    writeI2C(SlaveAddress,CTRL_REG1,&temp,1);
}


