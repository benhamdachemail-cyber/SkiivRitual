#pragma once
#include <JuceHeader.h>
#include "Presets.h"
#include <array>
#include <vector>
class SkiivRitualSoundsAudioProcessor : public juce::AudioProcessor {
public:
  SkiivRitualSoundsAudioProcessor();
  ~SkiivRitualSoundsAudioProcessor() override = default;
  void prepareToPlay(double, int) override;
  void releaseResources() override;
  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override { return true; }
  const juce::String getName() const override { return "Skiiv Ritual Sounds"; }
  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 8.0; }
  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const juce::String getProgramName(int) override { return {}; }
  void changeProgramName(int,const juce::String&) override {}
  void getStateInformation(juce::MemoryBlock&) override;
  void setStateInformation(const void*,int) override;
  bool isBusesLayoutSupported(const BusesLayout&) const override;
  void loadMasterPreset(int);
  juce::AudioProcessorValueTreeState state;
  std::atomic<float> detectedHostBpm { 120.f };
  std::atomic<bool> hostTempoAvailable { false };
private:
  static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
  float parameter(const char* id) const { return state.getRawParameterValue(id)->load(); }
  void processSample(float inL,float inR,float& outL,float& outR,float bpm);
  std::array<std::vector<float>,2> delayBuffer, chorusBuffer, reverseBuffer, grainBuffer;
  int delayPos=0, chorusPos=0, reversePos=0, grainPos=0;
  double sr=44100.0;
  float chorusPhase=0.f, toneL=0.f,toneR=0.f;
  juce::Reverb reverb;
  std::array<float,2> reverbL {0,0};
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SkiivRitualSoundsAudioProcessor)
};
