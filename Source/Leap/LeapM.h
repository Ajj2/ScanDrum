/*
  ==============================================================================

    LeapM.h
    Created: 19 Jul 2017 1:48:07pm
    Author:  Alex Jones

  ==============================================================================
*/

#ifndef LEAPM_H_INCLUDED
#define LEAPM_H_INCLUDED

#include <deque>
#include "../../JuceLibraryCode/JuceHeader.h"
#include <LeapMath.h>
#include <Leap.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Osc.h"
#include "Cursor.h"

//==============================================================================
/*
*/
class LeapM    : public juce::Component,
                 public juce::Timer,
                 public juce::ActionListener 
{
public:
    LeapM(Osc& oscRef_);
    ~LeapM();

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;
    
    void sendPositionMessage();
    
    void actionListenerCallback (const juce::String &message) override;

private:
    
    Leap::Controller controller;
    Leap::Frame currentFrame;
    
    juce::TextEditor textDisplay;
    
    Osc& oscRef;
    
    juce::Atomic<int> gotOnset;
    
    double m_currentTime;
    double m_prevTime;
    
    const int maxNumCursors = 8;
    
    std::deque<Cursor*> inactiveCursors;
    std::deque<Cursor*> activeCursors;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LeapM)
};


#endif  // LEAPM_H_INCLUDED
