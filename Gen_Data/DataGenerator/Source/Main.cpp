/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../dexed/Source/PluginProcessor.h"

//==============================================================================
int main (int argc, char* argv[])
{
    ScopedJuceInitialiser_GUI scopedJuce; // initialise JUCE (otherwise will fail on ASSERT_JUCE_MESSAGE_MANAGER_EXISTS)

    DexedAudioProcessor dexed; // create instance of dexed

    // test accessing dexed function
    auto mono = dexed.isMonoMode();
    if (mono)
        std::cout << "Mono" << std::endl;
    else
        std::cout << "Stereo" << std::endl;

    return 0;
}
