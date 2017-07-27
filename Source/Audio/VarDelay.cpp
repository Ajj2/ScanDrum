/*
  ==============================================================================

    VarDelay.cpp
    Created: 21 Jul 2017 2:58:25pm
    Author:  Alex Jones

  ==============================================================================
*/

#include "VarDelay.h"

using namespace juce;

//==============================================================================

Threshold::Threshold()
{
    threshold = 0.0001;
    previous = 0.0002;
}

Threshold::~Threshold(){}

void Threshold::setThreshold(float newThreshold)
{
    threshold = newThreshold;
}

bool Threshold::testThreshold(float input)
{
    bool result;
    if (input < threshold)
    {
        
    }
    if (previous <= threshold && input > threshold)
    {
        result = true;
    }
    else
    {
        result = false;
    }
    previous = input;
    return result;
}

//==============================================================================

VarDelay::VarDelay(int indexID) : ID (indexID)
{
    mix = 1;
    feedback = 0.95;
    delayTime = 0.5;
    delTimeSmooth = 0.5;
    pos1 = pos2 = Pdiff = Vdiff = 0;
    readableDelayTime = 0;
    
    modSpeed = 0.01;
    modDepth = 0.1;
    outGain = 1;
    
    modOsc[0] = &sinOsc[0];
    modOsc[1] = &sinOsc[1];
    
    parameters = {"delayTime", "feedback", "mix", "modSpeed", "modDepth"};
}

VarDelay::~VarDelay()
{
    
}

void VarDelay::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    scratchBuffer[0].setSize (2, samplesPerBlockExpected);
    scratchBuffer[1].setSize (2, samplesPerBlockExpected);
    
    for (int ch = 0; ch < 2; ch++)
    {
        sampleBuffer[ch].setSize(2, maxDelayTime * sampleRate);
        sampleBuffer[ch].clear();
        bufferSize[ch] = (int)(maxDelayTime * sampleRate);
        bufferWritePos[ch] = 0;
    }
    sR = sampleRate;
    for (int ch = 0; ch < 2; ch++)
    {
        sinOsc[ch].setSampleRate(sR);
        triOsc[ch].setSampleRate(sR);
        sqrOsc[ch].setSampleRate(sR);
        
        winOsc[ch].setSampleRate(sR);
        winOsc[ch].setAmplitude(1);
        winOsc[ch].setFrequency(10);
        
        delWindowValues[ch][0].setValue(0);
        delWindowValues[ch][1].setValue(0);
    }
    
    modValues[0][0].reset(sR, 0.5);
    modValues[0][1].reset(sR, 0.5);
    modValues[1][0].reset(sR, 0.5);
    modValues[1][1].reset(sR, 0.5);
    
    delTimeSmooth.reset(sR, 1);
    outGainSmooth.reset(sR, 0.3);
}

void VarDelay::releaseResources()
{
    
}

void VarDelay::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    int numSamples = bufferToFill.numSamples;
    float totalNumInputChannels = bufferToFill.buffer->getNumChannels();
    
    //----------------------------------------------------------------------------------------------------
    
    juce::AudioBuffer<float> inputSampleBuffer[2];
    
    // Refer the buffer to the scratch buffers, which should be the correct size.
    for (int i = 0; i < 2; i++)
    {
        inputSampleBuffer[i] = juce::AudioBuffer<float>(scratchBuffer[i].getArrayOfWritePointers(), totalNumInputChannels, numSamples);
    }
    
    // Copy the data from the data from the incoming buffer in our buffer.
    for (int ch = 0; ch < totalNumInputChannels; ch++)
    {
        inputSampleBuffer[ch].copyFrom(ch, 0, *bufferToFill.buffer, 0, 0, numSamples);
    }
    
    //----------------------------------------------------------------------------------------------------
    
    for (int ch = 0; ch < bufferToFill.buffer->getNumChannels(); ch++)
    {
        float bufferReadPos[2];
        float delSig[2];
        float modValInSec;
        float modValInSamples;
        float modOffset = 0;
        
        float windowVal[2];
        float invWindowVal[2];
        float numSamples = bufferToFill.numSamples;
        float* outP = bufferToFill.buffer->getWritePointer(ch);
        float* inP  = inputSampleBuffer[ch].getWritePointer(ch);
        
        float modDepthLocal = modDepth.get();
        float modSpeedLocal = modSpeed.get();
        modOsc[ch]->setAmplitude(modDepthLocal);
        modOsc[ch]->setFrequency(modSpeedLocal);
        
        while (numSamples--)
        {
            bufferWritePos[ch]++;
            
            if (bufferWritePos[ch] > bufferSize[ch])
                bufferWritePos[ch] = 0;
            
            // Get the next value from the windowing oscillator and an inverted version.
            windowVal[ch] = (winOsc[ch].nextSample() * 0.5) + 0.5;
            invWindowVal[ch] = 1.0f - windowVal[ch];
            
            (modSpeedLocal != 0) ? modValInSec = modOsc[ch]->nextSample() : modValInSec = 0;
            
            modValInSamples = modValInSec * sR;
            
            float delTimeInSec = delTimeSmooth.getNextValue();
            
            ((delTimeInSec - modDepthLocal) < 0) ? modOffset = (modDepthLocal - delTimeInSec) + 0.0001 : modOffset = 0;
            
            // The delay time is the delay time set by the user +/- any modulation value
            float modDelTimeInSec  = (delTimeInSec + modValInSec) + modOffset;
            
            readableDelayTime = modDelTimeInSec;
            
            float delTimeInSamples = (modDelTimeInSec * sR);
            
            // The threshold class returns true when the signal passes the threshold. This allows the delay time to be changed for that buffer.
            if (threshold[ch][0].testThreshold(windowVal[ch]))
            {
                delWindowValues[ch][0].setValue(delTimeInSamples);
            }
            
            if (threshold[ch][1].testThreshold(invWindowVal[ch]))
            {
                delWindowValues[ch][1].setValue(delTimeInSamples);
            }
            
            bufferReadPos[0] = bufferWritePos[ch] - delWindowValues[ch][0].getNextValue();;
            bufferReadPos[1] = bufferWritePos[ch] - delWindowValues[ch][1].getNextValue();
            
            for (int i = 0 ; i < 2; i++)
            {
                if (bufferReadPos[i] < 0)
                {
                    bufferReadPos[i] += sampleBuffer[i].getNumSamples();
                }
                if (bufferReadPos[i] > sampleBuffer[i].getNumSamples()-1)
                {
                    bufferReadPos[i] -= sampleBuffer[i].getNumSamples();
                }
            }
            
            delSig[0] = read(bufferReadPos[0], ch, 0);
            delSig[1] = read(bufferReadPos[1], ch, 1);
            
            // The outgoing signal is combination of the output of the 2 windowed buffers, mulitplied by the sine wave used in the threshold test
            float reconstructedSignal = (delSig[0] * windowVal[ch]) + (delSig[1] * invWindowVal[ch]);
            
            write(bufferWritePos[ch], *inP + (reconstructedSignal * feedback.get()), ch, 0);
            write(bufferWritePos[ch], *inP + (reconstructedSignal * feedback.get()), ch, 1);
            
            *outP = ( (reconstructedSignal * mix.get()) + (*inP * (1.0f - mix.get() ) ) ) * outGainSmooth.getNextValue();
            
            inP++;
            outP++;
        }
    }
}

void VarDelay::write(int writePos, float input, int ch, int win)
{
    if(writePos < 0)
        writePos = 0;
    else if (writePos > bufferSize[ch]-1)
        writePos = bufferSize[ch]-1;
    
    float* sampleP = sampleBuffer[ch].getWritePointer(win, writePos);
    *sampleP = input;
}

float VarDelay::read(float readPos, int ch, int win)
{
    pos1 = (int)readPos;
    
    pos2 = pos1 + 1;
    
    if(pos1 == sampleBuffer[ch].getNumSamples()-1)
    {
        pos2 = pos1;
    }
    
    if(pos2 == bufferSize[ch])
        pos2 = 0;
    
    Pdiff = readPos - pos1;
    
    float* samplePos2 = sampleBuffer[ch].getWritePointer(win, pos2);
    float* samplePos1 = sampleBuffer[ch].getWritePointer(win, pos1);
    
    Vdiff = *samplePos2 - *samplePos1;
    
    return *samplePos1 + (Vdiff * Pdiff);
}

void VarDelay::setParameter (int paramType, float newValue)
{
    switch (paramType)
    {
        case delayTimeP:
            delayTime = newValue;
            delTimeSmooth.setValue(delayTime.get());
            break;
        case feedbackP:
            feedback = newValue;
            break;
        case mixP:
            mix = newValue;
            break;
        case modDepthP:
            modDepth = newValue;
            break;
        case modSpeedP:
            modSpeed = newValue;
            break;
        default:
            break;
    }
}

float VarDelay::getParameter (int paramType)
{
    float result = 0;
    switch (paramType)
    {
        case delayTimeP:
            result = delayTime.get();
            break;
        case feedbackP:
            result = feedback.get();
            break;
        case mixP:
            result = mix.get();
            break;
        case modDepthP:
            result = modDepth.get();
            break;
        case modSpeedP:
            result = modSpeed.get();
            break;
        default:
            break;
    }
    return result;
}

float VarDelay::getModulatedDelayTime()
{
    return readableDelayTime.get();
}

void VarDelay::setOutGain (float newOutGain)
{
    outGain = newOutGain;
    outGainSmooth.setValue(outGain.get());
}

void VarDelay::setModWaveShape (int newModWaveshape)
{
    switch (newModWaveshape)
    {
        case sine:
            for (int i = 0; i < 2; i++)
            {
                modOsc[i] = &sinOsc[i];
                modOsc[i]->reset();
            }
            break;
        case triangle:
            for (int i = 0; i < 2; i++)
            {
                modOsc[i] = &triOsc[i];
                modOsc[i]->reset();
            }
            break;
        case square:
            for (int i = 0; i < 2; i++)
            {
                modOsc[i] = &sqrOsc[i];
                modOsc[i]->reset();
            }
            break;
        default:
            for (int i = 0; i < 2; i++)
            {
                modOsc[i] = &sinOsc[i];
                modOsc[i]->reset();
            }
            break;
    }
}

void VarDelay::resetModOscillators ()
{
    modOsc[0]->reset();
    modOsc[1]->reset();
}

float VarDelay::getMaxDelayTime()
{
    return maxDelayTime;
}

float VarDelay::getDelayTime()
{
    return delayTime.get();
}

juce::StringArray VarDelay::getParameters()
{
    return parameters;
}

juce::String VarDelay::getParameterByIndex(int index)
{
    return parameters[index];
}

int VarDelay::getIndexID()
{
    return ID;
}

//==============================================================================
