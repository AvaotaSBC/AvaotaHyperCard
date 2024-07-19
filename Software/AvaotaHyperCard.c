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
sfr P5M1 = 0xc9;
sfr P5M0 = 0xca;

//

sbit SEL_OUT = P3 ^ 2;
sbit READ_VCC_EN = P3 ^ 3;

sfr P5 = 0xC8;
sbit SEL_LED = P5 ^ 4;
sbit SEL_BTN = P5 ^ 5;

bit BTN_STATUS = 0;
bit OLD_BTN_STATUS = 0;
unsigned int READ_KEY_TIME = 0;
unsigned int OLD_READ_KEY_TIME = 0;

bit READER_MODE = 0;
/*
unsigned char mbits = 0;
unsigned long OLD_PRESSED_TIME = 0;*/

unsigned long millis = 0;

const unsigned int cycles_per_ms = 11059200 / 12 / 1000;  // @11.0592MHz

void delay_ms(unsigned int t) {  // max 65535
  unsigned int i;
  do {
    i = cycles_per_ms;
    while (--i)
      ;
  } while (--t);
  // unsigned int i = millis + t;
  // while (i >= millis);
}

void ChangeMode(bit m) {  // 0: Board, 1: Card Reader
  SEL_OUT = m;
  SEL_LED = m;
  READ_VCC_EN = m;
}

bit LongPressKey(unsigned int thres) {  // thres: 连续按下多少ms
  BTN_STATUS = SEL_BTN;
  if (BTN_STATUS) {
    OLD_BTN_STATUS = BTN_STATUS;
    OLD_READ_KEY_TIME = millis;
  } else {
    READ_KEY_TIME = millis;
    if (OLD_READ_KEY_TIME + thres <= READ_KEY_TIME && OLD_BTN_STATUS) {
      // <=: 偏小0~1ms, <: 偏大0~1ms.
      OLD_BTN_STATUS = BTN_STATUS;  // 只返回一次1
      return 1;
    }
    return 0;
  }
  return 0;
}
/*
unsigned int PressedTime() {  // max 65535, 阻塞.
  unsigned long i = millis;
  while (!SEL_BTN)
    ;  // 等待松开按键
  return millis - i;
}*/

//

void TM0_Isr() interrupt 1 {
  millis++; /*
   if (!SEL_BTN)
     OLD_PRESSED_TIME = millis;*/
}

void init() {
  P3M0 = 0x04;
  P3M1 = 0x00;
  P5M0 = 0x00;
  P5M1 = 0x00;

  TMOD = 0x03;  // 模式 3
  TL0 = (0x10000 - cycles_per_ms) % 0x100;
  TH0 = (0x10000 - cycles_per_ms) / 0x100;
  TR0 = 1;  // 启动定时器
  ET0 = 1;  // 使能定时器中断
  // EA = 1;// 不受 EA 控制

  ChangeMode(READER_MODE);
}

void loop() {
  // 按下并保持0.5s后(继续按下超过2s跳过切换模式步骤)松开,
  // 切换模式, 之后100ms内不响应.
  if (LongPressKey(500)) {
    unsigned long i = millis;
    while (!SEL_BTN) {
      if (i + 2000 >= millis) {
        delay_ms(100);
        SEL_LED = !SEL_LED;
      } else {
        SEL_LED = READER_MODE;
      }
    }
    if (i + 2000 >= millis) {
      READER_MODE = !READER_MODE;
    }
    ChangeMode(READER_MODE);
    delay_ms(100);
  }
  /* 不够灵敏, 还是上面的好用
  // 双击
  unsigned int pt = PressedTime();
  if (pt >= 10 && pt < 1000) {
    while (millis - OLD_PRESSED_TIME <= 10)
      ;
    mbits++;
  } else if (pt >= 1000) {
    mbits = 0;
  }
  if (millis - OLD_PRESSED_TIME > 500) {
    if (mbits == 2) {
      READER_MODE = !READER_MODE;
      ChangeMode(READER_MODE);
      delay_ms(500);
    }
    mbits = 0;
  }*/
}

//

void main() {
  init();
  while (1) {
    loop();
  }
}
