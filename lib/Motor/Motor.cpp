#include "Motor.hpp"

Motor::Motor(const uint8_t& direction, const uint8_t& pulse, 
             const unsigned int& frequency, const char& id):
            m_directionPin{direction}, m_pulsePin{pulse}, 
            m_frequency{frequency}, m_id{id}
{
    pinMode(m_directionPin, OUTPUT);
    pinMode(m_pulsePin, OUTPUT);
    m_halfPeriod = 385;
}

void Motor::set(const unsigned int& steps, const bool& direction)
{

    m_endTime = millis() + m_halfPeriod;
    Serial.print("HALF: ");
    Serial.println(m_halfPeriod);
    digitalWrite(m_directionPin, direction);
    m_confirmed = false;
    m_steps = steps;
}
void Motor::drive()
{
    if(m_steps != 0)
    {
        if(m_startTime + m_halfPeriod <= micros())
        {
            digitalWrite(m_pulsePin, !digitalRead(m_pulsePin));
            m_startTime = micros();
            //--m_steps;
        }
    }
}
const bool Motor::finished()
{
    return m_confirmed;
}
const bool Motor::getConfirmStatus(){ return m_confirmed;}
void Motor::setConfirmStatus(){m_confirmed = true;}
const char Motor::getId() const{return m_id;}
const unsigned int Motor::getSteps() const {return m_steps;}
// const bool Motor::getDirection() const {return m_direction;}
Motor::~Motor(){}