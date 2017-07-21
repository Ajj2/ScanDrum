/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Osc.h"
#include "LeapM.h"
#include <aubio/aubio.h>
#include "Stk.h"

//==============================================================================

class MainContentComponent   : public AudioAppComponent,
                               public ActionBroadcaster
{
public:
    //==============================================================================
    MainContentComponent()
    {
        osc = new Osc;
        leapM = new LeapM (*osc);
        
        addAndMakeVisible(leapM);
     
        setAudioChannels (1, 2);
        
        addActionListener(leapM);
        
        setSize (800, 600);
        
        myRand.setSeed (Time::getMillisecondCounter());
    }

    ~MainContentComponent()
    {
        delete osc;
        delete leapM;
        shutdownAudio();
    }

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        hopSize = samplesPerBlockExpected/4;
        onsetIn = new_fvec(hopSize);
        onsetOut = new_fvec(2);
        onsetDetect = new_aubio_onset("default", samplesPerBlockExpected, hopSize, sampleRate);
        
        sine.reset();
        sine.setRate((float)sampleRate/(float)samplesPerBlockExpected);
        sine.setFrequency(500);
        
        coeff = IIRCoefficients::makeLowPass(sampleRate, 400);
        filter.setCoefficients(coeff);
        filter.reset();
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        for (int i=0; i<4; ++i)
        {
            // Copy from the incoming buffer into the aubio analysis buffer
            memcpy(onsetIn->data, &(bufferToFill.buffer->getWritePointer(0)[i*hopSize]), hopSize*sizeof(float));
            
            // Onset detection
            aubio_onset_do(onsetDetect, onsetIn, onsetOut);
            
            // If an onset is detected, display something about it
            static int counter = 0;
            if(onsetOut->data[0] != 0)
            {
                std::cout << "got onset " << counter++ << std::endl;
                
                // Send the current coordinates from the leap class
                
                sendActionMessage("onsetDetected");
            }
        }
        
        float* outP = bufferToFill.buffer->getWritePointer(0);
        int numSamples = bufferToFill.numSamples;
        
        while (numSamples--)
        {
//            stk::StkFloat data = sine.tick();
//            *outP = data;
            
            *outP = ((myRand.nextFloat() * 2.0) - 1.0) * 0.5;
            *outP = filter.processSingleSampleRaw(*outP);
            
            outP++;
        }
    }
    
    void releaseResources() override{}

    //==============================================================================
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::darkgrey);
    }

    void resized() override
    {
        leapM->setBounds(getLocalBounds());
    }
    
    //==============================================================================

private:
    //==============================================================================
    Osc* osc;
    LeapM* leapM;
    
    uint_t hopSize;
    fvec_t* onsetIn;
    fvec_t* onsetOut;
    fvec_t* analysisBuffer;
    aubio_onset_t * onsetDetect;
    
    stk::SineWave sine;
    
    Random myRand;
    
    IIRFilter filter[2];
    IIRCoefficients coeff[2];
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }

#endif  // MAINCOMPONENT_H_INCLUDED
