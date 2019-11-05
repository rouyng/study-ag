#include <string.h>

String command; // String input from command prompt
boolean stringComplete = false;
char * numA = NULL;
char * numB = NULL;
char * numC = NULL;
char delim = ',';
char inByte; // Byte input from USB connection
char carray[6]; // character array for string to int // manipulation
const int fan1 = 3;           // the PWM pin controlling fan voltage
long maxSpeed = 255;
unsigned long onTime = 5000;
unsigned long offTime = 30000;
long NUMA = 2150000000;
long NUMB = 2150000000;
long NUMC = 2150000000;
unsigned long pTimer1 = 0;
unsigned long pTimer2 = 0;
unsigned long pTimer3 = 0;


void setup(){
  pinMode(fan1, OUTPUT);
  Serial.begin(9600);
  command.reserve(200);
}

void serialEvent() {
  while(Serial.available() > 1 && stringComplete == false) {
    char inByte = (char)Serial.read();
    if (inByte == 'X') {
      stringComplete = true;
    }
    else {command += inByte;}
  }
  if (stringComplete == true){
    command.toCharArray(carray,35);
    numA = strtok(carray, &delim);
    numB = strtok(NULL, &delim);
    numC = strtok(NULL, &delim);
    NUMA = atol(numA);
    NUMB = atol(numB);
    NUMC = atol(numC);
    Serial.println("GOT:");
    Serial.println(NUMA);
    Serial.println(NUMB);
    Serial.println(NUMC);
    command = "";
    maxSpeed = map(NUMA, -2150000000, 2150000000, 50, 255);
    offTime = map(NUMB, -2150000000, 2150000000, 5000, 120000);
    onTime = map(NUMC, -2150000000, 2150000000, 1000, 10000);
    stringComplete = false;
  }
}


void loop(){
  unsigned long cTimer = millis();
  if(cTimer - pTimer1 > 10000){
    pTimer1 = cTimer;
    serialEvent();
  }
  analogWrite(fan1, maxSpeed);
  delay(onTime);
  analogWrite(fan1, 0);
  delay(offTime);
}