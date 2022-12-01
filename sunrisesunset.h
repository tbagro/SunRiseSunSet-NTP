#include <Time.h>
#include <TimeLib.h>
#include <DS3232RTC.h>
#include <WiFiUdp.h>
#include <sunMoon.h>

//******************** NTP *******************//
static const char ntpServerName[] = "b.ntp.br"; //Servidor (pode ser a.ntp.br / b.ntp.br / c.ntp.br )
#define timeZone  -4 // Insira o fuso horario local

WiFiUDP Udp;
unsigned int localPort = 8888;

time_t getNtpTime();

void sendNTPpacket(IPAddress &address);

const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

time_t getNtpTime()
{
  IPAddress ntpServerIP;

  while (Udp.parsePacket() > 0) ;
  Serial.println(F("Transmitindo NTP Request"));
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println(F("Resposta recebida do NTP"));
      Udp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long secsSince1900;
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("Sem resposta do NTP");
  return 0;
}

void sendNTPpacket(IPAddress &address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
time_t prevDisplay = 0;

void printDigits(int digits){ 
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay(){ 
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}


//*****************************Geo**********************************//
#define OUR_latitude    -11.3895               // sua coordenada
#define OUR_longtitude  -58.7278
#define OUR_timezone    -240                     // fuso UTC em minutos

sunMoon  sm;

                            // specific time


void printDate(time_t date) {
  char buff[20];
  sprintf(buff, "%2d-%02d-%4d %02d:%02d:%02d",
  day(date), month(date), year(date), hour(date), minute(date), second(date));
  Serial.print(buff);
}

void RiseSet(){ // obtem nascer e por do sol
sunMoon  sm;
  sm.init(OUR_timezone, OUR_latitude, OUR_longtitude);
//
  
  uint32_t jDay = sm.julianDay();               // Optional call
  byte mDay = sm.moonDay();
  time_t sRise = sm.sunRise();
  time_t sSet  = sm.sunSet();
  Serial.print("Hoje é "); Serial.print(jDay); Serial.println(" dia Juliano"); // imprime dia Juliano
  Serial.print("dia da lua "); Serial.print(mDay); // imprime dia do cliclo da Lua de 29 dias
  Serial.println("hoje nascer e por do sol: ");
  printDate(sRise); Serial.print("; ");// imprime nascer do sol
  printDate(sSet);  Serial.println("");// imprime por do sol

 }
