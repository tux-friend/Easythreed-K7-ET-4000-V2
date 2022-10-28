/*
  blinkm.cpp - Library for controlling a BlinkM over i2c
  Created by Tim Koster, August 21 2013.
*/
#include "Marlin.h"
#include "cardreader.h"
#include "temperature.h"
#include "cardreader.h"
#include "stepper.h"

#include "configuration_store.h"



#define  BLINKM

#if ENABLED(BLINKM)
#include "blinkm.h"


uint16_t blink_time = 0;  
uint8_t print_key_flag = 0; 
float z_height_stop = 0; 
float temperature_protect_last = 0;
uint32_t protect_time = 0;
uint8_t print_pause = 0; //用来表示打印暂停

uint32_t sys_time = 0;

uint8_t print_cancel_raise;

void GoHomeCheck(void)
{
	static int pin_time;
	static int once=0;
	if(card.sdprinting==true||print_key_flag==2) 
	{
		return;
	} 
	if(!READ(HOME_PIN))
	{
		pin_time++;
	}
	else
	{
		once = 0;
		pin_time = 0;
	}
	
	if(pin_time>32)
	{
		if(once==0)
		{	
			once=1;
		}
	}
}

void BlinkLed(void)
{
	static uint32_t blink_time_previous=0;
	static uint32_t blink_time_start=0;
	if(blink_time == 0)
	{
		WRITE(LED_PIN,1);
		return;
	}
	if(blink_time > 3000)
	{
		WRITE(LED_PIN,0);
		return;
	}
	if(blink_time_previous!=blink_time)
	{
		blink_time_previous = blink_time;
		blink_time_start = millis();
	}
	if(millis()<blink_time_start+blink_time)
	{
		WRITE(LED_PIN,0);
	}
	else if(millis()<blink_time_start+2*blink_time)
	{
		WRITE(LED_PIN,1);
	}
	else
	{
		blink_time_start = millis();
	}	
}

void SendColors(byte red, byte grn, byte blu) {
  Wire.begin();
  Wire.beginTransmission(0x09);
  Wire.write('o');                    //to disable ongoing script, only needs to be used once
  Wire.write('n');
  Wire.write(red);
  Wire.write(grn);
  Wire.write(blu);
  Wire.endTransmission();
}


//
//
//
void stop_sys_current(void)
{

	
}


void PrintOneKey(void)
{
	static uint8_t key_status=0;
	static uint32_t key_time = 0;
	static uint8_t pause_flag = 0;

	if(key_status == 0) 
	{
		if(!READ(PRINT_START_PIN))
		{
			key_time = millis();
			key_status = 1;
		}
	}
	else if(key_status == 1) 
	{
		if(key_time+30<millis())
		{
			if(!READ(PRINT_START_PIN)) 
			{
				key_time = millis();
				key_status = 2;
			}
			else
			{
				key_status = 0;
			}
		}	
	}
	else if(key_status == 2) 
	{
		if(READ(PRINT_START_PIN))
		{
			if(key_time + 1200 > millis())
			{
				if(print_key_flag == 0) 
				{
					if(card.sdprinting == false) 
					{
						card.initsd();
						if(card.cardOK==false)
						{
							BLINK_LED(LED_OFF); 
							key_status = 0;
							key_time = 0;
							return;
						}
						uint16_t filecnt = card.getnrfilenames();
						card.getfilename(filecnt);
						while(card.filenameIsDir)
						{
							if(filecnt>1)
							{
								filecnt--;
								card.getfilename(filecnt);
							}
						}
						card.openFile(card.filename,true);
						card.startFileprint();
						BLINK_LED(LED_BLINK_2); 
						print_key_flag = 1;
					}
				}
				else if(print_key_flag == 1) 
				{
			
				//	MYSERIAL.print("pause");
					BLINK_LED(LED_ON);	
					card.pauseSDPrint();
					print_pause = 1;
					print_key_flag = 2;
				}
				else if(print_key_flag == 2) 
				{
				//	MYSERIAL.print("back");
					if(temperature_protect_last > 60)
					{
						target_temperature[0]= temperature_protect_last;
						temperature_protect_last = 0;
					}
					BLINK_LED(LED_BLINK_0);
					card.startFileprint();
					print_pause = 0;
					print_key_flag = 1;
				}
				else
				{
					print_key_flag = 0;
				}		
				
			}
			else 
			{
				if(print_key_flag==0)
				{
					if(IsRunning()) 
					{
						destination[Z_AXIS] += 10;
						prepare_move();
						
						z_height_stop = destination[Z_AXIS];
						int i=Z_HEIGHT_EEPROM;
						EEPROM_WRITE_VAR(i,z_height_stop);
						
				//		MYSERIAL.print("UP");
				//		MYSERIAL.print(z_height_stop);
					}
					
				}
				else
				{
					z_height_stop = get_absolute_position(Z_AXIS); 
					
			//		MYSERIAL.print("cancel");
			//		MYSERIAL.print(z_height_stop);
							  
					card.sdprinting = false;
					cancel_heatup = true; 
					cancel_gohome_ms = millis()+1000;
										
					card.closefile();; // switch off all heaters.
					
					quickStop();
					
					
				//	MYSERIAL.print("current_position[Z_AXIS]=");
				//	MYSERIAL.print(current_position[Z_AXIS]);
					
					BLINK_LED(LED_OFF);
				}
				print_key_flag = 0;	
				//key_status = 0;
				//key_time = 0;
			}
			key_status = 0;
			key_time = 0;
		}	
	}
	else
	{
		key_status = 0;
		key_time = 0;
	}
}





void feed_filament(void)
{
	if(IsRunning())
	{
	    MYSERIAL.print("jin_liao");
	    destination[E_AXIS] += 100;
	    feedrate = 1.5*60;
	    prepare_move();
	}
}

void retract_filament(void)
{
	if(IsRunning())
	{
  		    MYSERIAL.print("tui_liao");
            destination[E_AXIS] += 12;
	        feedrate = 3*60;
	        prepare_move();
		    destination[E_AXIS] += 7;
	        feedrate = 1.5*60;
	        prepare_move();

		destination[E_AXIS] -= 120;
		feedrate = 3*60;
		prepare_move();
	}
}



void LoadFilament(void)
{
	static uint32_t filament_time = 0;
	static uint8_t filament_status=0;
	static uint8_t filament_action=0;
	
	if(card.sdprinting==true||print_key_flag==2) 
	{
		return;
	} 
	
	if(filament_status == 0) //
	{
		if((!READ(RETRACT_PIN))||(!READ(FEED_PIN)))
		{
			filament_status++;
			filament_time = millis();
		}
	}
	else if(filament_status == 1) 
	{
		if(filament_time+20<millis())
		{
			if((!READ(RETRACT_PIN))||(!READ(FEED_PIN)))
			{
				setTargetHotend0(195); 
                setTargetHotend0(195);
				BLINK_LED(LED_BLINK_7);
				protect_time = millis();
				filament_status++;
			}
			else
			{
				filament_status = 0;
			}
		}	
	}
	else if(filament_status == 2) 
	{
		if(current_temperature[0] > 180)
		{
			filament_status++;		
			BLINK_LED(LED_BLINK_5);
		}	
		if((READ(RETRACT_PIN))&&(READ(FEED_PIN)))
		{
			cleaning_buffer_counter=2;
			BLINK_LED(LED_ON);
			filament_status = 0;	
		}
	}
	else if(filament_status == 3) 
	{
		static uint8_t flag = 0;
		if(!READ(RETRACT_PIN)) 
		{
			if(flag ==0)
			{
				retract_filament();
				BLINK_LED(LED_BLINK_5);
				flag = 1;
			}
		}
		if(!READ(FEED_PIN))  
		{
			if(flag ==0)
			{
				feed_filament();
				BLINK_LED(LED_BLINK_5);
				flag = 1;
			}
		}
		if((READ(RETRACT_PIN))&&(READ(FEED_PIN))) 
		{
			flag = 0;
			filament_status = 0;
			cleaning_buffer_counter=2;
			BLINK_LED(LED_ON);
		}
	}
	else
	{
		filament_status = 0;
	}	
}


void heat_protect(void)
{
	if(current_temperature[0] < 60) 
	{
		
		return;	
	}
	if((card.sdprinting==false)) 
	{
		if(protect_time+300000l < millis())
		{
			protect_time = millis();
			temperature_protect_last = target_temperature[0]; 
			disable_all_heaters();
			MYSERIAL.print("extruder heating off!");
		}	
	}
	else
	{
		if(protect_time != millis())
		{
			protect_time = millis();		
		}
	}		
}

#endif 

