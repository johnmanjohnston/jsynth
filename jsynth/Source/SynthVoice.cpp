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
    clearCurrentNote();
    adsr.noteOff();
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    adsr.noteOn();
    
 mainOsc.setFrequency( juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    osc2.setFrequency((juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber + 12) / 2.f ) + 1.5f);
    osc3.setFrequency((juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber) * 2.f) - 1.2f);
    osc4.setFrequency((juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber) / 2.f));
    osc5.setFrequency((juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber) / 2.f) + 1.2f);
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
 mainOsc.process(juce::dsp::ProcessContextReplacing<float>(audoBlock));
    osc2.process(juce::dsp::ProcessContextReplacing<float>(audoBlock));
    osc3.process(juce::dsp::ProcessContextReplacing<float>(audoBlock));
    osc4.process(juce::dsp::ProcessContextReplacing<float>(audoBlock));
    osc5.process(juce::dsp::ProcessContextReplacing<float>(audoBlock));
    
    gain.process(juce::dsp::ProcessContextReplacing<float>(audoBlock));
    adsr.applyEnvelopeToBuffer(outputBuffer, 0, numSamples);
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    DBG("prepareToPlay()");

    adsrParams.attack = 0.3f;
    adsrParams.decay = 5.f;
    adsrParams.sustain = 1.f;
    adsrParams.release = 0.5f;

    adsr.setSampleRate(sampleRate);
    adsr.setParameters(adsrParams);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = outputChannels;

    mainOsc.prepare(spec);
    osc2.prepare(spec);
    osc3.prepare(spec);
    osc4.prepare(spec);
    osc5.prepare(spec);
    gain.prepare(spec);
    gain.setGainLinear(0.1f);

    this->isPrepared = true;
}
