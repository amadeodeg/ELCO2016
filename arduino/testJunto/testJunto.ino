#include <DHT.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include <stdlib.h>

//A4=SDA A5=SCL

#define BMP_MODE_PRESSURE 3
#define UVOUT  A0 //Output from the sensor
#define REF_3V3  A1 //3.3V power on the Arduino board
#define ADDRESSHMC 0x1E //0011110b, I2C 7bit address of HMC5883
#define DHTPIN 5
#define DHTTYPE DHT11

#define PLUVPIN 2
#define FACTOR 0.5474

SFE_BMP180 bmp180;
DHT dht(DHTPIN, DHTTYPE);

int* magnetometerResult=(int*)malloc(3*sizeof(int));
double* bmpResult=(double*)malloc(2*sizeof(double));
float uvResult;
float* humidity=(float*)malloc(sizeof(float));
double* precip=(double*)malloc(sizeof(double));

int cnt = 0;

bool pluctrl = true;
int plu1=0;
int plu2=0;

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
  if(pluctrl){
    plu1++;
  }else{
    plu2++;
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
  result[0]=x;
  result[1]=y;
  result[2]=z; 
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

void sendData(){
  Serial.print(cnt++);
  Serial.print("\tT1= ");
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
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setupBmp();
  setUV();
  //setupMagnetometer();
  setupPluv();
}

void loop() {
  // put your main code here, to run repeatedly:

  while(1){
    uvResult = getUV();
    //getMagnetometer(magnetometerResult);
    getBMP(bmpResult);
    getHumidity(humidity);
    getPluv(precip);

    sendData();
    delay(1000);
  }

}
