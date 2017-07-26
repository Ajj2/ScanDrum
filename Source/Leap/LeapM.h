/*
  ==============================================================================

    LeapM.h
    Created: 19 Jul 2017 1:48:07pm
    Author:  Alex Jones

  ==============================================================================
*/

#ifndef LEAPM_H_INCLUDED
#define LEAPM_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"
#include <LeapMath.h>
#include <Leap.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Osc.h"

//==============================================================================
/*
*/
class LeapM    : public Component,
                 public Timer,
                 public ActionListener 
{
public:
    LeapM(Osc& oscRef_);
    ~LeapM();

    void paint (Graphics&) override;
    void resized() override;
    
    void timerCallback() override;
    
    void sendPositionMessage();
    
    void actionListenerCallback (const String &message) override;

private:
    
    Leap::Controller controller;
    Leap::Frame currentFrame;
    
    TextEditor textDisplay;
    
    Osc& oscRef;
    
    Atomic<int> sendPosition;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LeapM)
};


#endif  // LEAPM_H_INCLUDED
