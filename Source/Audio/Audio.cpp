/*
  ==============================================================================

    Audio.cpp
    Created: 4 Aug 2017 12:37:31pm
    Author:  Alex Jones

  ==============================================================================
*/

#include "Audio.h"

Audio::Audio() : varDelay (1), mixer (1)
{
    setAudioChannels(1, 2);
}

Audio::~Audio()
{
    shutdownAudio();
}

//==============================================================================
void Audio::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    StringArray devices = MidiOutput::getDevices();
    
    deviceManager.addMidiInputCallback(String::empty, this);
    deviceManager.setMidiInputEnabled("Launch Control XL", true);
    
    Sr = sampleRate;
    
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
    
    varDelay.prepareToPlay(samplesPerBlockExpected, sampleRate);
    varDelay.setOutGain(1);
    varDelay.setModWaveShape(VarDelay::sine);
    varDelay.setParameter(VarDelay::delayTimeP, 0.1);
    varDelay.setParameter(VarDelay::feedbackP, 0.95);
    varDelay.setParameter(VarDelay::mixP, 0.5);
    varDelay.setParameter(VarDelay::modSpeedP, 0.05);
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
        multiFilter.addFilter(sampleRate, 50, 2);
    }
    multiFilter.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void Audio::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) 
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
            
            // Signal that a new multiFilter should be created
            env.set(Envelope::Points(0, 0)(1, 1)(2, 0), Sr);
        }
    }
    
    float* outP = bufferToFill.buffer->getWritePointer(0);
    int numSamples = bufferToFill.numSamples;
    
    while (numSamples--)
    {
        //            stk::StkFloat data = sine.tick();
        //            *outP = data;
        
        *outP = ((myRand.nextFloat() * 2.0) - 1.0); //* env.tick();;
        outP++;
    }
    
    multiFilter.getNextAudioBlock(bufferToFill);
    
    //varDelay.getNextAudioBlock(bufferToFill);
    //mixer.getNextAudioBlock(bufferToFill);
}

void Audio::releaseResources()
{
}

//==============================================================================

void Audio::handleIncomingMidiMessage(MidiInput *source, const MidiMessage& message)
{
    if (message.isController())
    {
        float value = (float)message.getControllerValue() / 127.0;
        switch (message.getControllerNumber())
        {
            case 77:
                multiFilter.setFilerFreq(0, (value * 9950.0) + 100.0, 1.2);
                break;
            case 78:
                multiFilter.setFilerFreq(1, (value * 9950.0) + 100.0, 1.2);
                break;
            case 79:
                multiFilter.setFilerFreq(2, (value * 9950.0) + 100.0, 1.2);
                break;
            case 80:
                multiFilter.setFilerFreq(3, (value * 9950.0) + 100.0, 1.2);
                break;
            case 81:
                multiFilter.setFilerFreq(4, (value * 9950.0) + 100.0, 1.2);
                break;
            case 82:
                multiFilter.setFilerFreq(5, (value * 9950.0) + 100.0, 1.2);
                break;
            case 83:
                multiFilter.setFilerFreq(6, (value * 9950.0) + 100.0, 1.2);
                break;
            case 84:
                multiFilter.setFilerFreq(7, (value * 9950.0) + 100.0, 1.2);
                break;
            default:
                break;
        }
    }
}

//==============================================================================

void Audio::newCursorCreated (int index, String& name, float averageBrightness)
{
    multiFilter.setFilterAmp(index, (averageBrightness / 100.0));
}