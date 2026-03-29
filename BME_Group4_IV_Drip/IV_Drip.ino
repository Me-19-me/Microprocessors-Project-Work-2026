#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 9);

int buttonPin = 2;
int buzzer = 8;
int greenLED = 12;
int redLED = 11;

int drops = 0;
int fluidLevel = 1000;   // 1000 = 100.0%
unsigned long lastTime = 0;
unsigned long emptyStartTime = 0;
bool bagEmpty = false;
bool lastButtonState = HIGH;

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  lcd.begin(16, 2);

  // Startup screen
  lcd.setCursor(0, 0);
  lcd.print("IV Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Level:100%");
  delay(2000);
  lcd.clear();

  // Start in normal state
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);
  digitalWrite(buzzer, LOW);

  lastTime = millis();
}

void loop()
{
  bool buttonState = digitalRead(buttonPin);

  // Detect one press = one drop
  if (buttonState == LOW && lastButtonState == HIGH && !bagEmpty)
  {
    drops++;

    // Reduce by 0.5% per drop
    fluidLevel -= 5;

    if (fluidLevel <= 0)
    {
      fluidLevel = 0;
      bagEmpty = true;
      emptyStartTime = millis();
    }

    delay(200); // debounce
  }

  lastButtonState = buttonState;

  // Auto refill after 30 seconds at 0%
  if (bagEmpty && (millis() - emptyStartTime >= 30000))
  {
    fluidLevel = 1000;   // refill to 100%
    bagEmpty = false;
    drops = 0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bag Refilled");
    lcd.setCursor(0, 1);
    lcd.print("Level:100%");
    delay(2000);
    lcd.clear();
    

    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzer, LOW);

    lastTime = millis();
  }

  // Update display every 5 seconds
  if (millis() - lastTime >= 5000)
  {
    int dpm = drops * 12;          // 5 sec count -> drops/min
    int displayLevel = fluidLevel / 10;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DPM:");
    lcd.print(dpm);

    lcd.setCursor(9, 0);
    lcd.print("L:");
    lcd.print(displayLevel);
    lcd.print("%");

    lcd.setCursor(0, 1);

    if (displayLevel == 0)
    {
      lcd.print("Status:EMPTY");

      digitalWrite(redLED, HIGH);
      digitalWrite(greenLED, LOW);
      digitalWrite(buzzer, HIGH);
    }
    else if (displayLevel < 20)
    {
      lcd.print("Status:LOW");

      digitalWrite(redLED, HIGH);
      digitalWrite(greenLED, LOW);
      digitalWrite(buzzer, HIGH);
    }
    else
    {
      lcd.print("Status:NORMAL");

      digitalWrite(greenLED, HIGH);
      digitalWrite(redLED, LOW);
      digitalWrite(buzzer, LOW);
    }

    drops = 0;
    lastTime = millis();
  }
}








