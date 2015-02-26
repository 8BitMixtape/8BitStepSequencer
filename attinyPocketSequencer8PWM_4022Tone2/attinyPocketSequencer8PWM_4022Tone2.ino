/*
*This code is for an attiny85 powered pocket sequencer and is part of an
*instructable at http://www.instructables.com/id/Attiny-Pocket-Sequencer/ . 
*
*The following code was written by: Adam Berger
*
*You are free to use my code in anyway you'd like, as long as
*you give credit where it is due. Thank you for your interest!
*/
const byte pot = 3, tonePin = 0, reset = 1, clock = 2, speaker = 4;
const int MAX_NOTE_LENGTH = 8000, MAX_FREQ = 255, NUMBER_OF_STEPS = 8, POT_THRESHOLD = 20;
int stepFreqs[] = {255,128,230,30,100,240,50,200};
int stepSustains[] = {820,882,842,860,820,862,840,700};
int tempo = 1000;

unsigned long previousMillis, functionMillis;
void setup(){
  
  //enable inpustepFreqsts
  pinMode(pot, INPUT);
  digitalWrite(pot, HIGH);
  //enable outputs
  pinMode(reset, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(tonePin, OUTPUT);
  
  digitalWrite(reset, LOW);
  delay(10);
  digitalWrite(reset, HIGH);
  delay(300);
  digitalWrite(reset, LOW);
  delay(10);
  //setFrequencies();
  //functionMillis=millis();
  setFrequencies();
  TCCR1 = TCCR1 & 0b11111001; //timer pre-scaler divided by 8, slower speed of the processor
  //setFrequencies();



}
void loop(){
  //for each step
  //TCCR1 = TCCR1 & 0b11111001; //timer pre-scaler divided by 8, slower speed of the processor
  
  for(byte a=0; a<NUMBER_OF_STEPS; a++){
    //turn LED on
    digitalWrite(clock, LOW);
    
    //tempo logic/update tempo variable
    previousMillis=millis();

    analogWrite(speaker, stepFreqs[a]);
    tone(tonePin, stepFreqs[a], stepSustains[a]);
    
    
    while(millis()-previousMillis<tempo){      
      tempo = 4 * map(analogRead(pot),POT_THRESHOLD ,1023,3, MAX_NOTE_LENGTH);
      
      //if(analogRead(pot)<20&&(millis()-functionMillis>5000))
        //setSustain();
    }  
  noTone(tonePin);
  digitalWrite(clock, HIGH);
  }
  digitalWrite(reset, HIGH);
  delay(1);
  
  digitalWrite(reset, LOW);
  delay(1);
}

void setFrequencies(){
  
  for(byte a=0; a<NUMBER_OF_STEPS; a++){
    //turn LED on
    digitalWrite(clock, LOW);
    delay(400);
    //tempo logic/update tempo variable
    while((analogRead(pot)>POT_THRESHOLD)){
     previousMillis=millis();
     int freq = map(analogRead(pot),POT_THRESHOLD,1023,0, MAX_FREQ);
     if(analogRead(pot)<POT_THRESHOLD+10) freq=0;
     stepFreqs[a] = freq;
     analogWrite(speaker, stepFreqs[a]);
     tone(tonePin, stepFreqs[a], stepSustains[a]);
     functionMillis=millis();
     while(millis()-functionMillis<200){
      if(analogRead(pot)<POT_THRESHOLD) break;
    }
    }
  digitalWrite(clock, HIGH);
  }
  functionMillis=millis();
  digitalWrite(reset, HIGH);
  delay(1);
  
  digitalWrite(reset, LOW);
  delay(1);
}

void setSustain(){
  for(byte a=0; a<NUMBER_OF_STEPS; a++){
    //turn LED on
    digitalWrite(clock, LOW);
    delay(400);
    //tempo logic/update tempo variable
    while((analogRead(pot)>POT_THRESHOLD)){
     stepSustains[a] = map(analogRead(pot),POT_THRESHOLD,1023,0, MAX_NOTE_LENGTH/100);
     tone(tonePin, stepFreqs[a], stepSustains[a]);
     functionMillis=millis();
      while(millis()-functionMillis<(stepSustains[a]+(stepSustains[a]/2))){
        //if(analogRead(pot)<20&&(millis()-functionMillis>50)) break;
        if(analogRead(pot)<POT_THRESHOLD) break;
    } 
    digitalWrite(clock, HIGH);
    }
  }
  functionMillis=millis();
  
}
