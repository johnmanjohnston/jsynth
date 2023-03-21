/*
  ==============================================================================

    SynthVoice.cpp
    Created: 9 Mar 2023 6:29:43pm
    Author:  Ethan John

  ==============================================================================
*/

#include "SynthVoice.h"
#include "PluginEditor.h"

void SynthVoice::controllerMoved(int controllerNumber, int newValue)
{
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    adsr.noteOff();
    clearCurrentNote();
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    adsr.noteOn();

    for (int i = 0; i < oscCount; ++i) {
        auto targetFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

        if (i % 2 != 0) { targetFrequency -= i*i; }
        else { targetFrequency += i*i; }

        oscillators[i].setFrequency(targetFrequency);
    }
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return nullptr != dynamic_cast<juce::SynthesiserSound*>(sound);
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    juce::AudioBuffer<float> synthesisBufferProxy(outputBuffer.getArrayOfWritePointers(), 2, 0, numSamples);
    juce::dsp::AudioBlock<float> audoBlock{ synthesisBufferProxy };

    for (int i = 0; i < oscCount; ++i) {
        oscillators[i].process(juce::dsp::ProcessContextReplacing<float>(audoBlock));
    }

    gain.process(juce::dsp::ProcessContextReplacing<float>(audoBlock));
    adsr.applyEnvelopeToBuffer(outputBuffer, 0, numSamples);
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    DBG("prepareToPlay()");

    adsrParams.attack = 0.1f;
    adsrParams.decay = 2.f;
    adsrParams.sustain = 1.f;
    adsrParams.release = 0.5f;

    adsr.setSampleRate(sampleRate);
    adsr.setParameters(adsrParams);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = outputChannels;

    for (int i = 0; i < oscCount; ++i) {
        oscillators[i].initialise([](float x) { return std::sin(x / juce::MathConstants<float>::twoPi); });
    }

    for (int i = 0; i < oscCount; ++i) {
        oscillators[i].prepare(spec);
    }

    gain.prepare(spec);
    gain.setGainLinear(0.1f);

    this->isPrepared = true;
}
