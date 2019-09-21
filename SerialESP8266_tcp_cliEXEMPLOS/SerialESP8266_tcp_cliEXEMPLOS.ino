
#include <SoftwareSerial.h>

SoftwareSerial esp8266 (2, 3); //Arduino TX (ESP8266 RX) connected to Arduino Pin 2, Arduino RX(ESP8266 TX) connected to Arduino Pin 3

String ssid = "ALHN-8E28";
String password = "4650730851";




String data = "{oi:oi2}";
String path = "/api/arduino/oiPost";
String server = "192.168.1.70";
String request = "POST " + path + " HTTP/1.1\r\n" + "Host: " + server + "\r\n" + "Connection: keep-alive\r\n\r\n "+ data.length() + "\r\n" +
           "Content-Type: application/json;charset=UTF-8\r\n\r\n"+
                data +"\r\n";
String requestLength = String(request.length());

const int timeout = 2000;

void setup() {
  Serial.begin(9600);
  esp8266.begin(9600); // Should match ESP's current baudrate
 
  setupESP8266();
  connectToWiFi();
  startTCPConnection();
  sendPostRequest();
 // sendGetRequest();
  closeTCPConnection();
}

void loop() {
 
}

String atCommand(String command, int timeout) {
  String response = "";
  esp8266.println(command);

  long int time = millis();

  while( (time+timeout) > millis() ) {
    while(esp8266.available()) {
      char c = esp8266.read();
      response += c;
    }
  }

  Serial.println(response);
  return response;
}

void setupESP8266() {
  atCommand("AT+RST", timeout);
  atCommand("AT+CWMODE=1", timeout);
}

void connectToWiFi() {
  String connect = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\"";
  atCommand(connect, 6000);
  atCommand("AT+CIFSR", timeout);
}

void startTCPConnection() {
  String connect = "AT+CIPSTART=\"TCP\",\"" + server + "\",8090";
  atCommand(connect, timeout);
}

void closeTCPConnection() {
  atCommand("AT+CIPCLOSE", timeout);
}

String sendGetRequest() {
  atCommand("AT+CIPSEND=" + requestLength, timeout);
  String response = atCommand(request, 6000);
  return response;
}

String sendPostRequest() {
  atCommand("AT+CIPSEND=" + requestLength, timeout);
  String response = atCommand(request, 6000);
  return response;
}
