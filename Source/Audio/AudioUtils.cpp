/*
  ==============================================================================

    AudioUtils.cpp
    Created: 21 Jul 2017 6:03:52pm
    Author:  Alex Jones

  ==============================================================================
*/

#include "AudioUtils.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace juce;
    
MultibandFilter::MultibandFilter()
{
    for (int i = 0; i < numFilters; i++)
    {
        filterBankFreq[i] = ((float)i+1.0) * 50;
        filterRef[i] = 100;
        filterBankAmps[i] = 0.5;
    }
}

MultibandFilter::~MultibandFilter()
{
    
}
    
void MultibandFilter::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    scratchMixBuffer = new juce::AudioBuffer<float> (2, samplesPerBlockExpected);

    for (int i = 0 ; i < numFilters; i++)
    {
        scratchBuffers.add(juce::AudioBuffer<float>(2, samplesPerBlockExpected));
    }
}

void MultibandFilter::releaseResources()
{
    
}

void MultibandFilter::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    for (int filter_ = 0; filter_ < numFilters; filter_++)
    {
        float freqToSet = filterBankFreq[filter_].get();
        
        if (freqToSet > 0 && freqToSet != filterRef[filter_])
        {
            setFilerFreq(filter_, freqToSet, 1.2);
            *filterCoeffs[filter_] = juce::IIRCoefficients::makeBandPass(Sr, freqToSet, 1.2);
            filterBank[filter_]->setCoefficients(*filterCoeffs[filter_]);
            filterRef[filter_] = freqToSet;
        }
    }
    
    int numSamples = bufferToFill.numSamples;
    int totalNumInputChannels = bufferToFill.buffer->getNumChannels();
    
    juce::AudioBuffer<float> inputSamples[numFilters];
    
    for (int i = 0; i < filterBank.size(); i++)
    {
        inputSamples[i] = juce::AudioBuffer<float>(scratchBuffers[i].getArrayOfWritePointers(), totalNumInputChannels, numSamples);
    }

    for (int ch = 0; ch < totalNumInputChannels; ch++)
    {
        for (int bank = 0; bank < filterBank.size(); bank++)
        {
            inputSamples[bank].copyFrom(ch, 0, *bufferToFill.buffer, 0, 0, numSamples);
        }
    }
    bufferToFill.clearActiveBufferRegion();

    for (int i = 0 ; i < filterBank.size(); i ++)
    {
        filterBank[i]->processSamples(inputSamples[i].getWritePointer(0) , numSamples);
    }
    
    juce::AudioBuffer<float> mixedBuffer(scratchMixBuffer->getArrayOfWritePointers(), 2, numSamples);
    mixedBuffer.clear();
    
    for (int i = 0; i < 1; i++)
    {
        int numSamp = numSamples;
        float* readPs[numFilters];
        
        for (int j = 0; j < filterBank.size(); j++)
        {
            // inputInfos now contains the wet signal from the varDelays --> get a pointer for each to read from them.
            readPs[j] = inputSamples[j].getWritePointer(i);
        }
        
        float* writeP = mixedBuffer.getWritePointer(i);
        
        while(numSamp--)
        {
            for (int p = 0; p < filterBank.size(); p++)
            {
                // Sum the contents into the mixedBuffer
                *writeP += *readPs[p] * filterBankAmps[p].get();
                readPs[p]++;
                
            }
            writeP++;
        }
    }
    
    // Copy mixed buffer into bufferToFill
    bufferToFill.buffer->copyFrom(0, 0, mixedBuffer, 0, 0, numSamples);
    bufferToFill.buffer->copyFrom(1, 0, mixedBuffer, 0, 0, numSamples);
}

void MultibandFilter::addFilter (double sampleRate, double frequency, double Q)
{
    Sr = sampleRate;
    juce::IIRFilter* newFilter;
    filterBank.add(newFilter = new juce::IIRFilter);
    juce::IIRCoefficients* newCoeff;
    
    filterCoeffs.add(newCoeff = new juce::IIRCoefficients);
    
    *newCoeff = juce::IIRCoefficients::makeBandPass(sampleRate, frequency, Q);
    
    newFilter->setCoefficients(*newCoeff);
}

void MultibandFilter::setFilerFreq (int index, double frequency, double Q)
{
    filterBankFreq[index] = frequency;
}

void MultibandFilter::setFilterAmp (int index, float newAmp)
{
    if (index < numFilters && index >= 0);
    {
        filterBankAmps[index] = newAmp;
    }
}

