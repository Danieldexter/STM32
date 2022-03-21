软件工程：
	Main_Ctrl
	
研发团队：
	电子科技大学中山学院微波能应用与智能电子控制实验室
	
工程描述：
	电子负载仪测试仪
	
版本	修改日期	修改说明	撰写人
V1.0	21-01-20	模板		豪

注意事项
	1、在初始化网络的时候需要连接网线，否则初始化不成功！！
	
CC计算公式
	DC-1路占空比 	Iset-CC电流设定		Vo-电压采集		
	Num-路数		T-总周期			Ton-打开时间	
	DC=((Iset*Re)/Vo)/Num
	Ton=T*DC

寄存器对应表
	/********LWIP线圈寄存器********/ 
	/*********ucSCoilBuf[0]*********/ 
	寄存器名称		MCU地址 	功能码		屏幕地址
	升压开关		0x01		0x 			1
	风机开关		0x02		0x 			2
	CC开关			0x04		0x 			3
	CV开关			0x08		0x 			4
	CW开关			0x10		0x 			5
	CR开关			0x20		0x 			6

	/**********************LWIP输入寄存器***********************/
	寄存器名称		MCU变量->MCU地址		功能码		屏幕地址
	电流*10			ucSDiscInBuf[0]->300	3x 			301
	断路器前电压*10	ucSDiscInBuf[1]->301	3x 			302
	断路器后电压*10	ucSDiscInBuf[2]->302	3x 			303
	温度*10			ucSDiscInBuf[3]->303	3x 			304
	电流			ucSDiscInBuf[4]->304	3x			305
	断路器前电压	ucSDiscInBuf[5]->305	3x 			306
	断路器后电压	ucSDiscInBuf[6]->306	3x 			307
	温度			ucSDiscInBuf[7]->307	3x 			308
	功率*10			ucSDiscInBuf[8]->308	3x			309
	功率			ucSDiscInBuf[9]->309	3x			310

	/******************LWIP输入保持寄存器**********************/
	寄存器名称		MCU变量->MCU地址		功能码		屏幕地址
	CC曲线图		usSRegHoldBuf[0]->400	4x			401
	CC电流设定		usSRegHoldBuf[1]->401	4x			402
	CV电压设定		usSRegHoldBuf[2]->402	4x			403
	CW功率设定		usSRegHoldBuf[3]->403	4x			404
	CR恒阻设定		usSRegHoldBuf[4]->404	4x			405
	CC历史曲线图读数	usSRegHoldBuf[5]->405	4x			406








