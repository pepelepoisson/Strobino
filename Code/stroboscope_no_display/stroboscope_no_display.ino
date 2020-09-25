// Stroboscope with no display
// the frequency can be changed with two push-buttons
// the pulselength can be changed by pressing both buttons simultaneously

// Started from https://www.instructables.com/id/Portable-Precision-Stroboscope/

//breadboard setup 
const byte button_up=12;
const byte button_dw=11;

int freq=200; //frequency in units of 0.1Hz
byte len=1;   //pulselength in units of 64 microseconds

void setup() {
  
  //setup TIMER1B for fast PWM with (output on pin D10)
  TCCR1A=B00100010;
  TCCR1B=B00011000;
  setfreq(freq,len);
  pinMode(10,OUTPUT);

  //set the pushbuttons to input with pull-up
  pinMode(button_up,INPUT_PULLUP);
  pinMode(button_dw,INPUT_PULLUP);


}

//set frequency in Hz, pulse length in units of 64mus
void setfreq(int f, int len){

  //calculate what to put into the timing registers
  long unsigned int ticks=(160000000+f/2)/f; // f_clock/0.1f
  unsigned int ps=1;
  if (ticks>    0xFFFF)ps=   8;
  if (ticks>  8*0xFFFF)ps=  64;
  if (ticks> 64*0xFFFF)ps= 256;
  if (ticks>256*0xFFFF)ps=1024;
  unsigned int val_ICR1=ticks/ps-1;
  unsigned int val_OCR1B=len*(1024/ps)-1;
  unsigned int val_TCCR1B=B00011001;
  if (ps==   8) val_TCCR1B=B00011010;
  if (ps==  64) val_TCCR1B=B00011011;
  if (ps== 256) val_TCCR1B=B00011100;
  if (ps==1024) val_TCCR1B=B00011101;

  //set the actual values in the timing registers
  //noInterrupts();
  if (TCNT1>val_ICR1)TCNT1=0;  //do not allow timer to exceed 'top'
  ICR1=val_ICR1;
  OCR1B=val_OCR1B;
  TCCR1B=val_TCCR1B;
  //interrupts();
}

byte prevbutstat=0;

unsigned long millis_butchanged=0;
unsigned long millis_valchanged=0;

void loop() {

  //check for buttons
  long unsigned millis_current=millis();
  bool change=false;
  byte butstat=(digitalRead(button_up)==LOW)*1+(digitalRead(button_dw)==LOW)*2;
  if (butstat!=prevbutstat)millis_butchanged=millis_current;

  int dt=200; int df=1;
  if (millis_current-millis_butchanged>1000)dt=100;
  if (millis_current-millis_butchanged>2000)dt=50;
  if (millis_current-millis_butchanged>3000)dt=20;
  if (millis_current-millis_butchanged>4000)dt=10;
  if (millis_current-millis_butchanged>5000)dt=5;
  if (millis_current-millis_butchanged>6000)dt=2;
  if (millis_current-millis_butchanged>7000)dt=1;
  if (millis_current-millis_butchanged>8000)df=2;
  if (millis_current-millis_butchanged>9000)df=5;
  if (millis_current-millis_butchanged>10000)df=10;
  if (butstat==3)dt=1000;
  if(millis_current-millis_valchanged>dt){
    if(butstat==1) freq=freq+df;
    if(butstat==2) freq=freq-df;
    if(butstat==3) len++;
    if(butstat>0){
        change=true; millis_valchanged=millis_current;
    }
  }
  prevbutstat=butstat;

  //keep frequency and pulselength wihin allowed domain
  freq=max(freq,3);    
  freq=min(freq,9999);
  if (len>15)len=1;
  if (len*freq>15625)len=1; //corresponds to 0.1 duty cycle
  
  if (change) setfreq(freq,len);


    
}
