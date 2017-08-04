/*
  ==============================================================================

    Audio.h
    Created: 4 Aug 2017 12:37:31pm
    Author:  Alex Jones

  ==============================================================================
*/

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include <JuceHeader.h>
#include <aubio/aubio.h>
#include "Stk.h"
#include "VarDelay.h"
#include "Mixer.h"
#include "AudioUtils.h"

class Audio : public AudioAppComponent,
              public MidiInputCallback,
              public ActionBroadcaster
{
public:
    
    Audio();
    ~Audio();
    
    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    
    void releaseResources() override;
    
    //==============================================================================
    
    void handleIncomingMidiMessage(MidiInput *source, const MidiMessage& message) override;
    
    void newCursorCreated (String& name);
    
private:
    
    stk::SineWave sine;
    Envelope env;
    
    juce::Random myRand;
    
    MultibandFilter multiFilter;
    
    VarDelay varDelay;
    Mixer mixer;
    
    int Sr;
    
    uint_t hopSize;
    fvec_t* onsetIn;
    fvec_t* onsetOut;
    fvec_t* analysisBuffer;
    aubio_onset_t * onsetDetect;
    
};



#endif  // AUDIO_H_INCLUDED
