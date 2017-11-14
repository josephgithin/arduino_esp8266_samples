#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

//static const uint8_t D0   = 16;
//static const uint8_t D1   = 5;
//static const uint8_t D2   = 4;
//static const uint8_t D3   = 0;
//static const uint8_t D4   = 2;
//static const uint8_t D5   = 14;
//static const uint8_t D6   = 12;
//static const uint8_t D7   = 13;
//static const uint8_t D8   = 15;
//static const uint8_t D9   = 3;
//static const uint8_t D10  = 1;

char dataSeperator = ';';
const int PIN_MODE = 0;
const int PIN_NO = 1;
const int PIN_VALUE = 2;

char path[] = "/ws";
char host[] = "alienz.ml";
int port = 443;
int pinmodes[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#define USE_SERIAL Serial

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {


  switch (type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[WS] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      {
        USE_SERIAL.printf("[WS] Connected to url: %s\n",  payload);

        // send message to server when Connected
        webSocket.sendTXT("LOGIN_DEV2");
      }
      break;
    case WStype_TEXT:
      {
        USE_SERIAL.printf("[WSc] get text: %s\n", payload);
        String data = (char*)payload;
        if (data.length() > 0) {
          Serial.print("Received data: ");
          Serial.println(data);
          //DATAFORMAT PINMODE;PINNUMBER;PINVALUE
          int rMode = getValue(data, dataSeperator, PIN_MODE).toInt();
          int rPinNo = getValue(data, dataSeperator, PIN_NO).toInt();
          int rPinValue = getValue(data, dataSeperator, PIN_VALUE).toInt();

          if (pinmodes[rPinNo] != rMode) {
            pinMode(rPinNo, rMode);
            pinmodes[rPinNo] = rMode;
          }

          digitalWrite(rPinNo, rPinValue);

          webSocket.sendTXT("Done!");
        }

        // send message to server
        // webSocket.sendTXT("message here");
      }
      break;

    case WStype_BIN:
      USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;


  }


}
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
void setup() {
  // USE_SERIAL.begin(921600);
  USE_SERIAL.begin(115200);

  //Serial.setDebugOutput(true);
  // USE_SERIAL.setDebugOutput(true);


  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFiMulti.addAP("Network_Z", "1qaz2wsx!@ZX");
  WiFiMulti.addAP("UTStarcom", "wifi1234");
  WiFiMulti.addAP("K8Note", "1qaz2wsx!@ZX");

  //WiFi.disconnect();
  while (WiFiMulti.run() != WL_CONNECTED) {
    USE_SERIAL.print(".");
    delay(100);
  }

  webSocket.beginSSL(host, port, path);
  webSocket.onEvent(webSocketEvent);

}

void loop() {
  webSocket.loop();
}
