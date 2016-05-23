//R=5cm => A=pi·25·10^-4 [m^2]    4.3ml cada salto
//litros/m^2 = N·(4.3·10^-3)/pi·25·10^-4= N·0.5474;

#define PLUVPIN 2
#define FACTOR 0.5474

bool ctrl = true;
int cnt1=0;
int cnt2=0;

void setupPluv(){
  attachInterrupt(digitalPinToInterrupt(PLUVPIN),pluvHandler,RISING);
}

void pluvHandler(){
  if(ctrl){
    cnt1++;
  }else{
    cnt2++;
  }
  //Serial.print("interrupcion");
}

void getPluv(double* data){
  ctrl = !ctrl;
  int datanumber = ctrl?cnt2:cnt1;
  if(ctrl){
    cnt2=0;
  }else{
    cnt1=0;
  }
  data[0]=datanumber*FACTOR; //operaciones pertinentes para pasarlo a ml
}

void setup() {
  Serial.begin(9600);
  setupPluv();
}

void loop() {
  double data=0;
  int sec;
  while(1){
    getPluv(&data);

    Serial.print(sec++);
    Serial.print("\t");
    Serial.println(data);
    delay(3000);
  }

}

