#include <Arduino.h>
#include <can.h>
#include <mcp2515.h>

#include <CanHacker.h>
#include <CanHackerLineReader.h>
#include <lib.h>

#include <SPI.h>

// #define USE_ARDUINO_UNO

#ifdef USE_ARDUINO_UNO

#else 
  #include <BluetoothSerial.h>
#endif

#ifdef USE_ARDUINO_UNO

const int SPI_CS_PIN = 10;
const int INT_PIN = 2;

#else 
//------ESP32 CAN BUS setting ---------------------------------------
const int SPI_CS_PIN = 5;
const int INT_PIN = 21;

BluetoothSerial bluetoothSerial;
#endif

Stream *interfaceStream = NULL;
Stream *debugStream = NULL;    
CanHackerLineReader *lineReader = NULL;
CanHacker *canHacker = NULL;

void setupSerialsAndStreams() {
    Serial.begin(115200);
    while (!Serial);
    SPI.begin();

#ifdef USE_ARDUINO_UNO
    //interfaceStream = &Serial;
    debugStream = &Serial;
#else 
    bluetoothSerial.begin("bluetoothserial");

    interfaceStream = &bluetoothSerial;
    debugStream = &Serial;
#endif
}

void setup() {
    setupSerialsAndStreams();

    canHacker = new CanHacker(interfaceStream, debugStream, SPI_CS_PIN);
    canHacker->setClock(MCP_8MHZ);
    canHacker->enableLoopback(); // uncomment this for loopback
    lineReader = new CanHackerLineReader(canHacker);
    
    pinMode(INT_PIN, INPUT);
}

void loop() {  
    if (digitalRead(INT_PIN) == LOW) {
        canHacker->processInterrupt();
    }

    CanHacker::ERROR error = lineReader->process();
    if(error != CanHacker::ERROR_OK) {
        Serial.print("Failure (code ");
        Serial.print((int)error);
        Serial.println(")");
    }
}
