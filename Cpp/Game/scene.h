#ifndef _PPGSO_SCENE_H
#define _PPGSO_SCENE_H

#include <memory>
#include <map>
#include <list>

#include <glm/glm.hpp>
#include <ppgso/ppgso.h>

#include "object.h"
#include "camera.h"


/*
 * Scene is an object that will aggregate all scene related data
 * Objects are stored in a list of objects
 * Keyboard and Mouse states are stored in a map and struct
 */
class Scene {
public:
    /*!
     * Update all objects in the scene
     * @param time
     */
    void update(float time);

    /*!
     * Render all objects in the scene
     */
    void render();

    /*!
     * Pick objects using a ray
     * @param position - Position in the scene to pick object from
     * @param direction - Direction to pick objects from
     * @return Objects - Vector of pointers to intersected objects
     */
    std::vector<Object*> intersect(const glm::vec3 &position, const glm::vec3 &direction);

    // Camera object
    std::unique_ptr<Camera> camera;

    // All objects to be rendered in scene
    std::list< std::unique_ptr<Object> > objects;

    // Keyboard state
    std::map< int, int > keyboard;

    // Set up lights
    void setLights(std::unique_ptr<ppgso::Shader> shader);

    // skore
    int score1,score2;
    // trigger pre zapnutie gravitacie pri vyhre
    bool winTime = false;

    // Lights
    glm::vec3 lightDirection{0.0f, 0.0f, -1.0f};
    glm::vec3 pointLightPositions[2] = {
            glm::vec3( 0.7f,  0.2f,  -2.0f),
            glm::vec3( 8.0f, 4.0f, -2.0f)
    };

    // Store cursor state
    struct {
        double x, y;
        bool left, right;
    } cursor;
};

#endif // _PPGSO_SCENE_H