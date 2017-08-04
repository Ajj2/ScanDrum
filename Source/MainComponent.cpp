/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "MainComponent.h"


MainContentComponent::MainContentComponent(Audio& audio_) : audio(audio_)
{
    osc = Osc::getInstance();
    leapM = new LeapM (audio);
    
    addAndMakeVisible(leapM);
    audio.addActionListener(leapM);
    
    setSize (800, 600);
}

MainContentComponent::~MainContentComponent(){}

//==============================================================================
void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colours::darkgrey);
}

void MainContentComponent::resized() 
{
    leapM->setBounds(getLocalBounds());
}

