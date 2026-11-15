#include "PluginEditor.h"
#include "Presets.h"
#include <cmath>
namespace {
const juce::Colour red (0xfff0444f), bg(0xff10090f), panel(0xff1e1018), soft(0xffffc5c8);
const char* paramIds[]={"delay","chorus","reverse","space","fracture"};
const char* onIds[]={"delayOn","chorusOn","reverseOn","spaceOn","fractureOn"};
const char* quickIds[]={"mix","feedback","width","tone"};
const char* quickLabels[]={"MIX","FEEDBACK","WIDTH","TONE"};
}
void SkiivRitualSoundsAudioProcessorEditor::GeometricLook::drawRotarySlider(juce::Graphics& g,int x,int y,int w,int h,float value,float start,float end,juce::Slider&) {
 auto area=juce::Rectangle<float>((float)x,(float)y,(float)w,(float)h).reduced(11.f);
 float size=juce::jmin(area.getWidth(),area.getHeight());
 area=area.withSizeKeepingCentre(size,size);
 auto center=area.getCentre();float r=size*.5f;
 g.setColour(red.withAlpha(.85f));
 for(int j=0;j<21;j++) {
  float a=start+(end-start)*(float)j/20.f;
  juce::Point<float> a1(center.x+std::sin(a)*(r+2),center.y-std::cos(a)*(r+2));
  juce::Point<float> a2(center.x+std::sin(a)*(r+7),center.y-std::cos(a)*(r+7));
  g.drawLine({a1,a2},j%5==0?2.f:1.f);
 }
 g.setColour(juce::Colours::black);g.fillEllipse(area.expanded(2));
 g.setColour(red);g.fillEllipse(area);
 float theta=start+value*(end-start);
 juce::Path line;line.startNewSubPath(center);line.lineTo(center.x+std::sin(theta)*r*.83f,center.y-std::cos(theta)*r*.83f);
 g.setColour(bg);g.strokePath(line,juce::PathStrokeType(4.f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
 g.fillEllipse(center.x-3,center.y-3,6,6);
}
SkiivRitualSoundsAudioProcessorEditor::SkiivRitualSoundsAudioProcessorEditor(SkiivRitualSoundsAudioProcessor& p)
 : AudioProcessorEditor(&p),proc(p) {
 setSize(1030,690);setResizable(true,true);setResizeLimits(820,610,1500,1050);
 getLookAndFeel().setColour(juce::ComboBox::backgroundColourId,bg);
 getLookAndFeel().setColour(juce::ComboBox::textColourId,soft);
 getLookAndFeel().setColour(juce::ComboBox::outlineColourId,red);
 title.setText("SKIIV.",juce::dontSendNotification);title.setFont(juce::Font(juce::FontOptions().withHeight(38.f).withStyle("Bold")));title.setColour(juce::Label::textColourId,red);addAndMakeVisible(title);
 subtitle.setText("R I T U A L   S O U N D S",juce::dontSendNotification);subtitle.setColour(juce::Label::textColourId,red);subtitle.setFont(juce::Font(juce::FontOptions().withHeight(14.f).withStyle("Bold")));addAndMakeVisible(subtitle);
 masters.addListener(this);category.addListener(this);previous.addListener(this);next.addListener(this);
 category.addItem("ALL (100)",1);category.addItem("AMBIENT",2);category.addItem("DREAM",3);category.addItem("RHYTHM",4);category.addItem("GLITCH",5);category.addItem("CINEMATIC",6);category.setSelectedId(1,juce::dontSendNotification);
 addAndMakeVisible(category);addAndMakeVisible(masters);addAndMakeVisible(previous);addAndMakeVisible(next);
 for(int i=0;i<(int)SkiivPresets::all().size();i++) masters.addItem(juce::String(i+1).paddedLeft('0',3)+" - "+SkiivPresets::all()[(size_t)i].name,i+1);
 masters.setSelectedId(1,juce::dontSendNotification);
 for(int i=0;i<5;i++) {
  knobNames[i].setText(juce::String(paramIds[i]).toUpperCase(),juce::dontSendNotification);
  knobNames[i].setJustificationType(juce::Justification::centred);knobNames[i].setColour(juce::Label::textColourId,soft);addAndMakeVisible(knobNames[i]);
  knobs[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);knobs[i].setTextBoxStyle(juce::Slider::TextBoxBelow,false,64,23);
  knobs[i].setLookAndFeel(&look);knobs[i].setColour(juce::Slider::textBoxTextColourId,soft);knobs[i].setColour(juce::Slider::textBoxBackgroundColourId,bg);
  addAndMakeVisible(knobs[i]);knobLinks[i]=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.state,paramIds[i],knobs[i]);
  toggles[i].setButtonText("ON");toggles[i].setColour(juce::ToggleButton::tickColourId,red);addAndMakeVisible(toggles[i]);
  toggleLinks[i]=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(proc.state,onIds[i],toggles[i]);
  selects[i].addListener(this);
  const auto& names=SkiivPresets::effectNames()[(size_t)i];
  for(int j=0;j<names.size();j++) selects[i].addItem(names[j],j+1);
  selects[i].setSelectedId(1,juce::dontSendNotification);addAndMakeVisible(selects[i]);
  knobs[i].onValueChange=[this,i] {selects[i].setSelectedId(juce::roundToInt(knobs[i].getValue()),juce::dontSendNotification);};
 }
 for(int i=0;i<4;i++) {
   quick[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);quick[i].setTextBoxStyle(juce::Slider::TextBoxBelow,false,55,21);
   quick[i].setLookAndFeel(&look);addAndMakeVisible(quick[i]);
   quickLinks[i]=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.state,quickIds[i],quick[i]);
 }
 division.addItemList({"1/4","1/8","1/16","1/32"},1);addAndMakeVisible(division);
 divisionLink=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(proc.state,"division",division);
 hostSync.setButtonText("FL STUDIO HOST SYNC");hostSync.setColour(juce::ToggleButton::tickColourId,red);addAndMakeVisible(hostSync);
 hostLink=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(proc.state,"hostSync",hostSync);
 manualTempo.setSliderStyle(juce::Slider::LinearHorizontal);manualTempo.setTextBoxStyle(juce::Slider::TextBoxRight,false,60,22);addAndMakeVisible(manualTempo);
 tempoLink=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.state,"manualBpm",manualTempo);
 tempoLabel.setColour(juce::Label::textColourId,soft);addAndMakeVisible(tempoLabel);
 help.setText("100 MASTER PRESETS  /  20 PER EFFECT",juce::dontSendNotification);help.setColour(juce::Label::textColourId,red);addAndMakeVisible(help);
 preview.setColour(juce::Label::textColourId,soft);preview.setJustificationType(juce::Justification::topLeft);addAndMakeVisible(preview);
 preview.setText("SELECT A MASTER PRESET TO LOAD ITS FIVE EFFECT SETTINGS",juce::dontSendNotification);
 startTimerHz(5);
}
SkiivRitualSoundsAudioProcessorEditor::~SkiivRitualSoundsAudioProcessorEditor() {
 stopTimer();for(auto& k:knobs) k.setLookAndFeel(nullptr);for(auto& k:quick)k.setLookAndFeel(nullptr);
}
void SkiivRitualSoundsAudioProcessorEditor::paint(juce::Graphics& g) {
 g.fillAll(bg);auto bounds=getLocalBounds().toFloat();
 g.setColour(red);
 for(int x=6;x<(int)bounds.getWidth();x+=43) {g.drawLine((float)x,0.f,(float)x,10.f,1.4f);g.fillEllipse((float)x,12.f,3.f,3.f);}
 for(int y=14;y<(int)bounds.getHeight();y+=43) {g.drawLine(0.f,(float)y,10.f,(float)y,1.4f);g.fillEllipse(13.f,(float)y,3.f,3.f);}
 auto content=getLocalBounds().reduced(21);
 auto upper=juce::Rectangle<int>(content.getX(),content.getY()+96,content.getWidth(),243);
 for(int i=0;i<5;i++) {
  int column=upper.getX()+i*(upper.getWidth()+7)/5;
  auto r=juce::Rectangle<float>((float)column,(float)upper.getY(),(float)(upper.getWidth()-28)/5,(float)upper.getHeight());
  g.setColour(panel);g.fillRoundedRectangle(r,7.f);g.setColour(red);g.drawRoundedRectangle(r,7.f,1.8f);
 }
 auto lower=juce::Rectangle<float>((float)content.getX(),(float)content.getY()+353.f,(float)content.getWidth(),(float)content.getHeight()-359.f);
 g.setColour(panel);g.fillRoundedRectangle(lower,8.f);g.setColour(red);g.drawRoundedRectangle(lower,8.f,2.f);
 g.setFont(14.f);g.setColour(soft);g.drawText("MASTER PRESET (100)",content.getX()+245,content.getY()+7,240,20,juce::Justification::centredLeft);
 g.drawText("QUICK CONTROLS",content.getRight()-310,content.getY()+368,200,20,juce::Justification::left);
 for(int i=0;i<4;i++)g.drawText(quickLabels[i],content.getRight()-318+i*77,content.getY()+480,70,20,juce::Justification::centred);
 g.drawText("PRESET LIBRARY",content.getX()+14,content.getY()+368,230,20,juce::Justification::left);
}
void SkiivRitualSoundsAudioProcessorEditor::resized() {
 auto content=getLocalBounds().reduced(21);int w=content.getWidth(),x=content.getX(),y=content.getY();
 title.setBounds(x+7,y,200,48);subtitle.setBounds(x+9,y+49,215,22);
 masters.setBounds(x+260,y+30,juce::jmax(200,w-600),32);previous.setBounds(x+226,y+30,30,32);next.setBounds(masters.getRight()+4,y+30,30,32);
 hostSync.setBounds(content.getRight()-215,y+4,210,25);tempoLabel.setBounds(content.getRight()-210,y+32,200,25);
 int cell=(w+7)/5;
 for(int i=0;i<5;i++) {
  int left=x+i*cell+8;
  knobNames[i].setBounds(left,y+103,cell-20,24);
  toggles[i].setBounds(left+cell-67,y+106,60,22);
  knobs[i].setBounds(left+juce::jmax(0,(cell-131)/2),y+133,131,135);
  selects[i].setBounds(left+1,y+277,cell-21,27);
 }
 int low=y+368;
 category.setBounds(x+14,low+25,200,27);help.setBounds(x+225,low+26,260,25);
 preview.setBounds(x+15,low+62,w/2-25,115);
 int quickX=content.getRight()-325;
 for(int i=0;i<4;i++)quick[i].setBounds(quickX+i*77,low+30,73,93);
 division.setBounds(quickX,low+145,100,29);manualTempo.setBounds(quickX+105,low+145,207,29);
}
void SkiivRitualSoundsAudioProcessorEditor::comboBoxChanged(juce::ComboBox* c) {
 if(c==&masters){selectedMaster=masters.getSelectedId()-1; if(selectedMaster>=0 && selectedMaster<(int)SkiivPresets::all().size()){proc.loadMasterPreset(selectedMaster);preview.setText(SkiivPresets::all()[(size_t)selectedMaster].name+"\n"+SkiivPresets::all()[(size_t)selectedMaster].category,juce::dontSendNotification);}}
 else if(c==&category){
   // Filter without renumbering IDs, so selection still maps to factory preset indices.
   const auto previousId=masters.getSelectedId();
   masters.clear(juce::dontSendNotification);
   const auto selectedCategory=category.getSelectedId();
   for(int i=0;i<(int)SkiivPresets::all().size();++i) {
     const auto& p=SkiivPresets::all()[(size_t)i];
     const juce::String categories[]={"", "Ambient", "Dream", "Rhythm", "Glitch", "Cinematic"};
     if(selectedCategory==1 || (selectedCategory>=2 && selectedCategory<=6 && p.category==categories[selectedCategory-1]))
       masters.addItem(juce::String(i+1).paddedLeft('0',3)+" - "+p.name,i+1);
   }
   masters.setSelectedId(previousId,juce::dontSendNotification);
 }
 else for(int i=0;i<5;i++)if(c==&selects[i]) knobs[i].setValue(selects[i].getSelectedId(),juce::sendNotificationSync);
}
void SkiivRitualSoundsAudioProcessorEditor::buttonClicked(juce::Button* b) {
 if(b==&previous||b==&next) {category.setSelectedId(1,juce::sendNotificationSync);masters.setSelectedId(1+(selectedMaster+(b==&next?1:99))%100,juce::sendNotificationSync);}
}
void SkiivRitualSoundsAudioProcessorEditor::timerCallback() {
 tempoLabel.setText((proc.hostTempoAvailable.load()?"HOST ":"MANUAL ")+juce::String(proc.detectedHostBpm.load(),1)+" BPM",juce::dontSendNotification);
}
