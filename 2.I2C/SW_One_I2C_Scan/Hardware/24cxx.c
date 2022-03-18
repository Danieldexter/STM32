#include "24cxx.h"

/**
 * @brief AT24CXX IO ��ʼ��
 * @retval none
 * @author halo
 * @date 2022-03-18
 */
void AT24CXX_Init(void)
{
	I2C_Init();
}

/**
 * @brief ��AT24CXXָ����ַ����һ������
 *		  ReadAddr:��ʼ�����ĵ�ַ  
 * @retval temp ����������
 * @author halo
 * @date 2022-03-18
 */
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;		  	    																 
    I2C_Start();  
	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0XA0);	   //����д����
		I2C_Wait_Ack();
		I2C_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ	    
	}else I2C_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	   
	I2C_Wait_Ack(); 
    I2C_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	I2C_Wait_Ack();	    
	I2C_Start();  	 	   
	I2C_Send_Byte(0XA1);           //�������ģʽ			   
	I2C_Wait_Ack();	 
    temp=I2C_Read_Byte(0);		   
    I2C_Stop();//����һ��ֹͣ����	    
	return temp;
}

/**
 * @brief ��AT24CXXָ����ַд��һ������
 *		  WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ 
 *		  DataToWrite:Ҫд�������
 * @retval none
 * @author halo
 * @date 2022-03-18
 */
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
    I2C_Start();  
	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0XA0);	    //����д����
		I2C_Wait_Ack();
		I2C_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ	  
	}else I2C_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	I2C_Wait_Ack();	   
    I2C_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	I2C_Wait_Ack(); 	 										  		   
	I2C_Send_Byte(DataToWrite);     //�����ֽ�							   
	I2C_Wait_Ack();  		    	   
    I2C_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

/**
 * @brief ���AT24CXX�Ƿ�����
 *		  ��������24XX�����һ����ַ(255)���洢��־��.
 *		  ���������24Cϵ��,�����ַҪ�޸�
 *		  WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ 
 *		  DataToWrite:Ҫд�������
 * @retval 1:���ʧ��
		   0:���ɹ�
 * @author halo
 * @date 2022-03-18
 */
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

