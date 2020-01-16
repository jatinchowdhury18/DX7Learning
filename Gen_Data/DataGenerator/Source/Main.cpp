/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "JuceHeader.h"
#include "../../dexed/Source/PluginProcessor.h"

class ScopedFormatManager : public AudioFormatManager
{
public:
    ScopedFormatManager()
    {
        registerBasicFormats();
    }

    ~ScopedFormatManager()
    {
        clearFormats();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopedFormatManager)    
};

int main (int argc, char* argv[])
{
    ScopedJuceInitialiser_GUI scopedJuce; // initialise JUCE (otherwise will fail on ASSERT_JUCE_MESSAGE_MANAGER_EXISTS)
    ScopedFormatManager formatManager; // audio format manager

    DexedAudioProcessor dexed; // create instance of dexed

    // create output file
    File outFile (File::getCurrentWorkingDirectory().getChildFile ("test.wav"));
    outFile.deleteFile();
    outFile.create();

    // Set up buffer
    const float sampleRate = 48000.0f;
    const float lengthSeconds = 2.0f;
    const int numChannels = 2;
    const int lengthSamples = int (sampleRate * lengthSeconds);

    AudioBuffer<float> buffer (numChannels, lengthSamples);
    
    int noteNum = 60; //  C3
    int midiChannel = 1;
    MidiBuffer midi;
    midi.addEvent (MidiMessage::noteOn  (midiChannel, noteNum, 0.5f), 0);
    midi.addEvent (MidiMessage::noteOff (midiChannel, noteNum), int ((float) lengthSamples * 0.75f));

    // synthesize buffer
    dexed.prepareToPlay ((double) sampleRate, lengthSamples);
    dexed.processBlock (buffer, midi);
    dexed.releaseResources();

    // write to file
    std::unique_ptr<AudioFormatWriter> writer (formatManager.findFormatForFileExtension ("wav")->createWriterFor 
        (outFile.createOutputStream(), (double) sampleRate, AudioChannelSet::stereo(), 16, StringPairArray(), 0));

    if (writer.get() == nullptr)
    {
        std::cout << "Unable to write audio to file" << std::endl;
        return 1;
    }

    writer->flush();
    writer->writeFromAudioSampleBuffer (buffer, 0, lengthSamples);

    return 0;
}
