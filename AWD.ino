#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <TimeLib.h>


#define FIREBASE_HOST "waterlevel-2250a-default-rtdb.asia-southeast1.firebasedatabase.app"  //Database link
#define FIREBASE_AUTH "dSaiDPbChFAbQY3UCiIqPNvc8YZkLwRMvAFqtq4F"  //Database secrate

#define WIFI_SSID "IOT"
#define WIFI_PASSWORD "12345678"

const char *timeApiUrl = "http://worldtimeapi.org/api/timezone/Asia/Dhaka";

unsigned long previousMillis = 0;
const long interval = 10000;

FirebaseData firebaseData,loadData;
FirebaseJson json;


const int trigPin = D6;
const int echoPin = D5;
bool low,high,filling;
int l=D1,h=D2,pump=D7;

long duration;
int distance;


void initFire(){
  pinMode(D4,OUTPUT);
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(D4,0);
    Serial.print(".");
    delay(200);
    digitalWrite(D4,1);
    Serial.print(".");
    delay(200);
    
  }
  
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  configTime(6 * 3600, 0, "pool.ntp.org", "time.nist.gov"); 

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}



String load(){
if (Firebase.getString(loadData, "/Setting")){
    return loadData.stringData();
  }
  return "0-0-0-0";
}


String setting="1-1-1-1";

int timeArray[4];


void setup() {
  pinMode(l,INPUT);
  pinMode(h,INPUT);
  pinMode(pump,OUTPUT);
  pinMode(D1,INPUT);
  pinMode(D2,INPUT);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  initFire();

  delay(500);

 // Firebase.setString(firebaseData, "/Hello","World" );
   setting=load();

     int startIndex = 0;
  for (int i = 0; i < 4; i++) {
    int endIndex = setting.indexOf('-', startIndex);
    if (endIndex == -1) {
      endIndex = setting.length();
    }
    String numString = setting.substring(startIndex, endIndex);
    timeArray[i] = numString.toInt();
    startIndex = endIndex + 1;
  }
  Serial.print("Extracted integers: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(timeArray[i]);
    if (i < 3) {
      Serial.print(", ");
    }
  }
}



int threshold=30;
int c1,c2;


unsigned long  startTime=0UL;

char _mode='S';


void loop() {

    

  Serial.println("\n\n");

    unsigned long currentMillis = millis();

    low=!digitalRead(l);
    high=!digitalRead(h);
    

    
   
  
    c1=!digitalRead(D1);
    c2=!digitalRead(D2);
  
    Serial.print(c1);
    Serial.print(" ");
    Serial.println(c2);
    
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    Serial.println("Distance: "+String(distance)+"cm");
    
//    if(distance<20)
//      distance=20;
//    else if(distance>100)
//      distance=100;
      
    //Firebase.setString(firebaseData,"/WaterLevel", String(distance));
    
    Firebase.setString(firebaseData,"/Pump", String(digitalRead(pump)));
     delay(200);
    
    operate();



  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    time_t now = time(nullptr);
    if (now != 0) {
      if(startTime==0)
          startTime=now;
      struct tm *timeInfo;
      timeInfo = localtime(&now);


    
      if(startTime<=now && now<startTime+(timeArray[0]*60)){
        _mode='O';
        Serial.println("======== First period running : ALWAYS_ON ========");
        
      }else if(startTime+(timeArray[0]*60)<=now && now<startTime+((timeArray[0]+timeArray[1])*60)){
         _mode='A';
         Serial.println("======== Second period running : AUTOMATIC ========");
      }
      else if(startTime+((timeArray[0]+timeArray[1])*60)<=now && now<startTime+((timeArray[0]+timeArray[1]+timeArray[2])*60)){
        _mode='O';
          Serial.println("======== Third period running : ALWAYS_ON ========");
      }
      else if((startTime+((timeArray[0]+timeArray[1]+timeArray[2])*60))<=now && now<(startTime+((timeArray[0]+timeArray[1]+timeArray[2]+timeArray[3])*60))){
        _mode='A';
        Serial.println("======== Forth period running : AUTOMATIC ========");
      }else{
        _mode='S';
         Serial.println("======== Cycle completed!!! ========");
      }
     
      

      char dateTimeString[20]; 
      sprintf(dateTimeString, "%04d'%02d'%02d'%02d'%02d'%02d",
              timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
              timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
      Serial.println(dateTimeString);
      Firebase.setString(firebaseData,"/WaterLevel/"+String(dateTimeString), String(distance));
    } else {
      Serial.println("Failed to obtain time");
    }
  }
}




void operate(){
  switch(_mode){
    case 'A':
        autoWaterLevel();
        break;
    case 'O':
        alwaysOn();
        break;
    default :

        break;
   
    
  }
}


void autoWaterLevel(){
   if(low==LOW){
      digitalWrite(pump,1);
      filling=1;
    }
    else if(filling && !high)
      digitalWrite(pump,1);
    
    else if(low && filling && high){
      filling=0;
      digitalWrite(pump,0);
    }
}

void alwaysOn(){
  digitalWrite(pump,!high);
}
