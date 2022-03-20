#include "hoto_modbus.h"
#include <string.h>

u8 Flash_Save_Flag = 0;
u8 FLASH_COM_SAVE = 0;
u16  MB_Save_Flag[2];

//MODBUS�Ĵ�����ַ
union {
    u16 MODBUS_halfword[60];//MODBUS�Ĵ���
    u8  MODBUS_bye[120];//MODBUS�Ĵ���
} MODBUS_sbuf;   //ZI-data  �����ж����˵�û�г�ʼ����ȫ�ֱ����;�̬����

#define MODBUS_16w(x)    MODBUS_sbuf.MODBUS_halfword[x]
#define MODBUS_8w(x)     MODBUS_sbuf.MODBUS_bye[x]

#define    MB_ADDRESS	MODBUS_16w(0) //������ַ
#define    UART_BPS		MODBUS_16w(1) //������
#define    Version		MODBUS_16w(2) //�汾��			 
#define    Count_PMH	MODBUS_16w(3) //
#define    Pm_Data_H	MODBUS_16w(4) //
#define    Count_PML_4S	MODBUS_16w(5) //4S�����
#define    Pm_Data_Kxy	MODBUS_16w(6) //��������
#define    Pm_Data_K2M	MODBUS_16w(7) //���˷�	 
#define    Bezier_Data	MODBUS_16w(8) //����5min�µ����
#define    Avr_4S		MODBUS_16w(9)

#define    liner_X0		MODBUS_16w(10) //
#define    liner_X1		MODBUS_16w(11) //
#define    liner_X2		MODBUS_16w(12) //
#define    liner_X3		MODBUS_16w(13) //
#define    liner_X4		MODBUS_16w(14) //
#define    liner_X5		MODBUS_16w(15) //
#define    liner_X6		MODBUS_16w(16) //
#define    liner_X7		MODBUS_16w(17) //
#define    liner_X8		MODBUS_16w(18) //
#define    liner_X9		MODBUS_16w(19) //
#define    liner_X10	MODBUS_16w(20) //


#define    liner_Y0		MODBUS_16w(21) //
#define    liner_Y1		MODBUS_16w(22) //
#define    liner_Y2		MODBUS_16w(23) //
#define    liner_Y3		MODBUS_16w(24) //
#define    liner_Y4		MODBUS_16w(25) //
#define    liner_Y5		MODBUS_16w(26) //
#define    liner_Y6		MODBUS_16w(27) //
#define    liner_Y7		MODBUS_16w(28) //
#define    liner_Y8		MODBUS_16w(29) //
#define    liner_Y9		MODBUS_16w(30) //
#define    liner_Y10	MODBUS_16w(31) //

#define    liner_B0         MODBUS_16w(32) //�Ƿ�1S�Զ��ϴ��ı�־
#define    liner_B1         MODBUS_16w(33) //�źŵ�����
#define    liner_B2         MODBUS_16w(34) //�źŸ�����
#define    liner_B3         MODBUS_16w(35) //���г���
#define    liner_B4         MODBUS_16w(36) //�ӵڼ������п�ʼȡֵ
#define    liner_B5         MODBUS_16w(37) //�궨TVOCʱ���¶�ֵ
#define    liner_B6         MODBUS_16w(38) //�궨TVOCʱ��ʪ��ֵ
#define    liner_B7         MODBUS_16w(39) //ͻ������
#define    liner_B8         MODBUS_16w(40) //PMֵ��
#define    liner_B9         MODBUS_16w(41) //PMֵ��

#define    liner_CID0       MODBUS_16w(42) // 
#define    liner_CID1       MODBUS_16w(43) // 
#define    liner_CID2       MODBUS_16w(44) // 
#define    liner_CID3       MODBUS_16w(45) // 
#define    liner_CID4       MODBUS_16w(46) // 
#define    liner_CID5       MODBUS_16w(47) // 
#define    liner_CID6       MODBUS_16w(48) // 
#define    liner_CID7       MODBUS_16w(49) // 

#define    Mdatapm25		MODBUS_16w(55) //
#define    MdataCO2			MODBUS_16w(56) //
#define    MdataTVOC		MODBUS_16w(57) //
#define    MdataTEM			MODBUS_16w(58) //
#define    MdataSHI			MODBUS_16w(59) //

//flash�洢��ַΪż��,�����ַ
#define  SystemInit_bz_FLaddr  (FLASH_SAVE_ADDR+0)  //ϵͳ��ʼ����־
#define  liner_X0_add      (FLASH_SAVE_ADDR+10)
#define  liner_Y0_add      (FLASH_SAVE_ADDR+32)
#define  liner_B0_add      (FLASH_SAVE_ADDR+54)
#define  liner_CID0_add     (FLASH_SAVE_ADDR+74)

u8	g_Modbus_Tx_Buf[Modbus_Tx_BufferSize];//ZI-data  

/*
*********************************************************************************************************
*	�� �� ��: HOTO_Flash_Init
*	����˵��: HOTO Flash ��ʼ����
*	��    ��: ��
*	�� �� ֵ: ��
*	��	  �ģ�halo
*	��    �ڣ�2022.03.20
*********************************************************************************************************
*/
void HOTO_Flash_Init(void)
{
	u16 SystemInit_bz;//ϵͳ��ʼ��
    u16 Flash_Buf[40];

    STMFLASH_Read(SystemInit_bz_FLaddr,&SystemInit_bz,1);
    if(SystemInit_bz!=0x0610)
    {
        SystemInit_bz = 0x0610;
        STMFLASH_Write(SystemInit_bz_FLaddr, &SystemInit_bz, 1);

        Flash_Buf[0] = liner_X0 = 7100; //1
        Flash_Buf[1] = liner_X1 = 7200;//50
        Flash_Buf[2] = liner_X2 = 7300;//100
        Flash_Buf[3] = liner_X3 = 7450;//200
        Flash_Buf[4] = liner_X4 = 0;//200
        Flash_Buf[5] = liner_X5 = 0;//300
        Flash_Buf[6] = liner_X6 = 0;//400
        Flash_Buf[7] = liner_X7 = 0;//500
        Flash_Buf[8] = liner_X8 = 0;//600
        Flash_Buf[9] = liner_X9 = 0;//800
        Flash_Buf[10] = liner_X10 = 0;//1000

        Flash_Buf[11] = liner_Y0 = 0;
        Flash_Buf[12] = liner_Y1 = 50;
        Flash_Buf[13] = liner_Y2 = 100;
        Flash_Buf[14] = liner_Y3 = 200;
        Flash_Buf[15] = liner_Y4 = 0;
        Flash_Buf[16] = liner_Y5 = 0;
        Flash_Buf[17] = liner_Y6 = 0;
        Flash_Buf[18] = liner_Y7 = 0;
        Flash_Buf[19] = liner_Y8 = 0;
        Flash_Buf[20] = liner_Y9 = 0;
        Flash_Buf[21] = liner_Y10 = 0;

        Flash_Buf[22] = liner_B0 = 0;//�ȿ����Զ��ϴ�
        Flash_Buf[23] = liner_B1 = 0;//250;//186=150mv;250=200mv;140 = 110mv;161 = 130mv;
        Flash_Buf[24] = liner_B2 = 0;//2500;//4095=3300mv;1860=1500mv;2500=2V;3100=2.5V;
        Flash_Buf[25] = liner_B3 = 100;//�ƶ�8λ
        Flash_Buf[26] = liner_B4 = 100;//ƽ��ֵ
        Flash_Buf[27] = liner_B5 = 250;//1S��������
        Flash_Buf[28] = liner_B6 = 350;//ϵ��
        Flash_Buf[29] = liner_B7 = 0;//ͻ������
        Flash_Buf[30] = liner_B8 = 0;//PM��ֵ
        Flash_Buf[31] = liner_B9 = 0;//PM��ֵ

        Flash_Buf[32] = liner_CID0 = 0x0101;
        Flash_Buf[33] = liner_CID1 = 0x1806;
        Flash_Buf[34] = liner_CID2 = 0x0112;
        Flash_Buf[35] = liner_CID3 = 0x3400;
        Flash_Buf[36] = liner_CID4 = 0x0000;
        Flash_Buf[37] = liner_CID5 = 0x0000;
        Flash_Buf[38] = liner_CID6 = 0x0000;
        Flash_Buf[39] = liner_CID7 = 0x0000;

        STMFLASH_Write(liner_X0_add, &Flash_Buf[0], 40);
    }
    else
    {
        STMFLASH_Read(liner_X0_add, &Flash_Buf[0], 40);
        liner_X0 = Flash_Buf[0];
        liner_X1 = Flash_Buf[1];
        liner_X2 = Flash_Buf[2];
        liner_X3 = Flash_Buf[3];
        liner_X4 = Flash_Buf[4];
        liner_X5 = Flash_Buf[5];
        liner_X6 = Flash_Buf[6];
        liner_X7 = Flash_Buf[7];
        liner_X8 = Flash_Buf[8];
        liner_X9 = Flash_Buf[9];
        liner_X10 = Flash_Buf[10];

        liner_Y0 = Flash_Buf[11];
        liner_Y1 = Flash_Buf[12];
        liner_Y2 = Flash_Buf[13];
        liner_Y3 = Flash_Buf[14];
        liner_Y4 = Flash_Buf[15];
        liner_Y5 = Flash_Buf[16];
        liner_Y6 = Flash_Buf[17];
        liner_Y7 = Flash_Buf[18];
        liner_Y8 = Flash_Buf[19];
        liner_Y9 = Flash_Buf[20];
        liner_Y10 = Flash_Buf[21];

        liner_B0 = Flash_Buf[22];
        liner_B1 = Flash_Buf[23];
        liner_B2 = Flash_Buf[24];
        liner_B3 = Flash_Buf[25];
        liner_B4 = Flash_Buf[26];
        liner_B5 = Flash_Buf[27];
        liner_B6 = Flash_Buf[28];
        liner_B7 = Flash_Buf[29];
        liner_B8 = Flash_Buf[30];
        liner_B9 = Flash_Buf[31];

        liner_CID0 = Flash_Buf[32];
        liner_CID1 = Flash_Buf[33];
        liner_CID2 = Flash_Buf[34];
        liner_CID3 = Flash_Buf[35];
        liner_CID4 = Flash_Buf[36];
        liner_CID5 = Flash_Buf[37];
        liner_CID6 = Flash_Buf[38];
        liner_CID7 = Flash_Buf[39];
    }
}

/*
*********************************************************************************************************
*	�� �� ��: Modbus_Init
*	����˵��: Modbus ��ʼ����
*	��    ��: ��
*	�� �� ֵ: ��
*	��	  �ģ�halo
*	��    �ڣ�2022.03.20
*********************************************************************************************************
*/
void Modbus_Init(void)
{
    MB_ADDRESS = 0x01;	//modbus ��ַ
    UART_BPS = 9600;	//���ڲ�����
    Version = 	610;	 //806 = 0x326  3��26��
    Mdatapm25 = 0;
    MdataCO2	= 0;
    MdataTVOC = 0;
    MdataTEM = 0;
    MdataSHI = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_ACK_10
*	����˵��: Modbus ������16 Ӧ�� д����Ĵ���
*	��    ��: adr-��ַ��start_jcq-��ʼ��ַ��dat_l-�Ĵ������ȣ�buf-��������
*	�� �� ֵ: ���ͳ���
*	��	  �ģ�halo
*	��    �ڣ�2022.03.20
*********************************************************************************************************
*/
u8 MODBUS_ACK_10(u8 adr,u16 start_jcq,u16 dat_l,u8 *buf)
{   u16 i;
	buf[0]=adr;
    buf[1]=0x10;
    buf[2]=start_jcq>>8;
    buf[3]=start_jcq;
    buf[4]=dat_l>>8;
    buf[5]=dat_l;
    i=N_CRC16(buf,6);
    buf[6]=i;
    buf[7]=i>>8;
    return 8;
}

/*
*********************************************************************************************************
*	�� �� ��: Modbus_Even
*	����˵��: Modbus �����¼�
*	��    ��: ��
*	�� �� ֵ: ��
*	��	  �ģ�halo
*	��    �ڣ�2022.03.20
*********************************************************************************************************
*/
void Modbus_Even(void)
{
	u8 fc_code,n;
	u16 crc,addr,addr1,len;
	if(rxtail!=rxhead)
	{
		crc = N_CRC16(rxbuf[rxtail].rxmsg,rxbuf[rxtail].len-2);//CRCУ��
		//modbus CRC �� ��ַ ȷ��
		if((rxbuf[rxtail].rxmsg[0] == MB_ADDRESS)&&
			(crc == ((rxbuf[rxtail].rxmsg[rxbuf[rxtail].len-1]<<8)|rxbuf[rxtail].rxmsg[rxbuf[rxtail].len-2])))
		{
			fc_code = rxbuf[rxtail].rxmsg[1];//������
			switch((MB_FunctionCode)fc_code)
			{
				case MB_FUNC_CODE_READ_HOLD_REG:{//���Ĵ��� 03
					g_Modbus_Tx_Buf[0] = MB_ADDRESS;//��ַ
					g_Modbus_Tx_Buf[1] = MB_FUNC_CODE_READ_HOLD_REG;//������
					g_Modbus_Tx_Buf[2] = rxbuf[rxtail].rxmsg[5]*2;//�Ĵ�������
					addr = ((rxbuf[rxtail].rxmsg[2]<<8)|rxbuf[rxtail].rxmsg[3]);
					n = 0;
					while((addr<130)&&(n<rxbuf[rxtail].rxmsg[5]))//��ַ�ͳ���
                    {
                        g_Modbus_Tx_Buf[3+n*2] = MODBUS_16w(addr)>>8;
                        g_Modbus_Tx_Buf[4+n*2] = MODBUS_16w(addr)&0x00ff;
                        n++;
                        addr++;
                    }
					crc = N_CRC16(g_Modbus_Tx_Buf,(n*2+3));//��������CRC����
					g_Modbus_Tx_Buf[n*2+3] = crc;
					g_Modbus_Tx_Buf[n*2+4] = crc>>8;
					usart1_byte_send(g_Modbus_Tx_Buf,n*2+3+2);//���ڷ���
				}
				break;
				case MB_FUNC_CODE_WRITE_SINGLE_HOLD_REG://д�����Ĵ��� 06
				{
					addr = ((rxbuf[rxtail].rxmsg[2]<<8)|rxbuf[rxtail].rxmsg[3]);//д���ַ
					switch(addr)
					{
						case 10:{//MODBUS_16w(10)
							liner_X0 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_0;	
						}
						break;	
						case 11:{
							liner_X1 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_1;	
						}
						break;
						case 12:{
							liner_X2 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_2;	
						}
						break;	
						case 13:{
							liner_X3 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_3;	
						}
						break;	
						case 14:{
							liner_X4 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_4;	
						}
						break;	
						case 15:{
							liner_X5 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_5;	
						}
						break;	
						case 16:{
							liner_X6 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_6;	
						}
						break;	
						case 17:{
							liner_X7 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_7;	
						}
						break;	
						case 18:{
							liner_X8 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_8;	
						}
						break;	
						case 19:{
							liner_X9 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_9;	
						}
						break;	
						case 20:{
							liner_X10 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_10;	
						}
						break;	
						case 21:{
							liner_Y0 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_11;	
						}
						break;	
						case 22:{
							liner_Y1 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_12;	
						}
						break;	
						case 23:{
							liner_Y2 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_13;	
						}
						break;	
						case 24:{
							liner_Y3 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_14;	
						}
						break;	
						case 25:{
							liner_Y4 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_15;	
						}
						break;	
						case 26:{
							liner_Y5 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[1] |= MBSave_0;	
						}
						break;	
						case 27:{
							liner_Y6 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_1;	
						}
						break;	
						case 28:{
							liner_Y7 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_2;	
						}
						break;	
						case 29:{
							liner_Y8 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_3;	
						}
						break;	
						case 30:{
							liner_Y9 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_4;	
						}
						break;	
						case 31:{
							liner_Y10 = (rxbuf[rxtail].rxmsg[4]<<8)|rxbuf[rxtail].rxmsg[5];
							MB_Save_Flag[0] |= MBSave_5;	
						}
						break;		
					}
					usart1_byte_send(rxbuf[rxtail].rxmsg,rxbuf[rxtail].len);
					Flash_Save_Flag = 1;
				}
				break;
				case MB_FUNC_CODE_WRITE_MULTI_REG://������10H д����Ĵ���
				{
					
					addr = ((rxbuf[rxtail].rxmsg[2]<<8)|rxbuf[rxtail].rxmsg[3]);//��ʼ��ַ
					addr1 = addr;
					len =	rxbuf[rxtail].rxmsg[5];//�Ĵ�������N
					n = 0;
					while(n<rxbuf[rxtail].rxmsg[6])//�ֽ���2��N
					{
						//д���Ӧmodbus����
						MODBUS_16w(addr) = rxbuf[rxtail].rxmsg[8+n-1]<<8|rxbuf[rxtail].rxmsg[8+n-0];
						n+=2;//�����Ӧλ��
						addr++;
						FLASH_COM_SAVE=1;
					}
					n = MODBUS_ACK_10(MB_ADDRESS, addr1, rxbuf[rxtail].rxmsg[5], &g_Modbus_Tx_Buf[0]);//10HӦ��
					usart1_byte_send(g_Modbus_Tx_Buf,n);
				}
				break;
				default:
					break;
			}
		}
		if(++rxtail >= 10)
		{
			rxtail = 0;
			memset(rxbuf,0,sizeof(rxbuf));//�������
		}
	}
	if(Flash_Save_Flag == 1)//�����Ĵ���flashд���־λ
    {
		if(MB_Save_Flag[0] & MBSave_0) {
			STMFLASH_Write(liner_X0_add, &liner_X0, 1);
			MB_Save_Flag[0] &= ~MBSave_0;}
		else if(MB_Save_Flag[0] & MBSave_1) {
			STMFLASH_Write(liner_X0_add+2, &liner_X0, 1);
			MB_Save_Flag[0] &= ~MBSave_1;}
		else if(MB_Save_Flag[0] & MBSave_2) {
			STMFLASH_Write(liner_X0_add+4, &liner_X0, 1);
			MB_Save_Flag[0] &= ~MBSave_2;}
		else if(MB_Save_Flag[0] & MBSave_3) {
			STMFLASH_Write(liner_X0_add+6, &liner_X0, 1);
			MB_Save_Flag[0] &= ~MBSave_3;}
		else if(MB_Save_Flag[0] & MBSave_4) {
			STMFLASH_Write(liner_X0_add+8, &liner_X0, 1);
			MB_Save_Flag[0] &= ~MBSave_4;}
		else if(MB_Save_Flag[0] & MBSave_5) {
			STMFLASH_Write(liner_X0_add+10, &liner_X0, 1);
			MB_Save_Flag[0] &= ~MBSave_5;}
		else if(MB_Save_Flag[0] & MBSave_6) {
			STMFLASH_Write(liner_X0_add+12, &liner_X0, 1);
			MB_Save_Flag[0] &= ~MBSave_6;}
		else if(MB_Save_Flag[0] & MBSave_7) {
			STMFLASH_Write(liner_X0_add+14, &liner_X0, 1);
			MB_Save_Flag[0] &= ~MBSave_7;}
		else if(MB_Save_Flag[0] & MBSave_8) {
			STMFLASH_Write(liner_X0_add+16, &liner_X0, 1);
			MB_Save_Flag[0] &= ~MBSave_8;}
		else if(MB_Save_Flag[0] & MBSave_9) {
			STMFLASH_Write(liner_X0_add+18, &liner_X0, 1);
			MB_Save_Flag[0] &= ~MBSave_9;}
		else if(MB_Save_Flag[0] & MBSave_10) {
			STMFLASH_Write(liner_X0_add+20, &liner_X0, 1);
			MB_Save_Flag[0] &= ~MBSave_10;}
		Flash_Save_Flag=0;
	}
	if(FLASH_COM_SAVE == 1)//����Ĵ���flashд���־λ
	{
		STMFLASH_Write(liner_X0_add+(addr1-10)*2, &MODBUS_16w(addr1), len);//д����Ĵ���
		FLASH_COM_SAVE = 0;
	}
	
}
