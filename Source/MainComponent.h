/*
  ==============================================================================

    MainComponent.h
    Created: 4 Aug 2017 12:52:44pm
    Author:  Alex Jones

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Osc.h"
#include "LeapM.h"
#include "Audio.h"

using namespace juce;

//==============================================================================

class MainContentComponent   :  public Component,
                                public ActionBroadcaster
{
public:
    //==============================================================================
    MainContentComponent(Audio& audio_);
    
    ~MainContentComponent();
    
    //==============================================================================
    void paint (Graphics& g) override;
    
    void resized() override;
    
    //==============================================================================
    
private:
    //==============================================================================
    Audio& audio;
    
    ScopedPointer<Osc> osc;
    ScopedPointer<LeapM> leapM;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

// (This function is called by the app startup code to create our main component)
//Component* createMainContentComponent()     { return new MainContentComponent(); }

#endif  // MAINCOMPONENT_H_INCLUDED
 
