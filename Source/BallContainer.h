#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct Ball: public Component {
    Ball(const Point<float>& position, const Point<float>& velocity, int size, int midiNum, Colour color, float& velocityMultiplier);
    void step();
    void paint(Graphics& gr);
    
    /** Helper function for converting MIDI number to pitch string. */
    static String keynumToPitch(int keynum);
    
    int midiNum_;
    Point<float> velocity_;
    Point<float> position_; // Top-left corner
    float& velocityMultiplier_;
    float mass_;
    Colour color_;
};

class BallContainer: public juce::AnimatedAppComponent {
public:
    BallContainer(ParticleArtsPluginAudioProcessor& audioProcessor);
    ~BallContainer() override;
    
    /** AnimatedAppComponent overrides. */
    void update() override;
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    /** Clears all balls from the BallContainer instance. */
    void clear();
    
    /** A Ball's size/mass is determined by pitch, and velocity is the MIDI velocity.
        Randomly generates the direction of the velocity and the starting position. */
    void addBall(int midiNoteNum, int midiVelocity);
    
    /** Allows the user to adjust ball velocities with data from the slider. */
    void setVelocityMultiplier(float value);

private:
    /** Returns true if two Ball instances are colliding. */
    bool checkBallCollision(Ball* first, Ball* second) const;

    /** Given two Ball instances, returns the new velocity for "ball" after a collision with "other". */
    Point<float> getPostCollisionVel(Ball* ball, Ball* other) const;
    
    /** Performs a particle collision by updating the velocities of both particles. */
    void handleCollision(Ball* first, Ball* second);
    
    /** Turns collision data into MidiMessages and passes them to the AudioProcessor. */
    void playCollisionSounds(Ball* first, Ball* second);
    
    /** Calculates a ball's linear momentum using (mass * velocity) */
    float calculateMomentum(Ball* ball) const;
    
    std::vector<std::unique_ptr<Ball>> balls_;
    ParticleArtsPluginAudioProcessor& audioProcessor_;
    float velocityMultiplier_;
    Random r_;
    
    const int MAX_VEL = 3;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BallContainer)
};


