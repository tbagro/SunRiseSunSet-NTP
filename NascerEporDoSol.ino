// Bibliotecas ------------------------------------------
#ifdef ESP8266
  // ESP8266
  #include <ESP8266WiFi.h>
  #include <coredecls.h>                          // settimeofday_cb()
#else
  // ESP32
  #include <WiFi.h>
#endif
#include <WiFiManager.h>

WiFiManager wifiManager;
WiFiServer server(80); 


#include "sunrisesunset.h" // inclui bibliotexa



void setup() {

  Serial.begin(115200);
  delay(250);
  Serial.println("");
  //Configura a GPIO como saida
  pinMode(LED_BUILTIN, OUTPUT);
  //Coloca a GPIO em sinal logico baixo
  digitalWrite(LED_BUILTIN, LOW);

    //Define o auto connect e o SSID do modo AP
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.autoConnect("ESPWebServer");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(F("Conectado"));
  server.begin();

  Udp.begin(localPort);
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  
  RiseSet(); // obtem o valor de nascer e por do sol

  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();  
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:


}
