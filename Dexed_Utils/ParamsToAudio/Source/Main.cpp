/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "JuceHeader.h"
#include "../../shared_funcs.h"
#include "../../dexed/Source/PluginProcessor.h"

void setDexedParameters (DexedAudioProcessor& dexed, String paramsString);
void getParams (DexedAudioProcessor& dexed);

int main (int argc, char* argv[])
{
    ScopedJuceInitialiser_GUI scopedJuce; // initialise JUCE (otherwise will fail on ASSERT_JUCE_MESSAGE_MANAGER_EXISTS)
    ScopedFormatManager formatManager; // audio format manager

    DexedAudioProcessor dexed; // create instance of dexed
    ArgumentList args (argc, argv); // Argument list
    int result = 0; // executable result

    // Create output directory
    File outDir (File::getCurrentWorkingDirectory().getChildFile ("testDir"));
    if (args.containsOption ("--outdir"))
        outDir = args.getFileForOption ("--outdir");
    outDir.deleteRecursively();
    outDir.createDirectory();

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

    File paramFile;
    if (args.containsOption ("--paramfile"))
        paramFile = args.getFileForOption ("--paramfile");

    if (! paramFile.existsAsFile())
    {
        std::cout << "Invalid parameter file" << std::endl;
    }

    StringArray paramMatrix;
    paramFile.readLines (paramMatrix);

    for (int i = 0; i < paramMatrix.size(); ++i)
    {
        if (paramMatrix[i].isEmpty())
            continue;

        // create output file
        File outFile (outDir.getChildFile ("sample_" + String (i) + ".wav"));
        outFile.deleteFile();
        outFile.create();
        
        // Set parameters
        setDexedParameters (dexed, paramMatrix[i]);
        
        // synthesize buffer
        buffer.clear();
        dexed.prepareToPlay ((double) sampleRate, lengthSamples);
        dexed.processBlock (buffer, midi);
        dexed.releaseResources();
        
        // write to file
        result = writeBufferToFile (formatManager, outFile, buffer, (double) sampleRate, lengthSamples);

        if (result > 0)
            break;
    }

    getParams (dexed);

    return result;
}

void getParams (DexedAudioProcessor& dexed)
{
    for (auto* ctrl : dexed.ctrl)
        std::cout << ctrl->label << ": " << ctrl->getValueHost() << std::endl;
}

void setDexedParameters (DexedAudioProcessor& dexed, String paramsString)
{
    StringArray params;
    params.addTokens (paramsString, ",", "");

    int paramIdx = 0;
    for (auto* ctrl : dexed.ctrl)
    {
        if (paramIdx >= params.size())
        {
            std::cout << "Not enough parameters" << std::endl;
            break;
        }

        ctrl->setValueHost (params[paramIdx].getFloatValue());
        paramIdx++;
    }
}
