#pragma once
#include <memory>

#include <ppgso/ppgso.h>

#include "scene.h"
#include "object.h"

/*!
 *
 */
class Star final : public Object {
private:
    // Static resources (Shared between instances)
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;


public:
    /*!
     * Create new star
     */
    Star();

    /*!
     * Update star
     * @param scene Scene to interact with
     * @param dt Time delta for animation purposes
     * @return
     */
    bool update(Scene &scene, float dt) override;

    /*!
     * Render star
     * @param scene Scene to render in
     */
    void render(Scene &scene) override;

private:
};