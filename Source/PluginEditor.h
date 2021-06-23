#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "BallContainer.h"

class ParticleArtsPluginAudioProcessorEditor: public juce::AudioProcessorEditor, Button::Listener, Slider::Listener {
public:
    ParticleArtsPluginAudioProcessorEditor (ParticleArtsPluginAudioProcessor&);
    ~ParticleArtsPluginAudioProcessorEditor() override;

    /** JUCE Component overrides. */
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    /** GUI component listeners. */
    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider *slider) override;
    
    /// Adds a new ball to the BallContainer randomly picked from some C's, D's, and G's.
    void addRandomBall();
    
private:
    /// A reference to the PluginProcessor
    ParticleArtsPluginAudioProcessor& audioProcessor;
    
    /// Components
    TextButton settingsButton_ {"Settings"};
    Slider velocitySlider_;
    TextButton clearButton_ {"Clear"};
    TextButton addBallButton_ {"Add Ball"};
    Label speedLabel_ = {"", "Speed:"};
    BallContainer ballContainer_ {audioProcessor};
    
    Random r_;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParticleArtsPluginAudioProcessorEditor)
};
