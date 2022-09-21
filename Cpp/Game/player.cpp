//
// Created by xgame on 19. 11. 2019.
//

#include "player.h"
#include <shaders/phong_vert_glsl.h>
#include <shaders/phong_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Player::mesh;
std::unique_ptr<ppgso::Texture> Player::texture;
std::unique_ptr<ppgso::Shader> Player::shader;

Player::Player() {
    // Set scale, position and rotation
    scale *= 0.2f;
    rotation.x += 1.57f;
    position.x += 6;
    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(phong_vert_glsl, phong_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("player-uv.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("player.obj");
}

bool Player::update(Scene &scene, float dt) {

    if(position.x > 0) {
        if (scene.keyboard[GLFW_KEY_T] && position.y < 3) {
            position.y += 10 * dt;
        } else if (scene.keyboard[GLFW_KEY_G] && position.y > -3) {
            position.y -= 10 * dt;
        }
    }
    else{
        if (scene.keyboard[GLFW_KEY_I] && position.y < 3) {
            position.y += 10 * dt;
        } else if (scene.keyboard[GLFW_KEY_J] && position.y > -3) {
            position.y -= 10 * dt;
        }
    }

    // Generate modelMatrix from position, rotation and scale
    generateModelMatrix(dt);



    return true;
}



void Player::render(Scene &scene) {
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
    shader->setUniform("material.ambient",glm::vec3(1.0f,0.5f,0.31f));
    shader->setUniform("material.diffuse",glm::vec3(1.0f,0.5f,0.31f));
    shader->setUniform("material.specular",glm::vec3(0.6f,0.6f,0.6f));
    shader->setUniform("material.shininess",50.0f);

    // use camera
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("Texture", *texture);
    mesh->render();
}