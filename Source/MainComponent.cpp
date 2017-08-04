/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "MainComponent.h"


MainContentComponent::MainContentComponent(Audio& audio_) : audio(audio_)
{
    osc = Osc::getInstance();
    leapM = new LeapM (*osc);
    
    addAndMakeVisible(leapM);
    
    addActionListener(leapM);
    setSize (800, 600);
}

MainContentComponent::~MainContentComponent()
{
    delete osc;
    delete leapM;
}

//==============================================================================
void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colours::darkgrey);
}

void MainContentComponent::resized() 
{
    leapM->setBounds(getLocalBounds());
}

