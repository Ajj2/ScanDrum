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
#include "VarDelay.h"
#include "Mixer.h"
#include "AudioUtils.h"

using namespace juce;

//==============================================================================

class MainContentComponent   : public AudioAppComponent,
                               public ActionBroadcaster,
                               public MidiInputCallback
{
public:
    //==============================================================================
    MainContentComponent() : varDelay (1), mixer (1)
    {
        osc = Osc::getInstance();
        leapM = new LeapM (*osc);
        
        addAndMakeVisible(leapM);
     
        setAudioChannels (1, 2);
        
        addActionListener(leapM);
        
        setSize (800, 600);
        
        myRand.setSeed (Time::getMillisecondCounter());
        
        StringArray devices = MidiOutput::getDevices();
    
        deviceManager.addMidiInputCallback(String::empty, this);
        deviceManager.setMidiInputEnabled("Launch Control XL", true);
        
        for (int i = 0; i < 8; i++)
        {
            filterBankFreq[i] = 50;
        }
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
        sine.setFrequency(1000);
        
        //env.setRate((float)sampleRate/(float)samplesPerBlockExpected);
        //env.setValue(0);
        
        env.initialise(sampleRate);
        env.set(Envelope::Points(0, 0)(1, 1)(2, 0), sampleRate);
        
        coeff = IIRCoefficients::makeHighPass(sampleRate, 200);
        filter.setCoefficients(coeff);
        filter.reset();
        
        varDelay.prepareToPlay(samplesPerBlockExpected, sampleRate);
        varDelay.setOutGain(1);
        varDelay.setModWaveShape(VarDelay::sine);
        varDelay.setParameter(VarDelay::delayTimeP, 0.02);
        varDelay.setParameter(VarDelay::feedbackP, 0.9);
        varDelay.setParameter(VarDelay::mixP, 0.8);
        varDelay.setParameter(VarDelay::modSpeedP, 0.1);
        varDelay.setParameter(VarDelay::modDepthP, 0.8);
        
//        mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
//        mixer.setInputGain(1);
//        mixer.setDelayParameter(0, VarDelay::delayTimeP, 0.001);
//        mixer.setDelayParameter(0, VarDelay::feedbackP, 0.9);
//        mixer.setDelayParameter(0, VarDelay::mixP, 0.8);
//        mixer.setDelayParameter(0, VarDelay::modSpeedP, 0.5);
//        mixer.setDelayParameter(0, VarDelay::modDepthP, 0.8);
        
        for (int i = 0 ; i < 8; i++)
        {
            multiFilter.addFilter(sampleRate, (i+1)*500, 10);
        }
        multiFilter.prepareToPlay(samplesPerBlockExpected, sampleRate);
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
        
        for (int filter = 0; filter < 8; filter++)
        {
            multiFilter.setFilerFreq(filter, filterBankFreq[filter].get(), 10);
        }
        
        float* outP = bufferToFill.buffer->getWritePointer(0);
        int numSamples = bufferToFill.numSamples;
        
        while (numSamples--)
        {
//            stk::StkFloat data = sine.tick();
//            *outP = data;
            
            *outP = ((myRand.nextFloat() * 2.0) - 1.0) * 0.5;
            //*outP = filter.processSingleSampleRaw(*outP) * env.tick();
        
            outP++;
        }
        
        multiFilter.getNextAudioBlock(bufferToFill);
        
        //varDelay.getNextAudioBlock(bufferToFill);
        //mixer.getNextAudioBlock(bufferToFill);
    }
    
    void releaseResources() override{}

    //==============================================================================
    
    void handleIncomingMidiMessage(MidiInput *source, const MidiMessage& message) override
    {
        if (message.isController())
        {
            float value = (float)message.getControllerValue() / 127.0;
            switch (message.getControllerNumber())
            {
                case 77:
                    filterBankFreq[0] = (value * 9950.0) + 50.0;
                    break;
                case 78:
                    filterBankFreq[0] = (value * 9950.0) + 50.0;
                    break;
                case 79:
                    filterBankFreq[0] = (value * 9950.0) + 50.0;
                    break;
                case 80:
                    filterBankFreq[0] = (value * 9950.0) + 50.0;
                    break;
                case 81:
                    filterBankFreq[0] = (value * 9950.0) + 50.0;
                    break;
                case 82:
                    filterBankFreq[0] = (value * 9950.0) + 50.0;
                    break;
                case 83:
                    filterBankFreq[0] = (value * 9950.0) + 50.0;
                    break;
                case 84:
                    filterBankFreq[0] = (value * 9950.0) + 50.0;
                    break;
                default:
                    break;
            }
        }
    }
    
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
    Envelope env;
    
    Random myRand;
    
    IIRFilter filter;
    IIRCoefficients coeff;
    
    MultibandFilter multiFilter;
    
    VarDelay varDelay;
    Mixer mixer;
    
    Atomic<float> filterBankFreq[8];
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }

#endif  // MAINCOMPONENT_H_INCLUDED
