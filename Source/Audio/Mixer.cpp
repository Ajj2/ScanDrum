/*
  ==============================================================================

    Mixer.cpp
    Created: 21 Jul 2017 3:26:42pm
    Author:  Alex Jones

  ==============================================================================
*/

#include "Mixer.h"

Mixer::Mixer(const int numDelays)
{
    for (int i = 0; i < numDelays ; i++)
    {
        VarDelay* aVarDelay;
        delays.add(aVarDelay = new VarDelay(i));
        aVarDelay->setParameter(delayTimeP, 1);
        aVarDelay->setOutGain(1.0f / (float)numDelays);
    }
    routingOption = parallel;
    inputGain = 0;
}

Mixer::~Mixer()
{
}

void Mixer::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    scratchMixBuffer.setSize (2, samplesPerBlockExpected);
    
    for (int i = 0 ; i < delays.size(); i++)
    {
        delays[i]->prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
    
    for (int i = 0 ; i < delays.size(); i++)
    {
        scratchBuffers.add(AudioBuffer<float>(2, samplesPerBlockExpected));
    }
    
    inputGainSmooth.reset(sampleRate / samplesPerBlockExpected, 0.2);
}

void Mixer::releaseResources()
{
    for (int i = 0 ; i < delays.size(); i++)
    {
        delays[i]->releaseResources();
    }
}

void Mixer::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // -------------------------------------------------------------------------------------------------
    
    int totalNumInputChannels = bufferToFill.buffer->getNumChannels();
    int numSamples = bufferToFill.numSamples;
    
    // There are 2 constants that need to be changed in order to change the number of delays in use. The other is in the private section of Audio.
    const static int numDelayToUse = 1;
    
    AudioBuffer<float> inputSamples[numDelayToUse];
    
    for (int i = 0; i < numDelayToUse; i++)
    {
        inputSamples[i] = AudioBuffer<float>(scratchBuffers[i].getArrayOfWritePointers(), totalNumInputChannels, numSamples);
    }
    
    // Copy the input data to multiple buffers so it can be processed in parallel. The buffers are declared in prepareToPlay to avoid doing memory allocation here
    for (int ch = 0; ch < totalNumInputChannels; ch++)
    {
        for (int buf = 0; buf < numDelayToUse; buf++)
        {
            inputSamples[buf].copyFrom(ch, 0, *bufferToFill.buffer, 0, 0, numSamples);
            inputSamples[buf].applyGain(inputGainSmooth.getNextValue());
        }
    }
    bufferToFill.clearActiveBufferRegion();
    
    if (routingOption == parallel)
    {
        /* -------------------------------------------------------------------------------------------------
         PARALLEL */
        //Convert to AudioSourceChannelInfo, as this is what audio sources accept in the call to getNextAudioBlock.
        Array<AudioSourceChannelInfo> inputInfos;
        
        for (int info = 0; info < numDelayToUse; info++)
        {
            inputInfos.add(AudioSourceChannelInfo(inputSamples[info]));
        }
        
        for (int i = 0 ; i < numDelayToUse; i ++)
        {
            delays[i]->getNextAudioBlock(inputInfos[i]);
        }
        
        AudioBuffer<float> mixedBuffer(scratchMixBuffer.getArrayOfWritePointers(), totalNumInputChannels, numSamples);
        mixedBuffer.clear();
        
        for (int i = 0; i < totalNumInputChannels; i++)
        {
            int numSamp = numSamples;
            float* readPs[numDelayToUse];
            
            for (int j = 0; j < numDelayToUse; j++)
            {
                // inputInfos now contains the wet signal from the varDelays --> get a pointer for each to read from them.
                readPs[j] = inputInfos[j].buffer->getWritePointer(i);
            }
            
            float* writeP = mixedBuffer.getWritePointer(i);
            
            while(numSamp--)
            {
                for (int p = 0; p < numDelayToUse; p++)
                {
                    // Sum the contents into the mixedBuffer
                    *writeP += *readPs[p];
                    readPs[p]++;
                }
                writeP++;
            }
        }
        
        for (int i = 0; i < totalNumInputChannels; i++)
        {
            // Copy mixed buffer into bufferToFill
            bufferToFill.buffer->copyFrom(i, 0, mixedBuffer, i, 0, numSamples);
        }
        /* ------------------------------------------------------------------------------------------------- */
    }
    else if (routingOption == series)
    {
        /* ------------------------------------------------------------------------------------------------- */
        //SERIES --> much more straight forward. Simply call getNextAudioBlock for each delay in sequence on the same buffer.
        AudioSourceChannelInfo inputInfo = AudioSourceChannelInfo(inputSamples[0]);
        
        for (int i = 0; i < numDelayToUse; i++)
        {
            delays[i]->setOutGain(0.6);
            delays[i]->getNextAudioBlock(inputInfo);
        }
        
        for (int i = 0; i < totalNumInputChannels; i++)
        {
            bufferToFill.buffer->copyFrom(i, 0, *inputInfo.buffer, i, 0, numSamples);
        }
        /* ------------------------------------------------------------------------------------------------- */
    }
}

void Mixer::setDelayParameter (int index, int parameter, float newValue)
{
    delays[index]->setParameter(parameter, newValue);
}

VarDelay& Mixer::getVarDelay (int index)
{
    return *delays[index];
}

const OwnedArray<VarDelay>& Mixer::getVarDelays()
{
    return delays;
}

int Mixer::getNumDelays()
{
    return delays.size();
}

StringArray Mixer::getParameters()
{
    return delays[0]->getParameters();
}

void Mixer::setParallelOrSeries (int newRoutingOption)
{
    routingOption = newRoutingOption;
}

void Mixer::setInputGain (float newInputGain)
{
    inputGain = newInputGain;
    inputGainSmooth.setValue(inputGain.get());
}