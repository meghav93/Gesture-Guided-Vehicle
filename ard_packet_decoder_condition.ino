int code[7],angle=0,dis=0,back=0,i=0,e;
void setup()
{
  Serial.begin(115200);
  
  pinMode(A0,OUTPUT);
  digitalWrite(A0,0);
  pinMode(A1,OUTPUT);
  digitalWrite(A1,0);
  pinMode(11,OUTPUT);
  digitalWrite(11,0);
  pinMode(12,OUTPUT);
  digitalWrite(12,0);
  
}
void loop()
{
  while(Serial.available()<7 && Serial.available()>0)
  {
    code[i]=Serial.read()-'0';
   // Serial.println(code[i]);
    i++; 
  }
  if(i==7)
  { 
      angle=0;
      dis=0;
      back=0;
    
      angle+=code[0]*100;
      angle+=code[1]*10;
      angle+=code[2];
      //Serial.println(angle);
     
      dis+=code[3]*100;
      dis+=code[4]*10;
      dis+=code[5];
      //Serial.println(dis);
      
      back=code[6];
      //Serial.println(b);
      if(angle>290)
      {
       digitalWrite(A1,1);
       digitalWrite(A0,0);
      }
      else if(angle<290)
      {
       digitalWrite(A1,0);
       digitalWrite(A0,1);
      }
      else
      {
       digitalWrite(A1,0);
       digitalWrite(A0,0);
      }
      dis=constrain(dis,350,500);
      analogWrite(11,map(dis-200,150,300,0,255));
      if(back==1) digitalWrite(12,1);
      else        digitalWrite(12,0);
      i=0;
      
  }
  
}
