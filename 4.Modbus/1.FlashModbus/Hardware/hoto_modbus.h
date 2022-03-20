#ifndef __HOTO_MODBUS_H__
#define __HOTO_MODBUS_H__

#include "main.h"
#include "hoto_flash.h"
#include "crc.h"

#define Modbus_Tx_BufferSize  100

#define    MBSave_0    0x0001
#define    MBSave_1    0x0002
#define    MBSave_2    0x0004
#define    MBSave_3    0x0008
#define    MBSave_4    0x0010
#define    MBSave_5    0x0020
#define    MBSave_6    0x0040
#define    MBSave_7    0x0080
#define    MBSave_8    0x0100
#define    MBSave_9    0x0200
#define    MBSave_10   0x0400
#define    MBSave_11   0x0800
#define    MBSave_12   0x1000
#define    MBSave_13   0x2000
#define    MBSave_14   0x4000
#define    MBSave_15   0x8000

#define    RST_1   0xfffe
#define    RST_2   0xfffd
#define    RST_3   0xfffb
#define    RST_4   0xfff7
#define    RST_5   0xffef
#define    RST_6   0xffdf
#define    RST_7   0xffbf
#define    RST_8   0xff7f
#define    RST_9   0xfeff
#define    RST_10  0xfdff
#define    RST_11  0xfbff
#define    RST_12  0xf7ff
#define    RST_13  0xefff
#define    RST_14  0xdfff
#define    RST_15  0xbfff
#define    RST_16  0x7fff

#define    INT_ZERO 32768

typedef enum {
    MB_FUNC_CODE_INVALID =0,
    MB_FUNC_CODE_READ_DISPERS_INPUT=2,//∂¡ ‰»Î¿Î…¢¡ø
    MB_FUNC_CODE_READ_COILS=1,//
    MB_FUNC_CODE_WRITE_SINGLE_COIL=5,
    MB_FUNC_CODE_WRITE_MULTI_COIL=15,
    MB_FUNC_CODE_READ_INPUT_REG=4,
    MB_FUNC_CODE_READ_HOLD_REG=3,
    MB_FUNC_CODE_WRITE_SINGLE_HOLD_REG=6,
    MB_FUNC_CODE_WRITE_MULTI_REG=16,
    MB_FUNC_CODE_RDWR_MULTI_REG=23,
    MB_FUNC_CODE_WRITE_MASK_REG=22,

} MB_FunctionCode;
void HOTO_Flash_Init(void);
void Modbus_Init(void);
void Modbus_Even(void);

#endif
