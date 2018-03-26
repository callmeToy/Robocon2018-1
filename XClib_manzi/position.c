/*
 * position.c
 *
 *  Created on: 2018/02/14
 *      Author: RoboPro2015
 */

#include "position.h"
#include "math.h"
void POSITION_INIT(){
	PS_MODE = POSITION;
	con1.Inref = 0.0;
	con1.Inref_P = 0.0;
	con1.Limit = 0.8;
	con1.P_Error = 0.0;
	//�� * ��n^2
	//con1.P_Gain = T_CONSTRPV_ACH * OMEGA_ACH * OMEGA_ACH;//126.0;
	con1.P_Gain = P_GAIN;

	con2.Inref = 0.0;
	con2.Inref_P = 0.0;
	con2.Limit = 0.8;
	con2.P_Error = 0.0;
	//con2.P_Gain = T_CONSTRPV_BCH * OMEGA_BCH * OMEGA_BCH;//126.0;
	con2.P_Gain = P_GAIN;

}
void LINEAR_INIT(){
	linear.Now_X_Point = 0.;//-0.3;
	linear.Now_Y_Point = 0.;//0.0;

	linear.Ref_X_Point = 0.;//-0.3;
	linear.Ref_Y_Point = 0.;//-0.3;

	linear.Tip_Length = 0.;//��[����
	linear.Tip_Angle = 0.;//��[�p�x
	linear.Tip_Angle_Deg = 0.;

	linear.Tip_Length_Ref = 0.;
	linear.Tip_Angle_Ref = 0.;
	linear.Tip_Angle_Deg_Ref = 0.;

	linear.Angle_Arm1 = 0.;//(50 * PI()) / 180;//ROOT
	linear.Angle_Arm2 = 0.;//(50 * PI()) / 180;//TIP

	linear.Step_X = 0.;
	linear.Step_Y = 0.;

	linear.Moving_X_Point = 0.;//Now_X_Point;
	linear.Moving_Y_Point = 0.;//Now_Y_Point;

	//linear.X = 0.;
	//linear.Y = 0.;

	linear.time = 1000;//1000; //ms

}
void ROTATION_INIT(){

}
void POSITION_CONTROLL(){

	if(protect.Lag_Count < PROTECT_TIME){
		con1.P_Gain = (protect.Lag_Count / PROTECT_TIME) * P_GAIN;
		con2.P_Gain = (protect.Lag_Count / PROTECT_TIME) * P_GAIN;
		PS_MODE = POSITION;
		protect.Lag_Count++;
	}
	else{
		con1.P_Gain = 1.;
		con2.P_Gain = 1.;
	}

	//Position_Controll
	if(PS_MODE == POSITION){
		con1.P_Error = (con1.Inref_P - henc1.qDeg) / 360;//�ڕW�Ƃ̍���
		con1.Inref = con1.P_Error * con1.P_Gain;//�����ɃQ�C���������đ��x�ڕW�l�ɂ���
	//	if(con1.Inref > con1.Limit) con1.Inref = con1.Limit;
	//	else if(con1.Inref < -con1.Limit) con1.Inref = -con1.Limit;

		con2.P_Error = (con2.Inref_P - henc2.qDeg) / 360;
		con2.Inref = con2.P_Error * con2.P_Gain;
	//	if(con2.Inref > con2.Limit) con2.Inref = con2.Limit;
	//	else if(con2.Inref < -con2.Limit) con2.Inref = -con2.Limit;

	//	setInRef(&hpi1,con1.Inref);//���x�ڕW�l����//Root
	//	setInRef(&hpi2,con2.Inref);				  //Tip
		setInRef(&hmtr1.pPIParm,con1.Inref);
		setInRef(&hmtr2.pPIParm,con2.Inref);

//		hpwm1 = getCalcPWM(&hpi1, &henc1);//PI�v�Z +PWM�o��
//		hpwm2 = getCalcPWM(&hpi2, &henc2);
		CalcVelPWM(&hmtr1, hmtr1.pPIParm.qInRef - hmtr1.pEncParm->qFreq);	//ref - res
		CalcVelPWM(&hmtr2, hmtr2.pPIParm.qInRef - hmtr2.pEncParm->qFreq);	//ref - res
		/*
		P_Error_1 = Inref_P_1 - henc1.qAngle;
		Inref_1 = P_Error_1 * P_Gain_1;

		P_Error_2 = Inref_P_2 - henc2.qAngle;
		Inref_2 = P_Error_2 * P_Gain_2;

		setInRef(&hpi1,Inref_1);
		setInRef(&hpi1,Inref_2);
		*/
	}
	else if(PS_MODE == SPEED){
		setInRef(&hmtr1.pPIParm,con1.Inref);
		setInRef(&hmtr2.pPIParm,con2.Inref);
		CalcVelPWM(&hmtr1, hmtr1.pPIParm.qInRef - hmtr1.pEncParm->qFreq);	//ref - res
		CalcVelPWM(&hmtr2, hmtr2.pPIParm.qInRef - hmtr2.pEncParm->qFreq);	//ref - res

	}
	else{
		//setInRef(&hpi1,0);
		//setInRef(&hpi2,0);
		setInRef(&hmtr1.pPIParm,0);
		setInRef(&hmtr2.pPIParm,0);
		CalcVelPWM(&hmtr1, hmtr1.pPIParm.qInRef - hmtr1.pEncParm->qFreq);	//ref - res
		CalcVelPWM(&hmtr2, hmtr2.pPIParm.qInRef - hmtr2.pEncParm->qFreq);	//ref - res

	}


	MotorDriveOpenLoop(&md1, hmtr1.qDuty);//Root
	MotorDriveOpenLoop(&md2, hmtr2.qDuty);//Tip
}

void SET_NOW_POSITION(){
	if(set_select == SET_LINEAR){
		linear.Angle_Arm1 = (henc1.qDeg * M_PI)/180;
		linear.Angle_Arm2 = (henc2.qDeg * M_PI)/180;

		linear.Now_X_Point = (ROOT_ARM * cos(linear.Angle_Arm1)) + (TIP_ARM * cos(linear.Angle_Arm1 + linear.Angle_Arm2));
		linear.Now_Y_Point = (ROOT_ARM * sin(linear.Angle_Arm1)) + (TIP_ARM * sin(linear.Angle_Arm1 + linear.Angle_Arm2));

		linear.Moving_X_Point = linear.Now_X_Point;
		linear.Moving_Y_Point = linear.Now_Y_Point;

		linear.Tip_Length = sqrt((linear.Now_X_Point * linear.Now_X_Point) + (linear.Now_Y_Point * linear.Now_Y_Point));
		linear.Tip_Angle_Deg = (double)((((int)(((atan2(linear.Now_Y_Point, linear.Now_X_Point)) * 180) / M_PI))) % 360);
		linear.Tip_Angle = (linear.Tip_Angle_Deg / 180) * M_PI;
	#if 1
		xprint(&huart4,"Angle_Arm1  %d.%d \r\n",gan(linear.Angle_Arm1),dec(linear.Angle_Arm1,100));
		xprint(&huart4," 		 2  %d.%d \r\n",gan(linear.Angle_Arm2),dec(linear.Angle_Arm2,100));
		xprint(&huart4,"Now_X_Point %d.%d \r\n",gan(linear.Now_X_Point),dec(linear.Now_X_Point,100));
		xprint(&huart4,"Now_Y_Point %d.%d \r\n",gan(linear.Now_Y_Point),dec(linear.Now_Y_Point,100));
		xprint(&huart4,"Tip_Length  %d.%d \r\n",gan(linear.Tip_Length),dec(linear.Tip_Length,100));
		xprint(&huart4,"Tip_Angle   %d.%d \r\n",gan(linear.Tip_Angle),dec(linear.Tip_Angle,100));
		xprint(&huart4,"       _Deg %d.%d \r\n",gan(linear.Tip_Angle_Deg),dec(linear.Tip_Angle_Deg,100));
	#endif
	}
	else if(set_select == SET_ROTATION){
		rotation.Now_Root_Angle = henc1.qDeg;
		rotation.Now_Tip_Angle = henc2.qDeg + henc1.qDeg;
	#if 1
		xprint(&huart4,"rotation.Now_Root_Angle  %d.%d \r\n",gan(rotation.Now_Root_Angle),dec(rotation.Now_Root_Angle,100));
		xprint(&huart4,"rotation.Now_Tip_Angle   %d.%d \r\n",gan(rotation.Now_Tip_Angle),dec(rotation.Now_Tip_Angle,100));
	#endif
	}
}
/*param
 * ref_x [m]
 * ref_y [m]
 * time	[ms]
 */
void SET_REF_LINEAR(double REF_X, double REF_Y, long int time){
	linear.Ref_X_Point = REF_X;
	linear.Ref_Y_Point = REF_Y;
	linear.time = time;

	linear.Step_X = (linear.Ref_X_Point - linear.Now_X_Point) / time;
	linear.Step_Y = (linear.Ref_Y_Point - linear.Now_Y_Point) / time;

	linear.Tip_Length_Ref = sqrt((linear.Ref_X_Point * linear.Ref_X_Point) + (linear.Ref_Y_Point * linear.Ref_Y_Point));
	linear.Tip_Angle_Deg_Ref = (double)((((int)(((atan2(linear.Ref_Y_Point, linear.Ref_X_Point)) * 180) / M_PI))) % 360);
	linear.Tip_Angle_Ref = (linear.Tip_Angle_Deg_Ref / 180) * M_PI;
}

void LINEAR_ORBIT(){
	if(LINEAR_MOVE == ACT){
		linear.Moving_X_Point += linear.Step_X;
		linear.Moving_Y_Point += linear.Step_Y;

		linear.Tip_Length = sqrt((linear.Moving_X_Point * linear.Moving_X_Point) + (linear.Moving_Y_Point * linear.Moving_Y_Point));
		linear.Tip_Angle_Deg = (double)((((int)(((atan2(linear.Moving_Y_Point, linear.Moving_X_Point)) * 180) / M_PI))) % 360);
		linear.Tip_Angle = (linear.Tip_Angle_Deg/180) * M_PI;
		//Tip_Angle = Tip_Angle / 1000;
		linear.Angle_Arm2 = 180 - ((acos(((ROOT_ARM * ROOT_ARM) + (TIP_ARM * TIP_ARM) - (linear.Tip_Length * linear.Tip_Length)) / (2 * ROOT_ARM * TIP_ARM)) * 180) / M_PI);
		linear.Angle_Arm1 = linear.Tip_Angle_Deg - ((acos(((ROOT_ARM * ROOT_ARM) + (linear.Tip_Length * linear.Tip_Length) - (TIP_ARM * TIP_ARM)) / (2 * ROOT_ARM * linear.Tip_Length)) * 180) / M_PI);


		//linear.Now_X_Point = (ROOT_ARM * cos(linear.Angle_Arm1)) + (TIP_ARM * cos(linear.Angle_Arm1 + linear.Angle_Arm2));
		//linear.Now_Y_Point = (ROOT_ARM * sin(linear.Angle_Arm1)) + (TIP_ARM * sin(linear.Angle_Arm1 + linear.Angle_Arm2));

		if(fabs(fabs(linear.Moving_X_Point) - fabs(linear.Ref_X_Point)) <= 0.01){
			//con1.Inref_P = linear.Ref_X_Point;
			if(fabs(fabs(linear.Moving_Y_Point) - fabs(linear.Ref_Y_Point)) <= 0.01){
				linear.Angle_Arm2 = 180 - ((acos(((ROOT_ARM * ROOT_ARM) + (TIP_ARM * TIP_ARM) - (linear.Tip_Length_Ref * linear.Tip_Length_Ref)) / (2 * ROOT_ARM * TIP_ARM)) * 180) / M_PI);
				linear.Angle_Arm1 = linear.Tip_Angle_Deg_Ref - ((acos(((ROOT_ARM * ROOT_ARM) + (linear.Tip_Length_Ref * linear.Tip_Length_Ref) - (TIP_ARM * TIP_ARM)) / (2 * ROOT_ARM * linear.Tip_Length_Ref)) * 180) / M_PI);

				con1.Inref_P = linear.Angle_Arm1;
				con2.Inref_P = linear.Angle_Arm2;
				LINEAR_MOVE = NACT;
			}
		}
		else{
			con1.Inref_P = linear.Angle_Arm1;
			con2.Inref_P = linear.Angle_Arm2;
		}
	}
	//Angle_Arm1 = ((Angle_Arm1 / 180) * M_PI);
	//Angle_Arm2 = ((Angle_Arm2 / 180) * M_PI);


	//if(Tip_Length <= 0.67)fprintf(file, "%f,%f,%f,%f,%f\n", Angle_Arm1, Angle_Arm2,X,Y,Tip_Angle);
	//else if (Tip_Length <= (Arm2 - Arm1)) fprintf(file, "miss min\n");
	//else fprintf(file, "miss\n");

}
/*param
 * ref_root	[deg]
 * ref_tip	[deg]
 * time	[ms]
 */
void SET_REF_ROTATION(double REF_ROOT, double REF_TIP, double time){
	rotation.Ref_Root_Angle = REF_ROOT;
	rotation.Ref_Tip_Angle = REF_TIP;
	rotation.time = time;
printf("set_ref_rotation\r\n");
/*
	xprint(&huart4,"REF_ROOT %d.%d \r\n",gan(rotation.Ref_Root_Angle),dec(rotation.Ref_Root_Angle,100));
	xprint(&huart4,"REF_TIP  %d.%d \r\n",gan(rotation.Ref_Tip_Angle),dec(rotation.Ref_Tip_Angle,100));
	xprint(&huart4,"TIME     %d.%d \r\n",gan(rotation.time),dec(rotation.time,100));
*/
	xprint(&huart4,"REF_ROOT %d.%d \r\n",gan(REF_ROOT),dec(REF_ROOT,100));
	xprint(&huart4,"REF_TIP  %d.%d \r\n",gan(REF_TIP),dec(REF_TIP,100));
	xprint(&huart4,"TIME     %d.%d \r\n",gan(time),dec(time,100));
	rotation.Step_Root = (rotation.Ref_Root_Angle - rotation.Now_Root_Angle) / time;
	rotation.Step_Tip = (rotation.Ref_Tip_Angle - rotation.Now_Tip_Angle) / time;

	xprint(&huart4," %d.%d %d.%d\r\n",gan(rotation.Step_Root),dec(rotation.Step_Root,100),gan(rotation.Step_Tip),dec(rotation.Step_Tip,100));
}
void ROTATION_ORBIT(){
	static long int count = 0;
	if(ROTATION_MOVE == ACT){
		rotation.Now_Root_Angle += rotation.Step_Root;
		rotation.Now_Tip_Angle += rotation.Step_Tip;
		count++;
		if(count > rotation.time){
	//	if(fabs(fabs(rotation.Ref_Root_Angle) - fabs(rotation.Now_Root_Angle)) <= 0.1){
	//		if(fabs(fabs(rotation.Ref_Tip_Angle) - fabs(rotation.Now_Tip_Angle)) <= 0.1){
			con1.Inref_P = rotation.Ref_Root_Angle;
			con2.Inref_P = rotation.Ref_Tip_Angle - rotation.Ref_Root_Angle;
			pRoot.Now_Angle = con1.Inref_P / 360.;
			pTip.Now_Angle = con2.Inref_P / 360.;
			#if 1
				printf("debug rotation finish");
				xprint(&huart4,"con1.Inref_P  %d.%d \r\n",gan(con1.Inref_P),dec(con1.Inref_P,100));
				xprint(&huart4,"con2.Inref_P  %d.%d \r\n",gan(con2.Inref_P),dec(con2.Inref_P,100));
				xprint(&huart4,"pRoot.Now_Angle  %d.%d \r\n",gan(pRoot.Now_Angle),dec(pRoot.Now_Angle,100));
				xprint(&huart4,"pTip.Now_Angle  %d.%d \r\n",gan(pTip.Now_Angle),dec(pTip.Now_Angle,100));
			#endif
			//count = 0;
			ROTATION_MOVE = NACT;
	//		}
	//	}
		}
		else{
			con1.Inref_P = rotation.Now_Root_Angle;
			con2.Inref_P = rotation.Now_Tip_Angle - rotation.Now_Root_Angle;
			pRoot.Now_Angle = con1.Inref_P / 360.;
			pTip.Now_Angle = con2.Inref_P / 360.;

		}
	}
	else if(ROTATION_MOVE == NACT){
		count = 0;
	//	xprint(&huart4,"count  %d.%d \r\n",gan(count),dec(count,100));
	}

}
