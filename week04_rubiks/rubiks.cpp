#include "rubiks.hpp"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Cubelet::model_matrix() const {
    glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
    glm::mat4 r = glm::mat4_cast(orient);
    return t * r;
}

RubiksCube::RubiksCube() {
    cubelets_.reserve(27);
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y)
            for (int z = -1; z <= 1; ++z)
                cubelets_.push_back({{x, y, z}, glm::quat(1, 0, 0, 0)});
}

void RubiksCube::queue_turn(FaceTurn t) {
    if (anim_active_) return;
    anim_active_ = true;
    anim_turn_   = t;
    anim_time_   = 0.0f;
}

void RubiksCube::update(float dt) {
    if (!anim_active_) return;
    anim_time_ += dt;
    if (anim_time_ >= anim_total_) {
        commit_turn();
    }
}

bool RubiksCube::cubelet_in_active_layer(const Cubelet& c) const {
    if (!anim_active_) return false;
    return c.pos[anim_turn_.axis] == anim_turn_.layer;
}

glm::mat4 RubiksCube::anim_layer_matrix() const {
    if (!anim_active_) return glm::mat4(1.0f);
    float t      = anim_time_ / anim_total_;
    if (t > 1.0f) t = 1.0f;
    float angle  = glm::radians(90.0f) * t * (float)anim_turn_.direction;
    glm::vec3 axis(0.0f);
    axis[anim_turn_.axis] = 1.0f;
    return glm::rotate(glm::mat4(1.0f), angle, axis);
}

void RubiksCube::commit_turn() {
    // TODO: For each cubelet whose pos[axis] == layer:
    //   - Rotate its `pos` around `axis` by ±90° (integer permutation).
    //   - Compose its `orient` with a 90° rotation about `axis`.
    // Then clear the animation flag.
    //
    // Reference for an x-axis +90° rotation: (x, y, z) -> (x, -z, y).
    // For each axis & direction work the permutation out on paper first, then
    // hard-code it. Keep it integer to avoid drift after many turns.
    anim_active_ = false;
}
