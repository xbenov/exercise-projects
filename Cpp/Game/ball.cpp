#include <glm/gtc/random.hpp>
#include "scene.h"
#include "ball.h"
#include "arena.h"
#include "player.h"
#include "star.h"

#include <shaders/phong_vert_glsl.h>
#include <shaders/phong_frag_glsl.h>


// shared resources
std::unique_ptr<ppgso::Mesh> Ball::mesh;
std::unique_ptr<ppgso::Shader> Ball::shader;
std::unique_ptr<ppgso::Texture> Ball::texture;

Ball::Ball() {
    // Set default random speed and direction
    speed = {glm::linearRand(-3.0f,3.0f), glm::linearRand(-3.0f,3.0f), 0.0f};

    if(abs(speed.x) < 0.8f)
        speed.x += speed.x < 0 ? -1.0f:1.0f;
    if(abs(speed.y) < 0.8f)
        speed.y += speed.y < 0 ? -1.0f:1.0f;
    accel = {speed.x < 0 ? -0.2f:0.2f, speed.y < 0 ? -0.2f:0.2f, 0.0f};

    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(phong_vert_glsl, phong_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("ball.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");
}

bool Ball::update(Scene &scene, float dt) {
    // Increase age
    age += dt;

    // animuj po urcitom case
    if (age > 2.0f) {
        // Accelerate
        speed += accel * dt;

        // Move the ball
        position += speed * dt;
    }


    // Collide with scene
    for (auto &obj : scene.objects) {
        // Ignore self in scene
        if (obj.get() == this) continue;

        // We only need to collide with arena and player, ignore other objects
        auto arena = dynamic_cast<Arena*>(obj.get()); // dynamic_pointer_cast<Asteroid>(obj);
        auto player = dynamic_cast<Player*>(obj.get()); //dynamic_pointer_cast<Projectile>(obj);
        if (!arena && !player) continue;


        if(arena) {
            // odrazi gulicku horizontalne
            if (abs(position.y - obj->position.y) > 3.5f)
                odrazHorizontalne();

            // znicit ball a pridat skore spravnej strane
            if (abs(position.x - obj->position.x) > 6.5f) {
                if (position.x > 0) {
                    auto star = std::make_unique<Star>();
                    star->position.x = -9;
                    star->position.y = 4 - scene.score2*1.5f;

                    scene.score2++;
                    scene.objects.push_back(move(star));
                } else {
                    auto star2 = std::make_unique<Star>();
                    star2->position.x = 9;
                    star2->position.y = 4 - scene.score1*1.5f;

                    scene.score1++;
                    scene.objects.push_back(move(star2));
                }

                auto ball = std::make_unique<Ball>();
                scene.objects.push_back(move(ball));

                return false;
            }
        }

        if(player)
            if (abs(position.x - obj->position.x) < 0.5 && abs(position.y - obj->position.y) < 1 && speed.x > 0 && position.x > 0) {
                // odrazi gulicku
                odrazVertikalne();
            }
            else if (abs(position.x - obj->position.x) < 0.5 && abs(position.y - obj->position.y) < 1 && speed.x < 0 && position.x < 0) {
                // odrazi gulicku
                odrazVertikalne();
            }

    }

    generateModelMatrix(dt);

    return true;
}

void Ball::odrazVertikalne() {
    speed.x = -speed.x;
    accel.x = -accel.x;
}
void Ball::odrazHorizontalne() {
    speed.y = -speed.y;
    accel.y = -accel.y;
}

void Ball::render(Scene &scene) {
    shader->use();

    // Set up lights
    shader->setUniform("viewPos",scene.camera->position);

    shader->setUniform("pointLights[0].position", scene.pointLightPositions[0] );
    shader->setUniform("pointLights[0].ambient",glm::vec3(0.3f,0.3f,0.3f));
    shader->setUniform("pointLights[0].diffuse",glm::vec3(0.5f,0.5f,0.5f));
    shader->setUniform("pointLights[0].specular",glm::vec3(1.f,1.f,1.f));
    shader->setUniform("pointLights[0].constant",1.0f);
    shader->setUniform("pointLights[0].linear",0.09f);
    shader->setUniform("pointLights[0].quadratic",0.032f);

    shader->setUniform("pointLights[1].position", scene.pointLightPositions[1] );
    shader->setUniform("pointLights[1].ambient",glm::vec3(0.3f,0.3f,0.3f));
    shader->setUniform("pointLights[1].diffuse",glm::vec3(0.5f,0.5f,0.5f));
    shader->setUniform("pointLights[1].specular",glm::vec3(1.f,1.f,1.f));
    shader->setUniform("pointLights[1].constant",1.0f);
    shader->setUniform("pointLights[1].linear",0.09f);
    shader->setUniform("pointLights[1].quadratic",0.032f);

    // Set up material
    shader->setUniform("material.ambient",glm::vec3(1.0f,0.8f,0.5f));
    shader->setUniform("material.diffuse",glm::vec3(1.0f,0.5f,0.31f));
    shader->setUniform("material.specular",glm::vec3(0.8f,0.8f,0.8f));
    shader->setUniform("material.shininess",60.0f);

    // use camera
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("Texture", *texture);
    mesh->render();
}


