#define LED 10
#define BUZZER 12
#define BUTTON 13
#define LDR A0
#define A 8
#define B 9
#define C 4
#define D 3
#define E 2
#define F 7
#define G 6 

int time_difference = 0;
int buttonState = 0;
long int time_var = 0;
int seconds = 0;
int password = 1234;  // senha definida
int alarm_input = 0;
int userInput = 0;
int attempts = 0;
int i =0;
bool pwd;

enum State {
  DEACTIVATED,
  DOOR_OPEN,
  ACTIVATED,
  WARNING,
  ALARM,
} state; 

void displayDigit(int digit) {
  // desliga todos os segmentos antes de exibir o numero
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);

  // liga os segmentos para cada digito
  switch (digit) {
    case 0:
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(F, HIGH);
      break;
    case 1:
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      break;
    case 2:
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(G, HIGH);
      break;
    case 3:
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(G, HIGH);
      break;
    case 4:
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
      break;
    case 5:
      digitalWrite(A, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
      break;
    case 6:
      digitalWrite(A, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
      break;
    case 7:
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      break;
    case 8:
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
      break;
    case 9:
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
      break;
    default:
      break;
  }
}

bool btnPressed() {
  buttonState = digitalRead(BUTTON);
  while (buttonState == LOW) {
    buttonState = digitalRead(BUTTON);
    if (buttonState == HIGH) {
      return true;
    }
  }
}

void deactivateAll() {
  digitalWrite(LED, LOW);
  digitalWrite(BUZZER, LOW);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);  
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);
}

void showState(){
  Serial.print("State: ");
  Serial.println(state);
}

void beep() {
  digitalWrite(LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(LED, LOW);      
  digitalWrite(BUZZER, LOW);
  delay(200);
}

void blinkLED() {
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
  delay(200);
}

bool isDoorClosed() {
  Serial.println("Please, close the door (type 1): ");
  time_var = millis();
  while (millis() - time_var < 10000) {
    if (Serial.available() > 0) {
      int doorState = Serial.parseInt();
      if (doorState == 1) {
        return true;
      }
    }
  }
  return false;  
}

void showDigit() {
  seconds = (10000 - time_difference);
  for (i=0; i<10; i++){
    if (seconds < (i+1)*1000 && i*1000< seconds ) {
      displayDigit(i);
    }
  }
}

bool enterPassword() {
  Serial.println("Enter password: ");
  // startCountdown();
  time_var = millis();
  Serial.read();
  while (millis() - time_var < 10000) {
    time_difference = millis() - time_var;
    showDigit();
    if (Serial.available() > 0) {
      int userInput = Serial.parseInt();
      if (userInput == password) {
        return true;
      } else {
        Serial.println("Wrong password. Try again. ");
        return false;
      }
    }
  }
  return false;  
}


void triggerAlarm() {
  digitalWrite(BUZZER, HIGH);
  digitalWrite(LED, HIGH);
  Serial.read();
  pwd = false;
  while (!pwd) {
    if (Serial.available() > 0) {
      int alarm_input = Serial.parseInt();
      if (alarm_input == password) {
        pwd = true;
      }
    }
  }
  state = DEACTIVATED;
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(LDR, INPUT); 
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);  
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  state = DEACTIVATED;
  Serial.begin(9600);
}

void loop() {
  switch(state){

    case DEACTIVATED:
      attempts = 0;
      showState();
      deactivateAll();
      if (btnPressed()) {
        state = DOOR_OPEN;
      }
      break;

    case DOOR_OPEN:
      beep();
      if (isDoorClosed()) {
        state = ACTIVATED;
      } else {
        state = ALARM;
      }
      break;

    case ACTIVATED:
      showState();
      Serial.println(analogRead(LDR));  
      if (analogRead(LDR) < 600) {  
        beep();
        beep();
        state = WARNING;
      }
      break;

    case WARNING:
      showState(); 
      pwd = false;
      while (attempts < 2 && !pwd) {
        if (pwd = enterPassword()) {
          state = DEACTIVATED;  // senha correta, volta para ativado
        } else {
          attempts++;
        }
      if (attempts == 2) {
        state = ALARM;
      }
      }
      break;

    case ALARM:
      showState();
      triggerAlarm();  // buzzer continuo e LED piscando
      break;
  }
}
