#include <LiquidCrystal_I2C.h>
#include <AFMotor.h>

// Initialize LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change the I2C address and LCD dimensions if needed

// Constants for button pin and modes
const int numModes = 2;
AF_DCMotor motorLeft(1);
AF_DCMotor motorRight(2);
AF_DCMotor motorDownward(3);

// Variables
int currentMode = 0;
int buttonState = HIGH;
int lastButtonState = HIGH;
const int joystickThreshold = 30;

// Ultrasonic sensor variables
const int trigPin = A1;
const int echoPin = A2;
const int joystickPinX = A0;
const int joystickPinY = A5;
const int joystickButtonPin = 2;
const int buttonPin = A3;

int joystickButtonState = HIGH;
int lastJoystickButtonState = HIGH;

void setup() {
  Serial.begin(9600);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.begin(16, 2);
  lcd.print("Mode: ");
  lcd.setCursor(7, 0);
  lcd.print(currentMode);

  // Initialize buttons
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(joystickButtonPin, INPUT_PULLUP);

  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set initial mode
  setMode(currentMode);
}

void loop() {
  // Read button states
  buttonState = digitalRead(buttonPin);
  joystickButtonState = digitalRead(joystickButtonPin);

  // Check for button press
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Increment mode and wrap around if necessary
    currentMode = (currentMode + 1) % numModes;

    // Update LCD
    lcd.setCursor(7, 0);
    lcd.print(currentMode);

    // Set the new mode
    setMode(currentMode);
  }

  // Check for joystick button press
  

  // Update last button states
  lastButtonState = buttonState;
  lastJoystickButtonState = joystickButtonState;

  // Add a delay to prevent infinite loop
  // Adjust the delay time as needed
  delay(300);
}

// Function to handle mode change
void setMode(int mode) {
  // Perform actions based on the selected mode
  switch (mode) {
    case 0:
      // Mode 0 actions
      lcd.setCursor(0, 1);
      lcd.print("~ MODE RACING ~       ");

      for (int i = 0; i < 200; i++) {
        int joystickX = analogRead(joystickPinX);
        int joystickY = analogRead(joystickPinY);

        Serial.print("Joystick X-axis: ");
        Serial.println(joystickX);
        Serial.print("Joystick Y-axis: ");
        Serial.println(joystickY);

        // Control the motors based on the joystick input
        if (joystickX < (512 - joystickThreshold)) {
          turnLeft();
        } else if (joystickX > (512 + joystickThreshold)) {
          turnRight();
        } else {
          if (joystickY < (512 - joystickThreshold)) {
            forward();
          } else if (joystickY > (512 + joystickThreshold)) {
            backward();
          }
        }

        // Check for joystick button press
        joystickButtonState = digitalRead(joystickButtonPin);
        if (joystickButtonState == LOW) {
          // Activate the downward motor
          performDCmotor();
        }else{
          deactivateDCmotor();
        }
          
        // Add a delay to prevent continuous loop
        delay(100);
      }
      lcd.setCursor(0, 1);
      lcd.print("ENGINE STOP                ");
      lcd.setCursor(0, 1);
      lcd.print("PRESS TO SCAN              ");
      stop();
      delay(100);
      break;

    case 1:
      // Mode 1 actions
      lcd.setCursor(0, 1);
      lcd.print("Mode 1 selected       ");

      for (int i = 0; i < 30; i++) {
        performUltrasonicOperation();
      }

      lcd.setCursor(0, 1);
      lcd.print("DONE SCANNING                ");
      lcd.setCursor(0, 1);
      lcd.print("PRESS TO MOVE                ");

      // Add your custom actions for Mode 1
      break;

    // Add more cases for additional modes if needed
  }
}

// Function to perform ultrasonic operations
void performUltrasonicOperation() {
  // Add your ultrasonic operations here
  // Example: Code for measuring distance using the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(350);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH);
  unsigned int distance = duration * 0.034 / 2;

  // Display the distance on the LCD
  if (distance < 30) {
    lcd.setCursor(0, 1);
    lcd.print("OBJECT NEAR !!   ");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print(" cm         ");
  }
}

// Function to activate the downward motor
void performDCmotor() {
  // Activate the downward motor
  motorDownward.setSpeed(255);
  motorDownward.run(FORWARD);
}
void deactivateDCmotor() {
  // Activate the downward motor
  motorDownward.setSpeed(0);
  motorDownward.run(FORWARD);
}
void forward() {
  motorLeft.setSpeed(255);
  motorRight.setSpeed(255);
  motorLeft.run(FORWARD);
  motorRight.run(FORWARD);
}

void turnLeft() {
  motorLeft.setSpeed(255);
  motorRight.setSpeed(0);
  motorLeft.run(FORWARD);
  motorRight.run(FORWARD);
}

void turnRight() {
  motorLeft.setSpeed(0);
  motorRight.setSpeed(255);
  motorLeft.run(FORWARD);
  motorRight.run(FORWARD);
}

void backward() {
  motorLeft.setSpeed(0);
  motorRight.setSpeed(0);
  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);
}

void stop() {
  motorLeft.setSpeed(0);
  motorRight.setSpeed(0);
  
  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);
}