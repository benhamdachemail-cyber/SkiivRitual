#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <memory>
class SkiivRitualSoundsAudioProcessorEditor : public juce::AudioProcessorEditor,
     private juce::ComboBox::Listener,private juce::Button::Listener,private juce::Timer {
public:
  explicit SkiivRitualSoundsAudioProcessorEditor(SkiivRitualSoundsAudioProcessor&);
  ~SkiivRitualSoundsAudioProcessorEditor() override;
  void paint(juce::Graphics&) override;
  void resized() override;
private:
  class GeometricLook : public juce::LookAndFeel_V4 {
  public:
    void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&) override;
  } look;
  void comboBoxChanged(juce::ComboBox*) override;
  void buttonClicked(juce::Button*) override;
  void timerCallback() override;
  SkiivRitualSoundsAudioProcessor& proc;
  juce::ComboBox masters,category,division;
  std::array<juce::ComboBox,5> selects;
  std::array<juce::Slider,5> knobs;
  std::array<juce::ToggleButton,5> toggles;
  std::array<juce::Label,5> knobNames;
  std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>,5> knobLinks;
  std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>,5> toggleLinks;
  std::array<juce::Slider,4> quick;
  std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>,4> quickLinks;
  juce::ToggleButton hostSync;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> hostLink;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> divisionLink;
  juce::Slider manualTempo;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tempoLink;
  juce::Label title,subtitle,tempoLabel,help,preview;
  juce::TextButton previous {"<"}, next {">"};
  int selectedMaster=0;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SkiivRitualSoundsAudioProcessorEditor)
};
