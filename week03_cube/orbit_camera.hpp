#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Orbit camera: yaw/pitch around a target point at a given radius.
class OrbitCamera {
   public:
    OrbitCamera() = default;

    void rotate(float dyaw, float dpitch) {
        yaw_   += dyaw;
        pitch_ += dpitch;
        // clamp pitch to avoid gimbal flip at the poles
        const float lim = glm::radians(89.0f);
        if (pitch_ >  lim) pitch_ =  lim;
        if (pitch_ < -lim) pitch_ = -lim;
    }

    void zoom(float factor) {
        radius_ *= factor;
        if (radius_ < 0.1f) radius_ = 0.1f;
    }

    glm::mat4 view() const {
        glm::vec3 eye = target_ + radius_ * glm::vec3(
            std::cos(pitch_) * std::sin(yaw_),
            std::sin(pitch_),
            std::cos(pitch_) * std::cos(yaw_));
        return glm::lookAt(eye, target_, glm::vec3(0, 1, 0));
    }

   private:
    glm::vec3 target_ = {0, 0, 0};
    float     radius_ = 4.0f;
    float     yaw_    = 0.6f;
    float     pitch_  = 0.4f;
};
