#pragma once
#include "raylib.h"

class Chrono {
public:
    static constexpr float STANDARD_TIME = 10.0f;
    static constexpr float STANDARD_INCREMENT = 2.0f;
    // On passe le temps en minutes pour que ce soit facile à configurer
    Chrono(float initialTimeMinutes, float incrementSeconds = 0.0f);

    void update();          
    void addIncrement();    
    bool isZero() const;    
    void setTime(float newTime);
    void setIncrement(float newIncrement);
    

    const char* getFormattedTime() const; 

private:
    float d_timeRemaining;
    float d_increment;

    
};