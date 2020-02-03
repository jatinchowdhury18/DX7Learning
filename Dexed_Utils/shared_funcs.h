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
    const static int numNotes = 5;
    const int noteNums[numNotes] = {36, 48, 55, 64, 72};
    const int lengths[numNotes] = {3000, 1000, 18000, 2000, 3000};
    const int offLength = 5000;

    int curSample = 0;
    int noteIdx = 0;
    bool turnOn = true;
    while (curSample < lengthSamples)
    {
        if (turnOn)
        {
            midi.addEvent (MidiMessage::noteOn (midiChannel, noteNums[noteIdx], 1.0f), curSample);
            curSample += lengths[noteIdx] / 3;
        }
        else
        {
            midi.addEvent (MidiMessage::noteOff (midiChannel, noteNums[noteIdx]), curSample);
            curSample += offLength;
            noteIdx++;
        }

        if (noteIdx >= numNotes)
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
