/*
 * pru_cape_demo.c
 *
 * This file contains the main() and other functions.
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include "hw_types.h"
#include "soc_AM335x.h"
#include "hw_control_AM335x.h"
#include "beaglebone.h"
#include "hw_cm_per.h"
#include "hw_prm_per.h"
#include "pru.h"
#include "pru_cape_demo.h"


/******************************************************************************
**              FUNCTION DEFINITIONS
******************************************************************************/

//******************************************************************************
//    UARTInit
//      This function initializes the UART console.
//******************************************************************************
void UARTInit(void)
{
	ConsoleUtilsInit();
}

//******************************************************************************
//    Main
//******************************************************************************
int main()
{

	//sets pin mux for PRU cape
	PRUCapePinmux();

    //Sets and Enables clock, Zeros memory, resets PRU
	PRUICSSInit();

	UARTInit();

	MainMenu();
}

//******************************************************************************
//    Main Menu
//******************************************************************************
void MainMenu(void)
{
	char choice[1];
	char LED; 
	char UART;
	char Audio;
	char Switch;
	char TempSensor;

	char* test="blank";
	unsigned int validChoice = 0;

	ConsoleUtilsPrintf("\n\n\n\n");
	ConsoleUtilsPrintf("\n**************************************************************\n");
	ConsoleUtilsPrintf("*                        PRU Cape Demo                       *\n");
	ConsoleUtilsPrintf("**************************************************************\n");
	ConsoleUtilsPrintf("                                                              \n");

	while(!validChoice)
	{
		ConsoleUtilsPrintf("\n\nWhat test would you like to run?\n\n");
		ConsoleUtilsPrintf("1. LEDs\n");
		ConsoleUtilsPrintf("2. Switches\n");
		ConsoleUtilsPrintf("3. Audio\n");
		ConsoleUtilsPrintf("4. Uart\n");
		ConsoleUtilsPrintf("5. Temp sensor\n");
		ConsoleUtilsPrintf("6. All\n");
		ConsoleUtilsGets(choice, 3);

		switch(choice[0])
		{
		case '1':
			test = "LEDs";
			ConsoleUtilsPrintf("\nYou chose %s\n", test);
			LED = LEDTest();
			break;
		case '2':
			test = "Switch";
			ConsoleUtilsPrintf("\nYou chose %s\n", test);
			Switch = SwitchTest();
			break;
		case '3':
			test = "Audio";
			ConsoleUtilsPrintf("\nYou chose %s\n", test);
			Audio = AudioTest();
			break;
		case '4':
			test = "Uart";
			ConsoleUtilsPrintf("\nYou chose %s\n", test);
			UART = UARTTest();

			break;
		case '5':
			test = "Temp sensor";
			ConsoleUtilsPrintf("\nYou chose %s\n", test);
			TempSensor = HDQTest();
			break;
		case '6':
			test = "All";
			ConsoleUtilsPrintf("\nYou chose %s\n", test);
			ConsoleUtilsPrintf("\nRunning all tests, loading... \n");

			LED = LEDTest();
			PRUICSSReset();

			Switch = SwitchTest();
			PRUICSSReset();

			Audio = AudioTest();
			PRUICSSReset();

			UART = UARTTest();
			PRUICSSReset();

			TempSensor = HDQTest();
			PRUICSSReset();

			ConsoleUtilsPrintf("\n\n\n**************************************************************\n");
			ConsoleUtilsPrintf("\n                     	 TEST RESULTS                             \n");
			ConsoleUtilsPrintf("\n**************************************************************\n");

			if((LED =='y') || (LED =='Y'))
				ConsoleUtilsPrintf("\nLED Pass");
			else
				ConsoleUtilsPrintf("\nLED Fail");

			if((Switch =='y') || (Switch =='Y'))
				ConsoleUtilsPrintf("\nSwitch Pass");
			else
				ConsoleUtilsPrintf("\nSwitch Fail");

			if((Audio =='y') || (Audio =='Y'))
				ConsoleUtilsPrintf("\nAUDIO Pass");
			else
				ConsoleUtilsPrintf("\nAUDIO Fail");

			if((UART =='y') || (UART =='Y'))
				ConsoleUtilsPrintf("\nUART Pass");
			else
				ConsoleUtilsPrintf("\nUART Fail");

			if((TempSensor =='y') || (TempSensor =='Y'))
				ConsoleUtilsPrintf("\nTemp Sensor Pass");
			else
				ConsoleUtilsPrintf("\nTemp Sensor Fail");

			ConsoleUtilsPrintf("\n\n\n");

			break;
		default:
			ConsoleUtilsPrintf("\nInvalid input.\n\n");
			break;
		}
	}

	validChoice = 0;

}

//******************************************************************************
//    LED Test
//      This function tests the 7 LEDs on the PRU Cape.
//******************************************************************************
int LEDTest(void)
{
	char answer[1];

	ConsoleUtilsPrintf("\n**************************************************************\n");
	ConsoleUtilsPrintf("\n                     	 LED TEST                            \n");
	ConsoleUtilsPrintf("\n**************************************************************\n");

	ConsoleUtilsPrintf("\nLoading PRU0 Instructions and Data...\n");

	PRUMemLoad(PRU_ICSS1, PRU0_IRAM, 0, sizeof(LED0_INST), (unsigned int*)LED0_INST);
	PRUMemLoad(PRU_ICSS1, PRU0_DRAM, 0, sizeof(LED0_DATA), (unsigned int*)LED0_DATA);

	ConsoleUtilsPrintf("\nLoading PRU1 Instructions and Data...\n");
	PRUMemLoad(PRU_ICSS1, PRU1_IRAM, 0, sizeof(LED1_INST), (unsigned int*)LED1_INST);
	PRUMemLoad(PRU_ICSS1, PRU1_DRAM, 0, sizeof(LED1_DATA), (unsigned int*)LED1_DATA);

	ConsoleUtilsPrintf("\nRunning PRUs...\n");
	PRUEnable(PRU_ICSS1, PRU0);
	PRUEnable(PRU_ICSS1, PRU1);

	ConsoleUtilsPrintf("\nDo you see all 7 LEDs flashing? y/n\n");
	ConsoleUtilsGets(answer, 3);

	PRUHalt(PRU_ICSS1, PRU0);
	PRUHalt(PRU_ICSS1, PRU1);

	return(answer[0]);
}

//******************************************************************************
//    Switch Test
//      This function tests the Push Button switches on the PRU Cape.
//      Pressing each switch will toggle an LED on the PRU Cape.
//******************************************************************************
int SwitchTest(void)
{
	char answer[1];

	ConsoleUtilsPrintf("\n**************************************************************\n");
	ConsoleUtilsPrintf("\n                     	 SWITCH TEST                            \n");
	ConsoleUtilsPrintf("\n**************************************************************\n");

	ConsoleUtilsPrintf("\nLoading PRU Instructions and Data.\n");
	PRUMemLoad(PRU_ICSS1, PRU0_IRAM, 0, sizeof(SW_INST), (unsigned int*)SW_INST);
	PRUMemLoad(PRU_ICSS1, PRU0_DRAM, 0, sizeof(SW_DATA), (unsigned int*)SW_DATA);

	ConsoleUtilsPrintf("\nRunning PRU0...\n");
	PRUEnable(PRU_ICSS1, PRU0);

	ConsoleUtilsPrintf("\nDo the switches turn on LEDs? y/n\n");
	ConsoleUtilsGets(answer, 3);

	PRUHalt(PRU_ICSS1, PRU0);

	return(answer[0]);
}

//******************************************************************************
//    Temp Sensor Test
//      This function tests the temp sensor on the PRU Cape. If the temperature
//      rises, a red LED will turn on.  If the temperature falls, a blue LED
//      will turn on.
//******************************************************************************
int HDQTest(void)
{
	char answer[1];

	ConsoleUtilsPrintf("\n**************************************************************\n");
	ConsoleUtilsPrintf("\n                     	 TEMP SENSOR TEST                            \n");
	ConsoleUtilsPrintf("\n**************************************************************\n");

	ConsoleUtilsPrintf("\nLoading PRU Instructions and Data.\n");

	PRUMemLoad(PRU_ICSS1, PRU0_IRAM, 0, sizeof(SLAVE_INST), (unsigned int*)SLAVE_INST);
	PRUMemLoad(PRU_ICSS1, PRU0_DRAM, 0, sizeof(SLAVE_DATA), (unsigned int*)SLAVE_DATA);

	PRUMemLoad(PRU_ICSS1, PRU1_IRAM, 0, sizeof(MASTER_INST), (unsigned int*)MASTER_INST);
	PRUMemLoad(PRU_ICSS1, PRU1_DRAM, 0, sizeof(MASTER_DATA), (unsigned int*)MASTER_DATA);

	PRUEnable(PRU_ICSS1, PRU1);
	PRUEnable(PRU_ICSS1, PRU0);

	ConsoleUtilsPrintf("\nDoes the red and blue LEDs light up during heating and \n cooling the temperature sensor? y/n\n");

	ConsoleUtilsGets(answer, 3);

	PRUHalt(PRU_ICSS1, PRU0);
	PRUHalt(PRU_ICSS1, PRU1);

	return(answer[0]);
}

//******************************************************************************
//    Audio Test
//      This function tests the audio output on the PRU Cape by playing a
//      single tone.
//******************************************************************************
int AudioTest(void)
{
	char answer[1];

	ConsoleUtilsPrintf("\n**************************************************************\n");
	ConsoleUtilsPrintf("\n                     	 AUDIO TEST                            \n");
	ConsoleUtilsPrintf("\n**************************************************************\n");

	ConsoleUtilsPrintf("\nLoading PRU Instructions and Data.\n");

	PRUHalt(PRU_ICSS1, PRU1);

	PRUMemLoad(PRU_ICSS1, PRU1_IRAM, 0, sizeof(AUDIO_INST), (unsigned int*)AUDIO_INST);
	PRUMemLoad(PRU_ICSS1, PRU1_DRAM, 0, sizeof(AUDIO_DATA), (unsigned int*)AUDIO_DATA);

	ConsoleUtilsPrintf("\nRunning PRU1...\n");

	PRUEnable(PRU_ICSS1, PRU1);

	ConsoleUtilsPrintf("\nAre the speakers making a tone? y/n\n");

	ConsoleUtilsGets(answer, 3);

	PRUHalt(PRU_ICSS1, PRU1);

	return(answer[0]);
}

//******************************************************************************
//    UART Test
//      This function tests the RS-232 UART port on the PRU Cape by requesting
//      the user type 5 characters in a console and echoing the characters back.
//******************************************************************************
int UARTTest(void)
{
	char answer[1];

	ConsoleUtilsPrintf("\n**************************************************************\n");
	ConsoleUtilsPrintf("\n                     	 UART TEST                            \n");
	ConsoleUtilsPrintf("\n**************************************************************\n");

	ConsoleUtilsPrintf("\nLoading PRU Instructions and Data.\n");

	PRUHalt(PRU_ICSS1, PRU1);

	PRUMemLoad(PRU_ICSS1, PRU1_IRAM, 0, sizeof(UART_INST), (unsigned int*)UART_INST);
	PRUMemLoad(PRU_ICSS1, PRU1_DRAM, 0, sizeof(UART_DATA), (unsigned int*)UART_DATA);

	PRUEnable(PRU_ICSS1, PRU1);

	ConsoleUtilsPrintf("\nDid the UART work? y/n\n");

	ConsoleUtilsGets(answer, 3);
	return(answer[0]);
}

//******************************************************************************
//    PRU Cape Pinmux
//      This function configures the pinmux for the PRU Cape.
//******************************************************************************
void PRUCapePinmux(void)
{

	//******************************************************************************
	//						    	LEDS	- PRU0
	//******************************************************************************

	//*********************************************
 	// Blue LED  = PR1_PRU0_GPO0
	//*********************************************
 	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_ACLKX ) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(5);

	//*********************************************
	// Green LED  = PR1_PRU0_GPO1
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_FSX ) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(5);

	//*********************************************
	// Orange LED = PR1_PRU0_GPO2
	//*********************************************
 	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_AXR0 ) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(5);

	//*********************************************
	// Red LED    = PR1_PRU0_GPO3
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_AHCLKR)  =  AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(5);



	//******************************************************************************
	//						    	LEDS	- PRU1
	//******************************************************************************

	//*********************************************
	// Blue LED  = PR1_PRU1_GPO3
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_LCD_DATA(3)) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(5);

	//*********************************************
	// Green LED = PR1_PRU1_GPO4
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_LCD_DATA(4)) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(5);

	//*********************************************
	// Red LED  = PR1_PRU1_GPO5
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_LCD_DATA(5)) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(5);



	//******************************************************************************
	//						    	Switches
	//******************************************************************************

	//*********************************************
	// Switch1 = PR1_PRU0_GPI5
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_FSR) = AM335X_PIN_INPUT | CONTROL_CONF_MUXMODE(6);

	//*********************************************
	// Switch2 = PR1_PRU0_GPI7
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_AHCLKX) = AM335X_PIN_INPUT | CONTROL_CONF_MUXMODE(6);


	//******************************************************************************
	//						      	    Audio
	//******************************************************************************

	//*********************************************
	// Audio DIN  = PR1_PRU1_GPO0
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_LCD_DATA(0)) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(5);

	//*********************************************
	// Audio SCLK = PR1_PRU1_GPO1
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_LCD_DATA(1)) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(5);

	//*********************************************
	// Audio SYNCn = PR1_PRU1_GPO2
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_LCD_DATA(2)) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(5);



	//******************************************************************************
	//						    	PRU HW UART
	//******************************************************************************

	//*********************************************
	// UART TXD  = PR1_UART0_TXD
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_UART_TXD(1)) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(5);

	//*********************************************
	// UART RXD = PR1_UART0_RXD
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_UART_RXD(1)) = AM335X_PIN_INPUT | CONTROL_CONF_MUXMODE(5);

	//*********************************************
	// UART RTS = PR1_UART0_RTS
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_D0) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(4);

	//*********************************************
	// UART CTS = PR1_UART0_CTS
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_SCLK) = AM335X_PIN_INPUT | CONTROL_CONF_MUXMODE(4);



	//******************************************************************************
	//						    	     LCD
	//******************************************************************************

	//*********************************************
	// LCD_RS  = PR1_PRU_EDIO_DATA_OUT6
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_LCD_DATA(6)) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(4);

	//*********************************************
	// LCD_E   = PR1_PRU_EDIO_DATA_OUT4
	//*********************************************
	*((unsigned int*) SOC_CONTROL_REGS + CONTROL_CONF_LCD_PCLK) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(4);

	//*********************************************
	// LCD_DATA4  = PR1_PRU_EDIO_DATA_OUT0
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_D1) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(6);

	//*********************************************
	// LCD_DATA5  = PR1_PRU_EDIO_DATA_OUT1
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_CS0) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(6);

	//*********************************************
	// LCD_DATA6  = PR1_PRU_EDIO_DATA_OUT2
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_LCD_VSYNC) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(4);

	//*********************************************
	// LCD_DATA7  = PR1_PRU_EDIO_DATA_OUT3
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_LCD_HSYNC) = AM335X_PIN_OUTPUT | CONTROL_CONF_MUXMODE(4);



	//******************************************************************************
	//						    	TEMP SENSOR
	//******************************************************************************

	//*********************************************
	// TEMP1 HDQ/1W output = PR1_PRU_EDIO_DATA_OUT5
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_LCD_AC_BIAS_EN) =  AM335X_PIN_INPUT | CONTROL_CONF_MUXMODE(4);

	//*********************************************
	// TEMP1 HDQ/1W input = PR1_PRU0_GPI14
	//*********************************************
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(14)) =  AM335X_PIN_INPUT | CONTROL_CONF_MUXMODE(6);

}
