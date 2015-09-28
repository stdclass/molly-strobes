
const int buttonFlashPin = 2;
int buttonFlashState = 0;
int buttonFlashLastState = 0;

const int buttonTempoPin = 3;
int buttonTempoState = 0;
int buttonTempoLastState = 0;

const int buttonModePin = 4;
int buttonModeState = 0;
int buttonModeLastState = 0;


const int relayPin = 5;

const int ledPin = 13;

unsigned long tapTimeFirst;
unsigned long tapTimePrevious;
float tapCount = 0;
float tapBPM = 120;

unsigned long lastStrobeTime;
int strobeDuration = 1000;
int strobeState = LOW;
int lastStrobeState = LOW;


int currentMode = 0;
// 0 = Off
// 1 = On
// 2 = Strobe
// 3 = Strobe short

void setup(){
  pinMode(buttonFlashPin, INPUT);
  pinMode(buttonTempoPin, INPUT);
  pinMode(buttonModePin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(9600);
}

void loop(){
  
  buttonFlashState = digitalRead(buttonFlashPin);
  buttonTempoState = digitalRead(buttonTempoPin);
  buttonModeState = digitalRead(buttonModePin);
  
  if( buttonFlashState != buttonFlashLastState && currentMode == 0 ){
    
    if( buttonFlashState == HIGH ){
      switchRelay(HIGH);
      delay(100);
      switchRelay(LOW);
    }
    
    buttonFlashLastState = buttonFlashState;
  }
  
  
  if( buttonModeState != buttonModeLastState ){
    
    if( buttonModeState == HIGH ){
      
      currentMode++;
      
      if( currentMode > 3 ){
        currentMode = 0;
      }
      
      switchedMode();
    }
    
    buttonModeLastState = buttonModeState;
  }
  
  
  
  if( buttonTempoState != buttonTempoLastState ){
    
    if( buttonTempoState == HIGH ){
      tappedTempo();
    }
    
    buttonTempoLastState = buttonTempoState;
  }
  
  if( currentMode == 2 || currentMode == 3 ){
    strobe();
  }
  
}

void switchedMode(){
  if( currentMode == 0 ){
    switchRelay(LOW);
  }
  
  if( currentMode == 1 ){
    switchRelay(HIGH);
  }
}

void strobe(){
  unsigned long now = millis();
  
  if( now - lastStrobeTime >= strobeDuration && strobeState == LOW ){
    lastStrobeTime = now;
    strobeState = HIGH;
    doStrobe();
  }else if( now - lastStrobeTime >= strobeDuration && strobeState == HIGH ){
    lastStrobeTime = now;
    strobeState = LOW;
    doStrobe();
  }
  
}

void doStrobe(){
  if( lastStrobeState != strobeState ){
    switchRelay(strobeState);
    
    lastStrobeState = strobeState;
    
    if( strobeState == HIGH && currentMode == 3 ){
      delay(100);
      switchRelay(LOW);
    }
  }
}

void switchRelay(int state){
  digitalWrite(relayPin, state);
  digitalWrite(ledPin, state);
}

void tappedTempo(){
  
  unsigned long time = millis();
  
  if( ( time - tapTimePrevious ) > 3000 ){
    tapCount = 0;
  }
  
  if( tapCount == 0 ){
    tapTimeFirst = millis();
  }else{
    tapBPM = 60000 * tapCount / ( time - tapTimeFirst );
  }
  
  tapTimePrevious = time;
  tapCount++;
  
  strobeDuration = 60 / tapBPM * 1000;
  
  if( strobeDuration < 100 || strobeDuration > 10000 ){
    strobeDuration = 1000;
  }
  lastStrobeTime = 0;
}
