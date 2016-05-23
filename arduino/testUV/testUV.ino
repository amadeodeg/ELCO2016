#define UVOUT  A0 //Output from the sensor
#define REF_3V3  A1 //3.3V power on the Arduino board

static int cnt=0;

void setUV(){
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

float getUV(){
  
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
  
  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputVoltage = 3.3 / refLevel * uvLevel;
  
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level
  return uvIntensity;
}
//Takes an average of readings on a given pin
//Returns the average
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void loop() {
  // put your main code here, to run repeatedly:
  float uvResult;
  long time1, time2;
  while(1){
  time1 = micros();
  uvResult = getUV();
  time2 = micros();
  // Print out the measurement:
  Serial.print(cnt++);
  Serial.print("\tIntensidad UV= ");
  Serial.print(uvResult);
  Serial.print("\t");
  Serial.println(time2-time1);
  delay(1000);
  }
}

