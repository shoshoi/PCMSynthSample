#include "MainComponent.h"

using namespace juce;

//==============================================================================
MainComponent::MainComponent()
{
    // MIDIKeyboadComponentƒNƒ‰ƒX‚ÌƒCƒ“ƒXƒ^ƒ“ƒX‚ðì¬. ˆø”‚É‚Í“ü—Í‚·‚éMIDIƒf[ƒ^‚ð•ÛŽ‚·‚éƒIƒuƒWƒFƒNƒg‚ð—^‚¦‚é
    keyboardComponent = new MidiKeyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard);

    // MIDIKeyboadComponent‚ðŽqƒIƒuƒWƒFƒNƒg‚Æ‚µ‚Ä”z’uE•\Ž¦‚·‚é
    addAndMakeVisible(keyboardComponent);

    // ƒ{ƒ^ƒ“ƒRƒ“ƒ|[ƒlƒ“ƒg‚ðŽqƒIƒuƒWƒFƒNƒg‚Æ‚µ‚Ä”z’uE•\Ž¦‚·‚é
    addAndMakeVisible(deviceSettingButton);
    deviceSettingButton.setButtonText("Device Setting");
    deviceSettingButton.addListener(this);

    addAndMakeVisible(sampleSelectButton);
    sampleSelectButton.setButtonText("Sample Select");
    sampleSelectButton.addListener(this);

    // AudioDeviceManagerƒNƒ‰ƒX‚ÌƒCƒ“ƒXƒ^ƒ“ƒX"deviceManager"‚É midiCollector‚ÌŽQÆ‚ð“o˜^‚·‚é
    // deviceManager‚ÍAŠî’êƒNƒ‰ƒX"AudioAppComponent"‚Å’è‹`‚³‚ê‚Ä‚¢‚é
    deviceManager.addMidiInputCallback(String(), &midiCollector);

    setSize(800, 600);

    // specify the number of input and output channels that we want to open
    setAudioChannels(2, 2);
}
MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

    //////////////////////////////////////////
    midiCollector.reset(sampleRate);

    synth.setCurrentPlaybackSampleRate(sampleRate);
    //////////////////////////////////////////
}
void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();

    ////////////
    // fill a midi buffer with incoming messages from the midi input.
    MidiBuffer incomingMidi;

    midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);

    keyboardState.processNextMidiBuffer(incomingMidi, 0, bufferToFill.numSamples, true);

    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
}
void MainComponent::showDeviceSetting()
{
    juce::AudioDeviceSelectorComponent selector(deviceManager,
        0, 256,
        0, 256,
        true, true,
        true, false);
    selector.setSize(400, 600);

    juce::DialogWindow::LaunchOptions dialog;
    dialog.content.setNonOwned(&selector);
    dialog.dialogTitle = "Audio/MIDI Device Settings";
    dialog.componentToCentreAround = this;
    dialog.dialogBackgroundColour = getLookAndFeel().findColour(ResizableWindow::backgroundColourId);
    dialog.escapeKeyTriggersCloseButton = true;
    dialog.useNativeTitleBar = false;
    dialog.resizable = false;
    dialog.useBottomRightCornerResizer = false;
    dialog.launchAsync();
}
void MainComponent::buttonClicked(Button* button)
{
    if (button == &deviceSettingButton)
        showDeviceSetting();
    else if (button == &sampleSelectButton)
        setupPcmSynth();
}
void MainComponent::resized()
{
    deviceSettingButton.setBoundsRelative(0.2, 0.2, 0.2, 0.2);

    sampleSelectButton.setBoundsRelative(0.6, 0.2, 0.2, 0.2);

    keyboardComponent->setBoundsRelative(0.0, 0.7, 1.0, 0.3);
}


void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::setupPcmSynth()
{
    AudioFormatManager formatManager;         // ƒI[ƒfƒBƒIƒtƒH[ƒ}ƒbƒg‚ðŽæ‚èˆµ‚¤ƒNƒ‰ƒX

    formatManager.registerBasicFormats();     // ƒTƒ|[ƒg‚·‚éƒtƒ@ƒCƒ‹ƒtƒH[ƒ}ƒbƒg‚ð“o˜^(JUCE•W€‘Î‰ž)

    /*
    // ƒtƒ@ƒCƒ‹‘I‘ð—p‚Ìƒ_ƒCƒAƒƒOƒEƒCƒ“ƒhƒE‚ÌÝ’è
    FileChooser chooser("Select a audio file to play...",
        File(), formatManager.getWildcardForAllFormats());
    
    Component po = FiComponent();
    // ƒtƒ@ƒCƒ‹‘I‘ðƒ_ƒCƒAƒƒOƒEƒCƒ“ƒhƒE‚ªŠJ‚­
    // ƒtƒ@ƒCƒ‹‚ðŠJ‚­‚Ì‚ª¬Œ÷‚µ‚½‚çif•¶“à‚Ìˆ—‚ªŽÀs‚³‚ê‚é
    if (chooser.browseForFileToOpen(component))
    */

    if (existFile("test.wav"))
    {
        WavAudioFormat wavFormat;

        AudioFormatReader* audioReader
            = wavFormat.createReaderFor(new MemoryInputStream(TestResource::file_wav,
                TestResource::file_wavSize,
                false),
                true);


        // ƒVƒ“ƒZƒCƒTƒCƒU[‚ªŽ‚ÂŒ®”Õ‚Ì”ÍˆÍ‚ðŒˆ‚ß‚éiBigIntegerŒ^‚Å’è‹`‚·‚é 00011100011...j
        BigInteger allNotes;
        allNotes.setRange(0, 128, true);  // MIDIƒm[ƒgNo.0`127‚ª—LŒø”ÍˆÍ‚Æ‚µ‚Ä‰ðŽß‚³‚ê‚é

        // ˆÈ‘O‚ÌSamplerVoiceƒCƒ“ƒXƒ^ƒ“ƒX‚ÆSamplerSoundƒCƒ“ƒXƒ^ƒ“ƒX‚ðÁ‹Ž‚·‚é
        synth.clearVoices();
        synth.clearSounds();


        // SamplerVoiceƒCƒ“ƒXƒ^ƒ“ƒX‚ð¶¬ASynthesiser‚É“o˜^‚·‚é

        //synth.addVoice(new SamplerVoice());       // Monophonic

        for (int i = 0; i < 128; i++) {             // Polyphonic
            synth.addVoice(new SamplerVoice());
        }

        // SamplerSoundƒCƒ“ƒXƒ^ƒ“ƒX‚ð¶¬ASynthesiser‚É‘Î‰ž•t‚¯‚é
        // ˆø” = 1:ƒpƒbƒ`–¼, 2:ƒtƒ@ƒCƒ‹ƒXƒgƒŠ[ƒ€, 3:Œ®”Õ‚Ì—LŒø”ÍˆÍ, 4:ƒ‹[ƒgƒL[‚Ìƒm[ƒgƒiƒ“ƒo[iŠî€‚Æ‚È‚éƒsƒbƒ`j, 
        //        5:ƒAƒ^ƒbƒNƒ^ƒCƒ€i•b’PˆÊj, 6:ƒŠƒŠ[ƒXƒ^ƒCƒ€i•b’PˆÊj, 7:ƒI[ƒfƒBƒIƒ\[ƒX‚©‚ç“Ç‚ÝŽæ‚éÅ‘å’·i•b’PˆÊj 
        synth.addSound(new SamplerSound("default", *audioReader, allNotes, 60, 0, 0.1, 10.0));
    }

}

int MainComponent::existFile(const char* path)
{
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        return 0;
    }

    fclose(fp);
    return 1;
}