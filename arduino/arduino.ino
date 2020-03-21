#include <SPI.h>
#include <UIPEthernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //definiçcoes de endereço
byte ip[] = { 192, 168, 15, 175 }; 
EthernetServer server(80); 

int ledPin = 4; 
bool ledStatus = 0;

String readString = String(30); //declaração da string que armazena o http

void setup(){
  Ethernet.begin(mac, ip); //inicia a comunicação com o modulo
  server.begin(); //inicia o servidor
  pinMode(ledPin, OUTPUT); 
  digitalWrite(ledPin, LOW); 
  }

void loop(){
    EthernetClient client = server.available(); 
    if(client){ //caso exista um cliente:
        while(client.connected()){ // enquanto estiver conectado
            if(client.available()){ //se o cliente estiver disponivel
                char c = client.read(); //le os caracteres da requisição
                if(readString.length() < 100){
                    readString += c; // transfere os caracteres para string
                }

                if(c == '\n'){//se encontrar o '\n' eh o final da requisição
                     if(!(readString.indexOf("?")<0)){ //verifica se existe o caractere '?'
                        if(readString.indexOf("ledParam=1")>=0){ // verifica se a requisição foi para ligar o LED
                              digitalWrite(ledPin, HIGH);
                        }else if(readString.indexOf("ledParam=0")>=0){ // verifica se a requisição foi para desligar o LED
                              digitalWrite(ledPin, LOW);
                        }else if(readString.indexOf("ledStatus")>=0){ // verifica se a requisição foi para checar o status do LED
                         // client.println("<h1>");
                          client.println(digitalRead(ledPin));
                        // client.println("</h1>");
                        }
                    }

                    client.println("HTTP/1.1 200 OK"); //retorna um OK
                    readString=""; //limpa a string para novas requisições
                    client.stop(); // desliga o cliente
                }
            }
        }
    }
}
