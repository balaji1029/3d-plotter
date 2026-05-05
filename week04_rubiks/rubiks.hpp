#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <array>
#include <vector>

// One small cubelet of the 3x3x3. We track integer position so we can
// recompute "which cubelets belong to layer X" without floating-point fuzz,
// and a quaternion for accumulated face turns.
struct Cubelet {
    glm::ivec3 pos;            // each component in {-1, 0, +1}
    glm::quat  orient = {1, 0, 0, 0};

    glm::mat4 model_matrix() const;
};

// A face of the cube identified by axis index (0=x, 1=y, 2=z) and layer
// (-1 or +1). Direction is +1 for clockwise (looking down +axis) or -1.
struct FaceTurn {
    int axis;       // 0,1,2
    int layer;      // -1 or +1
    int direction;  // -1 or +1
};

class RubiksCube {
   public:
    RubiksCube();

    // Submit a face turn. If a turn is currently animating, the request is
    // ignored (or queue it — your choice as a TODO).
    void queue_turn(FaceTurn t);

    // Advance any in-progress animation by dt seconds.
    void update(float dt);

    bool is_animating() const { return anim_active_; }

    const std::vector<Cubelet>& cubelets() const { return cubelets_; }

    // While animating, the layer being turned has this extra rotation applied
    // on top of each cubelet's stored orientation.
    glm::mat4 anim_layer_matrix() const;
    bool      cubelet_in_active_layer(const Cubelet& c) const;

   private:
    std::vector<Cubelet> cubelets_;

    bool      anim_active_ = false;
    FaceTurn  anim_turn_   = {0, 0, 0};
    float     anim_time_   = 0.0f;
    float     anim_total_  = 0.25f;  // seconds per face turn

    // Snap cubelets in the active layer at the end of an animation.
    // TODO: rotate each affected cubelet's `pos` and `orient` by 90 degrees
    // about the relevant axis, then clear anim_active_.
    void commit_turn();
};

// 6 face colors for the 3x3x3 (standard Western scheme).
namespace face_color {
inline constexpr std::array<float, 3> WHITE  = {1.00f, 1.00f, 1.00f};
inline constexpr std::array<float, 3> YELLOW = {1.00f, 0.85f, 0.10f};
inline constexpr std::array<float, 3> RED    = {0.85f, 0.10f, 0.10f};
inline constexpr std::array<float, 3> ORANGE = {1.00f, 0.45f, 0.00f};
inline constexpr std::array<float, 3> BLUE   = {0.10f, 0.30f, 0.85f};
inline constexpr std::array<float, 3> GREEN  = {0.10f, 0.70f, 0.20f};
inline constexpr std::array<float, 3> BLACK  = {0.05f, 0.05f, 0.05f};
}  // namespace face_color
