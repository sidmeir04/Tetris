#ifndef FALLING_STATE_HPP
#define FALLING_STATE_HPP

struct FallingState {
    int baseMaxTimeOnGround = 60;
    int maxTimeGrounded = baseMaxTimeOnGround;
    int rotations = 0;
    float timeSinceLastDrop = 0;
    float timeOnGround = 0;

    void rotate() {
        rotations++;
        maxTimeGrounded += maxTimeGrounded >> (rotations);
    }

    void reset() {
        timeSinceLastDrop = 0;
        timeOnGround = 0;
        rotations = 0;
        maxTimeGrounded = baseMaxTimeOnGround;
    }
};

#endif
