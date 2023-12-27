#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <time.h> 
#include <ArduinoJson.h>
#include <Preferences.h>


Preferences prefs;
MDNSResponder mdns;
WiFiUDP ntpUDP;
const char* ssid = "WiFi-BBEvol6082";
const char* password = "****************";
IPAddress ip(192,168,1,2);
IPAddress dns(8,8,8,8);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
ESP8266WebServer server(80);
int gpio0_pin = 15;
int gpio2_pin = 2;
int  state= 0;
bool stateAuto;
bool stateManu;

 
struct DayProgram
{
  int start;
  int end;
};

DayProgram weekProgram[7];
String weekProgramString;

void UpdateWeekProgramString(){
  String res = "[";
  for(auto i=0; i<7; ++i) {
    if(i>0) 
      res += ",";        
    res += weekProgram[i].start;
    res += ",";
    res += weekProgram[i].end;
  }
  res += "]";
  weekProgramString = res;
}


void setup() {
  pinMode(gpio0_pin, OUTPUT);
  digitalWrite(gpio0_pin, LOW);
  pinMode(gpio2_pin, OUTPUT);
  digitalWrite(gpio2_pin, HIGH);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  WiFi.config(ip,gateway,subnet,dns);
 

  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
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

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/api/RelaySwitch/SetState/1",HTTP_GET, []() {
    state = 1;
    if(stateManu==true)
    {
      digitalWrite(gpio0_pin, HIGH);
    }
    auto sJ = String( String(state));
    server.send(200, "text/plain", sJ);
     

   
  });
  
  server.on("/api/RelaySwitch/SetState/0",HTTP_GET, []() {
    state = 0;
    if(stateManu==true)
    {
      digitalWrite(gpio0_pin, LOW);
    }
    auto sJ = String( String(state) );
    server.send(200, "text/plain", sJ);
    
   
  });
 
  server.on("/api/RelaySwitch/GetState",HTTP_GET, []() {
    auto sJ = String( String(state));
    server.send(200, "text/plain", sJ);
  });

  server.on("/api/RelaySwitch/GetProgrammAuto",HTTP_GET, []() {
    auto sJ = String(stateAuto);
    server.send(200, "application/json", sJ);
  });

  server.on("/api/RelaySwitch/GetProgrammManu",HTTP_GET, []() {
    auto sJ = String(stateManu);
    server.send(200, "application/json", sJ);
  });

  server.on("/api/RelaySwitch/GetWeekProgram",HTTP_GET, []() {  
    server.send(200, "application/json", weekProgramString);
  });

  
  server.on("/api/RelaySwitch/SetDataInz", HTTP_POST, [](){      
    auto json = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    //serializeJson(doc, Serial);
    server.send ( 200, "text/json", "{success:true}" );
    const char* xxxx = doc["xxxx"];
    long long int timeInz = doc["timeInz"];
    int day = doc["day"];
    weekProgram[day].start= timeInz;
     Serial.println(weekProgram[day].start);
    UpdateWeekProgramString();
    prefs.begin("abc",false);      
    prefs.putBytes("WP", &weekProgram, sizeof(weekProgram));
    prefs.end();
   });

server.on("/api/RelaySwitch/SetDataFin", HTTP_POST, [](){
    
  auto json = server.arg("plain");
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);
  server.send ( 200, "text/json", "{success:true}" );
  const char* xxxx = doc["xxxx"];
  long long int timeFin = doc["timeFin"];
  int day = doc["day"];
  weekProgram[day].end = timeFin;
  Serial.println(weekProgram[day].end);
  UpdateWeekProgramString();
  prefs.begin("abc",false);
  prefs.putBytes("WP", &weekProgram, sizeof(weekProgram));
  prefs.end();
});


  server.on("/api/RelaySwitch/stateProgrammAuto",HTTP_POST,[](){
      auto json = server.arg("plain");
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, json);
      server.send ( 200, "text/json", "{success:true}" );
      stateAuto= doc["stateProgrammAuto"];
      prefs.begin("abc",false);
      prefs.putBool("AU", stateAuto);
      prefs.end();


  });

  server.on("/api/RelaySwitch/stateProgrammManu",HTTP_POST,[](){
      auto json = server.arg("plain");
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, json);
      server.send ( 200, "text/json", "{success:true}" );
      stateManu= doc["stateProgrammManu"];
      prefs.begin("abc",false);
      prefs.putBool("MA", stateManu);
      prefs.end();


  });
  server.begin();
  Serial.println("HTTP server avviato");

  prefs.begin("abc",false);

  if (not prefs.isKey("TZ")) {
   int yy= prefs.putString("TZ", "CET-1CEST,M3.5.0,M10.5.0/3");
    Serial.println(yy);
  }
 
  String tz = prefs.getString("TZ");
  Serial.println(tz);

 
  configTime(0, 0, "time.nist.gov");
  // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.json
  //"Europe/Rome":"CET-1CEST,M3.5.0,M10.5.0/3",
  setenv("TZ",tz.c_str(),1);
  tzset();

   if(prefs.isKey("WP")){
   auto t = prefs.getBytes("WP", &weekProgram, sizeof(weekProgram));
  }
  UpdateWeekProgramString();
  if(prefs.isKey("AU")){
    stateAuto=prefs.getBool("AU", stateAuto);
  }
  if(prefs.isKey("MA"))
  {
    stateManu =prefs.getBool("MA", stateManu);
    //Serial.print(y);
  }
  prefs.end();  

}

void loop() {
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  auto t_sec = timeinfo.tm_sec + timeinfo.tm_min * 60 + timeinfo.tm_hour * 3600;
  auto& thisDayProgram = weekProgram[timeinfo.tm_wday];
  if(stateAuto==true)
  {
    state=  t_sec > thisDayProgram.start && t_sec < thisDayProgram.end;
    digitalWrite(gpio0_pin, state);
  }
  server.handleClient();
}
