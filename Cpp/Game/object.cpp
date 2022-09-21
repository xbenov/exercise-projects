#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include "object.h"

void Object::generateModelMatrix(float dt) {

    if(gravity){
        if(position.y < -7.0f) {
            speed.y = -speed.y;
            speed.y += speed.y < 0 ? hardness:-hardness;
        }
        speed += gravityAccel * dt;
        position += speed * dt;
    }

    modelMatrix =
        glm::translate(glm::mat4(1.0f), position)
        * glm::orientate4(rotation)
        * glm::scale(glm::mat4(1.0f), scale);
}