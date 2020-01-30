#ifndef SHAREDFUNCS_H_INCLUDED
#define SHAREDFUNCS_H_INCLUDED

#include "JuceHeader.h"

class ScopedFormatManager : public AudioFormatManager
{
public:
    ScopedFormatManager() { registerBasicFormats(); }
    ~ScopedFormatManager() { clearFormats(); }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopedFormatManager)    
};

void getMidiBuffer (MidiBuffer& midi, int lengthSamples)
{
    midi.clear();

    const int midiChannel = 1;
    const int noteNums[7] = {36, 42, 48, 54, 60, 66, 72};
    const int lengths[7] = {10000, 3000, 50000, 5000, 8000, 70000, 6000};
    const int offLength = 30000;

    int curSample = 0;
    int noteIdx = 0;
    bool turnOn = true;
    while (curSample < lengthSamples)
    {
        if (turnOn)
        {
            midi.addEvent (MidiMessage::noteOn (midiChannel, noteNums[noteIdx], 1.0f), curSample);
            curSample += lengths[noteIdx];
        }
        else
        {
            midi.addEvent (MidiMessage::noteOff (midiChannel, noteNums[noteIdx]), curSample);
            curSample += offLength;
            noteIdx++;
        }

        if (noteIdx >= 7)
            break;

        turnOn = ! turnOn;
    }
}

int writeBufferToFile (ScopedFormatManager& formatManager, File& outFile,
                       const AudioBuffer<float>& buffer, double sampleRate, int lengthSamples)
{
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

#endif //SHAREDFUNCS_H_INCLUDED
