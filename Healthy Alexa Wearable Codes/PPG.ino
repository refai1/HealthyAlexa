/* Author Hannaneh Hojaiji
 *  NIRS I2BL CORELAB
 *  This is the V1 of Multimode code to controle PPG measurement
 */

int red4 = 7;//red 4
int red3 = 4;//red 3
int red2 = 3;//red 2
int red1 = 9;//red 1

int ir4 = 6;//IR 4
int ir3 = 5;//IR 3
int ir2 = 2;//IR 2
int ir1 = 8;//IR 1

int pd1 = A0;//Reads A0 un-inverted PD 1 (mid), inv 1, 2 filt
int pd2 = A5;//Reads A5 un-inverted PD 2 (left), inv 3
int pd3 = A6;//Reads A6 un-inverted PD 3 (right), inv 4
int photodiode =0;
int photodiode2 =0;
int photodiode3 =0;
int i = 0;

void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL);

  // Setup the 6 LEDs as Output
  pinMode(red1, OUTPUT);
  pinMode(red2, OUTPUT);
  pinMode(red3, OUTPUT); 
  pinMode(red4, OUTPUT);

  pinMode(ir1, OUTPUT);
  pinMode(ir2, OUTPUT);
  pinMode(ir3, OUTPUT); 
  pinMode(ir4, OUTPUT);

}

void loop() {

//Red 1  
  digitalWrite(red1, HIGH);
  delayMicroseconds(625);
  photodiode = 0;
  photodiode2 = 0;
  photodiode3 = 0;

  for (i=0;i<10;i++){
      photodiode = analogRead(pd1);
      photodiode2 = photodiode2 + analogRead(pd2);
      photodiode3 = analogRead(pd3);
      delayMicroseconds(5);
  }
      
  //Serial.print("PD1 RED1:, ");
  //Serial.print(photodiode);
  //delayMicroseconds(5);

 // Serial.print(",PD2 RED1:, ");
  Serial.print(photodiode2/10);
  //delayMicroseconds(5);

  //Serial.print(",PD3 RED1:, ");
  Serial.println(",");
  //Serial.print(photodiode3);
  //delayMicroseconds(5);
  
  digitalWrite(red1, LOW);

//IR 1  
  digitalWrite(ir1, HIGH);
  delayMicroseconds(625);
  photodiode = 0;
  photodiode2 = 0;
  photodiode3 = 0;
  
  for (i=0;i<10;i++){
      photodiode = analogRead(pd1);
      photodiode2 = photodiode2 + analogRead(pd2);
      photodiode3 = analogRead(pd3);
      delayMicroseconds(5);
  }
  
  //delayMicroseconds(625);
    
  //Serial.print(",PD1 IR1:, ");
  //Serial.print(photodiode);
  //delayMicroseconds(5);

  //Serial.print(",PD2 IR1:, ");
   Serial.println(photodiode2/10);
  //delayMicroseconds(5);

  //Serial.print(",PD3 IR1:, ");
  //Serial.println(photodiode3);
  digitalWrite(ir1, LOW);
}
