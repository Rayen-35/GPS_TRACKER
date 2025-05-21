#include <WiFi.h>
#include <HTTPClient.h>

#include <GSheet32.h>
GSheet32 Sheet("AKfycbxMBjnUGoUsH7gpEy1pL9CjXY8BN_r5oX9VI2MkPRHr07KPAYJ62LsAnADfpzzi9nzp");
#include <TinyGPS++.h>
TinyGPSPlus gps;

#define RXD2 4
#define TXD2 2
HardwareSerial neogps(1);

//ENTER_GOOGLE_DEPLOYMENT_ID
const char * ssid = "ETUD";
const char * password = "Etudiant@2022";
String GOOGLE_SCRIPT_ID = "AKfycbxMBjnUGoUsH7gpEy1pL9CjXY8BN_r5oX9VI2MkPRHr07KPAYJ62LsAnADfpzzi9nzp";

const int sendInterval = 2000;
/************************************************************************************
 *  setup function starts
 **********************************************************************************/
 void setup() {
  //--------------------------------------------
  Serial.begin(115200);
  //start serial communication with Serial Monitor
  //--------------------------------------------
  //start serial communication with Neo6mGPS
  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(10);
  //--------------------------------------------
   Sheet.connectWiFi(ssid, password);
  
  //--------------------------------------------
}
/************************************************************************************
 *  loop function starts
 **********************************************************************************/
void loop() {
    
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (neogps.available())
    {
      if (gps.encode(neogps.read()))
        {newData = true;}
    }
  }

  //If newData is true
  if(newData == true)
  {
    newData = false;
    print_speed();
  }
  else
  {
    Serial.println("No new data is received.");
  }  

  //delay(sendInterval);
}




/************************************************************************************
 *  loop function starts
 **********************************************************************************/
void print_speed()
{
  if (gps.location.isValid() == 1)
  {
  // Sheet.connectWiFi(ssid, password);
    String param;
    String latitude  = String(gps.location.lat());
    String longitude =String(gps.location.lng());
    String speed=String(gps.speed.kmph());
    String satellites=String(gps.satellites.value());
    String altitude =String(gps.altitude.meters());
    String gps_time=String(gps.time.hour())+":"+String(gps.time.minute())+":"+String(gps.time.second());
    String gps_date=String(gps.date.day())+"/"+String(gps.date.month())+"/"+String(gps.date.year());
        Serial.println("Sending Data");
        param="gps_date="+gps_date+"&gps_time="+gps_time+"&latitude="+latitude+"&longitude="+longitude+"&speed="+speed+"&satellites="+satellites+"&altitude="+altitude;
    Serial.println(param);
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+param;

    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 

    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);    
    }
    http.end();
  
 
  delay(1000);

  //Sheet.sendData(latitude,longitude,speed,altitude,satellites);

  }
  else
  {
    Serial.println("No any valid GPS data.");
  }  

}
