#include <ESP8266WiFi.h> 
#include <BlynkSimpleEsp8266.h> 
#include <DHT.h> 

#define BLYNK_AUTH_TOKEN "Token Blynk" 
char auth[] = BLYNK_AUTH_TOKEN; 
char ssid[] = "Nama Wifi"; 
char pass[] = "Password Wifi"; 

#define DHTPIN 5 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE); 
int sensorPin = A0; 
int guvaValue = 0; 
int lampuPin = 16; 
int waktuMundur = 0;

bool timerActive = false; 
bool networkAvailable = false; 
unsigned long previousMillis = 0; 
const unsigned long interval = 1000; 

void checkNetworkStatus() { 
  if (WiFi.status() == WL_CONNECTED) { 
    networkAvailable = true; 
  } else { 
    networkAvailable = false;
  }
}

void setup() { 
  pinMode(lampuPin, OUTPUT);
  Blynk.begin(auth, ssid, pass); 
  Blynk.virtualWrite(V1, LOW); 
  dht.begin(); 
  checkNetworkStatus(); 
} 
void loop() { 
  checkNetworkStatus(); 
  if (networkAvailable) { 
    Blynk.run(); 
    if (timerActive) { 
    unsigned long currentMillis = millis(); 
    if (currentMillis - previousMillis >= interval) { 
      previousMillis = currentMillis; 
      waktuMundur--; 
      if (waktuMundur >= 0) { 
        char formattedTime[6]; 
        sprintf(formattedTime, "%02d:%02d", waktuMundur / 60, waktuMundur % 60); 
        Blynk.virtualWrite(V11, formattedTime);
        } else {
          digitalWrite(lampuPin, LOW); 
          Blynk.virtualWrite(V2, 0); 
          Blynk.virtualWrite(V3, 0); 
          Blynk.virtualWrite(V12, 0); 
          timerActive = false;
        }
    }
    }
    float suhu = dht.readTemperature(); 
    float kelembapan = dht.readHumidity(); 
    guvaValue = analogRead(sensorPin); 
    Blynk.virtualWrite(V4, suhu); 
    Blynk.virtualWrite(V5, kelembapan); 
    Blynk.virtualWrite(V6, guvaValue); 
    delay(1000);
  } else {
    digitalWrite(lampuPin, LOW); 
    Blynk.virtualWrite(V2, 0); 
  }
}

BLYNK_WRITE(V1) { 
  if (!timerActive) { 
    int lampuStatus = param.asInt(); 
    if (lampuStatus == HIGH) {  
      digitalWrite(lampuPin, HIGH); 
      Blynk.virtualWrite(V2, 1); 
    } else {  
      digitalWrite(lampuPin, LOW); 
      Blynk.virtualWrite(V2, 0);
    }
  }
}

BLYNK_WRITE(V3) { 
  int buttonState = param.asInt(); 
  if (buttonState == HIGH) {
    digitalWrite(lampuPin, HIGH); 
    Blynk.virtualWrite(V2, 1); 
    delay(500);
    if (waktuMundur > 0) {
      timerActive = true; 
      previousMillis = millis();
    } 
  }
  else if (buttonState == LOW) {
    digitalWrite(lampuPin, LOW); 
    Blynk.virtualWrite(V2, 0);
    timerActive = false; 
    waktuMundur = 0; 
    Blynk.virtualWrite(V11, "00:00");
  }
}

BLYNK_WRITE(V12) { 
  if (!timerActive) { 
    int waktuMundurMenit = param.asInt(); 
    waktuMundur = waktuMundurMenit * 60; 
    char formattedTime[6]; 
    sprintf(formattedTime, "%02d:%02d", waktuMundur / 60, waktuMundur % 60); 
    Blynk.virtualWrite(V11, formattedTime); 
  } 
}