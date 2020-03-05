bool lampState = false;

void setup(){
	pinMode(3, OUTPUT);
}

void loop(){
 	controlLamp(); 
}

// Função para ligar/desligar lâmpada

void controlLamp(){
  if(lampState){
    digitalWrite(3, LOW);
    lampState = false;
  }else {
    digitalWrite(3, HIGH);
    lampState = true;
  }
  
}
