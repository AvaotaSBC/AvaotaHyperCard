/* 
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>

 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#include <intrins.h>
#include <reg51.h>

sfr P3M1 = 0xb1;
sfr P3M0 = 0xb2;
sfr P5M1 = 0xC9;
sfr P5M0 = 0xCA;

sbit SEL_OUT = P3^2;
sbit READ_VCC_EN = P3^3;

sfr P5   = 0xC8;
sbit SEL_LED = P5^4;
sbit SEL_BTN = P5^5;

bit BTN_STATUS = 0;
bit OLD_BTN_STATUS = 0;

void Delay20ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 36;
	j = 217;
	do
	{
			while (--j);
	} while (--i);
}

void ChangeToBoard()
{
	READ_VCC_EN = 0;
	Delay20ms();
	Delay20ms();
	SEL_OUT = 0;
	SEL_LED = 0;
}

void ChangeToRead()
{
	SEL_OUT = 1;
	SEL_LED = 1;
	Delay20ms();
	Delay20ms();
	READ_VCC_EN = 1;
}

void InitIO()
{
    P3M0 = 0x04;
    P3M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
}

void main()
{
	InitIO();
	ChangeToBoard();

	while(1) {
		OLD_BTN_STATUS = BTN_STATUS;
			
		if(SEL_BTN == 0) {
			Delay20ms();
			if(SEL_BTN == 0)
				BTN_STATUS = ~BTN_STATUS;
			while(!SEL_BTN);
		}
		if(OLD_BTN_STATUS != BTN_STATUS){
			if(BTN_STATUS) {
				ChangeToRead();
			} else {
				ChangeToBoard();
			}
		}
	}
}


