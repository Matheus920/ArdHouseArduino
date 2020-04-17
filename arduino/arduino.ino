#include <Servo.h>
#include <DHT.h>
#include <DHT_U.h>
#include <UIPEthernet.h>
#include <SPI.h>




int ledPin = 4;
int dhtPin = A0;
int ventPin = 6;
int servoPin = 7;
int servoTeste = 9;
int alarmPin = 8;
bool portaOP=false;
long int finalTime=0;
int vent;
String id;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //definiçcoes de endereço
byte ip[] = { 192, 168, 15, 175 };
EthernetServer server(80);

DHT dht(dhtPin, DHT11);
Servo myservo;

int processaString(String http);

String readString = String(30); //declaração da string que armazena o http

void setup() {
  Ethernet.begin(mac, ip); //inicia a comunicação com o modulo
  server.begin(); //inicia o servidor
  pinMode(ledPin, OUTPUT);
  pinMode(ventPin, OUTPUT);
  pinMode(servoTeste, OUTPUT);
  digitalWrite(ledPin, LOW);
  myservo.attach(servoPin);
  dht.begin();
}

void loop() {
  EthernetClient client = server.available();
  if (client) { //caso exista um cliente:
    while (client.connected()) { // enquanto estiver conectado
      if (client.available()) { //se o cliente estiver disponivel
        char c = client.read(); //le os caracteres da requisição
        if (readString.length() < 100) {
          readString += c; // transfere os caracteres para string
        }

        if (c == '\n') { //se encontrar o '\n' eh o final da requisição
          if (!(readString.indexOf("?") < 0)) { //verifica se existe o caractere '?'

              client.println("HTTP/1.1 200 OK");//retorna um OK
              client.println("Connection: close");
              client.println("Content-Type:text/html");
              client.println();

           if (readString.indexOf("ledParam=1") >= 0) { // verifica se a requisição foi para ligar o LED
              digitalWrite(ledPin, HIGH);
            } else if (readString.indexOf("ledParam=0") >= 0) { // verifica se a requisição foi para desligar o LED
              digitalWrite(ledPin, LOW);
            } else if (readString.indexOf("ledStatus") >= 0) { // verifica se a requisição foi para checar o status do LED
              //client.println("<h1>");
              client.println(digitalRead(ledPin));
              client.stop();
              //client.println("</h1>");
            } else if (readString.indexOf("tempUmi") >= 0) {
              client.println(dht.readTemperature());
              client.println(dht.readHumidity());
            } else if (readString.indexOf("vent=") >= 0) {
              analogWrite(ventPin, processaString(readString));
            } else if(readString.indexOf("ventStatus")>=0){
              client.print(vent);
            } else if(readString.indexOf("portOpen")>=0){
              portaOP=true;    
              finalTime=millis()+5000;        
            }else if(readString.indexOf("alarmStatus")>=0){
              client.print(digitalRead(alarmPin));
            }else if(readString.indexOf("alarmParam=1")>=0){
              digitalWrite(alarmPin,HIGH);
            }else if(readString.indexOf("alarmParam=0")>=0){
              digitalWrite(alarmPin,LOW);
            }else if(readString.indexOf("setId=")>=0){
              id=processaId(readString);
            }
            
          }
          readString = ""; //limpa a string para novas requisições
          client.stop(); // desliga o cliente
        }
      }
    }
  }


  if(portaOP && (millis()<finalTime)){
    //myservo.write(180);
    digitalWrite(servoTeste,HIGH);
  }else{
    //myservo.write(0);
    digitalWrite(servoTeste,LOW);
  }
}

int processaString(String http) {

  String aux = "";
  bool finish = false;
  bool found = false;

  for (int i = 0; i < http.length(); i++) {
    char c = http[i];
    if (found && (c == ' ' || c == '\n')) {
      vent=aux.toInt();
      return aux.toInt();
    }

    if (found) {
      aux += c;
    }

    if (c == '=') {
      found = true;
    }
  }
  vent=aux.toInt();
  return aux.toInt();
}

String processaId(String Id){
  String aux = "";
  bool finish = false;
  bool found = false;

  for (int i = 0; i < Id.length(); i++) {
    char c = Id[i];
    if (found && (c == ' ' || c == '\n')) {
      return aux;
    }

    if (found) {
      aux += c;
    }

    if (c == '=') {
      found = true;
    }
  }
  return aux;
}
