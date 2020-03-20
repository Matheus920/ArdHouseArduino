#include <SPI.h>
#include <UIPEthernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
byte ip[] = { 192, 168, 15, 175 }; 
EthernetServer server(80); 

int ledPin = 4; 

String readString = String(30); 
int status = 0; 

void setup(){
  Ethernet.begin(mac, ip); 
  server.begin(); 
  pinMode(ledPin, OUTPUT); 
  digitalWrite(ledPin, LOW); 
  }

void loop(){
    EthernetClient client = server.available();
    if(client){
        while(client.connected()){
            if(client.available()){
                char c = client.read();
                if(readString.length() < 100){
                    readString += c;
                }

                if(c == '\n'){
                    if(readString.indexOf("?")<0){

                    }else{
                        if(readString.indexOf("ledParam=1")>0){
                            digitalWrite(ledPin,HIGH);
                        } else{
                            digitalWrite(ledPin, LOW);
                        }
                         client.println("HTTP/1.1 200 OK");                       
                    }
                    readString="";
                    client.stop();
                }
            }
        }
    }
}