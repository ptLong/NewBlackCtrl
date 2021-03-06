/*
 * ======== Standard MSP430 includes ========
 */

#include "main.h"
/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/Grace.h>

typedef enum {
	CTRL_Buttons_none = 0,
	CTRL_Buttons_Timer,
	CTRL_Buttons_Lamp,
	CTRL_Buttons_Minus,
	CTRL_Buttons_Plus,
	CTRL_Buttons_Auto
} CTRL_Buttons_e;

//#define TICAPS_NUM_OF_SCKEY	5
//654
#define TIMER_THRESHOLD		(418)
//1016
#define PLUS_THRESHOLD		(538)
//932
#define MINUS_THRESHOLD		(488)
//946
#define LIGHT_THRESHOLD		(488)
//931
#define AUTO_THRESHOLD		(488)


TICAPS_Sckey_Obj gTimer_key = {
	.inputPxselRegister = (uint8_t *)&P3SEL,
	.inputPxsel2Register = (uint8_t *)&P3SEL2,
	.inputBits = BIT0,

	.threshold = TIMER_THRESHOLD,
};

TICAPS_Sckey_Obj gLight_key = {
	.inputPxselRegister = (uint8_t *)&P2SEL,
	.inputPxsel2Register = (uint8_t *)&P2SEL2,
	.inputBits = BIT0,

	.threshold = LIGHT_THRESHOLD,
};

TICAPS_Sckey_Obj gAuto_key = {
	.inputPxselRegister = (uint8_t *)&P3SEL,
	.inputPxsel2Register = (uint8_t *)&P3SEL2,
	.inputBits = BIT1,

	.threshold = AUTO_THRESHOLD,
};

TICAPS_Sckey_Obj gMinus_key = {
	.inputPxselRegister = (uint8_t *)&P1SEL,
	.inputPxsel2Register = (uint8_t *)&P1SEL2,
	.inputBits = BIT4,

	.threshold = MINUS_THRESHOLD,
};

TICAPS_Sckey_Obj gPlus_key = {
	.inputPxselRegister = (uint8_t *)&P1SEL,
	.inputPxsel2Register = (uint8_t *)&P1SEL2,
	.inputBits = BIT5,

	.threshold = PLUS_THRESHOLD,
};



TICAPS_Sckey_Obj* gSckeys[] = {
	&gTimer_key,
	&gLight_key,
	&gMinus_key,
	&gPlus_key,
	&gAuto_key,
	NULL   // terminator
};

MAIN_Obj gMain;

#ifdef DEBUGGING_TOUCH
uint16_t data[16];

void FLASH_write_segC (uint16_t value[16]);
void FLASH_read_segC (uint16_t value[16]);

#endif

/*
 *  ======== main ========
 */
int main(void)
{
	gMain.initialized = 0;
	//uint16_t i;
	Grace_init();                   // Activate Grace-generated configuration
	//All TP are now high
	//while(LED_INPUT_STATE)
	{

	}
	BUTTON_NONE_DETECTED;
	//wait for LED input to turn low.
	//delay
	//make all TP low

	//BUTTON_NONE_DETECTED;
	//OUTPUT_SEC_HIGH; //make sure the SEC line is high when power on
	__enable_interrupt();

	TICAPS_sckey_init(gSckeys);

	gMain.initialized = 1;
	//FLASH_write_segC (data);
	while(1)
	{
		gMain.initialized = 1;

#ifdef DEBUGGING_TOUCH


		if(gAuto_key.detected){
			FLASH_read_segC(data);
			if(data[0] < gTimer_key.max_delta)
				data[0] = gTimer_key.max_delta;
			if(data[1] < gLight_key.max_delta)
				data[1] = gLight_key.max_delta;
			if(data[2] < gMinus_key.max_delta)
				data[2] = gMinus_key.max_delta;
			if(data[3] < gPlus_key.max_delta)
				data[3] = gPlus_key.max_delta;
			if(data[4] < gAuto_key.max_delta)
				data[4] = gAuto_key.max_delta;

				data[5] = 0x0A0A;
				data[6] = 0x0A0A;
				data[7] = 0x0A0A;
				data[8] = 0x0A0A;

			FLASH_write_segC (data);
		}
#endif

	}

	//return (0);
}

#ifdef DEBUGGING_TOUCH
void FLASH_write_segC (uint16_t value[16])
{
	uint16_t *Flash_ptr;                          // Flash pointer
	uint16_t i;

	Flash_ptr = (uint16_t *) 0x1040;              // Initialize Flash pointer to segment C
	FCTL1 = FWKEY + ERASE;                    // Set Erase bit
	FCTL3 = FWKEY;                            // Clear Lock bit
	*Flash_ptr = 0;                           // Dummy write to erase Flash segment

	FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

	for (i=0; i<16; i++){
		*Flash_ptr++ = value[i];                   // Write value to flash
	}

	FCTL1 = FWKEY;                            // Clear WRT bit
	FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
void FLASH_read_segC(uint16_t value[16])
{
	uint16_t *Flash_ptr;                          // Flash pointer
	uint16_t i;

	Flash_ptr = (uint16_t *) 0x1040;              // Initialize Flash pointer to segment C

	for (i=0; i<16; i++){
		value[i] = *Flash_ptr++ ;                   // read value fr flash
	}
}
#endif


void CTRL_run(void)
{
	CTRL_Buttons_e bt_detected = CTRL_Buttons_none;


	if(gTimer_key.detected){
		LED_TIMER_TOGGLE;
		bt_detected = CTRL_Buttons_Timer;
	}
	if(gLight_key.detected){
		LED_LAMP_TOGGLE;
		bt_detected = CTRL_Buttons_Lamp;
	}
	if(gPlus_key.detected){
		LED_PLUSMINUS_TOGGLE;
		bt_detected = CTRL_Buttons_Plus;
	}
	if(gMinus_key.detected){
		LED_PLUSMINUS_TOGGLE;
		bt_detected = CTRL_Buttons_Minus;
	}
	if(gAuto_key.detected){
		LED_AUTO_TOGGLE;
		bt_detected = CTRL_Buttons_Auto;

	}

	switch(bt_detected){
	case CTRL_Buttons_Timer:
		BUTTON_TIMER_DETECTED;
		break;
	case CTRL_Buttons_Plus:
		BUTTON_PLUS_DETECTED;
		break;
	case CTRL_Buttons_Minus:
		BUTTON_MINUS_DETECTED;
		break;
	case CTRL_Buttons_Lamp:
		BUTTON_LAMP_DETECTED;
		break;

	default:
		if(LED_INPUT_STATE){
			LED_ALL_ON;
		}else{
			LED_ALL_OFF;
		}
		BUTTON_NONE_DETECTED;
		break;

	};

/*
	if(LED_INPUT_STATE){
		LED_ALL_ON;
	}else{
		LED_ALL_OFF;
	}
*/
}

void MainISR_1s(void)
{
	static uint8_t cnt_16hz=0;
	static uint8_t cnt_16hz_2 = 0;

	if(cnt_16hz == 16){
		cnt_16hz = 0;
		OUTPUT_SEC_TOGGLE;
	#ifdef DEBUGGING
		//LED_AUTO_TOGGLE;
	#endif
	}else{
		cnt_16hz++;
	}

	if(cnt_16hz_2 == 1){
		cnt_16hz_2=0;
		if(gMain.initialized == 1){
			TICAPS_sckey_run(gSckeys);
			CTRL_run();
		}
	}else{
		cnt_16hz_2++;
	}


}
