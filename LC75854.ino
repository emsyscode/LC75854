/**********************************************************************/
/*This code is not clean and OFFCOURSE will be possible do it better! */ 
/*this is only a group of functions to be used as independent code,   */ 
/*and by this way will be possible to any person can do the changes   */ 
/*of code and see what happen.                                        */
/*The code don't depend of any external library or extenal functions  */
/*complicated.                                                        */
/*I'm let the values to sent as binnary, this allow swap bit by bit   */
/* to is possible test segment by segment without convert to HEX      */
/**********************************************************************/
/* This code show how is possible work with the driver LC75854 Sony */
/*Begining of Auto generated code by Atmel studio */
#define VFD_in 8  // This is the pin number 8 on Arduino UNO  // You can use a resistor of 1Kohms to protect this line!
#define VFD_clk 9 // This is the pin number 9 on Arduino UNO  // You can use a resistor of 1Kohms to protect this line!
#define VFD_ce 10 // This is the pin number 10 on Arduino UNO // You can use a resistor of 1Kohms to protect this line!

unsigned int shiftBit=0;
unsigned int nBitOnBlock=0; // Used to count number of bits and split to 8 bits... (number of byte)
unsigned int nByteOnBlock=0; 
unsigned int sequencyByte=0x00;
byte Aa,Ab,Ac,Ad,Ae,Af,Ag,Ah;
byte blockBit =0x00;

#define BUTTON_PIN 2 //Att check wich pins accept interrupts... Uno is 2 & 3
volatile byte buttonReleased = false;

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

bool forward = false;
bool backward = false;
bool isRequest = true;
bool allOn=false;
bool cycle=false;
/*
//ATT: On the Uno and other ATMEGA based boards, unsigned ints (unsigned integers) are the same as ints in that they store a 2 byte value.
//Long variables are extended size variables for number storage, and store 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647.
*/
void setup() {
  pinMode(VFD_clk, OUTPUT);
  pinMode(VFD_in, OUTPUT);
  pinMode(VFD_ce, OUTPUT);
 //initialize the LED pin as an output:
  pinMode(13, OUTPUT);
  //pinMode(ledPin, OUTPUT);
  //initialize the pushbutton pin as an input:
  //pinMode(buttonPin, INPUT);  //Next line is the attach of interruption to pin 2
  pinMode(BUTTON_PIN, INPUT);
  //
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),
                    buttonReleasedInterrupt,
                    FALLING);
  //
  Serial.begin(115200); // only to debug  
  Serial.println("You reach a Reset Hardware!");               
}
void send_char(unsigned char a){
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
  data=a;
  digitalWrite(VFD_ce, LOW); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(5);
  digitalWrite(VFD_clk,LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
  delayMicroseconds(5);
  for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
  digitalWrite(VFD_clk,LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(5);
    //
    digitalWrite(VFD_ce, HIGH); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(5);
  }
}
void send_char_without(unsigned char a){
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
  data=a;
  for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
  digitalWrite(VFD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(5);
  }
}
void send_char_8bit_stb(unsigned char a){
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
 int i = -1;
  data=a;
  for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
   i++;
   digitalWrite(VFD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// 
    delayMicroseconds(1);
    if (i==7){
    //Serial.println(i);
    digitalWrite(VFD_ce, HIGH);
    delayMicroseconds(1);
    }  
  }
}
void allON(){
  // CCB address........42H
  // D1 to D220.........Display data
  // S0,S1..............Sleep control data
  // K0,K1..............Key scan output/segment output selection data
  // P0 to P1...........Segment output port/general-purpose output port selection data
  // SC.................Segment on/off control data
  // DR.................1/2 bias or 1/3 bias drive selection data
  //
  //last 16 bits: 0,0,S0,S1,K0,K1,P0,P1,SC,DR,0,0
  //The p0, p1 at 0, means all pins from s1 to s3 will belongs to segments, other's settings tell will Px is a port general purpose!
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(10);
  send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B11111111);  send_char_without(0B11111111); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B11111111);  send_char_without(0B11111111); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third  
  send_char_without(0B11111111);  send_char_without(0B00001111); // 39:32  -47:40// 0,0,S0,S1,K0,K1,P0,P1,SC,DR,0,0 Last 2 bits is "DD" data direction, and is used
  send_char_without(0B00001001); // Here we active bit S1 and P1 to define Sx and GPIO as Port 1 and port 2
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(10); 
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B11111111);  send_char_without(0B11111111); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B11111111);  send_char_without(0B11111111); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B11111111);  send_char_without(0B00000000); // 39:32  -47:40// 0,0,S0,S1,K0,K1,P0,P1,SC,DR,0,0 Last 2 bits is "DD" data direction, and is used
  send_char_without(0B01000000); // 80:62
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(10); 
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B11111111);  send_char_without(0B11111111); //  7:0   -15:8 //  First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B11111111);  send_char_without(0B11111111); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B11111111);  send_char_without(0B00000000); // 39:32  -47:40// 0,0,S0,S1,K0,K1,P0,P1,SC,DR,0,0 Last 2 bits is "DD" data direction, and is used
  send_char_without(0B10000000); // 80:62
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(10);
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B11111111);  send_char_without(0B11111111); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B11111111);  send_char_without(0B11111111); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B11111111);  send_char_without(0B00000000); // 39:32  -47:40// 0,0,S0,S1,K0,K1,P0,P1,SC,DR,0,0 Last 2 bits is "DD" data direction, and is used
  send_char_without(0B11000000); // 80:62
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(10);
}
void allOFF(){
  //send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
  //The p0, p1 at 0, means all pins from s1 to s4 will belongs to segments, other's settings tell will Px is a port general purpose!
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(1);
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000001);  send_char_without(0B00000000); //  8:1   -16:9// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 24:17  -32:25// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 40:33  -48:41// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
  send_char_without(0B00001001); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
  //
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000000);  send_char_without(0B00000000); // 60:53  -68:61// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 76:69  -84:77// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 92:85 -100:93// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
  send_char_without(0B10000000); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
  //
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000000);  send_char_without(0B00000000); // 112:105  -120:113// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 128:121  -136:129// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 144:137  -152:145// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
  send_char_without(0B01000000); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
  //
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000000);  send_char_without(0B00000000); // 164:157  -172:165// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 180:173  -188:181// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 196:189  -204:197// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display
  send_char_without(0B11000000); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
}
void testGPIOs(){
  //send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
  //The p0, p1, p2 & p3 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(1);
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  //Note: They make mention as Data1 and is direct because at fluxgram they start on D1, don't use D0, pay attention this.
  //by this reason when you need use the P0 or P1 to define pins as GPIO's is necessary take that in account!!!
  send_char_without(0B00000001);  send_char_without(0B00000000); //  8:1   -16:9// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 24:17  -32:25// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B11111111);  send_char_without(0B00000000); // 39:32  -47:40// 0,0,S0,S1,K0,K1,P0,P1,SC,DR,0,0 Last 2 bits is "DD" data direction, and is used
  send_char_without(0B00001001); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
  //
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000000);  send_char_without(0B00000000); // 60:53  -68:61// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 76:69  -84:77// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 92:85 -100:93// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
  send_char_without(0B10000000); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
  //
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000000);  send_char_without(0B00000000); // 112:105  -120:113// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 128:121  -136:129// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 144:137  -152:145// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
  send_char_without(0B01000000); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
  //
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000000);  send_char_without(0B00000000); // 164:157  -172:165// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 180:173  -188:181// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 196:189  -204:197// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display
  send_char_without(0B11000000); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
}
void msgHiFolks(){
  // CCB address........42H
  // D1 to D220.........Display data
  // S0,S1..............Sleep control data
  // K0,K1..............Key scan output/segment output selection data
  // P0 to P2...........Segment output port/general-purpose output port selection data
  // SC.................Segment on/off control data
  // DR.................1/2 bias or 1/3 bias drive selection data
  //
  //last 16 bits: 0,0,S0,S1,K0,K1,P0,P1,SC,DR,0,0
  //send total of 72 bits more first byte of address.
  //The p0, p1 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(10);
  send_char_8bit_stb(0B01000010); //(0x41) firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000001);  send_char_without(0B00000000); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00110000); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third  
  send_char_without(0B01000010);  send_char_without(0B00000110); // 39:32  -47:40// 0,0,S0,S1,K0,K1,P0,P1,SC,DR,0,0 Last 2 bits is "DD" data direction, and is used
  send_char_without(0B00001001); // 80:62
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(10); 
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000100);  send_char_without(0B00000111); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B01100001);  send_char_without(0B00000011); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000001);  send_char_without(0B00000000); // 39:32  -47:40// 0,0,S0,S1,K0,K1,P0,P1,SC,DR,0,0 Last 2 bits is "DD" data direction, and is used
  send_char_without(0B01000000); // 80:62
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(10); 
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B10000100);  send_char_without(0B00000011); //  7:0   -15:8 //  First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00100111);  send_char_without(0B00000000); // 39:32  -47:40// 0,0,S0,S1,K0,K1,P0,P1,SC,DR,0,0 Last 2 bits is "DD" data direction, and is used
  send_char_without(0B10000000); // 80:62
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(10);
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00110000);  send_char_without(0B10000010); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B01100001);  send_char_without(0B01010010); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000010);  send_char_without(0B00000000); // 39:32  -47:40// 0,0,S0,S1,K0,K1,P0,P1,SC,DR,0,0 Last 2 bits is "DD" data direction, and is used
  send_char_without(0B11000000); // 80:62
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(10);
}
void numbers_0to7(){
  //send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
  //The p0, p1, p2 & p3 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(1);
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //Burst 1
  send_char_without(0B00000001);  send_char_without(0B00000000); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B01110000); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00010000);  send_char_without(0B00000110); // 39:32  -47:40// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
  send_char_without(0B00001001); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
  //
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //Burst 2
  send_char_without(0B00000000);  send_char_without(0B01101000); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00100101);  send_char_without(0B01000101); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00100100);  send_char_without(0B00000000); // 39:32  -47:40// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
  send_char_without(0B10000000); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
  //
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //Burst 3
  send_char_without(0B01100101);  send_char_without(0B00100010); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B01100100);  send_char_without(0B00100110); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00100101);  send_char_without(0B00000000); // 39:32  -47:40// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
  send_char_without(0B01000000); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
  //
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //Burst 4
  send_char_without(0B01110000);  send_char_without(0B01010010); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B01000010);  send_char_without(0B00000000); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000110);  send_char_without(0B00000000); // 39:32  -47:40// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display
  send_char_without(0B11000000); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
}
void numbers_8to9(){
  //send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
  //The p0, p1, p2 & p3 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
  digitalWrite(VFD_ce, LOW); //
  delayMicroseconds(1);
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000001);  send_char_without(0B00000000); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B01110000); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B01010010);  send_char_without(0B00000110); // 39:32  -47:40// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
  send_char_without(0B00001001); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
  //
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00100110);  send_char_without(0B01100100); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 39:32  -47:40// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
  send_char_without(0B10000000); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
  //
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000000);  send_char_without(0B00000000); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 39:32  -47:40// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
  send_char_without(0B01000000); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
  //
  send_char_8bit_stb(0B01000010); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000000);  send_char_without(0B00000000); //  7:0   -15:8 // First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 23:16  -31:24// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 39:32  -47:40// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display
  send_char_without(0B11000000); // 53:49
  // to mark the 4 groups of 64 bits, 00, 01, 10, 11.
  delayMicroseconds(1);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(1);
}
void searchOfSegments(){
  uint8_t group = 0x00;
  byte nBit = 0x00;
  byte nMask = 0b00000001;
  unsigned int block =0;
  unsigned int nSeg=0x00;  
  Serial.println();
  Serial.println("We start the test of segments!");
  Serial.println("All segments is ON and switch one to OFF.");
  allON();
  
  for(block=0; block<4; block++){  //This is the last 2 bit's marked as DD, group: 0x00, 0x01, 0x10, 0x11;
    for( group=0; group<6; group++){   // Do until n bits 15*8 bits
        //for(int nBit=0; nBit<8; nBit++){
          for (nMask = 0b00000001; nMask>0; nMask <<= 1){
            Aa=0xFF; Ab=0xFF; Ac=0xFF; Ad=0xFF; Ae=0xFF; Af=0xFF; Ag=0xFF; Ah=0x00;
                  switch (group){
                    case 0: Aa= (Aa & ~nMask); break;
                    case 1: Ab= (Ab & ~nMask); break;
                    case 2: Ac= (Ac & ~nMask); break;
                    case 3: Ad= (Ad & ~nMask); break;
                    case 4: Ae= (Ae & ~nMask); break;
                    case 5: Af= (Af & ~nMask); break;  //LC75854 use 4 burst of 44 in the first and 40 bits on remaining burst
                    case 6: Ag= (Ag & ~nMask); break;
                    case 7: Ah=0x00; break; //This byte belongs to the control of driver status, by this reason follow with all bits at "0"
                  }
            
           nSeg++;
           if((nSeg >=0) && (nSeg<44)){
            blockBit=0;
            }
            if((nSeg >=45) && (nSeg<84)){
            blockBit=1;
            }
            if((nSeg >=85) && (nSeg<124)){
            blockBit=2;
            }
            if((nSeg >=125) && (nSeg<164)){
            blockBit=3;
            }
           
            if (nSeg >=165){
              nSeg=0;
              group=0;
              block=0;
              break;
            }
          
      //This start the control of button to allow continue teste! 
                      while(1){
                            if(!buttonReleased){
                              delay(200);
                            }
                            else{
                              delay(15);
                               buttonReleased = false;
                               break;
                               }
                         }
            delay(50);
            segments();
            Serial.print(nSeg, DEC); Serial.print(", group: "); Serial.print(group, DEC);Serial.print(", BlockBit: "); Serial.print(blockBit, HEX);Serial.print(", nMask: "); Serial.print(nMask, BIN);Serial.print("   \t");
            Serial.print(Ah, HEX);Serial.print(", ");Serial.print(Ag, HEX);Serial.print(", ");
            Serial.print(Af, HEX);Serial.print(", ");Serial.print(Ae, HEX);Serial.print(", ");Serial.print(Ad, HEX);Serial.print(", ");
            Serial.print(Ac, HEX);Serial.print(", ");Serial.print(Ab, HEX);Serial.print(", ");Serial.print(Aa, HEX); Serial.print("; ");
            
            Serial.println();
            delay (250);  
                }         
           }        
      }
}
void segments(){ 
      digitalWrite(VFD_ce, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address, every fixed as (0B01000001), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(Aa);  send_char_without(Ab);    //   7:0     -15:8 // 
          send_char_without(Ac);  send_char_without(Ad);    //  23:16    -31:24//
          send_char_without(Ae);  send_char_without(Af);    //  39:32    -47:40// 
          //send_char_without(Ag);  send_char_without(Ah);  //  55:48    -63:56//   
          //LC75854 only use burst of 48 bits
          //The next switch finalize the last burst //  
              switch (blockBit){ //Last 2 bits is "DD" data direction, and is used to mark the 4 groups of 36 bits, 00, 01, 10, 11.                                 
                case 0: send_char_without(0B00001001); break; //Block 00
                case 1: send_char_without(0B10001001); break; //Block 01
                case 2: send_char_without(0B01001001); break; //Block 10
                case 3: send_char_without(0B11001001); break; //Block 11
              }
      delayMicroseconds(1);
      digitalWrite(VFD_ce, LOW); //                   
}
void loop() {
  for(int i=0; i<4; i++){
      allON(); // All on
      delay(1500);
      numbers_0to7();
      delay(1500);
      numbers_8to9();
      delay(1500);
      allOFF();
      delay(500);
      msgHiFolks();
      delay(1500);
      allOFF();
  }
  //searchOfSegments(); //Uncomment this line if you want run the find segments                
}
void buttonReleasedInterrupt() {
  buttonReleased = true; // This is the line of interrupt button to advance one step on the search of segments!
}
