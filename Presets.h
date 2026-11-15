#pragma once
#include <JuceHeader.h>
#include <array>
namespace SkiivPresets {
struct FactoryPreset { juce::String name, category; std::array<int,5> macros; float bpm; };
const std::vector<FactoryPreset>& all();
const std::array<juce::StringArray,5>& effectNames();
}
