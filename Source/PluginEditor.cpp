//#include "PluginProcessor.h"
#include "PluginEditor.h"

ParticleArtsPluginAudioProcessorEditor::ParticleArtsPluginAudioProcessorEditor (ParticleArtsPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (608, 412);
    
    // Clear Button
    addAndMakeVisible(clearButton_);
    clearButton_.addListener(this);
    
    // Add Ball Button
    addAndMakeVisible(addBallButton_);
    addBallButton_.addListener(this);
    
    // Settings Button
    addAndMakeVisible(settingsButton_);
    settingsButton_.addListener(this);
    
    // Speed Label
    speedLabel_.setColour(juce::Label::textColourId, Colours::white);
    speedLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible(speedLabel_);
    
    // Velocity Slider
    addAndMakeVisible(velocitySlider_);
    velocitySlider_.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, velocitySlider_.getTextBoxHeight());
    velocitySlider_.setRange(0.0, 5.0, 0.1);
    velocitySlider_.setTextValueSuffix("x");
    velocitySlider_.setValue(1.0);
    velocitySlider_.setSkewFactorFromMidPoint(1.0);
    velocitySlider_.addListener(this);
    speedLabel_.attachToComponent (&velocitySlider_, true);
    
    // Ball Container
    addAndMakeVisible(ballContainer_);
}

ParticleArtsPluginAudioProcessorEditor::~ParticleArtsPluginAudioProcessorEditor() {}

void ParticleArtsPluginAudioProcessorEditor::paint(juce::Graphics& g) {
}

void ParticleArtsPluginAudioProcessorEditor::resized() {
    auto area = getLocalBounds().reduced(8);
    auto topRow = area.removeFromTop(24);
    
    // TODO: Add more settings options
    //settingsButton_.setBounds(topRow.removeFromLeft(100));
    //topRow.removeFromLeft(8);
    addBallButton_.setBounds(topRow.removeFromLeft(100));
    topRow.removeFromLeft(8);
    clearButton_.setBounds(topRow.removeFromLeft(100));

    velocitySlider_.setBounds(topRow.removeFromRight(250));
    speedLabel_.setBounds(topRow.removeFromRight(50));
    area.removeFromTop(8);
    ballContainer_.setBounds(area);
}

void ParticleArtsPluginAudioProcessorEditor::buttonClicked(Button* button) {
    if (button == &clearButton_) {
        ballContainer_.clear();
    } else if (button == &addBallButton_) {
        addRandomBall();
    } else if (button == &settingsButton_) {
        
    }
}

void ParticleArtsPluginAudioProcessorEditor::sliderValueChanged(Slider *slider) {
    if (slider == &velocitySlider_) {
        ballContainer_.setVelocityMultiplier(velocitySlider_.getValue());
    }
}

void ParticleArtsPluginAudioProcessorEditor::addRandomBall() {
    std::vector<int> midiNotes = {48, 60, 67, 72, 79, 86, 91, 96, 98, 103};
    int randomIndex = r_.nextInt(midiNotes.size() - 1.0);
    int chosenNote = midiNotes[randomIndex];
    ballContainer_.addBall(chosenNote, r_.nextInt(127));
}
