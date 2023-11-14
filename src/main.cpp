#include <Arduino.h>
#include <definitions.hpp>
#include <ArduinoJson.h>
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
    TCCR1A = 0b00000000;
    TCCR1B = 0b00000010;
    TCCR1C = 0;
    OCR1A = compare;
    TCNT1H = 0;
    TCNT1L = 0;
    TIMSK1 = _BV(OCIE1A);
}
/**
 * @brief Interrupt service routine 
 * Flips pulse pin of motor if number of steps is not reached.
 * 
 */
ISR(TIMER1_COMPA_vect)
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
 * @brief Confirm that motors have performed steps.
 * 
 */
void sendResponse(const char* msg)
{
    Serial.println(msg);
    done = true;
}
/**
 * @brief Message from raspberry pi
 * Assign steps and direction to correct motor.
 * Or activate trigger mechanism.
 */
void handleIncomingMessage()
{
    StaticJsonDocument<100> document;
    if(deserializeJson(document, Serial) != DeserializationError::Ok) return;
    if(document["A"] && document["P"])
    {
        azimuth.steps = document["A"]["S"];
        digitalWrite(TurretSyndrome::Azimuth::direction, document["A"]["D"]);
        pitch.steps = document["P"]["S"];
        digitalWrite(TurretSyndrome::Pitch::direction, document["P"]["D"]);
        done = false;
    }
    else if(document["T"] && document["T"] == 1)
    {
        unsigned long deadline = millis() + 1000;
        bool confirmed = false;
        digitalWrite(TurretSyndrome::Trigger::output, HIGH);
        while (not (confirmed or millis() > deadline))
        {
            if (digitalRead(TurretSyndrome::Trigger::input) == LOW)
            {
                confirmed = true;
                sendResponse("Fired");
            }
        }
        digitalWrite(TurretSyndrome::Trigger::output, LOW);
    }
}
void setup()
{
    Serial.begin(115200);
    pinMode(TurretSyndrome::Trigger::input, INPUT);
    pinMode(TurretSyndrome::Trigger::output, OUTPUT);
    for (auto& m: motors)
    {
        pinMode(m->direction, OUTPUT);
        pinMode(m->pulse, OUTPUT);
        m->steps = 0;
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