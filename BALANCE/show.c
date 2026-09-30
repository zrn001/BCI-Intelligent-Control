#include "show.h"
int Voltage_Show;
extern u8 Car_Mode;
extern int Time_count;
/**************************************************************************
Function: Read the battery voltage, buzzer alarm, start the self-test, send data to Host, OLED display task
Input   : none
Output  : none
函数功能：读取电池电压、蜂鸣器报警、开启自检、向上位机发送数据、OLED显示屏显示任务
入口参数：无
返回  值：无
**************************************************************************/
int Buzzer_count=25;
void show_task(void *pvParameters)
{
   u32 lastWakeTime = getSysTickCnt();
   while(1)
   {	
		int i=0;
		static int LowVoltage_1=0, LowVoltage_2=0;
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_20_HZ));//This task runs at 20Hz //此任务以20Hz的频率运行
		
		//开机时蜂鸣器短暂蜂鸣，开机提醒
		//The buzzer will beep briefly when the machine is switched on
		if(Time_count<50)Buzzer=1; 
		else if(Time_count>=51 && Time_count<100)Buzzer=0;
		 
		if(LowVoltage_1==1 || LowVoltage_2==1)Buzzer_count=0;
		if(Buzzer_count<5)Buzzer_count++;
		if(Buzzer_count<5)Buzzer=1; //The buzzer is buzzing //蜂鸣器蜂鸣
		else if(Buzzer_count==5)Buzzer=0;
		
		//Read the battery voltage //读取电池电压
		for(i=0;i<10;i++)
		{
			Voltage_All+=Get_battery_volt(); 
		}
		Voltage=Voltage_All/10;
		Voltage_All=0;
		 
		if(LowVoltage_1==1)LowVoltage_1++; //Make sure the buzzer only rings for 0.5 seconds //确保蜂鸣器只响0.5秒
		if(LowVoltage_2==1)LowVoltage_2++; //Make sure the buzzer only rings for 0.5 seconds //确保蜂鸣器只响0.5秒
		if(Voltage>=12.6f)Voltage=12.6f;
		else if(10<=Voltage && Voltage<10.5f && LowVoltage_1<2)LowVoltage_1++; //10.5V, first buzzer when low battery //10.5V，低电量时蜂鸣器第一次报警
		else if(Voltage<10 && LowVoltage_2<2)LowVoltage_2++; //10V, when the car is not allowed to control, the buzzer will alarm the second time //10V，小车禁止控制时蜂鸣器第二次报警
					
		Host_Show();	 //Send data to the Host //向上位机发送数据
		oled_show(); //Tasks are displayed on the screen //显示屏显示任务
   }
}  

/**************************************************************************
Function: The OLED display displays tasks
Input   : none
Output  : none
函数功能：OLED显示屏显示任务
入口参数：无
返回  值：无
**************************************************************************/
void oled_show(void)
{  
   static int count=0;	 

	
	 //Collect the tap information of the potentiometer, 
	 //and display the car model to be fitted when the car starts up in real time
	 //采集电位器档位信息，实时显示小车开机时要适配的小车型号
	 Divisor_Mode=2048/CAR_NUMBER+CAR_NUMBER;
	 Car_Mode=(int) ((Get_adc_Average(Potentiometer,10))/Divisor_Mode);	
	 if(Car_Mode>(CAR_NUMBER-1))Car_Mode=CAR_NUMBER-1;
	 Voltage_Show=Voltage*100; 
	 count++;

	
			//OLED_Clear();
			//The first line of the display shows the content // 
		 //显示屏第1行显示内容//
		 switch(Car_Mode)
		 {
			case Akm_Car:       OLED_ShowString(0,0,"Akm             "); break; 
			case Diff_Car:      OLED_ShowString(0,0,"Diff            "); break; 
		 }
		 
		  //The second line of the display shows the content // 
		  //显示屏第2行显示内容//
		  if(Car_Mode==Akm_Car||Car_Mode==Diff_Car)
		 {
			 //阿克曼、差速小车显示电机A的目标速度和当前实际速度//
			 OLED_ShowString(0,10,"L:");
			 if( MOTOR_A.Motor_Pwm<0)	OLED_ShowString(15,10,"-"),
															OLED_ShowNumber(20,10,-MOTOR_A.Motor_Pwm,5,12);
			 else                 	OLED_ShowString(15,10,"+"),
															OLED_ShowNumber(20,10, MOTOR_A.Motor_Pwm,5,12);  
			 if( MOTOR_A.Encoder<0)	OLED_ShowString(60,10,"-"),
															OLED_ShowNumber(75,10,-MOTOR_A.Encoder*1000,5,12);
			 else                 	OLED_ShowString(60,10,"+"),
															OLED_ShowNumber(75,10, MOTOR_A.Encoder*1000,5,12);
		 }
		 
			//The third line of the display shows the content // 
			//显示屏第3行显示内容//
			//阿克曼、差速小车显示电机B的目标速度和当前实际速度//
			OLED_ShowString(0,20,"R:");
			if( MOTOR_B.Motor_Pwm<0)	OLED_ShowString(15,20,"-"),
															OLED_ShowNumber(20,20,-MOTOR_B.Motor_Pwm,5,12);
			 else                 	OLED_ShowString(15,20,"+"),
															OLED_ShowNumber(20,20, MOTOR_B.Motor_Pwm,5,12);  
				
			 if( MOTOR_B.Encoder<0)	OLED_ShowString(60,20,"-"),
															OLED_ShowNumber(75,20,-MOTOR_B.Encoder*1000,5,12);
			 else                 	OLED_ShowString(60,20,"+"),
															OLED_ShowNumber(75,20, MOTOR_B.Encoder*1000,5,12);
		 
		  //The fourth line of the display shows the content // 
			//显示屏第4行显示内容//
		 OLED_ShowString(00,30,"                ");
		 if(Car_Mode==Akm_Car)
		 {
				//阿克曼小车显示舵机的PWM的数值//

				OLED_ShowString(00,30,"SERVO:");
				if( Servo<0)		      OLED_ShowString(60,30,"-"),
															OLED_ShowNumber(80,30,-Servo,4,12);
				else                 	OLED_ShowString(60,30,"+"),
															OLED_ShowNumber(80,30, Servo,4,12); 
		 }
		 	 else if(Car_Mode==Diff_Car)
		 {
			 //差速小车显示左电机的PWM的数值//
															 OLED_ShowString(00,30,"MA");
			 if( MOTOR_A.Motor_Pwm<0)OLED_ShowString(40,30,"-"),
															 OLED_ShowNumber(50,30,-MOTOR_A.Motor_Pwm,4,12);
			 else                 	 OLED_ShowString(40,30,"+"),
															 OLED_ShowNumber(50,30, MOTOR_A.Motor_Pwm,4,12); 
		 }	
		 
		 //The 5th line of the display shows the content // 
		 //显示屏第5行显示内容//
		 if(Car_Mode==Diff_Car)
		 {
			 //差速小车显示右电机的PWM的数值//
															 OLED_ShowString(00,40,"MB");
			 if(MOTOR_B.Motor_Pwm<0) OLED_ShowString(40,40,"-"),
															 OLED_ShowNumber(50,40,-MOTOR_B.Motor_Pwm,4,12);
			 else                 	 OLED_ShowString(40,40,"+"),
															 OLED_ShowNumber(50,40, MOTOR_B.Motor_Pwm,4,12);
		 }			 

	
			//The 6th line of the display shows the content // 
			//显示屏第6行显示内容// 
	
			
		 //显示当前小车是否允许控制//
		 if(EN==1&&Flag_Stop==0)   	OLED_ShowString(45,50,"O N");  
		 else                      	OLED_ShowString(45,50,"OFF"); 
			
																OLED_ShowNumber(75,50,Voltage_Show/100,2,12);
			                          OLED_ShowString(88,50,".");
																OLED_ShowNumber(98,50,Voltage_Show%100,2,12);
			                          OLED_ShowString(110,50,"V");
		 if(Voltage_Show%100<10) 		OLED_ShowNumber(92,50,0,2,12);
	
		OLED_Refresh_Gram();
}
/**************************************************************************
Function: Send data to the Host
Input   : none
Output  : none
函数功能：向上位机发送数据
入口参数：无
返回  值：无
**************************************************************************/
void Host_Show(void)
{    
	 int Left_Figure,Right_Figure,Voltage_Percent;
	
	 //The battery voltage is processed as a percentage
	 //对电池电压处理成百分比形式
	 Voltage_Percent=(Voltage*1000-10000)/26;
	 if(Voltage_Percent>100)Voltage_Percent=100; 
	
	 //Wheel speed unit is converted to 0.01m/s for easy display in Host
	 //车轮速度单位转换为0.01m/s，方便在上位机显示
	 Left_Figure=MOTOR_A.Encoder*100;  if(Left_Figure<0)Left_Figure=-Left_Figure;	
	 Right_Figure=MOTOR_B.Encoder*100; if(Right_Figure<0)Right_Figure=-Right_Figure; 		 
	 
	 printf("{C%d:%d:%d}$",(int)Left_Figure,(int)Right_Figure,(int)Voltage_Percent);
}

