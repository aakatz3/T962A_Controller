/*
 * keypad.c - Keypad interface for T-962 reflow controller
 *
 * Copyright (C) 2014 Werner Johansson, wj@unifiedengineering.se
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> /* For uint32_t definition */
#include <stdbool.h> /* For true/false definition */
#include "common_defs.h"
#include "keypad.h"
#include "io.h"
#include "schedule.h"




// keyboard key mappings
#define F1KEY_PORTBIT (BIT_0)
#define F2KEY_PORTBIT (BIT_1)
#define F3KEY_PORTBIT (BIT_2)
#define F4KEY_PORTBIT (BIT_3)
#define S_KEY_PORTBIT (BIT_4)

#define KEYREPEATDELAY (6)



// ********************************************** //
// ****  global variables  declarations here **** //

// declare the main config global structure
extern _REFLOW_CONFIG_ g_Config;

// ********************************************** //
// ********************************************** //







//static uint32_t Keypad_GetRaw(void)
uint32_t Keypad_GetRaw(void)
{
    // return the keypad state
    return (~PORTE & KEYBOARD);
}
/**/
/***********************************************************************************/


/**
**  @brief Keypad_Work the function for 
**	@param[in,out]
**	@return 
*/
static int32_t Keypad_Work(void) 
{
	static uint32_t laststate = 0;
	static uint16_t laststateunchangedctr = 0;
	uint32_t keypadstate = 0;
	uint32_t inverted = Keypad_GetRaw();
	uint32_t changed = inverted ^ laststate;

	// At this point we only care about when button is pressed, not released
	changed &= inverted;

	if (laststate != inverted)
    {
		laststate = inverted;
		laststateunchangedctr = 0;
	} 
    else
    {
		laststateunchangedctr++;
		if (laststateunchangedctr > KEYREPEATDELAY)
        {
			changed = laststate; // Feed key repeat
			// For accelerating key repeats
			keypadstate |= ((laststateunchangedctr - KEYREPEATDELAY) << 16);
		}
	}

	if (changed) 
    {
		if (changed & F1KEY_PORTBIT) keypadstate |= KEY_F1;
		if (changed & F2KEY_PORTBIT) keypadstate |= KEY_F2;
		if (changed & F3KEY_PORTBIT) keypadstate |= KEY_F3;
		if (changed & F4KEY_PORTBIT) keypadstate |= KEY_F4;
		if (changed & S_KEY_PORTBIT) keypadstate |= KEY_S;
	}

	g_Config.latchedkeypadstate &= 0xffff;
	g_Config.latchedkeypadstate |= keypadstate; // Make sure software actually sees the transitions

	if (keypadstate & 0xff) 
    {
		//printf("[KEYPAD %02x]",keypadstate & 0xff);
		Sched_SetState(MAIN_WORK, 2, 0); // Wake up main task to update UI
	}
    // return next time tick..
	return TICKS_MS(100);
}
/**/
/***********************************************************************************/


/**
**  @brief Keypad_Get the function for 
**	@param[in,out]
**	@return 
*/
uint32_t Keypad_Get(void)
{
	uint32_t retval = g_Config.latchedkeypadstate;
	g_Config.latchedkeypadstate = 0;
	return retval;
}
/**/
/***********************************************************************************/



/**
**  @brief Keypad_Init the function for 
**	@param[in,out]
**	@return 
*/
void Keypad_Init(void)
{
    #ifndef KEYBOARD_INTERRUPTS
	Sched_SetWorkfunc(KEYPAD_WORK, Keypad_Work);    
    #endif

	printf("\nWaiting for keys to be released... ");
	// Note that if this takes longer than ~1 second the watchdog will bite
	while (Keypad_GetRaw());
	printf("Done!");

    #ifndef KEYBOARD_INTERRUPTS
	// Potential noise gets suppressed as well
	Sched_SetState(KEYPAD_WORK, 1, TICKS_MS(250)); // Wait 250ms before starting to scan the keypad
    #endif

    return;
}
/**/
/***********************************************************************************/




//void __attribute__((vector(_CHANGE_NOTICE_E_VECTOR), interrupt(IPL7AUTO), nomips16)) CNInterrupt(void)
void __ISR_AT_VECTOR (_CHANGE_NOTICE_E_VECTOR, IPL7SRS) CNInterrupt(void)
{  
    uint8_t keyspressed = 0;
    
    // clear keyboard ints
    keyspressed = (CNFE & KEYBOARD);
    CNFECLR = 0xFF;
    IFS3CLR =_IFS3_CNEIF_MASK;
    
    if (keyspressed != 0) {
        Sched_SetState(MAIN_WORK, 2, 0); // Wake up main task to update UI
        g_Config.latchedkeypadstate = keyspressed;
    }
}
/**/
/********************************************************************************************************/