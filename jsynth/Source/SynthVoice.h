/*
  ==============================================================================

    SynthVoice.h
    Created: 9 Mar 2023 6:29:43pm
    Author:  Ethan John

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SynthSound.h"

#define SINE_WAVE(x) std::sin(x);
#define SQUARE_WAVE(x) std::sin(x) < 0.0f ? -1.0f : 1.0f;
#define SAW_WAVE(x) x / juce::MathConstants<float>::pi
#define TRIANGLE_WAVE(x) (2 / juce::MathConstants<float>::pi) * std::asin(std::sin(x))

class SynthVoice : public juce::SynthesiserVoice
{
public:
    void controllerMoved(int controllerNumber, int newValue) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock, int ouputChannels);

private:
    juce::ADSR::Parameters adsrParams;
    juce::ADSR adsr;

    // Main oscillator, no detune, no transposation
    juce::dsp::Oscillator<float> mainOsc
    { [](float x) 
        {
            return SAW_WAVE(x);
        } 
    };

    // Slight detune, lower pitch (noteNumber / 2)
    juce::dsp::Oscillator<float> osc2
    { [](float x)
        {
            return SQUARE_WAVE(x);
        }
    };

    // Slight detune, higher pitch (noteNumber * 2)
    juce::dsp::Oscillator<float> osc3
    { [](float x)
        {
            return SAW_WAVE(x);
        }
    };

    // No detune, sine wave bass oscillator
    juce::dsp::Oscillator<float> osc4
    { [](float x)
        {
            return SAW_WAVE(x);
        }
    };

    // Bass saw wave, slight detune
    juce::dsp::Oscillator<float> osc5
    { [](float x)
        {
           return SQUARE_WAVE(x);
        }
    };

    juce::dsp::Gain<float> gain;
    bool isPrepared{ false };
}; 