#include "BallContainer.h"

// -------- Ball functions ---------------------

Ball::Ball(const Point<float>& position, const Point<float>& velocity, int size, int midiNum, Colour color, float& velocityMultiplier) : velocityMultiplier_(velocityMultiplier) {
    setSize(size, size);
    setBounds(position.x, position.y, size, size);
    position_ = position;
    mass_ = size;
    midiNum_ = midiNum;
    velocity_ = velocity;
    color_ = color;
}

void Ball::step() {
    position_ += (velocity_ * velocityMultiplier_);
    setBounds(position_.x, position_.y, getWidth(), getHeight());
    
    // Ensures balls are within bounds at all times and
    // handles wall collisions (helps with resizing window)
    if (position_.x < 0) {
        position_.x = 0;
        velocity_.x = -velocity_.x;
    }
    if (position_.y < 0) {
        position_.y = 0;
        velocity_.y = -velocity_.y;
    }
    if (position_.x + getWidth() > getParentWidth()) {
        position_.x = getParentWidth() - getWidth();
        velocity_.x = -velocity_.x;
    }
    if (position_.y + getHeight() > getParentHeight()) {
        position_.y = getParentHeight() - getHeight();
        velocity_.y = -velocity_.y;
    }
}

void Ball::paint(Graphics& gr) {
    gr.setColour(color_);
    gr.fillEllipse(0, 0, getWidth(), getHeight());
    
    gr.setColour (Colours::black);
    gr.setFont (10.0f);
    gr.drawText (keynumToPitch(midiNum_), Rectangle<float> (0, 0, getWidth(), getHeight()), Justification::centred, false);
}

String Ball::keynumToPitch(int keynum) {
    StringArray noteNames = {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};
    StringArray octaves = {"00", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    return noteNames[keynum % 12] + octaves[keynum / 12];
}

// -------- Main component functions -----------

BallContainer::BallContainer(ParticleArtsPluginAudioProcessor& p) : audioProcessor_(p) {
    setFramesPerSecond(60);
}

BallContainer::~BallContainer() {}

void BallContainer::update() {
    // Makes sure all balls have been added
    while (balls_.size() < audioProcessor_.getAddedBallsSize()) {
        auto ballInfo = audioProcessor_.getBall((int) balls_.size());
        addBall(ballInfo.first, ballInfo.second);
    }
    
    // Steps and handles all collisions
    for (auto it = balls_.begin(); it != balls_.end(); ++it) {
        auto ball = it->get();
        for (auto it2 = it; it2 != balls_.end(); ++it2) {
            if (it2 == it) continue;
            auto secondBall = it2->get();
            if (checkBallCollision(ball, secondBall)) {
                playCollisionSounds(ball, secondBall);
                handleCollision(ball, secondBall);
            }
        }
        ball->step();
    }
}

void BallContainer::paint (juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void BallContainer::resized() {
    // Nothing needed here since the width/height is controlled by the parent container
}

void BallContainer::setVelocityMultiplier(float value) {
    velocityMultiplier_ = value;
}

void BallContainer::addBall(int midiNoteNum, int midiVelocity) {
    // Turn midiNoteNum into a diameter ranging 10 - 100
    float diameter = 90 * ((127 - midiNoteNum) / 127.0) + 10;
    
    // Calculate the ball's starting position (not out of bounds)
    float startX = (r_.nextFloat() * (getWidth() - diameter));
    float startY = (r_.nextFloat() * (getHeight() - diameter));
    Point<float> startPosition = Point<float>(startX, startY);
    
    // Calculate starting velocity
    float velX = r_.nextBool() ? r_.nextFloat() * MAX_VEL * -1 : r_.nextFloat() * MAX_VEL;
    float velY = r_.nextBool() ? r_.nextFloat() * MAX_VEL * -1 : r_.nextFloat() * MAX_VEL;
    Point<float> velocity = Point<float>(velX, velY);
    
    // Add ball and make visible
    balls_.push_back(std::make_unique<Ball>(startPosition, velocity, diameter, midiNoteNum, Colours::white, velocityMultiplier_));
    addAndMakeVisible(balls_[balls_.size() - 1].get());
}

void BallContainer::clear() {
    balls_.clear();
    audioProcessor_.clearAddedBalls();
}

// ------------ Private helper functions ----------

void BallContainer::handleCollision(Ball *ball, Ball *secondBall) {
    auto new_vel_1 = getPostCollisionVel(ball, secondBall);
    auto new_vel_2 = getPostCollisionVel(secondBall, ball);
    ball->velocity_ = new_vel_1;
    secondBall->velocity_ = new_vel_2;
}

void BallContainer::playCollisionSounds(Ball* first, Ball* second) {
    
    float ballOneVel = (calculateMomentum(first) / 424.0) * 127;
    float ballTwoVel = (calculateMomentum(second) / 424.0) * 127;

    auto ballOneOn = juce::MidiMessage::noteOn (1, first->midiNum_, (juce::uint8) ballOneVel);
    auto ballOneOff = juce::MidiMessage::noteOff (1, first->midiNum_, (juce::uint8) ballOneVel);
    auto ballTwoOn = juce::MidiMessage::noteOn (1, second->midiNum_, (juce::uint8) ballTwoVel);
    auto ballTwoOff = juce::MidiMessage::noteOff (1, second->midiNum_, (juce::uint8) ballTwoVel);

    ballOneOn.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
    ballOneOff.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
    ballTwoOn.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
    ballTwoOff.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
    
    // TODO: Find a way to potentially make these instantaneous midi messages longer

    audioProcessor_.addToMessageQueue(ballOneOn);
    audioProcessor_.addToMessageQueue(ballOneOff);
    audioProcessor_.addToMessageQueue(ballTwoOn);
    audioProcessor_.addToMessageQueue(ballTwoOff);
}

bool BallContainer::checkBallCollision(Ball* first, Ball* second) const {
    // Calculates the distance between the two balls' centers
    Point<float> firstCenter = Point<float>(first->position_.x + (first->getWidth() / 2.0), first->position_.y + (first->getHeight() / 2.0));
    Point<float> secondCenter = Point<float>(second->position_.x + (second->getWidth() / 2.0), second->position_.y + (second->getHeight() / 2.0));
    float dist = firstCenter.getDistanceFrom(secondCenter);
    
    // Checks if the two balls are moving towards each other
    auto velDif = first->velocity_ - second->velocity_;
    auto posDif = first->position_ - second->position_;
    auto dotProd = (velDif.x * posDif.x) + (velDif.y * posDif.y);
    bool movingTowards = dotProd < 0;
    
    return movingTowards && dist <= (first->getWidth() / 2.0) + (second->getWidth() / 2.0);
}

Point<float> BallContainer::getPostCollisionVel(Ball *ball, Ball *other) const {
    auto vel_dif = ball->velocity_ - other->velocity_;
    auto pos_dif = ball->position_ - other->position_;
    
    float dot_prod = (vel_dif.x * pos_dif.x) + (vel_dif.y * pos_dif.y);
    float len_squared = pow(pos_dif.x, 2) + pow(pos_dif.y, 2);
    float mass_factor = (2 * other->mass_) / (ball->mass_ + other->mass_);
    
    return ball->velocity_ - (mass_factor * (dot_prod / len_squared) * pos_dif);
}

float BallContainer::calculateMomentum(Ball* ball) const {
    float vel = sqrt(pow(ball->velocity_.x, 2) + pow(ball->velocity_.y, 2));
    return vel * ball->mass_;
}
