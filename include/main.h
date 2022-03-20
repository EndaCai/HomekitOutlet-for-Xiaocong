/*
 * @Date: 2022-03-13 20:46:23
 * @LastEditors: Enda Cai
 * @E-mail: EndaCai@qq.com
 * @LastEditTime: 2022-03-13 22:48:41
 * @FilePath: /111/include/main.h
 */
#ifndef _MAIN_H_
#define _MAIN_H_

void hardware_init();
void my_homekit_setup();
void my_homekit_loop();

#define PIN_BUTTON      5
#define PIN_LED_RED     4
#define PIN_LED_GREEN   16
#define PIN_RELAY       14

#endif