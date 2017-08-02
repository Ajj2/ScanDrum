/*
  ==============================================================================

  The Synthesis ToolKit in C++ (STK) is a set of open source audio
  signal processing and algorithmic synthesis classes written in the
  C++ programming language. STK was designed to facilitate rapid
  development of music synthesis and audio processing software, with
  an emphasis on cross-platform functionality, realtime control,
  ease of use, and educational example code.  STK currently runs
  with realtime support (audio and MIDI) on Linux, Macintosh OS X,
  and Windows computer platforms. Generic, non-realtime support has
  been tested under NeXTStep, Sun, and other platforms and should
  work with any standard C++ compiler.

  STK WWW site: http://ccrma.stanford.edu/software/stk/include/

  The Synthesis ToolKit in C++ (STK)
  Copyright (c) 1995-2011 Perry R. Cook and Gary P. Scavone

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  Any person wishing to distribute modifications to the Software is
  asked to send the modifications to the original developer so that
  they can be incorporated into the canonical version.  This is,
  however, not a binding provision of this license.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
  ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  ==============================================================================
*/

/*
BEGIN_JUCE_MODULE_DECLARATION

    ID:               stk_wrapper
    vendor:           adamski
    version:          1.0.0
    name:             STK Library
    description:      JUCE wrapper module for the STK Library, for audio synthesis and effects. 
    website:          https://github.com/thestk/stk
    dependencies:     juce_core

END_JUCE_MODULE_DECLARATION 
*/

#ifndef __STK_STKHEADER__
#define __STK_STKHEADER__

#if JUCE_LITTLE_ENDIAN && ! defined (__LITTLE_ENDIAN__)
 #define __LITTLE_ENDIAN__
#endif

#if JUCE_MAC
 #define __MACOSX_CORE__
#endif

#if JUCE_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4127 4702 4244 4305 4100 4996 4309)
#endif

//=============================================================================
#include "ADSR.h"
#include "Asymp.h"
#include "BandedWG.h"
#include "BeeThree.h"
#include "BiQuad.h"
#include "Blit.h"
#include "BlitSaw.h"
#include "BlitSquare.h"
#include "BlowBotl.h"
#include "BlowHole.h"
#include "Bowed.h"
#include "BowTable.h"
#include "Brass.h"
#include "Chorus.h"
#include "Clarinet.h"
#include "Cubic.h"
#include "Delay.h"
#include "DelayA.h"
#include "DelayL.h"
#include "Drummer.h"
#include "Echo.h"
#include "Effect.h"
#include "Envelope.h"
#include "FileLoop.h"
#include "FileRead.h"
#include "FileWrite.h"
#include "FileWvIn.h"
#include "FileWvOut.h"
#include "Filter.h"
#include "Fir.h"
#include "Flute.h"
#include "FM.h"
#include "FMVoices.h"
#include "FormSwep.h"
#include "Function.h"
#include "Generator.h"
#include "Granulate.h"
#include "HevyMetl.h"
#include "Iir.h"
#include "Instrmnt.h"
#include "JCRev.h"
#include "JetTable.h"
#include "LentPitShift.h"
#include "Mandolin.h"
#include "Mesh2D.h"
#include "MidiFileIn.h"
#include "Modal.h"
#include "ModalBar.h"
#include "Modulate.h"
#include "Moog.h"
#include "Noise.h"
#include "NRev.h"
#include "OnePole.h"
#include "OneZero.h"
#include "PercFlut.h"
#include "Phonemes.h"
#include "PitShift.h"
#include "Plucked.h"
#include "PoleZero.h"
#include "PRCRev.h"
#include "ReedTable.h"
#include "Resonate.h"
#include "Rhodey.h"
#include "Sampler.h"
#include "Saxofony.h"
#include "Shakers.h"
#include "Simple.h"
#include "SineWave.h"
#include "SingWave.h"
#include "Sitar.h"
#include "Skini.h"
#include "Sphere.h"
#include "StifKarp.h"
#include "Stk.h"
#include "TapDelay.h"
#include "TubeBell.h"
#include "Twang.h"
#include "TwoPole.h"
#include "TwoZero.h"
#include "Vector3D.h"
#include "Voicer.h"
#include "VoicForm.h"
#include "Whistle.h"
#include "Wurley.h"
#include "WvIn.h"
#include "WvOut.h"

#if JUCE_MSVC
 #pragma warning (pop)
#endif

#endif   // __STK_STKHEADER__
