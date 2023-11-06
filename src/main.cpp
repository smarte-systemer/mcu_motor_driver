#include <Arduino.h>
#include <definitions.hpp>
#include <ArduinoJson.h>
#include <Servo.h>

Servo triggerServo;

/**
 * @brief 
 * Struct to represent motors
 */
struct motor
{
    uint8_t pulse;
    uint8_t direction;
    unsigned int steps;
};

volatile motor azimuth = {.pulse=TurretSyndrome::Azimuth::pulse,
                 .direction = TurretSyndrome::Azimuth::direction,
                 .steps = 0};
volatile motor pitch = {.pulse=TurretSyndrome::Pitch::pulse,
                 .direction = TurretSyndrome::Pitch::direction,
                 .steps = 0};
volatile motor* motors[] = {&azimuth, &pitch};
volatile bool done = true;
volatile bool canShoot = false;
/**
 * @brief 
 *  Start timer1, set compare with value in microseconds
 * @param value 
 */
void startTimerOne(unsigned int compare)
{
    //Approx. 4us runtime
    TCCR2A = 0b00000000;
    TCCR2B = 0b00000010;
    OCR2A = compare;
    TIMSK2 = _BV(OCIE2A);
}
/**
 * @brief Interrupt service routine 
 * Flips pulse pin of motor if number of steps is not reached.
 * 
 */
ISR(TIMER2_COMPA_vect)
{
    startTimerOne(TurretSyndrome::ticks);
    // For loop takes approx. 32us
    for(auto& m: motors)
    {
        if(m->steps != 0)
        {
            digitalWrite(m->pulse, !digitalRead(m->pulse));
            if(!digitalRead(m->pulse))--m->steps;
        }
    }
}
/**
 * @brief Sets or unsets reload state for trigger servo.
 * If servo is set to not shoot, servo will be set back to reload position.
 * 
 */
void buttonPress()
{
    canShoot = digitalRead(TurretSyndrome::Trigger::button);
    if(!canShoot)triggerServo.write(TurretSyndrome::Trigger::reload);     
}
/**
 * @brief Message from raspberry pi
 * Assign steps and direction to correct motor.
 */
void handleIncomingMessage()
{
    StaticJsonDocument<100> document;
    if(deserializeJson(document, Serial) != DeserializationError::Ok) return;
    if(document["A"] && document["B"])
    {
        azimuth.steps = document["A"]["S"];
        digitalWrite(TurretSyndrome::Azimuth::direction, document["A"]["D"]);
        pitch.steps = document["P"]["S"];
        digitalWrite(TurretSyndrome::Pitch::direction, document["P"]["D"]);
        done = false;
    }
    else if(document["T"])
    {
        if(canShoot)
        {
            triggerServo.write(TurretSyndrome::Trigger::shoot);
            sendResponse("Fired");
        }
    }
   
}
/**
 * @brief Confirm that motors have performed steps.
 * 
 */
void sendResponse(const char* msg)
{
    Serial.println(msg);
    done = true;
}

void setup()
{
    Serial.begin(115200);
    pinMode(TurretSyndrome::Trigger::button, INPUT);
    pinMode(TurretSyndrome::Trigger::pin, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(TurretSyndrome::Trigger::button), buttonPress, CHANGE);
    triggerServo.attach(TurretSyndrome::Trigger::pin);
    for (auto& m: motors)
    {
        pinMode(m->direction, OUTPUT);
        pinMode(m->pulse, OUTPUT);
       //m->steps = 100;
    }   
    startTimerOne(TurretSyndrome::ticks);
}
void loop()
{
    if(!azimuth.steps && !pitch.steps)
    {
        if(Serial.available()) handleIncomingMessage();
        else if(!done)sendResponse("Done");
    }
}