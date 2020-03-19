#include <etherShield.h>
#include <ETHER_28J60.h>

static uint8_t mac[6] = {0x54, 0x55, 0x58, 0x10, 0x00, 0x24};  
static uint8_t ip[4] = {192, 168, 3, 55}; 
static uint16_t port = 80;
int lampada = 3; 

ETHER_28J60 client;

void setup() {
client.setup(mac, ip, port);
pinMode(lampada, OUTPUT);
digitalWrite(lampada, LOW);
}

void loop() {
  char* params;
  if (params = client.serviceRequest()) {    
         
    if (strcmp(params, "?lampada") == 0) {
      digitalWrite(lampada, !digitalRead(lampada));
    }
 
    client.respond();
  }
}
