/*
 * @Date: 2022-03-13 20:46:23
 * @LastEditors: Enda Cai
 * @E-mail: EndaCai@qq.com
 * @LastEditTime: 2022-04-22 00:16:35
 * @FilePath: /HomekitOutlet-for-Xiaocong/include/main.h
 */
#ifndef _MAIN_H_
#define _MAIN_H_

void hardware_init();
void my_homekit_setup();
void my_homekit_loop();

#define XIAOCONG_BOARD (0)
#define ZD_IOT_BOARD (1)

#ifdef XIAOCON_BOARD
#define PIN_BUTTON      (5)
#define PIN_LED_RED     (4)
#define PIN_LED_GREEN   (16)
#define PIN_RELAY       (14)
#elif ZD_IOT_BOARD
#define PIN_BUTTON      (12)
#define PIN_LED_RED     (13)
#define PIN_LED_GREEN   (15) /* Blue */
#define PIN_RELAY       (4)
#endif

#endif