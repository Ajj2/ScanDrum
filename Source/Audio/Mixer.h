/*
  ==============================================================================

    Mixer.h
    Created: 21 Jul 2017 3:26:42pm
    Author:  Alex Jones

  ==============================================================================
*/

#ifndef MIXER_H_INCLUDED
#define MIXER_H_INCLUDED

#include "VarDelay.h"
#include "../JuceLibraryCode/JuceHeader.h"

/** This class contains all of the Mixer used by the App. It performs the routing and mixing necessary for all the Mixer.*/

class Mixer : public AudioSource
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
    /** An enum that defines the routing options available.*/
    enum routingOptions
    {
        parallel = 1,
        series,
        numRoutingOptions
    };
    
    /** Mixer Constructor.
     @param numDelay - the number of delay being used. Set in Audio.
     */
    Mixer(int numDelays);
    /** Mixer Destructor.*/
    ~Mixer();
    
    /** Tells the source to prepare for playing */
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    
    /** Allows the source to release anything it no longer needs after playback has stopped. */
    void releaseResources() override;
    
    /** Called repeatedly to fetch subsequent blocks of audio data.*/
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    
    /** Sets a single parameter of one varDelay specified by the index argument
     @param index - the index of the delay to be updated.
     @param parameter - the parameter to be set. uses the delayParams enum.
     @param newValue - the value to set the parameter to.
     */
    void setDelayParameter (int index, int parameter, float newValue);
    
    /** Returns an reference to a single varDelay
     @param index - the index of the delay to return.
     @return A reference to a single VarDelay
     */
    VarDelay& getVarDelay (int index);
    /** Returns an reference to an array of Mixer contained in this component
     @return a reference to an OwnedArray that contains all the Mixer.
     */
    const OwnedArray<VarDelay>& getVarDelays();
    /** Returns the number of delays
     @return the number of delays in use.
     */
    int getNumDelays();
    /** Returns a string array of all the parameters that a single delay has
     @return A StringArray with all the parameters for a single VariableDelay class.
     */
    StringArray getParameters();
    /** Defines whether the delay lines are mixed in series or parallel
     @param newRoutingOption - references the routingOptions enum to set a new routing option
     */
    void setParallelOrSeries (int newRoutingOption);
    
    /** Sets the gain of the incoming audio to be processed
     @param newInputGain - The new input gain to use
     */
    void setInputGain (float newInputGain);
    
private:
    //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    
    OwnedArray<VarDelay> delays;
    
    //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    
    AudioBuffer<float> scratchMixBuffer;
    Array<AudioBuffer<float>> scratchBuffers;
    Array<AudioSampleBuffer> inputSampleB;
    
    Atomic<float> inputGain;
    LinearSmoothedValue<float> inputGainSmooth;
    
    int routingOption;
};


#endif  // MIXER_H_INCLUDED
