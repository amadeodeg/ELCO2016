#include <DHT.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include <stdlib.h>
#include <SPI.h>
#include "RF24.h"

//A4=SDA A5=SCL

#define BMP_MODE_PRESSURE 3
#define UVOUT  A0 //Output from the sensor
#define REF_3V3  A1 //3.3V power on the Arduino board
#define ADDRESSHMC 0x1E //0011110b, I2C 7bit address of HMC5883
#define DHTPIN 5
#define DHTTYPE DHT11
#define PLUVPIN 2
#define ANEMOMETERPIN 6
#define FACTOR 0.5474
#define FACTOR_T_ANEM 15
#define FACTOR_R_ANEM 63

SFE_BMP180 bmp180;
DHT dht(DHTPIN, DHTTYPE);

int* magnetometerResult=(int*)malloc(3*sizeof(int));
double* bmpResult=(double*)malloc(2*sizeof(double));
float uvResult;
float* humidity=(float*)malloc(sizeof(float));
double* precip=(double*)malloc(sizeof(double));
double* anemometer=(double*)malloc(sizeof(double));
int res = 0;

int cnt = 0;

bool pluctrl = true;
int plu1=0;
int plu2=0;
bool anemometerctrl = true;
int ane1=0;
int ane2=0;
//---------------------TRANSMITTER------------------
bool radioNumber = 0;
RF24 radio(9,10);
byte addresses[][6] = {"1Node","2Node"};
bool role = 1;
long* array=(long*)malloc(6*sizeof(long));
bool success=0;
int last=0;
void setupTransmit(){
  Serial.println(F("ESTACION METEOROLOGICA"));
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));
  
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);

  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  radio.startListening();
  }
  
void transmit(long* array){ 
      if (role == 1)  {
          radio.stopListening();                                    // First, stop listening so we can talk.
          Serial.println(F("\nNow sending"));
          if (!radio.write( array, sizeof(long))){
             Serial.println(F("\nfailed"));
           }
          Serial.print(F("\nSending... "));
          Serial.print(*array);
          
          radio.startListening();                                    // Now, continue listening
          unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
          boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
          
          while ( ! radio.available() ){                             // While nothing is received
            if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
                timeout = true;
                break;
            }      
          }
              
          if ( timeout ){                                             // Describe the results
              Serial.println(F("\nFailed, response timed out."));
               Serial.println(last);
              success=0;
          }else{
              long got_value;
              unsigned long end_time = micros();
              radio.read( &got_value, sizeof(double));
              Serial.print(F(", Got response "));
              Serial.print(got_value);
              success=1;
          }
          delay(1000);
        }
         /****************** Change Roles via Serial Commands ***************************/
        if ( Serial.available() )
        {
          char c = toupper(Serial.read());
          if ( c == 'T' && role == 0 ){      
            Serial.println(F("***  TRANSMIT ROLE ***"));
            role = 1;                  // Become the primary transmitter (ping out)
         }
        }
  }
//--------------------------------------------------

void setupBmp(){
   bmp180.begin();
}

void setUV(){
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
}

void setupMagnetometer(){
  Wire.begin(); //creo que esta línea es innecesaria
  Wire.beginTransmission(ADDRESSHMC); //open communication with HMC5883
  Wire.write(0x02); //select mode register : Pag 11 datasheet
  Wire.write(0x00); //continue measurement mode : Pag 14 datasheet
  Wire.endTransmission();
}

void setupPluv(){
  attachInterrupt(digitalPinToInterrupt(PLUVPIN),pluvHandler,RISING);
}
void pluvHandler(){
  Serial.println("HOLAAA");
  if(pluctrl){
    plu1++;
  }else{
    plu2++;
  }
}
void setupAnemometer(){
  attachInterrupt(digitalPinToInterrupt(3),anemometerHandler,RISING);
}
void anemometerHandler(){
  if(anemometerctrl){
    ane1++;
  }else{
    ane2++;
  }
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
}

float getUV(){
  
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
  
  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputVoltage = 3.3 / refLevel * uvLevel;
  
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level
  return uvIntensity;
}

int averageAnalogRead(int pinToRead){
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setupHumidity(){
  dht.begin();
}

void getMagnetometer(int* result){
  int x, y, z;  //16 bit : 2 byte
  Wire.beginTransmission(ADDRESSHMC);
  Wire.write(0x03); //select register 3, X MSB register -> Pag: 11 datasheet
  Wire.endTransmission();
  //Read data from each axis, 2 registers per axis
  Wire.requestFrom(ADDRESSHMC, 6); //Wire.requestFrom(address, quantity)                              //quantity: the number of bytes to request
  if(6<=Wire.available()){ //receive() : The next byte received.
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
  }else{
    x=-1;
    y=-1;
    z=-1;
  }
  //result[0]=x;
  //result[1]=y;
  //result[2]=z;
  if(magnetometerResult[0]==-1 && magnetometerResult[1]==-1 && magnetometerResult[2]==-1){
    Serial.println("ERROR");
  }else{
    
   if(750 < x & x < 1000 & -666 < z & z < 0){
      res = 0;}
    if(-110 < x & x < 650 & 630 > z & z > 0){
      res = 30;}
    if(-950 < x & x < -110 & 1840 > z & z > 600){
      res = 60;}
    if(-1650 < x & x < -950 & 1840 > z & z > 1500){
      res = 90;}
    if(-1650 < x & x < -900 & -500 < z & z < 1500){
      res = 120;}
    if(-900 < x & x < 50 & -1420 < z & z < -500){
      res = 150;}
    if(50 < x & x < 700 & -1600 < z & z < -1420){
      res = 180;}
    if(700 < x & x < 945 & -1450 < z & z < -1350){
      res = -150;}
    if(945 < x & x < 1000 & -1500 < z & z < -1300){
      res = -120;}
    if(1000 < x & -1130 < z & z < -950){
      res = -90;}
    if(1000 < x & -950 < z & z < -800){
      res = -60;}
    if(950 < x & x < 1050 & -800 < z & z < -620){
      res = -30;}
}
}
void getHumidity(float* h){
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    h[0]= -1;
  }else{
    h[0] = humidity;
  }
}

void getPluv(double* data){
  pluctrl = !pluctrl;
  int datanumber = pluctrl?plu2:plu1;
  if(pluctrl){
    plu2=0;
  }else{
    plu1=0;
  }
  data[0]=datanumber*FACTOR; //operaciones pertinentes para pasarlo a ml
}
void getAnemometer(double* data){
  anemometerctrl = !anemometerctrl;
  int datanumber = anemometerctrl?ane2:ane1;
  if(anemometerctrl){
    ane2=0;
  }else{
    ane1=0;
  }
  data[0]=datanumber;//*FACTOR_R_ANEM/FACTOR_T_ANEM; //Por ahora, vueltas por periodo de timer
}
void sendData(){
  Serial.print(cnt++);
  Serial.print("\n\tT1= ");
  if(bmpResult[0]==0){
    Serial.print("error");
  }else{
    Serial.print(bmpResult[0]);
  }
  Serial.print("C");
  Serial.print("\tP= ");
  if(bmpResult[1]==0){
    Serial.print("error");
  }else{
    Serial.print(bmpResult[1]);
  }
  Serial.print("mb");
//  if(magnetometerResult[0]==-1 && magnetometerResult[1]==-1 && magnetometerResult[2]==-1){
//    Serial.println("ERROR");
//  }else{
//    Serial.print("\tx= ");
//    Serial.print(magnetometerResult[0]);
//    Serial.print("\ty= ");
//    Serial.print(magnetometerResult[1]);
//    Serial.print("\tz= ");
//    Serial.print(magnetometerResult[2]);  
//  }
  Serial.print("\tDireccion del viento= ");
  Serial.print(res);
  Serial.print("\tUV= ");
  Serial.print(uvResult);
  Serial.print("mW/cm2");
  Serial.print("\tH= ");
  if(humidity[0] ==-1){
    Serial.print("error");
  }else{
    Serial.print(humidity[0]);
    Serial.print("%");
  }
  Serial.print("\tPre= ");
  Serial.print(precip[0]);
  Serial.print(" l/m2");
  Serial.print("\tAnemometro= ");
  Serial.print(anemometer[0]);
  Serial.print(" vuelta/s");
  Serial.println();
}

void setup() {
  //Serial.begin(115200);
  setupTransmit();
  setupBmp();
  setUV();
  setupMagnetometer();
  setupPluv();
  setupAnemometer();
  
}

void loop() {
  // put your main code here, to run repeatedly:

  while(1){
    for(int i=0; i<7; i++){
      switch(i){
        case 0:
        getBMP(bmpResult);
        array[i] =  bmpResult[0]*100;
        break;

        case 1:
        array[i] =  bmpResult[1]*100;
        break;

        case 2:
        getHumidity(humidity);
        array[i] =  humidity[0]*100;
        break;

        case 3:
        uvResult = getUV();
        array[i] = (long) (uvResult*100);
        break;

        case 4:
        getMagnetometer(&res);
        array[i] = res*100;
        break;

        case 5:
        getPluv(precip);
        array[i] =  precip[0]*100;
        break;

        case 6:
        getAnemometer(anemometer);
        array[i] =  anemometer[0]*100;
        break;
        
        }
      last = i;  
      transmit(&array[i]);
      delay(1000);
      
      if(success==0){
        i=last-1;}
      }
    //uvResult = getUV();
    //getMagnetometer(magnetometerResult);
    //getBMP(bmpResult);
    //getHumidity(humidity);
    //getPluv(precip);

    sendData();
    delay(60000);
  }

}
