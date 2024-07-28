#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <LiquidCrystal_I2C.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// Your Firebase Project Web API Key
#define API_KEY ""
// Your Firebase Realtime database URL
#define DATABASE_URL ""

FirebaseData fbdo,fbdo_s1,fbdo_s2;
FirebaseAuth auth;
FirebaseConfig config;

bool signUpOK=false;
unsigned long sendDataPrevMillis = 0;
bool ledStatus=false;
float temp=0;
float humidity=0;
#define RELAY_PIN 19  // connected to LED 4
#define BUZZER_PIN 4  // Connedted to Buzzer
#define MQ2 12 // Gas Sensor  
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <DHT.h>  
DHT dht(13,DHT11);
int MQ2Value,f;
int floodStatus = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(MQ2, INPUT);
  pinMode(RELAY_PIN,OUTPUT);
  pinMode(BUZZER_PIN,OUTPUT);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);

  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" MADANI IoT");

  while(WiFi.status()!= WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  lcd.setCursor(0, 1);      
  lcd.print(" Connected");
  Serial.print("Connected With Ip : ");
  Serial.println(WiFi.localIP());
  Serial.println();
  config.api_key=API_KEY;
  config.database_url=DATABASE_URL;

  if(Firebase.signUp(&config,&auth, "", "")){
    Serial.println("SignUp Ok");
    signUpOK=true;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T:     C");
  lcd.setCursor(0,1);
  lcd.print("H:     %");
  lcd.setCursor(9,1);
  lcd.print("Gas: ");

  config.token_status_callback=tokenStatusCallback;

  Firebase.begin(&config,&auth);
  Firebase.reconnectWiFi(true);

  // init stream for sirens and flood sensor
  if(!Firebase.RTDB.beginStream(&fbdo_s1,"data/0/relay/sirens")){
    Serial.println("FAILED1: "+fbdo_s1.errorReason());
  }
  if(!Firebase.RTDB.beginStream(&fbdo_s2,"data/0/sensor/flood")){
    Serial.println("FAILED1: "+fbdo_s2.errorReason());
  }
}


void loop() {
  tempHumiMeasurement();
  updateSirensStatus();
  updateFloodStatus();
  leakGasDetection();
}

void tempHumiMeasurement() {
  // DHT sensor
  if(Firebase.ready() && signUpOK && (millis()-sendDataPrevMillis>5000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis =millis();
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();

    if(Firebase.RTDB.setFloat(&fbdo,"data/0/sensor/temp_data",temp)){
      Serial.println();
      Serial.print(temp);
      lcd.setCursor(2,0);
      lcd.print(temp);
      Serial.println("-successfully saved to "+fbdo.dataPath());
      Serial.println("("+fbdo.dataType()+")");
      if(temp > 80.0){
        Firebase.RTDB.setBool(&fbdo,"data/0/sensor/is_danger", true);
      } else {
        Firebase.RTDB.setBool(&fbdo,"data/0/sensor/is_danger", false);
      }
    } else {
      Serial.println("FAILED2 : "+fbdo.errorReason());
      Serial.println(temp);
      Serial.println(humidity);
    }

    // put your main code here, to run repeatedly:
    if(Firebase.RTDB.setFloat(&fbdo,"data/0/sensor/humidity_data",humidity)){
      Serial.println();
      Serial.print(humidity);
      lcd.setCursor(2,1);
      lcd.print(humidity);
      Serial.println("-successfully saved to "+fbdo.dataPath());
      Serial.println("("+fbdo.dataType()+")");
    } else {
      Serial.println("FAILED3 : "+fbdo.errorReason());
    }
  }
}

void leakGasDetection() {
  // MQA
  MQ2Value = digitalRead(MQ2);
  if ((MQ2Value==0)&&(f==0)){        
    lcd.setCursor(13,1);
    lcd.print(MQ2Value);
    Firebase.RTDB.setInt(&fbdo,"data/0/sensor/gas", MQ2Value);
    Serial.println("-successfully saved to "+fbdo.dataPath());
    f=1;
  }
  else if ((MQ2Value==1)&&(f==1)){        
    lcd.setCursor(13,1);
    lcd.print(MQ2Value);
    Firebase.RTDB.setInt(&fbdo,"data/0/sensor/gas", MQ2Value);
    Serial.println("-successfully saved to "+fbdo.dataPath());
    f=0;
  } 
}

void updateFloodStatus() {
  // Steam builder for Flood Sensor
  if(Firebase.ready() && signUpOK){  
    if(!Firebase.RTDB.readStream(&fbdo_s2)){
      Serial.println("FAILED4: "+fbdo_s2.errorReason());
    }
     
    if(fbdo_s2.streamAvailable()){
      if(fbdo_s2.dataType()== "string"){
        String floodStatusStr=fbdo_s2.stringData();
        Serial.println("Successfully read from"+fbdo_s2.dataPath()+ ":" + "("+fbdo_s2.dataType()+")");
        bool floodStatus = (floodStatusStr == "ON");
        if(floodStatus){
          digitalWrite(RELAY_PIN,HIGH);
          digitalWrite(BUZZER_PIN,HIGH);   
        } else{
          digitalWrite(RELAY_PIN,LOW);
          digitalWrite(BUZZER_PIN,LOW);
        }
      } else {
        Serial.println("FAILED5: "+fbdo_s2.errorReason());
      }
    }
  }
}

void updateSirensStatus() {
  // Steam builder for Sirens
  if(Firebase.ready() && signUpOK){  
    if(!Firebase.RTDB.readStream(&fbdo_s1)){
      Serial.println("FAILED4: "+fbdo_s1.errorReason());
    }
     
    if(fbdo_s1.streamAvailable()){
      if(fbdo_s1.dataType()== "string"){
        String sirensStatusStr=fbdo_s1.stringData();
        Serial.println("Successfully read from"+fbdo_s1.dataPath()+ ":" + "("+fbdo_s1.dataType()+")");
        bool sirensStatus = (sirensStatusStr == "ON");
        if(sirensStatus){
          digitalWrite(RELAY_PIN,HIGH);
          digitalWrite(BUZZER_PIN,HIGH);   
        } else{
          digitalWrite(RELAY_PIN,LOW);
          digitalWrite(BUZZER_PIN,LOW);
        }
      } else {
        Serial.println("FAILED5: "+fbdo_s1.errorReason());
      }
    }
  }
}