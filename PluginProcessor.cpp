#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
SkiivRitualSoundsAudioProcessor::SkiivRitualSoundsAudioProcessor()
  : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(),true)
                                      .withOutput("Output",juce::AudioChannelSet::stereo(),true)),
    state(*this,nullptr,"SKIIV_PARAMETERS",makeLayout()) {}
juce::AudioProcessorValueTreeState::ParameterLayout SkiivRitualSoundsAudioProcessor::makeLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;
  for (const auto* id : {"delay","chorus","reverse","space","fracture"})
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{id,1},juce::String(id).toUpperCase()+" preset",1,20,1));
  for (const auto* id : {"delayOn","chorusOn","reverseOn","spaceOn","fractureOn","hostSync"})
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{id,1},juce::String(id),true));
  for (const auto* id : {"mix","feedback","width","tone"})
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id,1},juce::String(id).toUpperCase(),juce::NormalisableRange<float>(0.f,1.f),.5f));
  layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"division",1},"Sync subdivision",juce::StringArray{"1/4","1/8","1/16","1/32"},1));
  layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"manualBpm",1},"Manual BPM",juce::NormalisableRange<float>(60.f,200.f,1.f),120.f));
  return layout;
}
bool SkiivRitualSoundsAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const {
  return l.getMainInputChannelSet()==juce::AudioChannelSet::stereo() && l.getMainOutputChannelSet()==juce::AudioChannelSet::stereo();
}
void SkiivRitualSoundsAudioProcessor::prepareToPlay(double sampleRate,int) {
  sr=sampleRate;
  const size_t length=static_cast<size_t>(std::ceil(sr*5.0));
  for(auto* pair : {&delayBuffer,&chorusBuffer,&reverseBuffer,&grainBuffer})
    for(auto& b : *pair) b.assign(length,0.f);
  delayPos=chorusPos=reversePos=grainPos=0;chorusPhase=toneL=toneR=0.f;
  reverb.setSampleRate(sr);reverb.reset();
}
void SkiivRitualSoundsAudioProcessor::releaseResources() {}
void SkiivRitualSoundsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,juce::MidiBuffer&) {
  juce::ScopedNoDenormals noDenormals;
  if (buffer.getNumChannels()<2) return;
  bool hasTempo=false;float tempo=parameter("manualBpm");
  if (parameter("hostSync")>=.5f) {
    if(auto* ph=getPlayHead()) if(auto pos=ph->getPosition()) if(auto hostBpm=pos->getBpm())
      if(std::isfinite(*hostBpm) && *hostBpm>0.0) {tempo=(float)*hostBpm;hasTempo=true;}
  }
  hostTempoAvailable.store(hasTempo);detectedHostBpm.store(tempo);
  juce::Reverb::Parameters rp;
  const float space=parameter("space")/20.f;
  rp.roomSize=juce::jlimit(0.f,1.f,.25f+.7f*space);
  rp.damping=.48f;rp.wetLevel=1.f;rp.dryLevel=0.f;
  rp.width=parameter("width");rp.freezeMode=0.f;
  reverb.setParameters(rp);
  auto* left=buffer.getWritePointer(0);auto* right=buffer.getWritePointer(1);
  for(int i=0;i<buffer.getNumSamples();++i) {
    float outL=left[i],outR=right[i];
    processSample(left[i],right[i],outL,outR,tempo);
    left[i]=outL;right[i]=outR;
  }
}
void SkiivRitualSoundsAudioProcessor::processSample(float inL,float inR,float& outL,float& outR,float bpm) {
  const auto delayN=delayBuffer[0].size(); if(delayN==0) {outL=inL;outR=inR;return;}
  const float mix=parameter("mix"), feedbackValue=juce::jlimit(0.f,.89f,parameter("feedback")*.76f+.08f);
  const float chorusWidth=parameter("width"), tone=parameter("tone");
  const int d=(int)parameter("delay"),c=(int)parameter("chorus"),r=(int)parameter("reverse"),s=(int)parameter("space"),g=(int)parameter("fracture");
  const float seconds=60.f/juce::jmax(30.f,bpm);
  const float divisions[]={1.f,.5f,.25f,.125f};
  int index=juce::jlimit(0,3,(int)parameter("division"));
  int timeSamples=juce::jlimit(1,(int)delayN-1,(int)(seconds*divisions[index]*(0.55f+(float)d/20.f)*sr));
  float wetL=0.f,wetR=0.f;
  for(int ch=0;ch<2;ch++) {
    float inputSample=ch==0?inL:inR;
    auto& db=delayBuffer[(size_t)ch];
    auto& cb=chorusBuffer[(size_t)ch];
    auto& rb=reverseBuffer[(size_t)ch];
    auto& gb=grainBuffer[(size_t)ch];
    int delayRead=(delayPos-timeSamples+(int)delayN)%(int)delayN;
    float delayed=db[(size_t)delayRead];
    db[(size_t)delayPos]=inputSample+std::tanh(delayed*feedbackValue)*.95f;
    if(parameter("delayOn")>.5f) (ch==0?wetL:wetR)+=delayed*(.14f+.40f*d/20.f);
    // Stereo chorus with a slowly moving delay tap
    cb[(size_t)chorusPos]=inputSample;
    float speed=.06f+.6f*c/20.f;
    float phase=chorusPhase+(ch==0?0.f:3.1415926f);
    int offset=(int)((.012f+.009f*(1.f+std::sin(phase))*(.3f+chorusWidth)) * sr);
    int cr=(chorusPos-offset+(int)delayN)%(int)delayN;
    if(parameter("chorusOn")>.5f) (ch==0?wetL:wetR)+=cb[(size_t)cr]*(.17f+.29f*c/20.f);
    // Reverse segment reading from a continually recorded circular buffer
    rb[(size_t)reversePos]=inputSample;
    int segment=juce::jlimit(256,(int)delayN/2,(int)(seconds*(.25f+.75f*r/20.f)*sr));
    int relative=reversePos%segment;
    int reverseRead=(reversePos-2*relative-1+(int)delayN*2)%(int)delayN;
    float envelope=std::sin(juce::MathConstants<float>::pi*(float)relative/(float)segment);
    if(parameter("reverseOn")>.5f) (ch==0?wetL:wetR)+=rb[(size_t)reverseRead]*envelope*(.15f+.44f*r/20.f);
    // Grain repeat/re-pitch by intermittent circular-buffer read jumps
    gb[(size_t)grainPos]=inputSample;
    int grainSize=juce::jlimit(70,(int)delayN/4,(int)(sr*(.025f+.15f*(1.f-g/21.f))));
    int phaseIndex=grainPos%grainSize;
    int oldSample=(grainPos-grainSize+(int)delayN)%(int)delayN;
    int read=(oldSample+(phaseIndex*(1+g/7))%grainSize)%(int)delayN;
    float grainWindow=std::sin(juce::MathConstants<float>::pi*(float)phaseIndex/(float)grainSize);
    if(parameter("fractureOn")>.5f) (ch==0?wetL:wetR)+=gb[(size_t)read]*grainWindow*(.12f+.39f*g/20.f);
  }
  chorusPhase+=2.f*juce::MathConstants<float>::pi*(.06f+.6f*c/20.f)/(float)sr;
  if(chorusPhase>juce::MathConstants<float>::twoPi) chorusPhase-=juce::MathConstants<float>::twoPi;
  delayPos=(delayPos+1)%(int)delayN;chorusPos=(chorusPos+1)%(int)delayN;
  reversePos=(reversePos+1)%(int)delayN;grainPos=(grainPos+1)%(int)delayN;
  // Stereo reverb; quality can be refined after first FL Studio listening test
  if(parameter("spaceOn")>.5f) {
    float rvL=inL,rvR=inR;
    reverb.processStereo(&rvL,&rvR,1);
    wetL+=rvL*(.10f+.42f*s/20.f);wetR+=rvR*(.10f+.42f*s/20.f);
  }
  float l=inL*(1.f-mix*.35f)+wetL*mix;
  float rr=inR*(1.f-mix*.35f)+wetR*mix;
  float a=juce::jlimit(.001f,.95f,.07f+.85f*tone);
  toneL+=a*(l-toneL);toneR+=a*(rr-toneR);
  outL=std::tanh(toneL*.85f);outR=std::tanh(toneR*.85f);
}
void SkiivRitualSoundsAudioProcessor::getStateInformation(juce::MemoryBlock& dest) {
  auto copy=state.copyState();std::unique_ptr<juce::XmlElement> xml(copy.createXml());copyXmlToBinary(*xml,dest);
}
void SkiivRitualSoundsAudioProcessor::setStateInformation(const void* bytes,int size) {
  if(auto xml=getXmlFromBinary(bytes,size)) if(xml->hasTagName(state.state.getType()))
    state.replaceState(juce::ValueTree::fromXml(*xml));
}
void SkiivRitualSoundsAudioProcessor::loadMasterPreset(int index) {
  const auto& all=SkiivPresets::all();if(index<0||index>=(int)all.size())return;
  const auto& p=all[(size_t)index];
  const char* ids[]={"delay","chorus","reverse","space","fracture"};
  for(int i=0;i<5;i++) if(auto* param=state.getParameter(ids[i]))
    param->setValueNotifyingHost(param->convertTo0to1((float)p.macros[(size_t)i]));
}
juce::AudioProcessorEditor* SkiivRitualSoundsAudioProcessor::createEditor() {return new SkiivRitualSoundsAudioProcessorEditor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new SkiivRitualSoundsAudioProcessor();}
