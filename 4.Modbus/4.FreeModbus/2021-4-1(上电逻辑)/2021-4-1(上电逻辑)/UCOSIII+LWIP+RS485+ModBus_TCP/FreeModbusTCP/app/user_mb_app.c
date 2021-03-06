/*
 * FreeModbus Libary: user callback functions and buffer define in slave mode
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: user_mb_app.c,v 1.60 2013/11/23 11:49:05 Armink $
 */
#include "user_mb_app.h"
#include "mbtcp.h"

/*------------------------Slave mode use these variables----------------------*/
//Slave mode:DiscreteInputs variables
//************************************************************************//
//**02功能码 
//**02 Read Discrete Inputs (1x)
//**起始地址 10000
//************************************************************************//
USHORT   usSDiscInStart                               = S_DISCRETE_INPUT_START;
#if S_DISCRETE_INPUT_NDISCRETES%8
UCHAR    ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES/8+1];
#else
UCHAR    ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES/8]  ;
#endif

//Slave mode:Coils variables
//************************************************************************//
//**01 05 15功能码
//**01 Read  Coils（0x）  
//**05 Write Single Coil
//**15 Write Multiple Coils
//**起始地址 00000
//************************************************************************//
USHORT   usSCoilStart                                 = S_COIL_START;
#if S_COIL_NCOILS%8
UCHAR    ucSCoilBuf[S_COIL_NCOILS/8+1]                ;
#else
UCHAR    ucSCoilBuf[S_COIL_NCOILS/8]                  ;
#endif

//Slave mode:InputRegister variables
//************************************************************************//
//**04功能码 (3x)
//**04 Read Input Registers
//**起始地址 30000
//************************************************************************//
USHORT   usSRegInStart                                = S_REG_INPUT_START;
USHORT   usSRegInBuf[S_REG_INPUT_NREGS]               ;

//Slave mode:HoldingRegister variables
//************************************************************************//
//**03 06 16功能码 
//**03 Read  Holding Registers(4x)
//**06 Write Single Registers
//**16 Write Multiple Registers
//**起始地址 40000
//************************************************************************//
USHORT   usSRegHoldStart                              = S_REG_HOLDING_START;
USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS]           ;

/******************************输入寄存器回调函数****************************************************
*	函数定义: eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )	*
*	描    述：输入寄存器相关的功能（读、连续读）												  	*		
*	入口参数：pucRegBuffer : 回调函数将Modbus寄存器的当前值写入的缓冲区							  	*
*			  usAddress    : 寄存器的起始地址，输入寄存器的地址范围是1-65535。					  	*
*			  usNRegs      : 寄存器数量															  	*
*	出口参数：eMBErrorCode : 这个函数将返回的错误码												  	*
*   功 能 码：0x04 -> eMBFuncReadInputRegister
*	备    注：Editor：豪 2020-11-13																  	*
****************************************************************************************************/
eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex;
    USHORT *        pusRegInputBuf;
    USHORT          REG_INPUT_START;
    USHORT          REG_INPUT_NREGS;
    USHORT          usRegInStart;

    pusRegInputBuf = usSRegInBuf;
    REG_INPUT_START = S_REG_INPUT_START;
    REG_INPUT_NREGS = S_REG_INPUT_NREGS;
    usRegInStart = usSRegInStart;
	

    /* 它已经在modbus功能方法中加了一个。*/
    usAddress--;//FreeModbus功能函数中已经加，为保证与缓冲区首地址一直，故减1

	//查询是否存在寄存器范围内
	//为了避免警告，修改为有符号整数
    if ((usAddress >= REG_INPUT_START)
            && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
    {
		//获得操作偏移量，本次操作起始地址-输入寄存器的初始地址
        iRegIndex = usAddress - usRegInStart;
        while (usNRegs > 0)
        {
			//赋值高字节
            *pucRegBuffer++ = (UCHAR) (pusRegInputBuf[iRegIndex] >> 8);
			//赋值高字节
            *pucRegBuffer++ = (UCHAR) (pusRegInputBuf[iRegIndex] & 0xFF);
			//赋值高字节
            iRegIndex++;
			//被操作寄存器数量递减
            usNRegs--;
        }
    }
    else
    {
		//返回错误状态，无寄存器
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

/******************************保持寄存器回调函数****************************************************
*	函数定义: eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress,					*
*        									USHORT usNRegs, eMBRegisterMode eMode)					*
*	描    述：保持寄存器相关的功能（读、连续读、写、连续写）										*		
*	入口参数：pucRegBuffer : 如果需要更新用户寄存器数值，这个缓冲区必须指向新的寄存器数值。			*
*			  				 如果协议栈想知道当前的数值，回调函数必须将当前值写入这个缓冲区			*
*			  usAddress    : 寄存器的起始地址。					  									*	
*			  usNRegs      : 寄存器数量															  	*
*	出口参数：eMBErrorCode : 如果该参数为eMBRegisterMode::MB_REG_WRITE，							*
*							 用户的应用数值将从pucRegBuffer中得到更新。								*
*							 如果该参数为eMBRegisterMode::MB_REG_READ，								*
*							 用户需要将当前的应用数据存储在pucRegBuffer中							*
*	功 能 码：0x06 -> eMBFuncWriteHoldingRegister													*
*			  0x16 -> eMBFuncWriteMultipleHoldingRegister											*
*             0x03 -> eMBFuncReadHoldingRegister													*
*             0x23 -> eMBFuncReadWriteMultipleHoldingRegister										*
*	备    注：Editor：豪 2020-11-13																  	*
****************************************************************************************************/
eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNRegs, eMBRegisterMode eMode)
{
	//错误状态
    eMBErrorCode    eStatus = MB_ENOERR;
	//偏移量
    USHORT          iRegIndex;
    USHORT *        pusRegHoldingBuf;
    USHORT          REG_HOLDING_START;
    USHORT          REG_HOLDING_NREGS;
    USHORT          usRegHoldStart;

    pusRegHoldingBuf = usSRegHoldBuf;
    REG_HOLDING_START = S_REG_HOLDING_START;
    REG_HOLDING_NREGS = S_REG_HOLDING_NREGS;
    usRegHoldStart = usSRegHoldStart;
	
    usAddress--;//FreeModbus功能函数中已经加，为保证与缓冲区首地址一直，故减1
	
	//判断寄存器是不是在范围内
    if ((usAddress >= REG_HOLDING_START)
            && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
		//计算偏移量
        iRegIndex = usAddress - usRegHoldStart;
        switch (eMode)
        {

			/* 将当前寄存器的值传递到协议栈。*/
        case MB_REG_READ:
            while (usNRegs > 0)
            {
                *pucRegBuffer++ = (UCHAR) (pusRegHoldingBuf[iRegIndex] >> 8);
                *pucRegBuffer++ = (UCHAR) (pusRegHoldingBuf[iRegIndex] & 0xFF);
                iRegIndex++;
                usNRegs--;
            }
            break;
			
		/* 使用协议栈中的新值更新当前寄存器值。*/    
        case MB_REG_WRITE:
            while (usNRegs > 0)
            {
                pusRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                pusRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    }
    else
    {
		//返回错误状态
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/**************************线圈状态寄存器回调函数****************************************************
*	函数定义: eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress,					*
*        								USHORT usNCoils, eMBRegisterMode eMode)						*
*	描    述：线圈状态寄存器相关的功能（读、连续读、写、连续写）									*		
*	入口参数：pucRegBuffer : 位组成一个字节，起始寄存器对应的位处于该字节pucRegBuffer的最低位LSB。	*
*			  				 如果回调函数要写这个缓冲区，											*	
*							 没有用到的线圈（例如不是8个一组的线圈状态）对应的位的数值必须设置位0。	*
*			  usAddress    : 第一个线圈地址					  										*	
*			  usNCoils     : 请求的线圈个数															*
*	出口参数：eMBErrorCode : 如果该参数为eMBRegisterMode::MB_REG_WRITE，							*
*							 用户的应用数值将从pucRegBuffer中得到更新。								*
*							 如果该参数为eMBRegisterMode::MB_REG_READ，								*
*							 用户需要将当前的应用数据存储在pucRegBuffer中							*
*	功 能 码：0x01 -> eMBFuncReadCoils																*	
*             0x05 -> eMBFuncWriteCoil																*
*             0x15 -> 写多个线圈 eMBFuncWriteMultipleCoils											*
*	备    注：Editor：豪 2020-11-13																  	*
****************************************************************************************************/
eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress,USHORT usNCoils, eMBRegisterMode eMode)
{
	//错误状态
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex , iRegBitIndex , iNReg;
    UCHAR *         pucCoilBuf;
    USHORT          COIL_START;
    USHORT          COIL_NCOILS;
    USHORT          usCoilStart;
    iNReg =  usNCoils / 8 + 1;	//占用寄存器数量

    pucCoilBuf = ucSCoilBuf;
    COIL_START = S_COIL_START;
    COIL_NCOILS = S_COIL_NCOILS;
    usCoilStart = usSCoilStart;

    usAddress--;//FreeModbus功能函数中已经加1，为保证与缓冲区首地址一致，故减1
	
	//检查寄存器是否在指定范围内
    if( ( usAddress >= COIL_START ) && ( usAddress + usNCoils <= COIL_START + COIL_NCOILS ) )
    {
		//检查寄存器是否在指定范围内
        iRegIndex = (USHORT) (usAddress - usCoilStart) / 8;
        iRegBitIndex = (USHORT) (usAddress - usCoilStart) % 8;
        switch ( eMode )
        {
        /* 将当前寄存器的值传递到协议栈。*/
        case MB_REG_READ:
            while (iNReg > 0)
            {
                *pucRegBuffer++ = xMBUtilGetBits(&pucCoilBuf[iRegIndex++],iRegBitIndex, 8);
                iNReg--;
            }
            pucRegBuffer--;
            usNCoils = usNCoils % 8; 						//余下的线圈数
            *pucRegBuffer = *pucRegBuffer << (8 - usNCoils);//高位补零
            *pucRegBuffer = *pucRegBuffer >> (8 - usNCoils);
            break;

            /* 使用协议栈中的新值更新当前寄存器值。*/ 
        case MB_REG_WRITE:
            while (iNReg > 1)
            {
                xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, 8,
                        *pucRegBuffer++);
                iNReg--;
            }
            usNCoils = usNCoils % 8;						//余下的线圈数
            if (usNCoils != 0)								//xMBUtilSetBits方法 在操作位数量为0时存在
            {
                xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, usNCoils,
                        *pucRegBuffer++);
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/**************************离散输入寄存器回调函数****************************************************
*	函数定义: eMBErrorCode eMBRegDiscreteCB(UCHAR* pucRegBuffer,USHORT usAddress,USHORT usNDiscrete)*
*	描    述：离散输入寄存器相关的功能（读、连续读）												*		
*	入口参数：pucRegBuffer : 用当前的线圈数据更新这个寄存器，										*
*							 起始寄存器对应的位处于该字节pucRegBuffer的最低位LSB。					*
*			  				 如果回调函数要写这个缓冲区，											*	
*							 没有用到的线圈（例如不是8个一组的线圈状态）对应的位的数值必须设置为0。	*
*			  usAddress    : 离散输入的起始地址				  										*	
*			  usNDiscrete  : 离散输入点数量															*
*	出口参数：eMBErrorCode : 这个函数将返回的错误码													*
*	功 能 码：0x02 -> eMBFuncReadDiscreteInputs														*
*	备    注：Editor：豪 2020-11-13																  	*
****************************************************************************************************/
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex , iRegBitIndex , iNReg;
    UCHAR *         pucDiscreteInputBuf;
    USHORT          DISCRETE_INPUT_START;
    USHORT          DISCRETE_INPUT_NDISCRETES;
    USHORT          usDiscreteInputStart;
    iNReg =  usNDiscrete / 8 + 1;//占用寄存器数量

    pucDiscreteInputBuf = ucSDiscInBuf;
    DISCRETE_INPUT_START = S_DISCRETE_INPUT_START;
    DISCRETE_INPUT_NDISCRETES = S_DISCRETE_INPUT_NDISCRETES;
    usDiscreteInputStart = usSDiscInStart;


    usAddress--;//FreeModbus功能函数中已经加1，为保证与缓冲区首地址一致，故减1

    if ((usAddress >= DISCRETE_INPUT_START)
            && (usAddress + usNDiscrete    <= DISCRETE_INPUT_START + DISCRETE_INPUT_NDISCRETES))
    {
        iRegIndex = (USHORT) (usAddress - usDiscreteInputStart) / 8;//每个寄存器存8个
        iRegBitIndex = (USHORT) (usAddress - usDiscreteInputStart) % 8;//相对于寄存器内部的位地址

        while (iNReg > 0)
        {
            *pucRegBuffer++ = xMBUtilGetBits(&pucDiscreteInputBuf[iRegIndex++],
                    iRegBitIndex, 8);
            iNReg--;
        }
        pucRegBuffer--;
        usNDiscrete = usNDiscrete % 8;//余下的线圈数
        *pucRegBuffer = *pucRegBuffer << (8 - usNDiscrete);//高位补零
        *pucRegBuffer = *pucRegBuffer >> (8 - usNDiscrete);
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

