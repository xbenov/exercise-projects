#include <glm/glm.hpp>

#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>


Camera::Camera(float fow, float ratio, float near, float far) {
    float fowInRad = (ppgso::PI/180.0f) * fow;

    projectionMatrix = glm::perspective(fowInRad, ratio, near, far);
}

void Camera::update() {

    viewMatrix = lookAt(position, glm::vec3(0), up);

}

void Camera::rotate(glm::vec3 rotation){
    position = position * glm::orientate3(glm::vec3(rotation));
}

void Camera::rotateToOrigin() {
    position.x = 0;
    position.y = -10;
    position.z = -18;
}

glm::vec3 Camera::cast(double u, double v) {
    // Create point in Screen coordinates
    glm::vec4 screenPosition{u,v,0.0f,1.0f};

    // Use inverse matrices to get the point in world coordinates
    auto invProjection = glm::inverse(projectionMatrix);
    auto invView = glm::inverse(viewMatrix);

    // Compute position on the camera plane
    auto planePosition = invView * invProjection * screenPosition;
    planePosition /= planePosition.w;

    // Create direction vector
    auto direction = glm::normalize(planePosition - glm::vec4{position,1.0f});
    return glm::vec3{direction};
}
