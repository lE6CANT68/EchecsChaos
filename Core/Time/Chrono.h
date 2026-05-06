#pragma once
#include "raylib.h"

class Chrono {
public:
    static constexpr float STANDARD_TIME = 10.f;
    static constexpr float STANDARD_INCREMENT = 2.0f;
    
    Chrono(float initialTimeMinutes, float incrementSeconds = 0.0f);

    void update();          
    void addIncrement();    
    bool isZero() const;    
    void setTime(float newTime);
    void setIncrement(float newIncrement);
    void addTime(float seconds);
    

    const char* getFormattedTime() const; 
    const char* getScrambledTime() const;

private:
    float d_timeRemaining;
    float d_increment;

    
};