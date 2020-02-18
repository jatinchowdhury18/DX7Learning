/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "JuceHeader.h"
#include "../../shared_funcs.h"
#include "../../dexed/Source/PluginProcessor.h"

void setRandomDexedParameters (DexedAudioProcessor& dexed, Array<File>& carts, Random& rand);
void getParams (DexedAudioProcessor& dexed);

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

    // create parameters file
    File paramsFile (File::getCurrentWorkingDirectory().getChildFile ("params.csv"));
    if (args.containsOption ("--paramfile"))
        paramsFile = args.getFileForOption ("--paramfile");
    paramsFile.deleteFile();
    paramsFile.create();

    // Set up buffer  
    const float sampleRate = 16000.0f;
    const float lengthSeconds = args.containsOption ("--length") ?
        args.getValueForOption ("--length").getFloatValue() : 2.0f;
    std::cout << lengthSeconds << std::endl;
    const int numChannels = 2;
    const int lengthSamples = int (sampleRate * lengthSeconds);
    
    AudioBuffer<float> buffer (numChannels, lengthSamples);
    
    // MIDI buffer
    MidiBuffer midi;
    getMidiBuffer (midi, lengthSamples);

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

        // write params
        std::stringstream ss;
        const int numCtrls = dexed.ctrl.size();
        for (int k = 0; k < numCtrls; ++k)
        {
            ss << dexed.ctrl[k]->getValueHost();
            if (k != numCtrls-1)
                ss << ',';
            else
                ss << '\n';
        }
        
        paramsFile.appendText (ss.str());

        if (result > 0)
            break;
    }

    // getParams (dexed);

    return result;
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
