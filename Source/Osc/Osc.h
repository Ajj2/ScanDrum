/*
  ==============================================================================

    Osc.h
    Created: 19 Jul 2017 1:22:35pm
    Author:  Alex Jones

  ==============================================================================
*/

#ifndef OSC_H_INCLUDED
#define OSC_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Osc   :   private OSCReceiver,
                private OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>
{
public:
    Osc();
    ~Osc();
    
    /** Called when the OSCReceiver receives a new OSC message.*/
    void oscMessageReceived (const OSCMessage& message) override;
    
    void sendOSCMessage (String ap, float val);
    void sendOSCMessage (String ap, float val, float val2);
    void sendOSCMessage (String ap, float val, float val2, float val3);

private:
    
    OSCSender sender;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Osc)
};


#endif  // OSC_H_INCLUDED
