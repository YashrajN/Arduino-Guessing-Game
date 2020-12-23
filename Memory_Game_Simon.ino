/*
Written by: Yashraj, Ryan, and Paul
Written for:  Mr. Kolch
Subject:  TEJ 3M1
Date: May 7th 2019
Description: Memory Game
*/


#define PLAYER_WAIT_TIME 3000 

byte sequence[100];     // Storage for the light sequence
byte curLen = 0;        // Length of sequence
byte inputCount = 0;    // Number of times player has pressed a correct button in a given turn 
byte lastInput = 0;     // Last input from the player
byte expRd = 0;         // Expected LED
bool btnDwn = false;    // Check if a button is pressed
bool wait = false;      // Is the program waiting for the user to press a button
bool resetFlag = false; // Indicates to the program that once the player lost
int waitTimeO = 1000;
int waitTimeI = 500;

byte soundPin = 5;         

byte noPins = 4;              
byte pins[] = {2, 13, 10, 8}; // Button input pins and LED ouput pins 
                           
                              
long inputTime = 0;           // Timer variable for the delay between user inputs

void setup() {
  Reset();
}

void setPinDirection(byte dir){   // Sets all the pins to INPUT or OUTPUT based on 'dir'
  for(byte i = 0; i < noPins; i++){
    pinMode(pins[i], dir); 
  }
}

void writeAllPins(byte val){    //send the same value to all the LED pins
  for(byte i = 0; i < noPins; i++){
    digitalWrite(pins[i], val); 
  }
}

void beep(byte freq){
  analogWrite(soundPin, 2);
  delay(freq);
  analogWrite(soundPin, 0);
  delay(freq);
}


void flash(short freq){  // Flashes all the LEDs together
  setPinDirection(OUTPUT); // Activates the LEDS 
  for(int i = 0; i < 3; i++){
    writeAllPins(HIGH);
    beep(50);
    delay(freq);
    writeAllPins(LOW);
    delay(freq);
  }
}

void Reset(){ // Resets all the game variables
  flash(500);
  curLen = 0;
  inputCount = 0;
  lastInput = 0;
  expRd = 0;
  btnDwn = false;
  wait = false;
  resetFlag = false;
}

void Lose(){
  flash(50);
}

void playSequence(){ // The arduino shows the user what must be memorized
  for(int i = 0; i < curLen; i++){   //Loop through the stored sequence and light the appropriate LEDs in turn
      digitalWrite(sequence[i], HIGH);
      delay(waitTimeO);
      waitTimeO -= 20;
      if (waitTimeO <= 200){
        waitTimeO = 300;
        }
       if (waitTimeI <= 100){
        waitTimeI = 150;
        }
      waitTimeI -= 10;
      digitalWrite(sequence[i], LOW);
      delay(waitTimeI);
    } 
}

void DoLoseProcess(){
  Lose();            
  delay(1000);
  Reset();         
}


void loop() {  
  if(!wait){    // Arduino's turn 

    setPinDirection(OUTPUT);   // Sets LEDs to OUTPUT
    
    randomSeed(analogRead(A0));                
    sequence[curLen] = pins[random(0,noPins)];   // Put a random value in the next position in the sequence 
    curLen++;    // Set the new Current length of the sequence
    
    playSequence();  // Show the sequence to the player
    beep(50); // Make a beep for the player to be aware
    
    wait = true;  // Set Wait to true as it's now going to be the turn of the player
    inputTime = millis(); // Store the time to measure the player's response time
  }else{   // Player's turn //

    setPinDirection(INPUT);   // Sets buttons to INPUT

    if(millis() - inputTime > PLAYER_WAIT_TIME){  // If the player takes more than the allowed time,
      DoLoseProcess();   
      return;
    }      
        
    if(!btnDwn){                                   
      expRd = sequence[inputCount];    // Find the expected value from the player
      
      for(int i = 0; i < noPins; i++){  // Loop through the all the pins
        if(pins[i]==expRd)                        
          continue;  
        if(digitalRead(pins[i]) == HIGH){  // Is the button pressed
          lastInput = pins[i];
          resetFlag = true;  // This means player lost
          btnDwn = true;   // Prevents  program from doing the same thing over and over again
        }
      }      
    }

    if(digitalRead(expRd) == 1 && !btnDwn)  // Player pressed the right button
    {
      inputTime = millis();                        
      lastInput = expRd;
      inputCount++;  // Player pressed a right button again
      btnDwn = true;   // Prevents  program from doing the same thing over and over again
    }else{
      if(btnDwn && digitalRead(lastInput) == LOW){  // Check if the player released the button
        btnDwn = false;
        delay(20);
        if(resetFlag){ 
          DoLoseProcess(); 
        }
        else{
          if(inputCount == curLen){  // Has the player finished repeating the sequence
            wait = false;   // If so, this will make the next turn the program's turn
            inputCount = 0;  // Reset the number of times that the player has pressed a button
            delay(1500);
          }
        }
      }
    }    
  }
}
