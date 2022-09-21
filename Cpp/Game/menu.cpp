#include "menu.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

std::unique_ptr<ppgso::Mesh> Menu::mesh;
std::unique_ptr<ppgso::Texture> Menu::texture;
std::unique_ptr<ppgso::Shader> Menu::shader;

std::string cesta;

Menu::Menu(std::string image)
{
    // Initialize static resources if needed
    cesta = "";
    if (!shader) shader = make_unique<Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
     texture = make_unique<Texture>(image::loadBMP(cesta.append(image)));
    mesh = make_unique<Mesh>("quad.obj");

    rotation = glm::vec3(PI,PI,0);
    scale = glm::vec3(1.9f,1.9f,1.9f);

    float dt = 1.0f;
    generateModelMatrix(dt);
}


bool Menu::update(Scene &scene, float dt)
{
    return true;
}

void Menu::render(Scene &scene) {
    shader->use();

    // Set up light
    shader->setUniform("LightDirection", scene.lightDirection);

    // use camera
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("Texture", *texture);
    mesh->render();
}