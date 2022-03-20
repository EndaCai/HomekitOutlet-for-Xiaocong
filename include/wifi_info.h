/*
 * @Date: 2022-03-13 16:56:15
 * @LastEditors: Enda Cai
 * @E-mail: EndaCai@qq.com
 * @LastEditTime: 2022-03-14 00:26:25
 * @FilePath: /111/include/wifi_info.h
 */

#ifndef WIFI_INFO_H_
#define WIFI_INFO_H_

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include "main.h"

const char *ssid = "A_A";
const char *password = "Cy607325";

void wifi_connect() {
	WiFi.persistent(false);
	WiFi.mode(WIFI_STA);
	WiFi.setAutoReconnect(true);
	WiFi.begin(ssid, password);
	Serial.println("WiFi connecting...");
    digitalWrite(PIN_LED_RED, LOW);
	while (!WiFi.isConnected()) {
		delay(100);
		Serial.print(".");
	}
    
	Serial.print("\n");
	Serial.printf("WiFi connected, IP: %s\n", WiFi.localIP().toString().c_str());
    digitalWrite(PIN_LED_RED, HIGH);
}

#endif /* WIFI_INFO_H_ */