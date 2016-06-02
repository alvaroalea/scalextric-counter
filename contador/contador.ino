#include <TM1638.h>
TM1638 module(7, 4, 2);
int vueltas1 = 10;
int vueltas2 = 10;

void vueltas(){
  int d1,d2,d3,d4;
  d2 = vueltas1 % 10;
  d1 = vueltas1 / 10;
  d4 = vueltas2 % 10;
  d3 = vueltas2 / 10;
  
  module.setDisplayDigit(d1, 0, false);
  module.setDisplayDigit(d2, 1, false);
  module.setDisplayDigit(d3, 6, false);
  module.setDisplayDigit(d4, 7, false);
}

void setup() {
  // put your setup code here, to run once:
  int i;
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  Serial.begin(115200);
  
  module.clearDisplay();              //clears the display from garbage if any
  module.setDisplayToString("ALEAsoft");    //prints the banner
  for(i=7;i!=0;i--){
    module.setupDisplay(true,i);
    delay(150);                        //small fadeoff about 1.5 sec
  }
  module.clearDisplay();              //clears the display from garbage if any
  module.setupDisplay(true,7);
  vueltas();
  

  int tecla;

  do {
    tecla = module.getButtons();
    } while (tecla != 0b00000001);
  do {
    tecla = module.getButtons();
    } while (tecla != 0b00000000);

  module.setLEDs(0x0003);
  delay(1000);
  module.setLEDs(0x000F);
  delay(1000);
  module.setLEDs(0x003F);
  delay(1000);
  module.setLEDs(0x00FF);
  delay(1000);
  module.setLEDs(0xFF00);

}

int pos1 = 0;
int pos2 = 0;
int in1 = 0;
int in2 = 0;
int cambio =0;
void loop() {
  // put your main code here, to run repeatedly: 
in1 = digitalRead(6);
in2 = digitalRead(5);
Serial.print(in1);
Serial.print(in2);

if ((pos1 ==0) && (in1 == 0)) {
  pos1 = 1;
  }
if ((pos2 ==0) && (in2 == 0)) {
  pos2 = 1;
  }
if ((pos1 ==1) && (in1 == 1)) {
  pos1 = 0;
  vueltas1 = vueltas1 -1;
  cambio =1;
  }
if ((pos2 ==1) && (in2 == 1)) {
  pos2 = 0;
  vueltas2 = vueltas2 -1;
  cambio =1;
  }
 
if (cambio==1){
  cambio =0;
  vueltas();
  module.setLEDs(0x0000);
  }  

if (vueltas1 ==0) {
    if (vueltas2 == 0) {
       module.setDisplayToString("-Empate-");    //prints the banner
       delay(5000);
    } else {
       module.setDisplayToString("GANA    ");    //prints the banner
       delay(5000);
    }
} else {
    if (vueltas2 == 0) {
       module.setDisplayToString("    GANA");    //prints the banner
       delay(5000);
    }
}    
  
delay(10);
}
