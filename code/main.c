/*
 * Dishwasher.c - ELECTROLUX
 *
 * Created: 6/24/2017 1:59:46 AM
 * Author : Neutral Atom

 * PINC0 - Door Switch
 * PINC1 - Tank LL	
 * PINC2 - Tank HL
 * PINC3 - Booster Temp.
 * PINC4 - Booster LL
 * PINC5 - Booster HL
 
 */


#include <atmel_start.h>
#include <avr/wdt.h>

#include "EEPROM_ATM328.h"
#define uiaddress 0x0001


int sec=0;
int min=0;
int state=0;
volatile int set_time,pause,rinse_time,delay;
int wash_flag;
int READBYTE=0x00;



enum status{  HOLD,
			  READY,
			  RUN		
			} wash_status;  


int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	cli();
	READBYTE=EEPROM_read(uiaddress);
	//READBYTE=2;

	OCR1A=7813;			// 1 sec Count
	sei();

	state=1;	
	
		while (1) {

		if(READBYTE<=0x64)
		{
			//state=1;
			wdt_reset();

			if (DoorSw_get_level())
			{
				Fill_set_level(false);
				Tank_Heater_set_level(false);
				Bst_Heater_set_level(false);
				Usr_def_set_level(false);
				Rinse_Pump_get_level(false);
				Wash_Pump_set_level(false);
				state=1;
		
			}

		// CYCLES for DISHWASHER

					switch (state)
					{
						case 1: fill_cycle();
								break;
						case 2: wash_cycle();
								//state=3;
								break;
						case 3: dwell_cycle();
								//state=4;
								break;
						case 4: rinse_cycle();
								//state=5;
								break;
						case 5: reset_cycle();
								//state=0;
								break;
						default: 
								break;
					}  //exit to switch
			} // exit to if
			else
			{
				wdt_reset();

			}
		}    //exit to while 1
	
}


void booster_cycle()
{
	wdt_reset();

	// BOOSTER FILLING CYCLE
	
	if(!(Bst_HL_get_level()) && (!(Bst_LL_get_level())))
	{
		Fill_set_level(false);
	}
	else if ((!(Bst_HL_get_level()) && (Bst_LL_get_level())))
	{
		Fill_set_level(false);
	}
	else if (Bst_LL_get_level())
	{
		Fill_set_level(true);
	}

	//BOOSTER TEMP CYCLE
	
	if((!(Bst_LL_get_level())) && (!(Bst_temp_get_level())))
	{
		Bst_Heater_set_level(true);
	}
	else if (Bst_LL_get_level())
	{
		Bst_Heater_set_level(false);
	}
	else
	{
		Bst_Heater_set_level(false);
	}

}


void fill_cycle()
{
	
	wdt_reset();
	
	if(!(DoorSw_get_level()))
	{
	
	if ((!(TankHL_get_level()))&& (!(TankLL_get_level())))
	{
		Fill_set_level(false);
		Tank_Heater_set_level(true);
		Bst_Heater_set_level(true);

			do 
			{
				wdt_reset();

				if (TankLL_get_level())
				{
					Tank_Heater_set_level(false);
					Bst_Heater_set_level(false);
				}
				

			} while (!(Bst_temp_get_level()));

			Tank_Heater_set_level(false);
			Bst_Heater_set_level(false);
			wash_status=READY;
			state=2;
	}
	else if (!(TankLL_get_level()))
	{
		Fill_set_level(true);
		Tank_Heater_set_level(true);
		Bst_Heater_set_level(false);
	}
	else if (TankLL_get_level())
	{
		Fill_set_level(true);
		Tank_Heater_set_level(false);
		Bst_Heater_set_level(false);
		Usr_def_set_level(true);
	}
	}
	else 
	{
		Fill_set_level(false);
		Tank_Heater_set_level(false);
		Bst_Heater_set_level(false);
		Usr_def_set_level(false);
		Rinse_Pump_set_level(false);
		Wash_Pump_set_level(false);
		state=1;
	}

}


void wash_cycle()
{

	wdt_reset();
	

	// Check if ss60,ss90,ss120 is low
		if(SS60_get_level())
		{
			set_time=60;
		}
		else if(SS90_get_level())
		{
			set_time=90;
		}
		else if(SS120_get_level())
		{
			set_time=120;
		}
		else
		{
			set_time=60;
		}

		//delay added for 3 sec

		delay=7;
		do
		{
			wdt_reset();

		} while (delay!=0);
		
	
		//DOOR ON/OFF SIGNAL TO MAKE WASH CYCLE ON

		if (DoorSw_get_level())
		{
			wash_status=READY;
		}
		else if ((wash_status==READY) && (!(DoorSw_get_level())))
		{
			wash_status=RUN;
			wash_flag=1;
		}

		
		//AFTER RUN STATUS WASHING CYCLE BEGINS

		if (wash_status==RUN)
		{
			wash_status=HOLD;
			wash_flag=1;
			do
			{
				wdt_reset();
				
				//booster_cycle();

				Wash_Pump_set_level(true);
				
				if(!(Bst_temp_get_level()))
				{
					Bst_Heater_set_level(true);
				}
				else
				{
					Bst_Heater_set_level(false);
				}
				
				if (DoorSw_get_level())
				{
					Wash_Pump_set_level(false);
					state=1;
					break;
				}

				// Added on 12/7/2017 ***********
				
				if (TankLL_get_level())
				{
					Tank_Heater_set_level(false);
				}
				else if (!(TankLL_get_level()))
				{
					Tank_Heater_set_level(true);
				}

				//*******************************
				
			} while (set_time!=0);

			Wash_Pump_set_level(false);
			state=3;
		}
		else
		{
			Wash_Pump_set_level(false);
		}		
	
}


void dwell_cycle()
{
	wdt_reset();
	pause=5;

	do 
	{
		wdt_reset();
		
		if(!(Bst_temp_get_level()))
		{
			Bst_Heater_set_level(true);
		}
		else
		{
			Bst_Heater_set_level(false);
		}

		// Added on 12/7/2017 ***********
						
		if (TankLL_get_level())
		{
			Tank_Heater_set_level(false);
		}
		else if (!(TankLL_get_level()))
		{
			Tank_Heater_set_level(true);
		}

		//*******************************

	} while (pause!=0);

	state=4;
}


void rinse_cycle()
{
	wdt_reset();

	if(DIPSW1_get_level())
	{
		rinse_time=9;
	}
	else
	{
		rinse_time=12;
	}
	

	do 
	{	
		if(DoorSw_get_level())
		{
			state=1;
			break;

		}

		if(!(Bst_temp_get_level()))
		{
			Bst_Heater_set_level(true);
		}
		else
		{
			Bst_Heater_set_level(false);
		}

		Fill_set_level(true);
		wdt_reset();

		// Added on 12/7/2017 ***********
		
		if (TankLL_get_level())
		{
			Tank_Heater_set_level(false);
		}
		else if (!(TankLL_get_level()))
		{
			Tank_Heater_set_level(true);
		}

		//*******************************

	} while (rinse_time!=0);

	state=5;

}


void reset_cycle()
{	
	wdt_reset();

	Fill_set_level(false);
	Tank_Heater_set_level(false);
	Bst_Heater_set_level(false);
	Usr_def_set_level(false);
	Rinse_Pump_get_level(false);
	Wash_Pump_set_level(false);
	
	if(!(Bst_temp_get_level()))
	{
		Bst_Heater_set_level(true);
	}
	else
	{
		Bst_Heater_set_level(false);
	}

	// Added on 12/7/2017 ***********
	
	if (TankLL_get_level())
	{
		Tank_Heater_set_level(false);
	}
	else if (!(TankLL_get_level()))
	{
		Tank_Heater_set_level(true);
	}

	//*******************************

	if(DoorSw_get_level())
	{
		state=1;
		READBYTE++;

		cli();
		EEPROM_write(uiaddress, READBYTE);
		sei();
	}

	
}
