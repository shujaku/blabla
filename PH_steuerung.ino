#define SensorPin A2            //pH meter Analog output to Arduino Analog Input 2
#define Offset 0.00            //deviation compensate
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40    //times of collection
#define PumpInterval  40 

#include <Wire.h>

int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex=0;
int Pumpe = 4;

void setup(void)
{
  pinMode(LED,OUTPUT);
  pinMode(Pumpe,OUTPUT);
  Serial.begin(9600);  
  Serial.println("pH meter experiment!");    //Test the serial monitor
  
}

void loop(void)
{
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;

  //prüfen ob die Pumpe eingeschalten oder ausgeschalten wurde//
  
  static int pumpeMode = 0;    //static = merkt sich die Zahlen hier vom pumpeMode//
  static int dosierStarts = 0;
 //
  static unsigned long pumpeActiveTime = millis();
  static unsigned long pumpeDownTime = millis();
  
  //messen und pH Berechnung//
  if(millis()-samplingTime > samplingInterval){
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
      samplingTime=millis();
  }
  //monitor Ausgabe//
  if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
     Serial.print("Voltage:");
     Serial.print(voltage,2);
     Serial.print("    pH value: ");
     Serial.println(pHValue,2);
     Serial.print("       dosierStarts ");
     Serial.println(dosierStarts);
     Serial.print("       pumpeMode ");
     Serial.println(pumpeMode);
     digitalWrite(LED,digitalRead(LED)^1);
     printTime=millis();
  }

  if(pumpeMode == 0){  //ausgelesener wert aus static int pumpeMode//
      if(dosierStarts=>10){
          pumpeMode = 3;
      } else if(pHValue>6.3){
          digitalWrite(Pumpe,LOW);
          pumpeMode = 1;     //schreibt in static eine 1//
          pumpeActiveTime = millis();
          dosierStarts = dosierStarts+1; //setzt dosierStarts//
       }
 } else if(pumpeMode == 1){   //ausgelesener wert aus static int pumpeMode//
    if(pHValue<5.7){
      digitalWrite(Pumpe,HIGH);
      pumpeMode = 0;      //schreibt in static eine 0//
      dosierStarts = 0; //rücksetzen der dosierStarts//
    }else if(millis()- pumpeActiveTime > 1000){  //laufzeit Pumpe //
      digitalWrite(Pumpe,HIGH);
      pumpeMode = 2;     //schreibt in static eine 2//
      pumpeDownTime = millis();
    }
  } else if(pumpeMode == 2)  {
    //2 Minuten Warten
    if(millis() - pumpeDownTime >2*40*1000){ //pausenzeit Pumpe//
      pumpeMode = 0;    //schreibt in static eine 0//
    }
  } else {
    if(pHValue<5.7){
      dosierStarts = 0;
      pumpeMode = 0;
    }
  }
}

double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}



