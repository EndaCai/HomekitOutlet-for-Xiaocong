/*
 * @Date: 2022-03-13 15:57:14
 * @LastEditors: Enda Cai
 * @E-mail: EndaCai@qq.com
 * @LastEditTime: 2022-03-15 00:33:27
 * @FilePath: /HomekitOutlet-for-Xiaocong/src/my_accessory.c
 */
#include <homekit/homekit.h>
#include <homekit/characteristics.h>

// Called to identify this accessory. See HAP section 6.7.6 Identify Routine
// Generally this is called when paired successfully or click the "Identify Accessory" button in Home APP.
void my_accessory_identify(homekit_value_t _value) {
	printf("accessory identify\n");
}

// For TEMPERATURE_SENSOR,
// the required characteristics are: CURRENT_TEMPERATURE
// the optional characteristics are: NAME, STATUS_ACTIVE, STATUS_FAULT, STATUS_TAMPERED, STATUS_LOW_BATTERY
// See HAP section 8.41 and characteristics.h

// (required) format: float; HAP section 9.35; min 0, max 100, step 0.1, unit celsius
homekit_characteristic_t hk_current_on = HOMEKIT_CHARACTERISTIC_(ON, 1);

// (optional) format: string; HAP section 9.62; max length 64
homekit_characteristic_t hk_name = HOMEKIT_CHARACTERISTIC_(NAME, "Smart Plug");

homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_outlet, .services=(homekit_service_t*[]) {
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "Smart Plug"),
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Yuantai Clubhouse"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0000001"),
            HOMEKIT_CHARACTERISTIC(MODEL, "ESP8266/XiaocongSmart"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.0"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
            NULL
        }),
        HOMEKIT_SERVICE(OUTLET, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
            &hk_current_on,
			&hk_name,//optional
            NULL
        }),
        NULL
    }),
    NULL
};

homekit_server_config_t config = {
		.accessories = accessories,
		.password = "123-11-123"
};