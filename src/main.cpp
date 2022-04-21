/*
 * @Date: 2022-03-13 03:38:38
 * @LastEditors: Enda Cai
 * @E-mail: EndaCai@qq.com
 * @LastEditTime: 2022-04-22 00:22:19
 * @FilePath: /HomekitOutlet-for-Xiaocong/src/main.cpp
 */
#include <Arduino.h>
#include <arduino_homekit_server.h>
// #include "wifi_info.h"
#include "main.h"
#include "ESPButton.h"
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h> //导入Flash库文件

#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);

extern DNSServer dnsServer;
extern ESP8266WebServer server;
extern void wifiConfigInitBasic(void);
extern void wifiConfigConnectWifi(void);
void setup() {
	wifiConfigInitBasic();
    hardware_init();
	wifiConfigConnectWifi();
    digitalWrite(PIN_LED_RED, HIGH);
	//homekit_storage_reset(); // to remove the previous HomeKit pairing storage when you first run this new HomeKit example
	my_homekit_setup();
}

void loop() {
    server.handleClient();
    dnsServer.processNextRequest();
    ESPButton.loop();
	my_homekit_loop();
	delay(10);
}

//==============================
// HomeKit setup and loop
//==============================

// access your HomeKit characteristics defined in my_accessory.cß
extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t hk_current_on;

static uint32_t next_heap_millis = 0;
bool on = true;

void button_progcess_cb(uint8_t id, ESPButtonEvent event)
{
    switch (event)
    {
    case ESPBUTTONEVENT_SINGLECLICK:
        on = !on;
        LOG_D("Switch: %s", on ? "ON" : "OFF");
        digitalWrite(PIN_RELAY, on ? HIGH : LOW);
        digitalWrite(PIN_LED_GREEN, on ? LOW : HIGH);
        hk_current_on.value.bool_value = on;
	    homekit_characteristic_notify(&hk_current_on, hk_current_on.value);
        break;
    case ESPBUTTONEVENT_DOUBLECLICK:
        break;
    case ESPBUTTONEVENT_LONGCLICK:
        LOG_D("Reset Homekit\n");
        homekit_server_reset();
        delay(5000);
        ESP.restart();
        break;
    default:
        break;
    }
}

void hardware_init()
{
    pinMode(PIN_RELAY, OUTPUT);
	digitalWrite(PIN_RELAY, HIGH); //Init 
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    digitalWrite(PIN_LED_GREEN, LOW); //Init 
    digitalWrite(PIN_LED_RED, LOW); //Init 
    

    ESPButton.add(1, PIN_BUTTON, false);
    ESPButton.setCallback(button_progcess_cb);
    ESPButton.begin();
}

//Called when the switch value is changed by iOS Home APP
void hk_current_on_setter(const homekit_value_t value) {
	on = value.bool_value;
	hk_current_on.value.bool_value = on;	//sync the value
	LOG_D("Switch: %s", on ? "ON" : "OFF");
	digitalWrite(PIN_RELAY, on ? HIGH : LOW);
    digitalWrite(PIN_LED_GREEN, on ? LOW : HIGH);
}

void my_homekit_setup() {
	hk_current_on.setter = hk_current_on_setter;
	arduino_homekit_setup(&config);
	hk_current_on.value.bool_value = on;
	homekit_characteristic_notify(&hk_current_on, hk_current_on.value);
}

void my_homekit_loop() {
	arduino_homekit_loop();
	const uint32_t t = millis();
	if (t > next_heap_millis) {
		// show heap info every 5 seconds
		next_heap_millis = t + 5 * 1000;
		LOG_D("Free heap: %d, HomeKit clients: %d",
				ESP.getFreeHeap(), arduino_homekit_connected_clients_count());

	}
}
