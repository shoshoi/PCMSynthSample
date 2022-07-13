#pragma once

#include <JuceHeader.h>
#include "TestResource.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, private juce::Button::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    int existFile(const char* path);

private:
    //==============================================================================
    // Your private member variables go here...
    juce::TextButton sampleSelectButton;
    juce::TextButton deviceSettingButton;
    juce::MidiMessageCollector midiCollector;
    juce::MidiKeyboardState keyboardState;              // MIDIƒf[ƒ^‚ðMIDIƒL[ƒ{[ƒh‚ÉÅ“K‚Èƒf[ƒ^‚É•ÏŠ·‚µ‚Ä•ÛŽ‚·‚éƒNƒ‰ƒX
    juce::MidiKeyboardComponent* keyboardComponent;     // MIDIKeyboadComponentƒNƒ‰ƒX‚Ìƒ|ƒCƒ“ƒ^
    juce::Synthesiser synth;

    void showDeviceSetting();
    void setupPcmSynth();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
        
};
