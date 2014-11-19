#include "common.h"
#include "sensors.h"
#include "usart.h"


RCC_ClocksTypeDef RCC_Clocks;

__IO uint32_t UserButtonPressed = 0;
__IO float HeadingValue = 0.0f;
float MagBuffer[3] = {0.0f}, AccBuffer[3] = {0.0f}, Buffer[3] = {0.0f};
uint8_t Xval, Yval = 0x00;

__IO uint8_t PrevXferComplete = 1;

float fNormAcc,fSinRoll,fCosRoll,fSinPitch,fCosPitch = 0.0f, RollAng = 0.0f, PitchAng = 0.0f;
float fTiltedX,fTiltedY = 0.0f;
uint8_t i = 0;

int main(void)
{
	/* SysTick end of count event each 10ms */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

	/* Initialize LEDs */
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	STM_EVAL_LEDInit(LED7);
	STM_EVAL_LEDInit(LED8);
	STM_EVAL_LEDInit(LED9);
	STM_EVAL_LEDInit(LED10);

	/* Initialize pushbutton */
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);



	USART2_Init(115200);
	printf("\fUART2 initialized\n\r");



	/* Reset UserButton_Pressed variable */
	UserButtonPressed = 0x00;

	/* Initialize compass */
	Compass_Config();

	/* Waiting User Button is pressed */
	while (1)
	{
		/* Read Compass data */
		Compass_ReadMag(MagBuffer);
		Compass_ReadAcc(AccBuffer);

		for(i=0;i<3;i++)
			AccBuffer[i] /= 100.0f;

		fNormAcc = sqrt((AccBuffer[0]*AccBuffer[0])+(AccBuffer[1]*AccBuffer[1])+(AccBuffer[2]*AccBuffer[2]));

		fSinRoll = -AccBuffer[1]/fNormAcc;
		fCosRoll = sqrt(1.0-(fSinRoll * fSinRoll));
		fSinPitch = AccBuffer[0]/fNormAcc;
		fCosPitch = sqrt(1.0-(fSinPitch * fSinPitch));
		if ( fSinRoll >0)
		{
			if (fCosRoll>0)
			{
				RollAng = acos(fCosRoll)*180/PI;
			}
			else
			{
				RollAng = acos(fCosRoll)*180/PI + 180;
			}
		}
		else
		{
			if (fCosRoll>0)
			{
				RollAng = acos(fCosRoll)*180/PI + 360;
			}
			else
			{
				RollAng = acos(fCosRoll)*180/PI + 180;
			}
		}

		if ( fSinPitch >0)
		{
			if (fCosPitch>0)
			{
				PitchAng = acos(fCosPitch)*180/PI;
			}
			else
			{
				PitchAng = acos(fCosPitch)*180/PI + 180;
			}
		}
		else
		{
			if (fCosPitch>0)
			{
				PitchAng = acos(fCosPitch)*180/PI + 360;
			}
			else
			{
				PitchAng = acos(fCosPitch)*180/PI + 180;
			}
		}

		if (RollAng >=360)
		{
			RollAng = RollAng - 360;
		}

		if (PitchAng >=360)
		{
			PitchAng = PitchAng - 360;
		}

		fTiltedX = MagBuffer[0]*fCosPitch+MagBuffer[2]*fSinPitch;
		fTiltedY = MagBuffer[0]*fSinRoll*fSinPitch+MagBuffer[1]*fCosRoll-MagBuffer[1]*fSinRoll*fCosPitch;

		HeadingValue = (float) ((atan2f((float)fTiltedY,(float)fTiltedX))*180)/PI;

		if (HeadingValue < 0)
		{
			HeadingValue = HeadingValue + 360;
		}

		if ((RollAng <= 40.0f) && (PitchAng <= 40.0f))
		{
			if (((HeadingValue < 25.0f)&&(HeadingValue >= 0.0f))||((HeadingValue >=340.0f)&&(HeadingValue <= 360.0f)))
			{
				STM_EVAL_LEDOn(LED10);
				STM_EVAL_LEDOff(LED3);
				STM_EVAL_LEDOff(LED6);
				STM_EVAL_LEDOff(LED7);
				STM_EVAL_LEDOff(LED4);
				STM_EVAL_LEDOff(LED8);
				STM_EVAL_LEDOff(LED9);
				STM_EVAL_LEDOff(LED5);
			}
			else  if ((HeadingValue <70.0f)&&(HeadingValue >= 25.0f))
			{
				STM_EVAL_LEDOn(LED9);
				STM_EVAL_LEDOff(LED6);
				STM_EVAL_LEDOff(LED10);
				STM_EVAL_LEDOff(LED3);
				STM_EVAL_LEDOff(LED8);
				STM_EVAL_LEDOff(LED5);
				STM_EVAL_LEDOff(LED4);
				STM_EVAL_LEDOff(LED7);
			}
			else  if ((HeadingValue < 115.0f)&&(HeadingValue >= 70.0f))
			{
				STM_EVAL_LEDOn(LED7);
				STM_EVAL_LEDOff(LED3);
				STM_EVAL_LEDOff(LED4);
				STM_EVAL_LEDOff(LED9);
				STM_EVAL_LEDOff(LED10);
				STM_EVAL_LEDOff(LED8);
				STM_EVAL_LEDOff(LED6);
				STM_EVAL_LEDOff(LED5);
			}
			else  if ((HeadingValue <160.0f)&&(HeadingValue >= 115.0f))
			{
				STM_EVAL_LEDOn(LED5);
				STM_EVAL_LEDOff(LED6);
				STM_EVAL_LEDOff(LED10);
				STM_EVAL_LEDOff(LED8);
				STM_EVAL_LEDOff(LED9);
				STM_EVAL_LEDOff(LED7);
				STM_EVAL_LEDOff(LED4);
				STM_EVAL_LEDOff(LED3);
			}
			else  if ((HeadingValue <205.0f)&&(HeadingValue >= 160.0f))
			{
				STM_EVAL_LEDOn(LED3);
				STM_EVAL_LEDOff(LED6);
				STM_EVAL_LEDOff(LED4);
				STM_EVAL_LEDOff(LED8);
				STM_EVAL_LEDOff(LED9);
				STM_EVAL_LEDOff(LED5);
				STM_EVAL_LEDOff(LED10);
				STM_EVAL_LEDOff(LED7);
			}
			else  if ((HeadingValue <250.0f)&&(HeadingValue >= 205.0f))
			{
				STM_EVAL_LEDOn(LED4);
				STM_EVAL_LEDOff(LED6);
				STM_EVAL_LEDOff(LED10);
				STM_EVAL_LEDOff(LED8);
				STM_EVAL_LEDOff(LED9);
				STM_EVAL_LEDOff(LED5);
				STM_EVAL_LEDOff(LED3);
				STM_EVAL_LEDOff(LED7);
			}
			else  if ((HeadingValue < 295.0f)&&(HeadingValue >= 250.0f))
			{
				STM_EVAL_LEDOn(LED6);
				STM_EVAL_LEDOff(LED9);
				STM_EVAL_LEDOff(LED10);
				STM_EVAL_LEDOff(LED8);
				STM_EVAL_LEDOff(LED3);
				STM_EVAL_LEDOff(LED5);
				STM_EVAL_LEDOff(LED4);
				STM_EVAL_LEDOff(LED7);
			}
			else  if ((HeadingValue < 340.0f)&&(HeadingValue >= 295.0f))
			{
				STM_EVAL_LEDOn(LED8);
				STM_EVAL_LEDOff(LED6);
				STM_EVAL_LEDOff(LED10);
				STM_EVAL_LEDOff(LED7);
				STM_EVAL_LEDOff(LED9);
				STM_EVAL_LEDOff(LED3);
				STM_EVAL_LEDOff(LED4);
				STM_EVAL_LEDOff(LED5);
			}
		}
		else
		{
			/* Toggle All LEDs */
			STM_EVAL_LEDToggle(LED7);
			STM_EVAL_LEDToggle(LED6);
			STM_EVAL_LEDToggle(LED10);
			STM_EVAL_LEDToggle(LED8);
			STM_EVAL_LEDToggle(LED9);
			STM_EVAL_LEDToggle(LED3);
			STM_EVAL_LEDToggle(LED4);
			STM_EVAL_LEDToggle(LED5);
			/* Delay 50ms */
			Delay(5);
		}
		
		printf("roll: %04f; pitch: %04f; yaw: %04f\n\r",
				RollAng, PitchAng, HeadingValue);
	}

}





#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif



/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                            */
/******************************************************************************/
/**
 * @brief  This function handles EXTI0_IRQ Handler.
 * @param  None
 * @retval None
 */
void EXTI0_IRQHandler(void)
{
	uint32_t i;
	if ((EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) == SET)&&(STM_EVAL_PBGetState(BUTTON_USER) != RESET))
	{
		/* Delay */
		for(i=0; i<0x7FFFF; i++);

		/* Wait for SEL button to be pressed  */
		while(STM_EVAL_PBGetState(BUTTON_USER) != RESET);
		/* Delay */
		for(i=0; i<0x7FFFF; i++);
		UserButtonPressed++;

		if (UserButtonPressed > 0x2)
		{
			UserButtonPressed = 0x0;
		}

		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
	}
}

