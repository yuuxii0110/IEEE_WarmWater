#include <Arduino.h>

#include "BluetoothSerial.h"

// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif

BluetoothSerial SerialBT;

int count = 0;
char msg_buf[32];
char msg;

void setup(){
    Serial.begin(115200);
    SerialBT.begin("User-test");
    delay(100);

    Serial.println("Bluetooth started");
}

void loop(){
    
    SerialBT.write(count);
    count++;
    
    delay(1000);
}