#include "scene.h"
#include "ball.h"
#include "star.h"

void Scene::update(float time) {
    camera->update();

    // Use iterator to update all objects so we can remove while iterating
    auto i = std::begin(objects);

    while (i != std::end(objects)) {
        // Update and remove from list if needed
        auto obj = i->get();

        auto star = dynamic_cast<Star*>(obj);
        auto ball = dynamic_cast<Ball*>(obj);

        // ball je svetlo
        if (ball)
            pointLightPositions[0] = ball->position;

        // ak vyhral niekto zapni hravitaciu na hviezdickach
        if(winTime) {
            if (star)
                obj->gravity = true;
            if(ball)
                obj->position = {0,0,0};
        }
        else
            if(star)
                obj->gravity = false;

        if (!obj->update(*this, time))
            i = objects.erase(i); // NOTE: no need to call destructors as we store shared pointers in the scene
        else
            ++i;
    }
}

void Scene::render() {
    // Simply render all objects
    for ( auto& obj : objects )
        obj->render(*this);
}

std::vector<Object*> Scene::intersect(const glm::vec3 &position, const glm::vec3 &direction) {
    std::vector<Object*> intersected = {};
    for(auto& object : objects) {
        // Collision with sphere of size object->scale.x
        auto oc = position - object->position;
        auto radius = object->scale.x;
        auto a = glm::dot(direction, direction);
        auto b = glm::dot(oc, direction);
        auto c = glm::dot(oc, oc) - radius * radius;
        auto dis = b * b - a * c;

        if (dis > 0) {
            auto e = sqrt(dis);
            auto t = (-b - e) / a;

            if ( t > 0 ) {
                intersected.push_back(object.get());
                continue;
            }

            t = (-b + e) / a;

            if ( t > 0 ) {
                intersected.push_back(object.get());
                continue;
            }
        }
    }

    return intersected;
}


void Scene::setLights(std::unique_ptr<ppgso::Shader> shader){

    // Set up lights
    shader->setUniform("viewPos",camera->position);

    shader->setUniform("pointLights[0].position", pointLightPositions[0] );
    shader->setUniform("pointLights[0].ambient",glm::vec3(0.3f,0.3f,0.3f));
    shader->setUniform("pointLights[0].diffuse",glm::vec3(0.5f,0.5f,0.5f));
    shader->setUniform("pointLights[0].specular",glm::vec3(1.f,1.f,1.f));
    shader->setUniform("pointLights[0].constant",1.0f);
    shader->setUniform("pointLights[0].linear",0.09f);
    shader->setUniform("pointLights[0].quadratic",0.032f);

    shader->setUniform("pointLights[1].position", pointLightPositions[1] );
    shader->setUniform("pointLights[1].ambient",glm::vec3(0.3f,0.3f,0.3f));
    shader->setUniform("pointLights[1].diffuse",glm::vec3(0.5f,0.5f,0.5f));
    shader->setUniform("pointLights[1].specular",glm::vec3(1.f,1.f,1.f));
    shader->setUniform("pointLights[1].constant",1.0f);
    shader->setUniform("pointLights[1].linear",0.09f);
    shader->setUniform("pointLights[1].quadratic",0.032f);

}
