#include <Servo.h>

const int SWITCH_PIN = 2;
const int SERVO_PIN = 9;

int switch_state = LOW;
int last_switch_state = LOW;
int last_debounce_time = 0;
int debounce_delay = 50;   

bool change = false;

Servo arm;

void setup() {
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(SERVO_PIN, OUTPUT);
  arm.attach(SERVO_PIN);
  Serial.begin(9600);
}

void loop() {
    switch_state = digitalRead(SWITCH_PIN);

    if (switch_state != last_switch_state && change == false) {
        last_debounce_time = millis();
        change = true;
        //Serial.print("last debounce ");
        //Serial.println(last_debounce_time);
    }

    //Serial.print("if: ");
    //Serial.println(millis() - last_debounce_time);
    if ((millis() - last_debounce_time) > debounce_delay) {
        if (last_switch_state != switch_state) {
            if (switch_state == LOW) {
                Serial.println("LOW\n");
                arm.write(90);
            } else {
                Serial.println("HIGH\n");
                arm.write(0);
            }
            last_switch_state = switch_state;
            change = false;
        }
    }
}
