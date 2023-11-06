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
 * @brief Message from raspberry pi
 * Assign steps and direction to correct motor.
 */
void handleIncomingMessage()
{
   // delay(200);
    StaticJsonDocument<100> document;
    if(deserializeJson(document, Serial) != DeserializationError::Ok) return;
    if(document["A"])
    {
        azimuth.steps = document["A"]["S"];
        digitalWrite(TurretSyndrome::Azimuth::direction, document["A"]["D"]);
        // azimuth.direction = document["A"]["D"];
    }
    if(document["P"])
    {
        pitch.steps = document["P"]["S"];
        digitalWrite(TurretSyndrome::Pitch::direction, document["P"]["D"]);
        // pitch.direction = document["P"]["D"];
    }
    
    done = false;
}
/**
 * @brief Confirm that motors have performed steps.
 * 
 */
void sendResponse()
{
    Serial.println("Done");
    done = true;
}

void setup()
{
    Serial.begin(115200);
    for (auto& m: motors)
    {
        pinMode(m->direction, OUTPUT);
        pinMode(m->pulse, OUTPUT);
       m->steps = 100;
    }   
    startTimerOne(TurretSyndrome::ticks);
}
void loop()
{
    if(!azimuth.steps && !pitch.steps)
    {
        if(Serial.available()) handleIncomingMessage();
        else if(!done)sendResponse();
    }
}