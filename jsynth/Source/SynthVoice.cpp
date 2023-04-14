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
    auto noteFreq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

    osc.setFrequency(noteFreq + this->detuneOsc1);
    osc2.setFrequency(noteFreq + this->detuneOsc2);

    subOsc.setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber - 12));
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

    subOsc.process(juce::dsp::ProcessContextReplacing<float>(audoBlock));
    osc.process(juce::dsp::ProcessContextReplacing<float>(audoBlock));
    osc2.process(juce::dsp::ProcessContextReplacing<float>(audoBlock));


    gain.process(juce::dsp::ProcessContextReplacing<float>(audoBlock));
    adsr.applyEnvelopeToBuffer(outputBuffer, 0, numSamples);
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    DBG("prepareToPlay()");

    adsrParams.attack = 0.1f;
    adsrParams.decay = 1.f;
    adsrParams.sustain = 1.f;
    adsrParams.release = 0.5f;

    adsr.setSampleRate(sampleRate);
    adsr.setParameters(adsrParams);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = outputChannels;

    osc.prepare(spec);
    osc2.prepare(spec);

    // Prepare sub oscillator
    subOsc.prepare(spec);

    gain.prepare(spec);
    gain.setGainLinear(0.07f);

    this->isPrepared = true;
}
