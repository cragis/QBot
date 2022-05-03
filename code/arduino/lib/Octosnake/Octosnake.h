#ifndef octosnake_h
#define octosnake_h

#include <Arduino.h>
#include <Servo.h>

#ifndef PI
  #define PI 3.14159
#endif


class Oscillator{

    public:
        Oscillator();  // sets parameters to default
        float refresh();  // recalculates output 
        void reset();  //set ref_time to now
        void start();  //runs reset and sets stop to false
        void start(unsigned long ref_time); // sets stop to false and ref_time to input parameter
        void stop(); //sets stop to true
        float time_to_radians(double time);
        float degrees_to_radians(float degrees);
        float degrees_to_time(float degrees);
        void setPeriod(int period);
        void setAmplitude(int amplitude);
        void setPhase(int phase);
        void setOffset(int offset);
        void setTime(unsigned long ref);
        float getOutput();
        float getPhaseProgress();
        unsigned long getTime();

    private:
        int _period;
        int _amplitude;
        int _phase;
        int _offset;
        float _output;
        bool _stop;
        unsigned long _ref_time = 0;
        float _delta_time = 0;
};

#endif
