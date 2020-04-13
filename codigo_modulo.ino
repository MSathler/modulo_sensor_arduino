#include <SD.h> //A4 e A5 sao utilizados no SPI
#include "RTClib.h"
#include <Wire.h>
#include <SPI.h>
#include <avr/wdt.h>

RTC_DS1307 rtc;

String DADOS_S;
const int chipSelect = 10; 
int tempo;
File Pasta;
char buff[20];
//    =============               Variaveis contador                ================ 
int i = 0, j = 0;
float leitura_analogica_MQ13X = A1;
float leitura_analogica_MQ4= A3;
float leitura_analogica_MQ7 = A0;
int LED_G = 7;
int LED_R = 4;
float DADOS[7]; // vetor [hora,minutos,segundos, MQ4, MQS2, MQ135, MQ7]
float sensorValue_MQ13X = 0;
float sensorValue_MQ4 = 0;
float sensorValue_MQ7 = 0;
float tempo_inicio = 0;
float tempo_fim = 0;

void setup() {
  
  
  Serial.begin(9600); //Baud rate 
  pinMode(LED_G,OUTPUT); 
  pinMode(LED_R,OUTPUT); 
  pinMode(leitura_analogica_MQ13X, INPUT);
  pinMode(leitura_analogica_MQ4, INPUT);
  pinMode(leitura_analogica_MQ7, INPUT);
  
  digitalWrite(LED_G, 1);
  digitalWrite(LED_R, 1);
  delay(1000);
  digitalWrite(LED_R, 0);
  digitalWrite(LED_G, 0);
  
  wdt_enable(WDTO_8S);
  //    =============            Configuraçao do Cartao SD             ================   
  //Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT); //10 no arduino UNO
  if (!SD.begin(chipSelect)) {
    //Serial.println("Card failed, or not present");
    while (i != 2){
      digitalWrite(LED_R,!digitalRead(LED_R));
      delay(3000);
      i++;
    }
    // don't do anything more:
    return;
  }
  //Serial.println("card initialized.");
  digitalWrite(LED_G, 1);
  delay(3000);
  

  //    =============          Fim da Configuraçao Cartao SD           ================  
  
  //  =============              Configuraço RTC           INICIO    ================
  //if (! rtc.begin()) {
    //Serial.println("Couldn't find RTC");
  //  while (1);
  //}

  //if (! rtc.isrunning()) {
  //  Serial.println("RTC is NOT running!");  //rtc.adjust(DateTime(2019, 9, 1, 18, 19, 0));
  //}
  //  =============              Configuraço RTC            FIM      ================
 rtc.adjust(DateTime(2019, 9, 11, 15, 15, 40));
 Pasta = SD.open("LOG_INS.txt", FILE_WRITE);
    if (Pasta){
      DateTime now = rtc.now();
      Pasta.print(now.year());
      Pasta.print(",");
      Pasta.print(now.month());
      Pasta.print(",");
      Pasta.print(now.day());
      Pasta.println("...");
      Pasta.close();
    } 
    

}

void loop() { 
  
  tempo_inicio = millis();
  
  digitalWrite(LED_G,!digitalRead(LED_G)); // Led pisca quando esta funcionando
  
  String dataString = "";
  
  
  //    =============          Media dos valores dos sensores  INIC ================
  for(int x = 0 ; x < 50 ; x++) 
  {
    
    sensorValue_MQ7 = sensorValue_MQ7 + analogRead(leitura_analogica_MQ7);
    delay(1);    
    sensorValue_MQ4 = sensorValue_MQ4 + analogRead(leitura_analogica_MQ4);
    delay(1);   
    sensorValue_MQ13X = sensorValue_MQ13X + analogRead(leitura_analogica_MQ13X);
    delay(1);
 
  }
  sensorValue_MQ7 = sensorValue_MQ7/50;
  sensorValue_MQ4 = sensorValue_MQ4/50;
  sensorValue_MQ13X = sensorValue_MQ13X/50; //Take average of readings
  
  //    =============          Media dos valores dos sensores   FIM ================         



  //---------------------------relogio RTC----------------------------------------
  DateTime now = rtc.now(); // retorna o valor atual
  DADOS[0] = (now.hour());
  DADOS[1] = (now.minute());
  DADOS[2] = (now.second());
  DADOS[3] = sensorValue_MQ4;
  DADOS[4] = sensorValue_MQ13X;
  DADOS[5] = sensorValue_MQ7;
  for (int j = 0; j <= 5 ; j++){
    dtostrf (DADOS[j], 4, 0 , buff); // transforma em string
    DADOS_S = buff; 
    dataString += DADOS_S;
    if (j <=4 ){
      dataString += ",";
    }else{
      dataString += ":";
    }
  }
  //===============================   Envio String Cartao SD ==========================
 
  Pasta = SD.open("LOG_INS.txt", FILE_WRITE);
    if (Pasta){
      Pasta.println(dataString);
      Pasta.close();
      wdt_reset();
    }
  
  //===============================   Envio String Cartao SD    =========================
  //if (i == 0 ){
  //  Serial.print("Fazendo a Captura de dados");
  //}
  Serial.println(dataString);
  //i ++;
  
  tempo_fim = millis();
  
  delay(1000-(tempo_fim-tempo_inicio)); //Tempo de execuço 1 segundo */


}
  
