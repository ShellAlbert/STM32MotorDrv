
#include <zlens_task.h>
#include "tim.h"
#include "cmsis_os.h"
extern osThreadId LensTaskHandle;

struct lensDevice lensDev;

//直流电机通过按钮+/-一次控制的增量
//产生pwm频率是固定的
//调整的只是占空比
//设当前编码器计数值为curVal
//目标编码器计数值为dstVal
//差值diffVal=dstVal-curVal
//这个差值diffVal控制的就是占空比
//控制的过程就是尽量使curVal向dstVal无限靠近，直至相等，此时差值为0
//占空比为0，则表示IO输出为一恒定的电平（高或低），也就没有pwm输出了
//占空比=CCR/ARR   (CaptureCompare Register/AutoReload Register,捕获比较寄存器/自动重装载寄存器)
//占空比越大，表示单位时间内有效电平的时间越长，电机得到的电流就越多，速转就越快
//当dstVal与curVal差距较大时，可以将占空比设置得较大一些，比如90%，
//但要有范围限制，不能超过100%,也不能是0%，否则就恒定电平没有输出了。
//当dstVal与curVal越来越接近的时候，趋向于0%，此时占空比为0%，pwm就没有输出了,电机也就停止转动了。
void zsy_LensInit()
{
	//TIM1-CH1/CH1N: DC Motor1 PWM Out+/Out- (Left Lens).
	//TIM3-CH1:Encoder1 A
	//TIM3-CH2:Encoder1 B
	lensDev.lftMotor.pwm_tim_handle = &htim1;
	lensDev.lftMotor.pwm_channel = TIM_CHANNEL_2;
	lensDev.lftMotor.encoder_tim_handle = &htim3;
	lensDev.lftMotor.goto_zp_flag = 1;				//GotoZeroPointFlag标志位置位
	lensDev.pidLft.maxOut = 500;
	lensDev.pidLft.inteLimit = 0;
	lensDev.pidLft.inputMaxErr = 0;
	lensDev.pidLft.kP	= 1.5;
	lensDev.pidLft.kI	= 0;
	lensDev.pidLft.kD	= 0;


	//TIM1-CH2/CH2N: DC Motor2 PWM Out+/Out- (Right Lens).	
	//TIM5-CH1:Encoder2 A
	//TIM5-CH2:Encoder2 B
	lensDev.rhtMotor.pwm_tim_handle = &htim1;
	lensDev.rhtMotor.pwm_channel = TIM_CHANNEL_1;
	lensDev.rhtMotor.encoder_tim_handle = &htim5;
	lensDev.rhtMotor.goto_zp_flag = 1;				//GotoZeroPointFlag标志位置位
	lensDev.pidRht.maxOut = 500;
	lensDev.pidRht.inteLimit = 0;
	lensDev.pidRht.inputMaxErr = 0;
	lensDev.pidRht.kP	= 1.5;
	lensDev.pidRht.kI	= 0;
	lensDev.pidRht.kD	= 0;

	//上电时自动对焦是关闭的,只能通过APP指令触发.
	lensDev.autoFocusFlag = 0;


	//TIM1-CH1/CH1N: DC Motor1 PWM Out+/Out- (Left Lens).
	//TIM1-CH2/CH2N: DC Motor2 PWM Out+/Out- (Right Lens).
	//TIM1-CH3/CH3N: DC Motor3 PWM Out+/Out- (Tamper).
	//上电即启动PWM,通过ADC堵转电流停止,设定机械零点/逻辑零点.
	//用于上电时电机复位,寻找零点.
	MX_TIM1_Init();
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);

	//TIM3-CH1:Encoder1 A
	//TIM3-CH2:Encoder1 B
	MX_TIM3_Init();
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_2);

	//TIM5-CH1:Encoder2 A
	//TIM5-CH2:Encoder2 B
	MX_TIM5_Init();
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_2);
}


void zsy_LensTaskLoop(void const * argument)
{
	uint32_t		period = osKernelSysTick();

	while (1)
		{
		float			fMotorOut;
		uint32_t		currentValue;

		//编码器的当前值与目标值作PID
		//占空比调节=CCR/ARR=x/1000,什么时间停止输出？
		//当CCR=0时，占空比为0,输出一直为高电平或一直为低电平，即为停止PWM.
		//说白了，
		//编码器当前值与目标值作PID后，结果趋向于0
		//就是CCR->0趋向于0，当CCR=0时,占空比CCR/ARR=0，此时PWM输出停止。
		
		//left motor.
		currentValue		= __HAL_TIM_GET_COUNTER(&htim3);
		fMotorOut			= zsy_PIDCalculate(&lensDev.pidLft, currentValue, lensDev.lftMotor.targetEncoder);
		lensDev.lftMotor.currentEncoder = fMotorOut;
		__HAL_TIM_SET_COMPARE(lensDev.lftMotor.pwm_tim_handle, lensDev.lftMotor.pwm_channel, 500 - fMotorOut);

		//right motor.
		currentValue		= __HAL_TIM_GET_COUNTER(&htim5);
		fMotorOut			= zsy_PIDCalculate(&lensDev.pidRht, currentValue, lensDev.rhtMotor.targetEncoder);
		lensDev.rhtMotor.currentEncoder = fMotorOut;
		__HAL_TIM_SET_COMPARE(lensDev.rhtMotor.pwm_tim_handle, lensDev.rhtMotor.pwm_channel, 500 - fMotorOut);

		//schedule after 5ms.
		osDelayUntil(&period, 5);
		}
}


void zsy_AbsLimit(float * a, float ABS_MAX)
{
	if (*a > ABS_MAX)
		{
		*a					= ABS_MAX;
		}

	if (*a < -ABS_MAX)
		{
		*a					= -ABS_MAX;
		}
}


/**
  * @brief	   calculate delta PID and position PID
  * @param[in] pid: control pid struct
  * @param[in] get: measure feedback value
  * @param[in] set: target value
  * @retval    pid calculate output 
  */
float zsy_PIDCalculate(struct pidDevice * pidDev, float getV, float setV)
{
	pidDev->getV		= getV;
	pidDev->setV		= setV;
	pidDev->err 		= setV - getV;

	if ((pidDev->inputMaxErr != 0) && (fabs(pidDev->err) > pidDev->inputMaxErr))
		{
		return 0;
		}

	pidDev->pOut		= pidDev->kP * pidDev->err;
	pidDev->iOut		+= pidDev->kI * pidDev->err;
	pidDev->dOut		= pidDev->kD * (pidDev->err - pidDev->last_err);

	zsy_AbsLimit(& (pidDev->iOut), pidDev->inteLimit);
	pidDev->out 		= pidDev->pOut + pidDev->iOut + pidDev->dOut;
	zsy_AbsLimit(& (pidDev->out), pidDev->maxOut);

	return pidDev->out;
}


void zsy_LftMotorGotPeakTorqueCurrentCallback(void)
{
	//左电机堵转电流引发模拟看门狗中断，当发生10000次时可以认为100%到达机械零点了
	static uint16_t nGotPeakTimes = 0;

	if (lensDev.lftMotor.goto_zp_flag)
		{
		if (nGotPeakTimes++ >= 10000)
			{
			nGotPeakTimes		= 0;
			lensDev.lftMotor.nArrivedZPFlag = 1;
			lensDev.lftMotor.goto_zp_flag = 0;

			//到达机械零点后，设置CCR=500.
			//因为ARR=1000,当CCR=500时，1/(1000/500)*100%=50%,高低电平占空比为50%.
			__HAL_TIM_SET_COMPARE(lensDev.lftMotor.pwm_tim_handle, lensDev.lftMotor.pwm_channel, 500);

			//只有左右电机都到达机械零点了，说明逻辑零点都找到了，
			//然后才唤醒LensThread线程,该线程上电创建时是挂起的,只有找到零点后才唤醒.
			if (lensDev.lftMotor.nArrivedZPFlag && lensDev.rhtMotor.nArrivedZPFlag)
				{
				lensDev.lftMotor.nArrivedZPFlag = 0;
				zsy_LensThreadResume();
				}
			}
		}

}


void zsy_RhtMotorGotPeakTorqueCurrentCallback(void)
{
	//右电机堵转电流引发模拟看门狗中断，当发生10000次时可以认为100%到达零点了
	static uint16_t nGotPeakTimes = 0;

	if (lensDev.rhtMotor.goto_zp_flag)
		{
		if (nGotPeakTimes++ >= 10000)
			{
			nGotPeakTimes		= 0;
			lensDev.rhtMotor.nArrivedZPFlag = 1;
			lensDev.rhtMotor.goto_zp_flag = 0;

			//到达机械零点后，设置CCR=500.
			//因为ARR=1000,当CCR=500时，1/(1000/500)*100%=50%,高低电平占空比为50%.
			__HAL_TIM_SET_COMPARE(lensDev.rhtMotor.pwm_tim_handle, lensDev.rhtMotor.pwm_channel, 500);

			//只有左右电机都到达零点了，说明逻辑零点都找到了，
			//然后才唤醒LensThread线程
			if (lensDev.lftMotor.nArrivedZPFlag && lensDev.rhtMotor.nArrivedZPFlag)
				{
				lensDev.rhtMotor.nArrivedZPFlag = 0;
				zsy_LensThreadResume();
				}
			}
		}
}


void zsy_LensThreadResume(void)
{
	if (!lensDev.autoFocusFlag)
		{
		lensDev.lftMotor.targetEncoder = LFT_MOTOR_ZEROPOINT_ENCODER_VALUE;
		lensDev.rhtMotor.targetEncoder = RHT_MOTOR_ZEROPOINT_ENCODER_VALUE;
		}

	//reset encoder value to 100.
	__HAL_TIM_SET_COUNTER(lensDev.lftMotor.encoder_tim_handle, 100);
	__HAL_TIM_SET_COUNTER(lensDev.rhtMotor.encoder_tim_handle, 100);
	osThreadResume(LensTaskHandle);
}


void zsy_LensSetAutoFocusFlag(uint8_t bEnabled)
{
	lensDev.autoFocusFlag = bEnabled;

	if (bEnabled)
		{
		osThreadSuspend(LensTaskHandle);

		lensDev.lftMotor.goto_zp_flag = 1;
		lensDev.rhtMotor.goto_zp_flag = 1;

		//因为TIMx初始化的时候,设置的ARR=1000
		//那么这里也设置为1000，
		//就相当于占空比为0了，相当于没有输出了
		__HAL_TIM_SET_COMPARE(lensDev.lftMotor.pwm_tim_handle, lensDev.lftMotor.pwm_channel, 1000);
		__HAL_TIM_SET_COMPARE(lensDev.rhtMotor.pwm_tim_handle, lensDev.rhtMotor.pwm_channel, 1000);

		}
}


void zsy_MotorAutoFocusWithDistance(uint32_t nDistance)
{
	uint32_t		i	= 0;

	//distance range repair.
	if (nDistance > 350)
		{
		nDistance			= 400;
		}
	else if ((nDistance > 300) && (nDistance <= 350))
		{
		nDistance			= 350;
		}
	else if (nDistance <= 28)
		{
		lensDev.lftMotor.targetEncoder = LFT_MOTOR_ZEROPOINT_ENCODER_VALUE;
		lensDev.rhtMotor.targetEncoder = RHT_MOTOR_ZEROPOINT_ENCODER_VALUE;
		return;
		}

	//遍历查表，找到距离值对应的编码器值
	for (i = 0; i < sizeof(g_Table_Distance2Encoder_LftMotor) / sizeof(g_Table_Distance2Encoder_LftMotor[0]); i++)
		{
		if (g_Table_Distance2Encoder_LftMotor[i][0] == nDistance)
			{
			lensDev.lftMotor.targetEncoder = g_Table_Distance2Encoder_LftMotor[i][1];
			break;
			}
		}

	for (i = 0; i < sizeof(g_Table_Distance2Encoder_RhtMotor) / sizeof(g_Table_Distance2Encoder_RhtMotor[0]); i++)
		{
		if (g_Table_Distance2Encoder_RhtMotor[i][0] == nDistance)
			{
			lensDev.rhtMotor.targetEncoder = g_Table_Distance2Encoder_RhtMotor[i][1];
			break;
			}
		}


	osThreadResume(LensTaskHandle);
	return;

}


/* left motor */
uint16_t		g_Table_Distance2Encoder_LftMotor[274][2] =
{
		{
		29, 12200
		},
		{
		30, 12400
		},
		{
		31, 12500
		},
		{
		32, 13000
		},
		{
		33, 13500
		},
		{
		34, 14000
		},
		{
		35, 14500
		},
		{
		36, 15000
		},
		{
		37, 15500
		},
		{
		38, 16083
		},
		{
		39, 16666
		},
		{
		40, 17249
		},
		{
		41, 17832
		},
		{
		42, 18415
		},
		{
		43, 19000
		},
		{
		44, 20000
		},
		{
		45, 21000
		},
		{
		46, 22000
		},
		{
		47, 23000
		},
		{
		48, 23307
		},
		{
		49, 23614
		},
		{
		50, 23921
		},
		{
		51, 24228
		},
		{
		52, 24535
		},
		{
		53, 24842
		},
		{
		54, 25149
		},
		{
		55, 25456
		},
		{
		56, 25763
		},
		{
		57, 26070
		},
		{
		58, 26377
		},
		{
		59, 26684
		},
		{
		60, 27000
		},
		{
		61, 27416
		},
		{
		62, 27832
		},
		{
		63, 28248
		},
		{
		64, 28664
		},
		{
		65, 29080
		},
		{
		66, 29500
		},
		{
		67, 29619
		},
		{
		68, 29738
		},
		{
		69, 29857
		},
		{
		70, 29976
		},
		{
		71, 30095
		},
		{
		72, 30214
		},
		{
		73, 30333
		},
		{
		74, 30452
		},
		{
		75, 30571
		},
		{
		76, 30690
		},
		{
		77, 30809
		},
		{
		78, 30928
		},
		{
		79, 31047
		},
		{
		80, 31166
		},
		{
		81, 31285
		},
		{
		82, 31404
		},
		{
		83, 31523
		},
		{
		84, 31642
		},
		{
		85, 31761
		},
		{
		86, 31880
		},
		{
		87, 32000
		},
		{
		88, 32050
		},
		{
		89, 32100
		},
		{
		90, 32150
		},
		{
		91, 32200
		},
		{
		92, 32250
		},
		{
		93, 32300
		},
		{
		94, 32350
		},
		{
		95, 32400
		},
		{
		96, 32450
		},
		{
		97, 32500
		},
		{
		98, 32550
		},
		{
		99, 32600
		},
		{
		100, 32650
		},
		{
		101, 32700
		},
		{
		102, 32750
		},
		{
		103, 32800
		},
		{
		104, 32850
		},
		{
		105, 32900
		},
		{
		106, 32950
		},
		{
		107, 33000
		},
		{
		108, 33050
		},
		{
		109, 33100
		},
		{
		110, 33150
		},
		{
		111, 33200
		},
		{
		112, 33250
		},
		{
		113, 33300
		},
		{
		114, 33350
		},
		{
		115, 33400
		},
		{
		116, 33450
		},
		{
		117, 33500
		},
		{
		118, 33578
		},
		{
		119, 33656
		},
		{
		120, 33734
		},
		{
		121, 33812
		},
		{
		122, 33890
		},
		{
		123, 33968
		},
		{
		124, 34046
		},
		{
		125, 34124
		},
		{
		126, 34202
		},
		{
		127, 34280
		},
		{
		128, 34358
		},
		{
		129, 34436
		},
		{
		130, 34514
		},
		{
		131, 34592
		},
		{
		132, 34670
		},
		{
		133, 34748
		},
		{
		134, 34826
		},
		{
		135, 34904
		},
		{
		136, 34982
		},
		{
		137, 35060
		},
		{
		138, 35138
		},
		{
		139, 35216
		},
		{
		140, 35294
		},
		{
		141, 35372
		},
		{
		142, 35450
		},
		{
		143, 35528
		},
		{
		144, 35606
		},
		{
		145, 35684
		},
		{
		146, 35762
		},
		{
		147, 35840
		},
		{
		148, 35918
		},
		{
		149, 35996
		},
		{
		150, 36074
		},
		{
		151, 36152
		},
		{
		152, 36230
		},
		{
		153, 36308
		},
		{
		154, 36386
		},
		{
		155, 36500
		},
		{
		156, 36513
		},
		{
		157, 36526
		},
		{
		158, 36539
		},
		{
		159, 36552
		},
		{
		160, 36565
		},
		{
		161, 36578
		},
		{
		162, 36591
		},
		{
		163, 36604
		},
		{
		164, 36617
		},
		{
		165, 36630
		},
		{
		166, 36643
		},
		{
		167, 36656
		},
		{
		168, 36669
		},
		{
		169, 36682
		},
		{
		170, 36695
		},
		{
		171, 36708
		},
		{
		172, 36721
		},
		{
		173, 36734
		},
		{
		174, 36747
		},
		{
		175, 36760
		},
		{
		176, 36773
		},
		{
		177, 36786
		},
		{
		178, 36799
		},
		{
		179, 36812
		},
		{
		180, 36825
		},
		{
		181, 36838
		},
		{
		182, 36851
		},
		{
		183, 36864
		},
		{
		184, 36877
		},
		{
		185, 36890
		},
		{
		186, 36903
		},
		{
		187, 36916
		},
		{
		188, 36929
		},
		{
		189, 36942
		},
		{
		190, 36955
		},
		{
		191, 36968
		},
		{
		192, 36981
		},
		{
		193, 36994
		},
		{
		194, 37007
		},
		{
		195, 37020
		},
		{
		196, 37033
		},
		{
		197, 37046
		},
		{
		198, 37059
		},
		{
		199, 37072
		},
		{
		200, 37085
		},
		{
		201, 37098
		},
		{
		202, 37111
		},
		{
		203, 37124
		},
		{
		204, 37137
		},
		{
		205, 37150
		},
		{
		206, 37163
		},
		{
		207, 37176
		},
		{
		208, 37189
		},
		{
		209, 37202
		},
		{
		210, 37215
		},
		{
		211, 37228
		},
		{
		212, 37241
		},
		{
		213, 37254
		},
		{
		214, 37267
		},
		{
		215, 37280
		},
		{
		216, 37293
		},
		{
		217, 37306
		},
		{
		218, 37319
		},
		{
		219, 37332
		},
		{
		220, 37345
		},
		{
		221, 37358
		},
		{
		222, 37371
		},
		{
		223, 37384
		},
		{
		224, 37397
		},
		{
		225, 37410
		},
		{
		226, 37423
		},
		{
		227, 37436
		},
		{
		228, 37449
		},
		{
		229, 37462
		},
		{
		230, 37475
		},
		{
		231, 37500
		},
		{
		232, 37502
		},
		{
		233, 37504
		},
		{
		234, 37506
		},
		{
		235, 37508
		},
		{
		236, 37510
		},
		{
		237, 37512
		},
		{
		238, 37514
		},
		{
		239, 37516
		},
		{
		240, 37518
		},
		{
		241, 37520
		},
		{
		242, 37522
		},
		{
		243, 37524
		},
		{
		244, 37526
		},
		{
		245, 37528
		},
		{
		246, 37530
		},
		{
		247, 37532
		},
		{
		248, 37534
		},
		{
		249, 37536
		},
		{
		250, 37538
		},
		{
		251, 37540
		},
		{
		252, 37542
		},
		{
		253, 37544
		},
		{
		254, 37546
		},
		{
		255, 37548
		},
		{
		256, 37550
		},
		{
		257, 37552
		},
		{
		258, 37554
		},
		{
		259, 37556
		},
		{
		260, 37558
		},
		{
		261, 37560
		},
		{
		262, 37562
		},
		{
		263, 37564
		},
		{
		264, 37566
		},
		{
		265, 37568
		},
		{
		266, 37570
		},
		{
		267, 37572
		},
		{
		268, 37574
		},
		{
		269, 37576
		},
		{
		270, 37578
		},
		{
		271, 37580
		},
		{
		272, 37582
		},
		{
		273, 37584
		},
		{
		274, 37586
		},
		{
		275, 37588
		},
		{
		276, 37590
		},
		{
		277, 37592
		},
		{
		278, 37594
		},
		{
		279, 37596
		},
		{
		280, 37598
		},
		{
		281, 37600
		},
		{
		282, 37602
		},
		{
		283, 37604
		},
		{
		284, 37606
		},
		{
		285, 37608
		},
		{
		286, 37610
		},
		{
		287, 37612
		},
		{
		288, 37614
		},
		{
		289, 37616
		},
		{
		290, 37618
		},
		{
		291, 37620
		},
		{
		292, 37622
		},
		{
		293, 37624
		},
		{
		294, 37626
		},
		{
		295, 37628
		},
		{
		296, 37630
		},
		{
		297, 37632
		},
		{
		298, 37634
		},
		{
		299, 37636
		},
		{
		300, 37700
		},
		{
		350, 37800
		},
		{
		400, 37900
		}
};


/* right motor */
uint16_t		g_Table_Distance2Encoder_RhtMotor[274][2] =
{
		{
		29, 18700
		},
		{
		30, 18900
		},
		{
		31, 19000
		},
		{
		32, 19333
		},
		{
		33, 19666
		},
		{
		34, 19999
		},
		{
		35, 20332
		},
		{
		36, 20665
		},
		{
		37, 21000
		},
		{
		38, 21750
		},
		{
		39, 22500
		},
		{
		40, 23250
		},
		{
		41, 24000
		},
		{
		42, 24750
		},
		{
		43, 25500
		},
		{
		44, 26500
		},
		{
		45, 27500
		},
		{
		46, 28500
		},
		{
		47, 29500
		},
		{
		48, 29807
		},
		{
		49, 30114
		},
		{
		50, 30421
		},
		{
		51, 30728
		},
		{
		52, 31035
		},
		{
		53, 31342
		},
		{
		54, 31649
		},
		{
		55, 31956
		},
		{
		56, 32263
		},
		{
		57, 32570
		},
		{
		58, 32877
		},
		{
		59, 33184
		},
		{
		60, 33500
		},
		{
		61, 33666
		},
		{
		62, 33832
		},
		{
		63, 33998
		},
		{
		64, 34164
		},
		{
		65, 34330
		},
		{
		66, 34500
		},
		{
		67, 34595
		},
		{
		68, 34690
		},
		{
		69, 34785
		},
		{
		70, 34880
		},
		{
		71, 34975
		},
		{
		72, 35070
		},
		{
		73, 35165
		},
		{
		74, 35260
		},
		{
		75, 35355
		},
		{
		76, 35450
		},
		{
		77, 35545
		},
		{
		78, 35640
		},
		{
		79, 35735
		},
		{
		80, 35830
		},
		{
		81, 35925
		},
		{
		82, 36020
		},
		{
		83, 36115
		},
		{
		84, 36210
		},
		{
		85, 36305
		},
		{
		86, 36400
		},
		{
		87, 36500
		},
		{
		88, 36666
		},
		{
		89, 36832
		},
		{
		90, 36998
		},
		{
		91, 37164
		},
		{
		92, 37330
		},
		{
		93, 37496
		},
		{
		94, 37662
		},
		{
		95, 37828
		},
		{
		96, 37994
		},
		{
		97, 38160
		},
		{
		98, 38326
		},
		{
		99, 38492
		},
		{
		100, 38658
		},
		{
		101, 38824
		},
		{
		102, 38990
		},
		{
		103, 39156
		},
		{
		104, 39322
		},
		{
		105, 39488
		},
		{
		106, 39654
		},
		{
		107, 39820
		},
		{
		108, 39986
		},
		{
		109, 40152
		},
		{
		110, 40318
		},
		{
		111, 40484
		},
		{
		112, 40650
		},
		{
		113, 40816
		},
		{
		114, 40982
		},
		{
		115, 41148
		},
		{
		116, 41314
		},
		{
		117, 41500
		},
		{
		118, 41552
		},
		{
		119, 41604
		},
		{
		120, 41656
		},
		{
		121, 41708
		},
		{
		122, 41760
		},
		{
		123, 41812
		},
		{
		124, 41864
		},
		{
		125, 41916
		},
		{
		126, 41968
		},
		{
		127, 42020
		},
		{
		128, 42072
		},
		{
		129, 42124
		},
		{
		130, 42176
		},
		{
		131, 42228
		},
		{
		132, 42280
		},
		{
		133, 42332
		},
		{
		134, 42384
		},
		{
		135, 42436
		},
		{
		136, 42488
		},
		{
		137, 42540
		},
		{
		138, 42592
		},
		{
		139, 42644
		},
		{
		140, 42696
		},
		{
		141, 42748
		},
		{
		142, 42800
		},
		{
		143, 42852
		},
		{
		144, 42904
		},
		{
		145, 42956
		},
		{
		146, 43008
		},
		{
		147, 43060
		},
		{
		148, 43112
		},
		{
		149, 43164
		},
		{
		150, 43216
		},
		{
		151, 43268
		},
		{
		152, 43320
		},
		{
		153, 43372
		},
		{
		154, 43424
		},
		{
		155, 43500
		},
		{
		156, 43532
		},
		{
		157, 43564
		},
		{
		158, 43596
		},
		{
		159, 43628
		},
		{
		160, 43660
		},
		{
		161, 43692
		},
		{
		162, 43724
		},
		{
		163, 43756
		},
		{
		164, 43788
		},
		{
		165, 43820
		},
		{
		166, 43852
		},
		{
		167, 43884
		},
		{
		168, 43916
		},
		{
		169, 43948
		},
		{
		170, 43980
		},
		{
		171, 44012
		},
		{
		172, 44044
		},
		{
		173, 44076
		},
		{
		174, 44108
		},
		{
		175, 44140
		},
		{
		176, 44172
		},
		{
		177, 44204
		},
		{
		178, 44236
		},
		{
		179, 44268
		},
		{
		180, 44300
		},
		{
		181, 44332
		},
		{
		182, 44364
		},
		{
		183, 44396
		},
		{
		184, 44428
		},
		{
		185, 44460
		},
		{
		186, 44492
		},
		{
		187, 44524
		},
		{
		188, 44556
		},
		{
		189, 44588
		},
		{
		190, 44620
		},
		{
		191, 44652
		},
		{
		192, 44684
		},
		{
		193, 44716
		},
		{
		194, 44748
		},
		{
		195, 44780
		},
		{
		196, 44812
		},
		{
		197, 44844
		},
		{
		198, 44876
		},
		{
		199, 44908
		},
		{
		200, 44940
		},
		{
		201, 44972
		},
		{
		202, 45004
		},
		{
		203, 45036
		},
		{
		204, 45068
		},
		{
		205, 45100
		},
		{
		206, 45132
		},
		{
		207, 45164
		},
		{
		208, 45196
		},
		{
		209, 45228
		},
		{
		210, 45260
		},
		{
		211, 45292
		},
		{
		212, 45324
		},
		{
		213, 45356
		},
		{
		214, 45388
		},
		{
		215, 45420
		},
		{
		216, 45452
		},
		{
		217, 45484
		},
		{
		218, 45516
		},
		{
		219, 45548
		},
		{
		220, 45580
		},
		{
		221, 45612
		},
		{
		222, 45644
		},
		{
		223, 45676
		},
		{
		224, 45708
		},
		{
		225, 45740
		},
		{
		226, 45772
		},
		{
		227, 45804
		},
		{
		228, 45836
		},
		{
		229, 45868
		},
		{
		230, 45900
		},
		{
		231, 46000
		},
		{
		232, 46002
		},
		{
		233, 46004
		},
		{
		234, 46006
		},
		{
		235, 46008
		},
		{
		236, 46010
		},
		{
		237, 46012
		},
		{
		238, 46014
		},
		{
		239, 46016
		},
		{
		240, 46018
		},
		{
		241, 46020
		},
		{
		242, 46022
		},
		{
		243, 46024
		},
		{
		244, 46026
		},
		{
		245, 46028
		},
		{
		246, 46030
		},
		{
		247, 46032
		},
		{
		248, 46034
		},
		{
		249, 46036
		},
		{
		250, 46038
		},
		{
		251, 46040
		},
		{
		252, 46042
		},
		{
		253, 46044
		},
		{
		254, 46046
		},
		{
		255, 46048
		},
		{
		256, 46050
		},
		{
		257, 46052
		},
		{
		258, 46054
		},
		{
		259, 46056
		},
		{
		260, 46058
		},
		{
		261, 46060
		},
		{
		262, 46062
		},
		{
		263, 46064
		},
		{
		264, 46066
		},
		{
		265, 46068
		},
		{
		266, 46070
		},
		{
		267, 46072
		},
		{
		268, 46074
		},
		{
		269, 46076
		},
		{
		270, 46078
		},
		{
		271, 46080
		},
		{
		272, 46082
		},
		{
		273, 46084
		},
		{
		274, 46086
		},
		{
		275, 46088
		},
		{
		276, 46090
		},
		{
		277, 46092
		},
		{
		278, 46094
		},
		{
		279, 46096
		},
		{
		280, 46098
		},
		{
		281, 46100
		},
		{
		282, 46102
		},
		{
		283, 46104
		},
		{
		284, 46106
		},
		{
		285, 46108
		},
		{
		286, 46110
		},
		{
		287, 46112
		},
		{
		288, 46114
		},
		{
		289, 46116
		},
		{
		290, 46118
		},
		{
		291, 46120
		},
		{
		292, 46122
		},
		{
		293, 46124
		},
		{
		294, 46126
		},
		{
		295, 46128
		},
		{
		296, 46130
		},
		{
		297, 46132
		},
		{
		298, 46134
		},
		{
		299, 46136
		},
		{
		300, 46200
		},
		{
		350, 46300
		},
		{
		400, 46400
		}
};



