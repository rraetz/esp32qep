#include "esp32qep.h"

// Pins
constexpr int PIN_A = 18;
constexpr int PIN_B = 19;
constexpr int PUSH_BUTTON = 15;


void setup() 
{
  Serial.begin(9600);
  pinMode(PUSH_BUTTON, INPUT);
  QEP_initialize(PIN_A, PIN_B);
}


void loop() 
{
  // Read the counter every 100 ms
  Serial.println(QEP_getCounter());

  // Reset counter if button is pressed
  if (digitalRead(PUSH_BUTTON) == HIGH)
  {
    QEP_resetCounter();
  }
  delay(100);
}
