#include <DHT.h>
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setupHumidity(){
  dht.begin();
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setupHumidity();
}

void getHumidity(float* h){
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    h[0]= -1;
  }else{
    h[0] = humidity;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  float* humidity=(float*)malloc(sizeof(float));
  //float humidity;
  long time1, time2;
  while(1){
    time1 = micros();
    getHumidity(humidity);
    time2= micros();
    if(humidity[0] ==-1){
      Serial.print("error");
    }else{
    Serial.print("humedad: ");
    Serial.print(humidity[0]);
    Serial.print("\t tiempo: ");
    Serial.print(time2-time1);
    Serial.println();
    delay(2000);
    }
  }
  
}

