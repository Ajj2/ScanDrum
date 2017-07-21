/*
  ==============================================================================

    VarDelay.h
    Created: 21 Jul 2017 2:58:25pm
    Author:  Alex Jones

  ==============================================================================
*/

#ifndef VARDELAY_H_INCLUDED
#define VARDELAY_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Oscillator.h"
#include "math.h"

/** This file contains audio FX that can be instantiated in any other audio class*/

//==============================================================================

/** A sine wave oscillator class. */
class SinOsc : public Oscillator
{
public:
    /** SinOsc Constructor.*/
    SinOsc(){}
    /** SinOsc Destructor.*/
    ~SinOsc(){}
    
    /** Renders the waveshape on a sample by sample basis.*/
    float renderWaveShape(const float currentPhase) override
    {
        return sinf(currentPhase);
    }
private:
    
    
};

/** A Triangle wave oscillator class.*/
class TriangleOsc : public Oscillator
{
public:
    /**TriangleOsc Constructor.*/
    TriangleOsc(){}
    /**TriangleOsc Destructor.*/
    ~TriangleOsc(){}
    
    /** Renders the waveshape on a sample by sample basis.*/
    float renderWaveShape(const float currentPhase) override
    {
        if (currentPhase < M_PI)
        {
            return (-1) + ((2 / M_PI) * currentPhase);
        }
        else
        {
            return 3 - ((2 / M_PI) * currentPhase);
        }
    }
private:
    
};

/** A square wave oscillator class.*/
class SquareOsc : public Oscillator
{
public:
    /**SquareOsc Constructor.*/
    SquareOsc(){}
    /**SquareOsc Destructor.*/
    ~SquareOsc(){}
    
    /** Renders the waveshape on a sample by sample basis.*/
    float renderWaveShape(const float currentPhase) override
    {
        if (currentPhase < M_PI)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }
    
private:
    
    
};

//==============================================================================

/** Threshold - This class test if a signal passes a given threshold.*/

class Threshold
{
public:
    /** Threshold constructor.*/
    Threshold();
    /** Threshold destructor.*/
    ~Threshold();
    
    /** Sets the threshold
     @param newThreshold - a new threshold which to test incoming signals against.
     */
    void setThreshold(float newThreshold);
    
    /** Tests the threshold against the previous value
     @param input - the input signal which will be testes againsy the threshold
     @return bool - true when the signal has past the threshold, false otherwise.
     */
    bool testThreshold(float input);
    
private:
    float previous;
    float threshold;
};

//==============================================================================

/** VarDelay - This class is a variable delay line, which uses windowing to mitigate the pitch shifting effects normally associated with this techinque. It has some built in modulation. */

class VarDelay : public AudioSource
{
public:
    /** An enum that defines the parameters that each delay has.*/
    enum delayParams
    {
        delayTimeP = 0,
        feedbackP,
        mixP,
        modSpeedP,
        modDepthP,
        numParams
    };
    /** An enum that defines the available modulator wave shapes.*/
    enum modWaveformTypes
    {
        sine = 1,
        triangle,
        square,
        
        numModWaveformTypes
    };
    
    /** VarDelay constructor
     @param indexID - a unique ID that can be used to address a specific delay (if there are multiple in use)
     */
    VarDelay(int indexID);
    /** VarDelay destructor.*/
    ~VarDelay();
    
    /** Tells the source to prepare for playing.*/
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    /** Allows the source to release anything it no longer needs after playback has stopped.*/
    void releaseResources() override;
    /** Called repeatedly to fetch subsequent blocks of audio data.*/
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    
    /** Write to a delay line specified by the win and channel arguments. There must be two buffers for every channel
     @param writePos    - an integer that defines the position in the buffer that will be written to.
     @param input       - the input signal that will be written into the buffer.
     @param ch          - if the buffer has more than one channel, then this defines which channel is to be written to.
     @param win         - Each var delay contains dual buffers for each channel, neccesary to perform the windowing action. This defines which              of the two buffers to write to.
     */
    void write(int writePos, float input, int ch, int win);
    /** Read from a delay line specified by the win and channel arguments.
     @param readPos - an integer that defines where in the buffer to read from.
     @param ch      - if the buffer has more than one channel, then this defines which channel is to be read from.
     @param win     - Each var delay contains dual buffers for each channel, neccesary to perform the windowing action. This defines which              of the two buffers to read from.
     */
    float read(float readPos, int ch, int win);
    
    /** Set a parameter specified by the paramType argument.
     @param paramType - the parameter to be set. uses the delayParams enum.
     @param newValue  - the value to set the parameter to.
     */
    void setParameter (int paramType, float newValue);
    /** Returns the value of a parameter specified by the paramType argument
     @param  paramType - the parameter to be set. uses the delayParams enum.
     @return the value of the parameter requested.
     */
    float getParameter (int paramType);
    
    /** Returns the current delay time, after is has been modulated.*/
    float getModulatedDelayTime();
    /** Returns the current delay time, before it has been modulated.*/
    float getDelayTime();
    
    /** Sets the output gain of the delay (useful when using multiple delays in parallel)
     @param newOutGain
     */
    void setOutGain (float newOutGain);
    
    /** Sets a new waveshape for the mod oscillators
     @param newModWaveshape - defines the waveshape to set in reference to the modWaveformTypes enum.
     */
    void setModWaveShape (int newModWaveshape);
    
    /** resets the modulating oscillators, so that all the modulating cycles are in synch with each other*/
    void resetModOscillators ();
    
    /** Returns the maximum possible delay time
     @return teh maximum possible delay time (defined by the buffer size.)
     */
    float getMaxDelayTime();
    
    /** Returns the ID of this delay - Useful when using an array of delay lines. Set in the constructor
     @return the ID of this varDelay
     */
    int getIndexID();
    
    /** Returns a string array of all the parameters that this class has
     @return A StringArray with all the parameter names.
     */
    StringArray getParameters();
    /** Returns the name of a sinlge parameter used by this delay
     @param index - the index of the parameter to return the name of
     */
    String getParameterByIndex(int index);
    
private:
    int ID;
    int pos1, pos2;
    float Pdiff, Vdiff;
    int bufferSize[2], bufferWritePos[2];
    float sR;
    const float maxDelayTime = 2.f;
    StringArray parameters;
    
    AudioSampleBuffer sampleBuffer[2];
    AudioBuffer<float> scratchBuffer[2];
    
    Oscillator* modOsc[2];
    TriangleOsc triOsc[2];
    SquareOsc sqrOsc[2];
    SinOsc sinOsc[2];
    SinOsc winOsc[2];
    Threshold threshold[2][2];
    
    LinearSmoothedValue<float> modValues[2][2];
    LinearSmoothedValue<float> delWindowValues[2][2];
    LinearSmoothedValue<float> delTimeSmooth;
    LinearSmoothedValue<float> outGainSmooth;
    
    Atomic<float> delayTime;
    Atomic<float> feedback;
    Atomic<float> mix;
    Atomic<float> modSpeed;
    Atomic<float> modDepth;
    Atomic<float> outGain;
    
    Atomic<float> readableDelayTime;
};



#endif  // VARDELAY_H_INCLUDED
