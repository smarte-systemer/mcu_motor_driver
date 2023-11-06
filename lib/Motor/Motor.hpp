#pragma once

#include <Arduino.h>


enum class Direction: bool
{
    Clockwise,
    CounterClockwise
};


class Motor
{
private:
    uint8_t m_directionPin;
    uint8_t m_pulsePin;
    unsigned int m_frequency;
    float m_halfPeriod{0};// in milliseconds
    unsigned long m_endTime{0};
    unsigned long m_startTime{0};
    unsigned int m_steps;
    bool m_started{0};
    bool m_confirmed{false};
    char m_id{};
 public:
    Motor(const uint8_t& direction, const uint8_t& pulse, 
          const unsigned int& frequency, const char& id);
    void set(const unsigned int& steps, const bool& direction);
    void drive();
    const bool finished();
    const bool getConfirmStatus();
    void setConfirmStatus();
    const char getId() const;
    const unsigned int getSteps() const;
    // const bool getDirection() const;
    ~Motor();
};

