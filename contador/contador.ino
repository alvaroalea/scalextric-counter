/* (C) 2014 Alvaro Alea Fernandez <alvaroalea@gmail.com>  *
 * Distribuido con licencia GPL Version 3                 */

#define NO_PORTB_PINCHANGES // to indicate that port b (D9 - D13) will not be used for pin change interrupts
#define NO_PORTC_PINCHANGES // to indicate that port c (A0 - A5) will not be used for pin change interrupts
// #define NO_PIN_STATE        // to indicate that you don't need the pinState
// #define NO_PIN_NUMBER       // to indicate that you don't need the arduinoPin
// #define       DISABLE_PCINT_MULTI_SERVICE
#include <PinChangeInt.h> // ckeck https://code.google.com/p/arduino-pinchangeint/

#include <TM1638.h> // ckeck https://code.google.com/p/tm1638-library/
TM1638 module(7, 4, 2);

#define ST_STOP 0
#define ST_RUN 50
int state = 0 ;

int tick = 0;
int vueltas1 ; 
int vueltas2 ;
int nvueltas = 11; //10 (+1) vueltas por defecto, hay que tener en cuenta que segun sales descuenta una.

// La inicializacion y banner de presentacion.
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
  module.setDisplayToString(" REAdy? ");    //prints the banner
}

// esto imprime el nº de vueltas en el display.
void vueltas(){
  int d1,d2,d3,d4;
  d2 = vueltas1 % 10;
  d1 = vueltas1 / 10;
  d4 = vueltas2 % 10;
  d3 = vueltas2 / 10;

  if ((vueltas1 == nvueltas) && (state>=50)) {
   module.setDisplayToString("GO    ", 0, 0);
  } else {  
    module.setDisplayDigit(d1, 0, false);
    module.setDisplayDigit(d2, 1, false);
    module.clearDisplayDigit(2, false);  
    module.clearDisplayDigit(3, false);
    module.clearDisplayDigit(4, false);
    module.clearDisplayDigit(5, false); 
  }
  if ((vueltas2 == nvueltas) && (state>=50)) {
   module.setDisplayToString("GO", 0, 6);
  } else {  
    module.setDisplayDigit(d3, 6, false);
    module.setDisplayDigit(d4, 7, false);
  }
PCintPort::attachInterrupt(5, &interrupcion, CHANGE); // Tambien admite RISING
PCintPort::attachInterrupt(6, &interrupcion, CHANGE);
}

// la interrupcion que lee los sensores
// Podria mejorarse, si solo se la llama en RISING y usa una funcion para cada uno.
volatile int sensores;
void interrupcion(){ // vector de interrupcion, mientras se ejecuta las interrupciones estas desactivades. ¿Seguro?
 static int laststate= 0x03;
 int in1, in2;
 in1=digitalRead(6);
 in2=digitalRead(5);
 if (((in1==1) && ((laststate & 0b001)==0)) && ((sensores & 0b001)==0)) sensores += 0b001 ; // deteccion de flanco, logica inversa, coche=0, vacio=1 
 if (((in2==1) && ((laststate & 0b010)==0)) && ((sensores & 0b010)==0)) sensores += 0b010 ; // si es un uno, antes era un cero, y sensores esta a cero, ponlo a uno. 
 laststate = in1 + ( 0b010 * in2 );
}

// Devuelve el bit de la tecla pulsada cuando esta se suelta.
int leetecla(){
static int tecla_prev = 0;
int tecla; 
int r= 0;
tecla = module.getButtons();
if (((tecla_prev & 0x01)!=0) && ((tecla & 0x01)==0)) r += 0x01 ;
if (((tecla_prev & 0x02)!=0) && ((tecla & 0x02)==0)) r += 0x02 ;
if (((tecla_prev & 0x04)!=0) && ((tecla & 0x04)==0)) r += 0x04 ;
if (((tecla_prev & 0x08)!=0) && ((tecla & 0x08)==0)) r += 0x08 ;
if (((tecla_prev & 0x10)!=0) && ((tecla & 0x10)==0)) r += 0x10 ;
if (((tecla_prev & 0x20)!=0) && ((tecla & 0x20)==0)) r += 0x20 ;
if (((tecla_prev & 0x40)!=0) && ((tecla & 0x40)==0)) r += 0x40 ;
if (((tecla_prev & 0x80)!=0) && ((tecla & 0x80)==0)) r += 0x80 ;
tecla_prev = tecla;
return r;  
}

/*  
// simple rutina que lee flancos en los sensores, como se llama aprox cada 10ms, actua ademas de antirebote.
int leesensores(){ // FIXME reescribir para que sea similar a leetecla
static int pos1 = 0;
static int pos2 = 0;
int r=0;
int in1;
int in2;

in1 = digitalRead(6); // recordad, con pull-up, =1 en reposo, =0 activa.
in2 = digitalRead(5);
if ((pos1 ==0) && (in1 == 0)) {
  pos1 = 1;
  }
if ((pos1 ==1) && (in1 == 1)) {
  pos1 = 0;
  r = r + 0b0001;
  }
if ((pos2 ==0) && (in2 == 0)) {
  pos2 = 1;
  }
if ((pos2 ==1) && (in2 == 1)) {
  pos2 = 0;
  r = r + 0b0010;
  }

return r;
}
*/

void loop() {
  // put your main code here, to run repeatedly: 
int teclas;
//int sensores;
int cambio =0;
static int tick1 =0;
static int tick2 =0;

teclas=leetecla();
if ((teclas & 0x01)!=0) { // empezar la carrera
  vueltas1 = nvueltas;
  vueltas2 = nvueltas;
  tick1 = tick2 = tick = 0 ;
  state = ST_RUN ;
  module.setDisplayToString("--    --");    //prints the banner
  cambio = 0;
  sensores = 0 ; //borramos sensores pendientes.
}
if ((teclas & 0x02)!=0) { // subir nº vueltas
  nvueltas +=1;
  if (nvueltas >=99) nvueltas = 99;
  vueltas1 = nvueltas-1;
  vueltas2 = nvueltas-1;
  state = ST_STOP ;
  cambio = 1;
}
if ((teclas & 0x04)!=0) { // bajar nº vueltas
  nvueltas -= 1;
  if (nvueltas <=2) nvueltas = 2;
  vueltas1 = nvueltas-1;
  vueltas2 = nvueltas-1;
  state = ST_STOP ;
  cambio = 1;
}

if (state >= 50) {  //Los modos >=50 el temporizador corre.
  tick +=1;
  // control de semaforo
  if (tick == 1) {
     module.setLEDs(0x0000);
  } else if (tick == 100) {
     module.setLEDs(0x0003);
  } else if (tick == 200) {
     module.setLEDs(0x000F);
  } else if (tick == 300) {
     module.setLEDs(0x003F);
  } else if (tick == 400) {
     module.setLEDs(0x00FF);
  } else if (tick == 500) { // LED EN VERDE USAR EL VALOR MAS ABAJO
     module.setLEDs(0xFF00);
     cambio = 1;
  } else if (tick == 600) {
     module.setLEDs(0xFF00);
  } else if (tick == 800) {
     module.setLEDs(0x0000);
  }

//sensores=leesensores();
// Evaluamos salida en falso
if (tick < 500){ // Valor de led en verde.
  if ((sensores & 0b0001)!=0) {
     sensores = sensores - 0b001;
     module.setDisplayToString("FAUL    ");    //prints the banner
     cambio = 0;
     state = ST_STOP;
  }
  if ((sensores & 0b0010)!=0) {
     sensores = sensores -0x010;
     module.setDisplayToString("    FAUL");    //prints the banner
     cambio = 0;
     state = ST_STOP;
  }
} else {
  // contador de vueltas
  if ((sensores & 0b0001)!=0) {
    if (tick1<tick) {
      vueltas1 = vueltas1 -1;
      tick1= tick + 50; // esto evita rebotes lentos, p.e. si el coche tiene 2 imanes.
      cambio =1;
    } else {
      sensores = sensores - 0b001; // si hubiera un rebote lo quitamos.
    }
  }
  if ((sensores & 0b0010)!=0) {
    if (tick2<tick) {
      vueltas2 = vueltas2 -1;
      tick2= tick + 50;
      cambio =1;
    } else {
      sensores = sensores - 0b010;   
    } 
  }
}
// Evaluamos si alguno gana
if (vueltas1 ==0) {
    if (vueltas2 == 0) {
       module.setDisplayToString("-Empate-");    
       cambio = 0;
       state = ST_STOP;
    } else {
       module.setDisplayToString("GANA    ");    
       cambio = 0;
       state = ST_STOP;
    }
} else {
    if (vueltas2 == 0) {
       module.setDisplayToString("    GANA");    
       cambio = 0;
       state = ST_STOP;
    }
}    

} // if (state >= 50)
  
if (cambio==1){
  cambio = 0;
  vueltas();
  }  
delay(10);
}

