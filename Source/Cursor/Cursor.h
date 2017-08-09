#ifndef CURSOR_H
#define CURSOR_H

#include "../../JuceLibraryCode/JuceHeader.h"

#include <string>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>

class Cursor
{
private:
    juce::String m_name;
    int m_index;
    long m_time;
    long m_duration;
    float m_position[3];
    float m_speed[3];
    float m_updateRate;
    float m_avBrightness;
    
public:
    Cursor(int index, const juce::String& name, const int& updateRate);
    void initialise(float pos[], float speed[],
                    const long& duration, std::vector<cv::Point>&, float avBrightness);
    bool update(const long& time);
    juce::String& getName () { return m_name; }
    int getIndex () { return m_index; }

};

#endif
