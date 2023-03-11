/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JsynthAudioProcessorEditor::JsynthAudioProcessorEditor (JsynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), keyboardComponent(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize(this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
    addAndMakeVisible(keyboardComponent);
}

JsynthAudioProcessorEditor::~JsynthAudioProcessorEditor()
{
}

//==============================================================================
void JsynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("JSYNTH", getLocalBounds(), juce::Justification::centred, 1);
}

void JsynthAudioProcessorEditor::resized()
{
    keyboardComponent.setBounds(getLocalBounds());
}
