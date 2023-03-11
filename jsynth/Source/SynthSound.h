/*
  ==============================================================================

    SynthSound.h
    Created: 9 Mar 2023 6:36:34pm
    Author:  Ethan John

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound 
{
public:
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }
};