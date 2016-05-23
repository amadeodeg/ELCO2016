#include <SFE_BMP180.h>
#include <Wire.h>
#include <stdlib.h>

//A4=SDA A5=SCL

#define BMP_MODE_PRESSURE 3

SFE_BMP180 bmp180;
static int cnt=0;


void setupBmp(){
   bmp180.begin();
}

void setup() {
  Serial.begin(9600);
  setupBmp();
}

void getBMP (double* result){  
    int error =0;
    //double result[2];
    char status;
    double T,P,p0,a;
    status = bmp180.startTemperature();
    if (status != 0){
      // Wait for the measurement to complete:
      delay(status); //4.5 -> 5 ms     
      status = bmp180.getTemperature(T); //Devuelve la temperatura en T
      if (status != 0){
        status = bmp180.startPressure(BMP_MODE_PRESSURE); //mejor resolución : 25.5 -> 26 ms
        if (status != 0){                 //pag 12 datasheet
          // Wait for the measurement to complete:
          delay(status);
          status = bmp180.getPressure(P,T);
          if (status != 0){
            
          }else { error =1; }
        }else { error =1; }
      }else { error =1; }
    }else { error =1; }

    if(error){
      result[0] = 0;
      result[1] = 0;
    }else{
      result[0] = T;
      result[1] = P;
    }
   // return result;
}

void loop() {
  // put your main code here, to run repeatedly:
  double* bmpResult=(double*)malloc(2*sizeof(double));
  long time1, time2;
  while(1){
  time1 = micros();
  getBMP(bmpResult);
  time2 = micros();
  // Print out the measurement:
  Serial.print(cnt++);
  Serial.print("\ttemperature: ");
  if(bmpResult[0]==0){
    Serial.print("error");
  }else{
    Serial.print(bmpResult[0]);
  }
  Serial.print(" C\t");
  // Print out the measurement:
  Serial.print("pressure: ");
  if(bmpResult[1]==0){
    Serial.print("error");
  }else{
    Serial.print(bmpResult[1]);
  }
  Serial.print(" mb\t");
  Serial.print(time2-time1);
  Serial.println(" uS");
  delay(1000);
  }
}

