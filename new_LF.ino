/*l298 wiring
 header no 1 -> in3
 header no 2 -> in4
 header no 3 -> enb
 header no 4 -> ena
 header no 5 -> in2
 header no 6 -> in1
*/

#include<LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(A1,A0,2,3,4,5);

int sensor[6];
int ref[6]={400,400,450,450,400,400};
//int ref[6];
int button1=7;
int button2=6;
int i,xsensor,var,start1,start2;
int s0,s1,s2,s3,s4,s5,flag;
unsigned char pwm_ka,pwm_ki;
int j;
int in1=13; 
int in2=12; 
int ena=11; 
int in3=8;
int in4=9;
int enb=10;
int laju,kecepatan;
int data_putih[6],data_gelap[6],data_sensor_depan[6];
int address,address_speed=10;

void maju(unsigned char kiri, unsigned char kanan){
  pwm_ka=kanan;   pwm_ki=kiri;
  analogWrite(ena,pwm_ka); analogWrite(enb,pwm_ki+10);
  digitalWrite(in2,HIGH);   digitalWrite(in4,LOW);
  digitalWrite(in1,LOW);  digitalWrite(in3,HIGH);
}
void mundur(unsigned char kiri, unsigned char kanan){
  pwm_ka=kanan;   pwm_ki=kiri;
  analogWrite(ena,pwm_ka); analogWrite(enb,pwm_ki);
  digitalWrite(in2,LOW);   digitalWrite(in4,HIGH);
  digitalWrite(in1,HIGH);  digitalWrite(in3,LOW);
}
void bel_ka(unsigned char kiri,unsigned char kanan){      
  pwm_ki=kiri;    pwm_ka=kanan;    
  analogWrite(ena,pwm_ka); analogWrite(enb,pwm_ki+10);
  digitalWrite(in2,LOW);   digitalWrite(in4,LOW);
  digitalWrite(in1,HIGH);  digitalWrite(in3,HIGH);   
}
void bel_ki(unsigned char kiri,unsigned char kanan){      
  pwm_ki=kiri;    pwm_ka=kanan;     
  analogWrite(ena,pwm_ka); analogWrite(enb,pwm_ki+10);
  digitalWrite(in2,HIGH);   digitalWrite(in4,HIGH);
  digitalWrite(in1,LOW);  digitalWrite(in3,LOW);   
}
void stopped(){
  analogWrite(ena,LOW); analogWrite(enb,LOW);
  digitalWrite(in2,HIGH);   digitalWrite(in4,HIGH);
  digitalWrite(in1,HIGH);  digitalWrite(in3,HIGH);
}
void baca_button(){
  start1=digitalRead(button1);
  start2=digitalRead(button2);
}
void baca_sensor(){
  sensor[0]=analogRead(A7); 
  sensor[1]=analogRead(A6);
  sensor[2]=analogRead(A5);
  sensor[3]=analogRead(A4);
  sensor[4]=analogRead(A3);
  sensor[5]=analogRead(A2);   
}
void convert_sensor(){
  baca_sensor();
  /*for(i=0;i<=5;i++){
    ref[i]=EEPROM.read(i);
  }*/
  xsensor=63;
  xsensor&=63;
  if(sensor[0]<ref[0]){s0=0; var=1;}   else{s0=1; var=0; flag=1;} xsensor&=~var; 
  if(sensor[1]<ref[1]){s1=0; var=2;}   else{s1=1; var=0;} xsensor&=~var; 
  if(sensor[2]<ref[2]){s2=0; var=4;}   else{s2=1; var=0;} xsensor&=~var;
  if(sensor[3]<ref[3]){s3=0; var=8;}   else{s3=1; var=0;} xsensor&=~var;
  if(sensor[4]<ref[4]){s4=0; var=16;}  else{s4=1; var=0;} xsensor&=~var;
  if(sensor[5]<ref[5]){s5=0; var=32;}  else{s5=1; var=0; flag=0;} xsensor&=~var;
}
void tampil_sensor(){
  convert_sensor();
  lcd.setCursor(0,0);
  lcd.print(s0);
  lcd.print(s1);
  lcd.print(s2);
  lcd.print(s3);
  lcd.print(s4);
  lcd.print(s5);
  Serial.print(s0);
  Serial.print(s1);
  Serial.print(s2);
  Serial.print(s3);
  Serial.print(s4);
  Serial.println(s5);
}
void tes_sensor(){ 
  baca_sensor();
  lcd.setCursor(0,0);
  lcd.print(sensor[0]);
  lcd.print("  ");
  lcd.setCursor(5,0);
  lcd.print(sensor[1]);
  lcd.print("  ");
  lcd.setCursor(10,0);
  lcd.print(sensor[2]);
  lcd.print("  ");
  lcd.setCursor(0,1);
  lcd.print(sensor[3]);
  lcd.print("  ");
  lcd.setCursor(5,1);
  lcd.print(sensor[4]);
  lcd.print("  ");
  lcd.setCursor(10,1);
  lcd.print(sensor[5]);
  lcd.print("  ");
  delay(100); 
}
void kalibrasi(){          
    lcd.clear();        lcd.setCursor(5,1);        lcd.print("PUTIH");
    delay(80);
    baca_button();    
    while(start1){baca_button();}   
    while(!start1){baca_button(); lcd.clear();}     
    baca_sensor();   
    data_putih[0]=sensor[0];
    data_putih[1]=sensor[1];
    data_putih[2]=sensor[2];
    data_putih[3]=sensor[3];
    data_putih[4]=sensor[4];
    data_putih[5]=sensor[5];
          
    lcd.clear();        lcd.setCursor(5,1);        lcd.print("HITAM");
    delay(80);
    
    baca_button();                                       
    while(start1){baca_button();}           
    while(!start1){baca_button(); lcd.clear();}              
    baca_sensor();   
    data_gelap[0]=sensor[0];
    data_gelap[1]=sensor[1];
    data_gelap[2]=sensor[2];
    data_gelap[3]=sensor[3];
    data_gelap[4]=sensor[4];
    data_gelap[5]=sensor[5];
    
    delay(5);                    
    for(i=0;i<=5;i++){
        ref[i]=((data_gelap[i]-data_putih[i])/2)+data_putih[i];
        EEPROM.write(address,ref[i]);
        address++;
        if(address == 6 ){
          address=0;
        }  
    }  
      
    lcd.clear();        lcd.setCursor(6,1);        lcd.print("DONE");
    delay(1000);
    lcd.clear();                 
}
void pid(unsigned char simpan){
    unsigned int kec_maks=160;
    unsigned int kec_min=90;
    float kp=0.25;    
    float kd=1.6;   
    static unsigned char ki=2.2;                                                           
    static int Error,lastError,error,right_speed,left_speed;
                 
    convert_sensor();                 
    switch(xsensor){
      case 7: error=-7;  break;
      case 1: error=-6;  break;
      case 3: error=-5;  break;
      case 2: error=-4;  break;
      case 6: error=-3;  break;
      case 4: error=-2;  break;
      case 28:  error=1;  break;
      case 12: error=0;  break;
      case 14:  error=1;  break;
      case 8:  error=2;  break;
      case 24: error=3;  break;   
      case 16: error=4;  break;
      case 48: error=5;  break;
      case 32: error=6;  break;      
      case 56: error=7;  break;
      case 0:  
        if(flag==0) {error=8;}
          else        {error=-8;} 
          break;
    } 
  int SetPoint = 0;                      // Setpoint yang diinginkan
  Error = SetPoint - error; 
  int outPID = kp * Error + kd * (Error - lastError) + ki*0.1;
  lastError = Error;
  
  double motorKi = simpan - outPID;     // Motor Kiri
  double motorKa = simpan + outPID;     // Motor Kanan
    
  lcd.setCursor(0,1);
  lcd.print(motorKi);
  lcd.print(", ");  
  lcd.print(motorKa);
  lcd.print(" "); 
  /*** Pembatasan kecepatan ***/
  if (motorKi > kec_maks)motorKi = kec_maks;
  if (motorKi < kec_min)motorKi = kec_min;
  if (motorKa > kec_maks)motorKa = kec_maks;
  if (motorKa < kec_min)motorKa = kec_min;
  //move(1, rightMotorSpeed, 1);//motor derecho hacia adelante
  //move(0, leftMotorSpeed, 1); 
  if(motorKi==motorKa){  
    maju(simpan,simpan);
  }
  else if(motorKi>motorKa){
    bel_ka(motorKi,motorKa);
  }
  else if(motorKa>motorKi){
    bel_ki(motorKi,motorKa);
  }    
}











void siku_kanan(){
  while(1){
  pid(150);
  if(s3==1 && s4==1 && s5==1)
    break;
  }
  maju(120,120);
  delay(100);
  while(1){
    convert_sensor();
    bel_ka(110,110);
    if(s5==1)break;
  }
  stopped();
  delay(100);
}
void siku_kiri(){
  while(1){
    pid(150);
    if(s0 && s1)break;
  }
  maju(120,120);
  delay(100);
  while(1){
    convert_sensor();
    bel_ki(130,130);
    if(s0)break;
  }
  stopped();
  delay(100);
}
void berhenti_hitam(){
  while(1){
    pid(150);
    if(s0 && s5)break;
  }  
  while(1){
    stopped();
  }
}
void putus_putus(){
  while(1){
    pid(150);
    if(!s0 && !s1 && !s2 && !s3 && !s4 && !s5)break;
  }  
  while(1){
    maju(130,130);
    convert_sensor();
    if(s1 || s2 || s3 || s4)break;
  }
}
void perempatan_lurus(){
  while(1){
  pid(130);
  if((s0==1 && s1==1 && s2==1 && s3==1)||
    (s2==1 && s3==1 && s4==1 && s5==1))
    break;
  }
  maju(130,130);
  delay(70);
}
void berhenti(){
  while(1){
    stopped();
  }
}
void maju_terus(){
  while(1){
  pid(150);
  if(s2== 1 && s3== 1)
  break;
  }  
  maju(130,130);
  delay(70);
}
void maju_start(){
  while(1){
    maju(100,100);
    convert_sensor();
    if(!s0 || !s5)break;
  }
}


void setup() {
  lcd.begin(16,2);
  lcd.setCursor(0,1);
  lcd.print("  IBI-Kosgoro");
  Serial.begin(9600);
  pinMode (in1, OUTPUT);
  pinMode (in2, OUTPUT);
  pinMode (ena, OUTPUT);
  pinMode (in3, OUTPUT);
  pinMode (in4, OUTPUT);
  pinMode (enb, OUTPUT);
  pinMode (button1, INPUT_PULLUP); 
  pinMode (button2, INPUT_PULLUP);
  
}

void loop(){
  /*maju(100,100);
  delay(1000);
  mundur(250,250);
  delay(2000);*/
  baca_button();
  while(start1 && start2){baca_button();}//tampil_sensor();}//} 
  if(!start1){
    lcd.clear();
    while(!start1){baca_button();}//tampil_sensor();}
    while(1){
      tes_sensor();
    }
  }
  else if(!start2){
    lcd.clear();
    while(!start2){baca_button();tampil_sensor();}
    while(1){
      ///TULIS PROGRAM DISINI
      //pid(150);
      // function maju(); mundur(); bel_ka(); bel_ki(); stopped(); siku_kanan(); siku_kiri(); berhenti_hitam(); putus_putus(); perempatan_lurus();
    
      //BERAKHIR DISINI
    }
  }
}
