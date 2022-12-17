#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include <time.h> 
#include <NTPClient.h>               
#include <TimeLib.h>
#include <ArduinoJson.h>
#include<Hash.h>
#include"CookiesStore.h"


CookiesStore<4> co;

MDNSResponder mdns;
WiFiUDP ntpUDP;
 HTTPClient http;

NTPClient timeClient(ntpUDP, "time.nist.gov",0, 1000);
//const char* host ="only google.com";
const char* ssid = "WiFi-BBEvol6082";
const char* password = "1BCE62731BC2F51BEECC434504";
IPAddress ip(192,168,1,2);
IPAddress dns(8,8,8,8);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
ESP8266WebServer server(80);
/*void handleRoot() {
  server.send(200, "text/plain", "Root of WebServer!");
}*/
String webPage = "";
String stateString = "";
int gpio0_pin = 15;
int gpio2_pin = 2;
 int  state= 0;
char dateTime[]={"00,00,00,00,00,0000"};
char aa[]={"00,00,00,00,00,0000"};
String Password;
//String msg;
String UserName;
String dateTimeString;
String aaString;
const char* UserNameAlby = "alberto";
const char* PasswordAlby = "ghiaccio";
time_t dateTimeUtc1;
time_t aaUtc1;
  double diff;
  double diff1;
  int randomNumber=0;
  bool y = false;
  int i =0;
   //tmElements_t ;
//int Year, Month, Day, Hour, Minute, Second ;

/*void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authenticated()) {
    server.sendHeader("Location", "/");
    server.sendHeader("Cache-Control ", "no-cache");
    server.send(301);
    return;
  }
}*/

void setup() {
  webPage += "<h1>casa</h1><p>reley 1 <a href=\"/\"><button>Read</button></a>&nbsp;<a href=\"reley1On\"><button>ON</button></a>&nbsp;<a href=\"reley1Off\"><button>OFF</button></a></p>";
  pinMode(gpio0_pin, OUTPUT);
  digitalWrite(gpio0_pin, LOW);
  pinMode(gpio2_pin, OUTPUT);
  digitalWrite(gpio2_pin, HIGH);
  delay(1000);
  Serial.begin(115200);
  if(!SPIFFS.begin()){
          Serial.println("An Error has occurred while mounting SPIFFS");
          return;
      }  
  WiFi.begin(ssid, password);
  WiFi.config(ip,gateway,subnet,dns);
 
 
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connesso a ");
  Serial.println(ssid);
  Serial.print("Indirizzo IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  digitalWrite(gpio2_pin, LOW);
  
   //  WiFiClient client = server.available();
//     header('Access-Control-Allow-Origin: http:/localhost:3000');
 /*server.enableCORS(true);
  server.sendHeader("Access-Control-Allow-Origin", "*");*/


  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }


   server.on("/x", []() {
    stateString = "<p>" + String(digitalRead(gpio0_pin)) + "</p";
    server.send(200, "text/html", webPage + stateString);
  });
 
  server.on("/RelaySwitch/SetState/1", []() {
    if(is_authenticated())
    {
     state = 1;
              digitalWrite(gpio0_pin, HIGH);
               auto sJ = String( String(state));
    server.send(200, "application/json", sJ);
    }
   
  });
  
  server.on("/RelaySwitch/SetState/0", []() {
      if(is_authenticated())
   {
     state = 0;
              digitalWrite(gpio0_pin, LOW);
               auto sJ = String( String(state) );
    server.send(200, "application/json", sJ);
}
   
  });
 
  server.on("/RelaySwitch/GetState", []() {
       if(is_authenticated())
    {
    auto sJ = String( String(state));
    server.send(200, "application/json", sJ);
    }
   
  });

server.on("/RelaySwitch/GetStateDataInz", []() {
     if(is_authenticated())
    {
    auto sJ = String( String (dateTimeString));
    server.send(200, "application/json", sJ);
    Serial.println(dateTimeString);
    }
      
  });

  server.on("/RelaySwitch/GetStateDataFin", []() {
     if(is_authenticated())
    {
  
    auto sJ = String( String (aaString));
    server.send(200, "application/json", sJ);
     Serial.println(aaString);
    }
         
  });

  
  server.on("/RelaySwitch/SetDataInz", HTTP_POST, [](){
       if(is_authenticated())
    {
    auto json = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    //serializeJson(doc, Serial);
    server.send ( 200, "text/json", "{success:true}" );
 const char* xxxx = doc["xxxx"];
long long int timeInz = doc["timeInz"];
   dateTimeString = xxxx;
   dateTimeUtc1= timeInz;
    
    for(int i =0; i <= dateTimeString.length(); i++)
   {
    dateTime[i] = dateTimeString[i];
    
   }
  
   //Serial.println(dateTimeUtc1);
      // Serial.println(dateTimeUtc1);
    }
         
   });


server.on("/RelaySwitch/SetDataFin", HTTP_POST, [](){
     if(is_authenticated())
    {
    auto json = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
   // serializeJson(doc, Serial);
    server.send ( 200, "text/json", "{success:true}" );
 const char* xxxx = doc["xxxx"];
 long long int timeFin = doc["timeFin"];
   aaString = xxxx;
   aaUtc1 = timeFin;
  
   for(int i =0; i <= aaString.length(); i++)
   {
    aa[i] = aaString[i];
   }
    
   //Serial.println(aaUtc1);
  //Serial.println(aaUtc1);
    }
  
   });

  
  
  server.on("/Signin", HTTP_POST,[](){
    
    auto json = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
  //serializeJson(doc, Serial);
    const char* UserName1 = doc["username"];
    const char* Password1 = doc["password"];
      Password = Password1;
      UserName = UserName1;       
     //encrypt();
       randomSeed(millis()); 
    randomNumber = random(1, 2147483647);
    handleLogin();
    if(is_authenticated()== true){
      server.send(200);
    }
     server.send(404);
   });
 
  

 #include "buildMap.h"
   server.collectHeaders( "Cookie");
  server.begin();
  if(i ==0){
    server.begin();
    i=1;
  }
  Serial.println("HTTP server avviato");
 timeClient.begin();
  /* Serial.println(dateTimeString);
    Serial.println(aaString);*/
  
    
}


void  handleLogin() {
    // Serial.println("ooo");
    
    
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
 
 if (Password == PasswordAlby && UserName == UserNameAlby) {
server.sendHeader("Location", "/");
server.sendHeader("cache-control" ,"no-cache");
String token = String(randomNumber);
server.sendHeader("Set-Cookie", "ESPSESSIONID=" + token);

//Cookie2[i]=token.toInt();
co.AddNewCookie(token.toInt());

Serial.println("Log in Successful" );
for(int i=0; i<= 3; i++)
{
  Serial.println(co.Get(i));
}
server.send(200);
  }
 }

bool is_authenticated() {
  Serial.println("Enter is_authenticated"); 
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    int cookie1 =cookie.indexOf("ESPSESSIONID=");
    if (cookie1 >=0 ) {
      int token = atoi(&cookie[cookie1+13]);
      if(token != 0 && co.CheckCookie(token) == true){
        Serial.println("Authentication Successful");
        return true;
      }
    }
  }
  Serial.println("Authentication Failed");
  return false;
  
}

  
void loop() {

    server.handleClient();
    

  timeClient.update();
 time_t unix_t =timeClient.getEpochTime();

diff=difftime(unix_t, dateTimeUtc1); // dateTimeUtc
diff1=difftime(unix_t, aaUtc1); // aaUtc*/
/*Serial.println(diff);
Serial.println("inz");
Serial.println(diff1);
Serial.println("fin");*/
if(diff > 0 && diff1 < 0 )
{
  digitalWrite(gpio0_pin, HIGH); // || state
   state =1;
  y = true;
 }else{
 while(y == true){
    state=0;
    y= false;
  }
  digitalWrite(gpio0_pin, state);
  
  }
}



  
