#pragma once
#include <ppgso/ppgso.h>

#include "object.h"

/*!
 * Object representing a ball that will accelerate from the origin point once created
 */
class Ball final : public Object {
private:
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

    float age{0.0f};
    glm::vec3 speed;
    glm::vec3 accel;
public:
    /*
     * Create new ball
     */
    Ball();

    bool update(Scene &scene, float dt) override;

    void odrazVertikalne();
    void odrazHorizontalne();

    void render(Scene &scene) override;

    void destroy();
};