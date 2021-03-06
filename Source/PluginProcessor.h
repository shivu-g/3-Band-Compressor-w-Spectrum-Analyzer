/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// helpful for auto-completion and eliminate chances of writing error
namespace Params
{
    enum Names 
    {
        Low_Mid_Crossover_Freq, 
        Mid_High_Crossover_Freq,

        Attack_Low_Band,
        Attack_Mid_Band,
        Attack_High_Band,

        Threshold_Low_Band,
        Threshold_Mid_Band,
        Threshold_High_Band,

        Release_Low_Band,
        Release_Mid_Band,
        Release_High_Band,

        Ratio_Low_Band,
        Ratio_Mid_Band,
        Ratio_High_Band,
        
        Bypassed_Low_Band,
        Bypassed_Mid_Band,
        Bypassed_High_Band,
    };

    inline const std::map <Names, juce::String>& GetParams()
    {
        static std::map<Names, juce::String> params = 
        {
            {Low_Mid_Crossover_Freq, "Low-Mid Crossover Freq"},
            {Mid_High_Crossover_Freq, "Mid-High Crossover Freq"},

            {Attack_Low_Band, "Attack Low Band"},
            {Attack_Mid_Band, "Attack Mid Band"},
            {Attack_High_Band, "Attack High Band"},

            {Threshold_Low_Band, "Threshold Low Band"},
            {Threshold_Mid_Band, "Threshold Mid Band"},
            {Threshold_High_Band, "Threshold High Band"},

            {Release_Low_Band, "Release Low Band" },
            {Release_Mid_Band, "Release Low Band"},
            {Release_High_Band, "Release Low Band"},

            {Ratio_Low_Band, "Ratio Low Band"},
            {Ratio_Mid_Band, "Ratio Mid Band"},
            {Ratio_High_Band, "Ratio High Band"},

            {Bypassed_Low_Band, "Bypassed Low Band"},
            {Bypassed_Mid_Band, "Bypassed Mid Band"},
            {Bypassed_High_Band, "Bypassed High Band"},

        };

        return params;
    }
}
struct CompressorBand
{
    juce::AudioParameterFloat* attack{ nullptr };
    juce::AudioParameterFloat* release{ nullptr };
    juce::AudioParameterFloat* threshold{ nullptr };
    juce::AudioParameterChoice* ratio{ nullptr };
    juce::AudioParameterBool* bypassed{ nullptr };

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        compressor.prepare(spec);
    }

    void updateCompressorSettings()
    {
        compressor.setAttack(attack->get());
        compressor.setRelease(release->get());
        compressor.setThreshold(threshold->get());
        compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);

        context.isBypassed = bypassed->get();

        compressor.process(context);
    }

private:
    juce::dsp::Compressor<float> compressor;
};

//==============================================================================
/**
*/
class SimpleMBCompAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SimpleMBCompAudioProcessor();
    ~SimpleMBCompAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //using alias for easement
    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();

    APVTS apvts{ *this, nullptr, "Parameters", createParameterLayout() };


private:

    std::array<CompressorBand, 3> compressors;
    CompressorBand& lowBandComp = compressors[0];
    CompressorBand& midBandComp = compressors[1];
    CompressorBand& highBandComp = compressors[2];

    using Filter = juce::dsp::LinkwitzRileyFilter<float>;

    //      fc0     fc1    -- filterCrossover
    Filter  LP1, AP2,
            HP1, LP2,
                 HP2;

    /*Filter invAP1, invAP2;
    juce::AudioBuffer<float> invAPBuffer;*/

    juce::AudioParameterFloat* lowMidCrossover{ nullptr };
    juce::AudioParameterFloat* midHighCrossover{ nullptr };

    std::array<juce::AudioBuffer<float>, 3> filterBuffers;



    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessor)
};
