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
    ScopedFormatManager() { registerBasicFormats(); }
    ~ScopedFormatManager() { clearFormats(); }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopedFormatManager)    
};

int writeBufferToFile (ScopedFormatManager& formatManager, File& outFile,
                       const AudioBuffer<float>& buffer, double sampleRate, int lengthSamples);
void setRandomDexedParameters (DexedAudioProcessor& dexed, Array<File>& carts, Random& rand);
void randomMidiBuffer (MidiBuffer& midi, int lengthSamples, Random& rand);

int main (int argc, char* argv[])
{
    ScopedJuceInitialiser_GUI scopedJuce; // initialise JUCE (otherwise will fail on ASSERT_JUCE_MESSAGE_MANAGER_EXISTS)
    ScopedFormatManager formatManager; // audio format manager

    DexedAudioProcessor dexed; // create instance of dexed
    ArgumentList args (argc, argv); // Argument list
    Random rand; // random number generator
    int result = 0; // executable result

    // Create output directory
    File outDir (File::getCurrentWorkingDirectory().getChildFile ("testDir"));
    if (args.containsOption ("--outdir"))
        outDir = args.getFileForOption ("--outdir");
    outDir.deleteRecursively();
    outDir.createDirectory();

    // Set up buffer  
    const float sampleRate = 48000.0f;
    const float lengthSeconds = args.containsOption ("--length") ?
        args.getValueForOption ("--length").getFloatValue() : 2.0f;
    std::cout << lengthSeconds << std::endl;
    const int numChannels = 2;
    const int lengthSamples = int (sampleRate * lengthSeconds);
    
    AudioBuffer<float> buffer (numChannels, lengthSamples);
    
    // MIDI buffer
    MidiBuffer midi;
    randomMidiBuffer (midi, lengthSamples, rand);

    // How many files to create
    int num = args.containsOption ("--num") ?
        jmax (0, args.getValueForOption ("--num").getIntValue()) : 1;

    // Set up cartridges
    auto carts = dexed.dexedCartDir.findChildFiles (File::TypesOfFileToFind::findFiles, true, "*.syx");
    std::cout << "Num available programs: " << carts.size()*32 << std::endl;

    for (int i = 0; i < num; ++i)
    {
        // create output file
        File outFile (outDir.getChildFile ("sample_" + String (i) + ".wav"));
        outFile.deleteFile();
        outFile.create();
        
        // Set parameters
        setRandomDexedParameters (dexed, carts, rand);
        
        // synthesize buffer
        buffer.clear();
        dexed.prepareToPlay ((double) sampleRate, lengthSamples);
        dexed.processBlock (buffer, midi);
        dexed.releaseResources();

        // check for empty buffer
        auto magDB = Decibels::gainToDecibels (buffer.getMagnitude (0, lengthSamples));
        if (magDB < -60.0f)
        {
            std::cout << "Silent buffer, running again..." << std::endl;
            --i;
        }
        
        // write to file
        result = writeBufferToFile (formatManager, outFile, buffer, (double) sampleRate, lengthSamples);

        if (result > 0)
            break;
    }

    return result;
}

void randomMidiBuffer (MidiBuffer& midi, int lengthSamples, Random& rand)
{
    const int midiChannel = 1;

    int curSample = 0;
    bool turnOn = true;
    int noteNum = 0;
    while (curSample < lengthSamples)
    {
        if (turnOn)
        {
            noteNum = rand.nextInt (Range<int> (24, 96)); // choose note between C2 and C7
            midi.addEvent (MidiMessage::noteOn  (midiChannel, noteNum, 1.0f), curSample);
        }
        else
        {
            midi.addEvent (MidiMessage::noteOff (midiChannel, noteNum), curSample);
        }

        turnOn = ! turnOn;
        curSample += rand.nextInt (Range<int> (1000, 40000));
    }
}

void getParams (DexedAudioProcessor& dexed)
{
    for (auto* ctrl : dexed.ctrl)
        std::cout << ctrl->label << ": " << ctrl->getValueHost() << std::endl;
}

void setRandomDexedParameters (DexedAudioProcessor& dexed, Array<File>& carts, Random& rand)
{
    // load random cartridge
    Cartridge thisCart;
    thisCart.load (carts[rand.nextInt (carts.size())]);
    dexed.loadCartridge (thisCart);

    // load random program
    int prog = rand.nextInt (32);
    dexed.setCurrentProgram (prog);
    std::cout << "Program: " << dexed.getProgramName (prog) << std::endl;

    // getParams (dexed);
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
