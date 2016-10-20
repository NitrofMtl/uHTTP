// This file is part of uHTTP.
//
// uHTTP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// uHTTP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.


#include <SD.h>
#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>

#include <uHTTP.h>

#define default_page "index.htm"  //set the default page to load on request, if not define, default: "index.htm"

//template to pass array number of element to function
template <typename T, size_t N>
inline
size_t SizeOfArray( const T(&)[ N ] )
{
  return N;
}

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF};
IPAddress ip(192, 168, 0, 110);
uHTTP *uHTTPserver = new uHTTP(80);
EthernetClient response;
EthernetServer server(80);

/*
  getContainer and putContainer old all dynamic GET and PUT request function. 
  Constant text file can be store on sd card and library will scan it automaticly.
  Remember to name file on SD on 8.3  format. ( For javascript file, use .jsx extention. )

*/
uHTTP_request getContainer[] = {    //resquest index : (ID, method, callback function)
/* {"ajax_inputs", writeJSONResponse},      //example how list dynamic request list
 {"ajax_alarms", writeJSON_Alarm_Response},
 {"configs", writeJSONConfigResponse} */
};

uHTTP_request putContainer[] = {    //resquest index : (ID, method, callback function)
/*  {"channels", parseJSONInputs},  //example how list dynamic request list
  {"alarms", parseJSONalarms},
  {"switch", parseJSONswitch},
  {"switchAlarms", parseJSONswitchAlarms},
  {"configs", parseJSONConfigs} */
};

void setup(){
  Serial.begin(9600);

  Serial.println("Startup !!!");
  Serial.print("Initializing SD card...");
  pinMode(SDC_PIN, OUTPUT);  //make sure that the default chip select pin is set to output
  digitalWrite(SDC_PIN, HIGH);
  delay(1);
  if (!SD.begin(SDC_PIN)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
  delay(1);
  bckup.begin();
  delay(10);
  
    Ethernet.begin(mac, ip);
  Serial.print("Ip address; "); Serial.println(Ethernet.localIP());
  server.begin();
  uHTTPserver->begin();
  uHTTPserver->uHTTPclient(&response);   //link uHTTP EthernetClient with sketch EthernetClient
  uHTTPserver->addToContainer(uHTTP_METHOD_GET, getContainer, SizeOfArray(getContainer)); //add get request to uhttp handeler
  uHTTPserver->addToContainer(uHTTP_METHOD_PUT, putContainer, SizeOfArray(putContainer)); //add put request to uhttp handeler
  
  
}


void loop(){
  uHTTPserver->requestHandler();
  delay(10);
}

/*
  this is example of data request using aduinoJson library, is just show the way how to post or receive it, not how to build or parse it.
  to do so , refer to arduinoJson docs:  https://github.com/bblanchon/ArduinoJson
  
 void writeJSONResponse() {
  StaticJsonBuffer<3000> jsonBuffer;
  JsonObject& json = JSONInputs(jsonBuffer);
  uHTTPserver->send_JSON_headers();
  json.printTo(response);
}

void parseJSONInputs() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(uHTTPserver->body());

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  //root.prettyPrintTo(Serial); Serial.println("inputs");  // for debug
  
}

  



*/
