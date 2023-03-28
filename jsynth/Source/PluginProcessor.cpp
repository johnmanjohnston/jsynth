/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthSound.h"
#include "SynthVoice.h"

//==============================================================================
JsynthAudioProcessor::JsynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    synth.clearSounds();
    synth.clearVoices();

    synth.addVoice(new SynthVoice());
    synth.addSound(new SynthSound());
}

JsynthAudioProcessor::~JsynthAudioProcessor()
{
}

//==============================================================================
const juce::String JsynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JsynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JsynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JsynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JsynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JsynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JsynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JsynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JsynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void JsynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void JsynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) 
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumInputChannels());
        }
    }
}

void JsynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JsynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void JsynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (auto midiData : midiMessages) 
    {
        auto midiMessage = midiData.getMessage();

        if (midiMessage.isNoteOn()) 
        {
            for (int i = 0; i < synth.getNumVoices(); ++i)
            {
                if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
                {
                    voice->activeNotes += 1;

                    for (int i = 0; i < 4; ++i) 
                    {
                        if (voice->noteNumbers[i] == 0) 
                        {
                            voice->noteNumbers[i] = midiMessage.getNoteNumber();
                            break;
                        }
                    }
                }
            }
        }

        else 
        {
            for (int i = 0; i < synth.getNumVoices(); ++i)
            {
                if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
                {
                    voice->activeNotes -= 1;

                    for (int i = 0; i < 4; ++i)
                    {
                        if (voice->noteNumbers[i] == midiMessage.getNoteNumber())
                        {
                            voice->noteNumbers[i] = 0;
                            break;
                        }
                    }
                }
            }
        }
    }

    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool JsynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JsynthAudioProcessor::createEditor()
{
    return new JsynthAudioProcessorEditor (*this);
}

//==============================================================================
void JsynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JsynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JsynthAudioProcessor();
}
