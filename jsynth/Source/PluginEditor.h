#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class JsynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    JsynthAudioProcessorEditor (JsynthAudioProcessor&);
    ~JsynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JsynthAudioProcessor& audioProcessor;
    juce::MidiKeyboardComponent keyboardComponent;

    const uint32_t ASPECT_RATIO_MULTIPLIER = 7;
    const uint32_t ASPECT_RATIO_X = 110;
    const uint32_t ASPECT_RATIO_Y = 14;
    const uint32_t WINDOW_WIDTH = ASPECT_RATIO_X * ASPECT_RATIO_MULTIPLIER;
    const uint32_t WINDOW_HEIGHT = ASPECT_RATIO_Y * ASPECT_RATIO_MULTIPLIER;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JsynthAudioProcessorEditor)
};
