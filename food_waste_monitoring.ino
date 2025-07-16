#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <stack>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <WiFiClient.h>
#include "HX711.h"
#include <Servo.h>

using namespace std;


#define WIFI_SSID "OnePlus Nord CE 3 Lite 5G"
#define WIFI_PASSWORD "yyyyyyyyyy"

#define FIREBASE_EMAIL "your-email@example.com"
#define FIREBASE_PASSWORD "your-password"
#define FIREBASE_API_KEY "AIzaSyBNCtRP31RiEN9uPGOBPHdDrmN8YvnJKSY" 


FirebaseData firebaseData;

FirebaseConfig config;
FirebaseAuth auth;
WiFiClient client;



#define DT D2
#define SCK D1

HX711 scale;
Servo myServo;

float calibration_factor = -7050;
int number_of_students = 0;

float total_weight = 0; 

float initialWeight = 0;
bool wasOpen = false;

void setup() {
  Serial.begin(115200);
  

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");


  config.api_key = FIREBASE_API_KEY;
  config.database_url = "https://smart-dustbin-d4009-default-rtdb.firebaseio.com/";


  auth.user.email = FIREBASE_EMAIL;
  auth.user.password = FIREBASE_PASSWORD;


  Firebase.begin(&config, &auth);

  if (Firebase.ready()) {
    Serial.println("Firebase initialized successfully");
  } else {
    Serial.println("Failed to initialize Firebase");
    return; 
  }

  scale.begin(DT, SCK);
  scale.set_scale(calibration_factor);
  scale.tare();  

  myServo.attach(D5);  
  myServo.write(0);    
}

void loop() 
{
  if (Firebase.getString(firebaseData, "/Dustbin/Dustbin/AddStudent")) 
  {
    String AddStudent = firebaseData.stringData();
    AddStudent.replace("\"", "");
    AddStudent.replace("\\", "");
    AddStudent.trim();

    if(AddStudent == "YES") 
    {
     
      if (Firebase.getInt(firebaseData, "/Dustbin/Dustbin/number_of_students")) 
      {
        int currentStudents = firebaseData.intData();
        currentStudents++;

        
        Firebase.setInt(firebaseData, "/Dustbin/Dustbin/number_of_students", currentStudents);

      
        Firebase.setInt(firebaseData, "/Dustbin/" + String(currentStudents) + "/TotalFoodWaste", 0);
        
        Firebase.getString(firebaseData, "/Dustbin/Dustbin/Password");
        String Password = firebaseData.stringData();
        Password.replace("\"", ""); 
        Password.replace("\\", ""); 
        Password.trim(); 

        while(Password == "")
        {

        }

        Firebase.setString(firebaseData, "/Dustbin/" + String(currentStudents) + "/" + String(currentStudents), Password);
        Firebase.setString(firebaseData, "/Dustbin/Dustbin/AddStudent", "NO");
      }
    }
  }

  Firebase.getString(firebaseData, "/Dustbin/Dustbin/StatusOfServo");
  String ServoStatus = firebaseData.stringData();
  ServoStatus.replace("\"", "");  
  ServoStatus.replace("\\", "");  
  ServoStatus.trim(); 


  Firebase.getString(firebaseData, "/Dustbin/Dustbin/Current");
  String Current = firebaseData.stringData();
  Current.replace("\"", "");
  Current.replace("\\", "");
  Current.trim();

  if (ServoStatus == "OPEN") {
    if (!wasOpen) {
      myServo.write(180);
      delay(500); // 

      float weight = scale.get_units(5);
      Serial.print("Initial Weight: ");
      Serial.print(weight, 2);
      Serial.println(" kg");

      initialWeight = (weight <= 0) ? 0 : weight;

      
      Firebase.setFloat(firebaseData, "/Dustbin/Dustbin/InitialWeight", initialWeight);

      wasOpen = true;
    }

  } else if (wasOpen) {
  
    float finalWeight = scale.get_units(5);
    Serial.print("Final Weight: ");
    Serial.println(finalWeight, 2);

    float wasteThrown = finalWeight - initialWeight;
    if (wasteThrown < 0) wasteThrown = 0;

  
    Firebase.getFloat(firebaseData, "/Dustbin/" + Current + "/TotalFoodWaste");
    float previousWaste = firebaseData.floatData();
    float updatedWaste = previousWaste + wasteThrown;

    Firebase.setFloat(firebaseData, "/Dustbin/" + Current + "/TotalFoodWaste", updatedWaste);
    Firebase.setFloat(firebaseData, "/Dustbin/" + Current + "/RecentWaste", wasteThrown); 

    
    Firebase.setFloat(firebaseData, "/Dustbin/Dustbin/TotalWeight", finalWeight);

    wasOpen = false;
    myServo.write(0); 
  }
}
