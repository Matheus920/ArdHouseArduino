#include <Servo.h>        //Bibliotecas essenciais
#include <DHT.h>
#include <DHT_U.h>
#include <UIPEthernet.h>
#include <SPI.h>
#include <EEPROM.h>




int ledPin = 4;       ///declaração de pinos
int dhtPin = A0;
int ventPin = 6;
int servoPin = 7;
int servoTeste = 9;
int alarmPin = 8;

bool portaOP=false;   //variaveis auxiliares
bool ultimoEstado=false;
long int finalTime=0;
int vent;
String id="";       

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //definições de endereço
byte ip[] = { 192, 168, 15, 175 };
char serverG[]="ardhousenotifier.herokuapp.com"; 

EthernetServer server(80); //declaração do servidor e cliente
EthernetClient webClient;
EthernetClient hero;


DHT dht(dhtPin, DHT11); //declaração do sensor de temperatura
Servo myservo;      //declaração do servo

int processaString(String http); //cabeçalho das funções necessárias
String processaId(String Id);

String readString; //declaração da string que armazena o http

void setup() {
  Ethernet.begin(mac, ip); //inicia a comunicação com o modulo
  server.begin(); //inicia o servidor
  
  pinMode(ledPin, OUTPUT);//configuração dos registradores I/O
  pinMode(ventPin, OUTPUT);
  pinMode(servoTeste, OUTPUT);
  pinMode(alarmPin, OUTPUT);
  pinMode(3,INPUT);
  digitalWrite(ledPin, LOW);

  myservo.attach(servoPin);//inicialização do servo
  dht.begin();  //inicialização do DHT11


  Serial.begin(9600); //inicialização da comunicação serial

  id=EEPROM.read(0);  //leitura do Id do dispositivo da EEPROM 

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

                Serial.println(readString);
              
              client.println("HTTP/1.1 200 OK");//retorna um OK
              client.println("Connection: close");
              client.println("Content-Type:text/html");
              client.println();

           if (readString.indexOf("ledParam=1") >= 0) { // verifica se a requisição foi para ligar o LED
              digitalWrite(ledPin, HIGH);
              client.println(F("Lampada acesa com sucesso"));
            } else if (readString.indexOf("ledParam=0") >= 0) { // verifica se a requisição foi para desligar o LED
              digitalWrite(ledPin, LOW);
              client.println(F("Lampada apagada com sucesso"));
            } else if (readString.indexOf("ledStatus") >= 0) { // verifica se a requisição foi para checar o status do LED
              client.println(digitalRead(ledPin));
            } else if (readString.indexOf("tempUmi") >= 0) { // verifica se a requisição foi para a leitura do DHT11
              client.println(dht.readTemperature());
              client.println(dht.readHumidity());
            } else if (readString.indexOf("vent=") >= 0) { // verifica se a requisição foi para a definição da velocidade
              analogWrite(ventPin, processaString(readString)); // PWM
              client.println(F("Velocidade definida com sucesso"));
            } else if(readString.indexOf("ventStatus")>=0){  // verifica se a requisição foi para a leitura da velocidade
              client.print(vent);
            } else if(readString.indexOf("portOpen")>=0){   // verifica se a requisição foi para abrir a porta
              portaOP=true;    
              finalTime=millis()+5000;                      //define o valor final do timer
              client.println(F("Porta aberta com sucesso"));
            }else if(readString.indexOf("alarmStatus")>=0){   // verifica se a requisição foi para ler o status do alarme
              client.print(digitalRead(alarmPin));
            }else if(readString.indexOf("alarmParam=1")>=0){ // verifica se a requisição foi para ligar o alarme
              digitalWrite(alarmPin,HIGH);
              client.println(F("Alarme ligado com sucesso"));
            }else if(readString.indexOf("alarmParam=0")>=0){ // verifica se a requisição foi para desligar o alarme
              digitalWrite(alarmPin,LOW);
              client.println(F("Alarme desligado com sucesso"));
            }else if(readString.indexOf("setId=")>=0){ // verifica se a requisição foi para gravar o ID do dispositivo
              id="";
              id=processaId(readString);          
              EEPROM.write(0,id.toInt());               //grava o ID na EEPROM
              client.println(F("ID gravado com sucesso"));
            }
            
          }
          readString = ""; //limpa a string para novas requisições
          client.stop(); // desliga o cliente
        }
      }
    }
  }


  if(portaOP && (millis()<finalTime)){  //controle do timer e estado da porta
    //myservo.write(180);
    digitalWrite(servoTeste,HIGH);
  }else{
    //myservo.write(0);
    digitalWrite(servoTeste,LOW);
  }

  if(digitalRead(3)==HIGH && ultimoEstado==false){            // envio da solicitação para o aplicativo
    ultimoEstado=true;                                        //controle da borda de subida
    Serial.println("conectando");
  if (hero.connect(serverG, 80)) {
    Serial.println("connected");
    hero.print("GET /");
    hero.print(id);
    hero.println(" HTTP/1.0");
    hero.println("Host: ardhousenotifier.herokuapp.com");
    hero.println("Connection: close");
    hero.println();
    hero.stop();
  }else{
    Serial.println("erro");
    }
  }else{
    ultimoEstado=false;
  }

  
}

int processaString(String http) {         //processamento da velocidade do ventilador
                                          //isola o valor do PWM

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

String processaId(String Id){       //processamento do ID
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
