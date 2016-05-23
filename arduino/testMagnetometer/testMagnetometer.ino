
//Tiempo ejecucion 960 uS
#include <Wire.h>

#define ADDRESSHMC 0x1E //0011110b, I2C 7bit address of HMC5883

static int cnt=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setupMagnetometer();
}


void setupMagnetometer(){
  Wire.begin(); //creo que esta línea es innecesaria
  Wire.beginTransmission(ADDRESSHMC); //open communication with HMC5883
  Wire.write(0x02); //select mode register : Pag 11 datasheet
  Wire.write(0x00); //continue measurement mode : Pag 14 datasheet
  Wire.endTransmission();
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


void loop() {
  // put your main code here, to run repeatedly:
  int* magnetometerResult=(int*)malloc(3*sizeof(int));
  long time1, time2;
  while(1){
  time1 = micros();
  getMagnetometer(magnetometerResult);
  time2 = micros();
  // Print out the measurement:
  Serial.print(cnt++);
  if(magnetometerResult[0]==-1 && magnetometerResult[1]==-1 && magnetometerResult[2]==-1){
    Serial.println("ERROR");
  }else{
    Serial.print("\tx= ");
    Serial.print(magnetometerResult[0]);
    Serial.print("\ty= ");
    Serial.print(magnetometerResult[1]);
    Serial.print("\tz= ");
    Serial.print(magnetometerResult[2]);  
  }
  Serial.print("\t");
  Serial.println(time2-time1);
  delay(1000);
  }
}
