#include "Chrono.h"

Chrono::Chrono(float initialTimeMinutes, float incrementSeconds) {
    d_timeRemaining = initialTimeMinutes * 60.0f; 
    d_increment = incrementSeconds;
}

void Chrono::update() {
    if (d_timeRemaining > 0) {
        d_timeRemaining -= GetFrameTime(); 
        if (d_timeRemaining < 0) d_timeRemaining = 0.0f; 
    }
}

void Chrono::addIncrement() {
    d_timeRemaining += d_increment;
}

bool Chrono::isZero() const {
    return d_timeRemaining <= 0.0f;
}

const char* Chrono::getFormattedTime() const {
    int mins = (int)d_timeRemaining / 60;
    int secs = (int)d_timeRemaining % 60;
    return TextFormat("%02d:%02d", mins, secs); 
}
void Chrono::setTime(float newTime){
    d_timeRemaining = newTime;
}
void Chrono::setIncrement(float newIncrement){
    d_increment = newIncrement;
}
void Chrono::addTime(float seconds) { d_timeRemaining += seconds; };
