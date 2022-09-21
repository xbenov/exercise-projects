#include <iostream>
#include <map>
#include <list>

// headers can be found here: https://github.com/drahosp/ppgso
#include <ppgso/ppgso.h>
#include "camera.h"
#include "scene.h"
#include "arena.h"
#include "player.h"
#include "ball.h"
#include "menu.h"

const unsigned int SIZE = 512;

int sceneN = 0;
bool animation = false;
float winTime = 8.0f;

/*!
 * Custom windows for our simple game
 */
class SceneWindow : public ppgso::Window {
private:
    Scene scene;
    bool animate = true;
    std::unique_ptr<Camera> zcamera,ycamera,xcamera;

    /*!
     * Reset and initialize the game scene
     * Creating unique smart pointers to objects that are stored in the scene object list
     */
    void initScene() {
        showMenu("menu.bmp");

    }

    void makeGameScene (){
        scene.objects.clear();
        scene.score1 = 0;
        scene.score2 = 0;

        // Create a camera
        zcamera = std::make_unique<Camera>(60.0f, 1.0f, 0.1f, 100.0f);
        zcamera->position.z = -19.0f;

        scene.camera = move(zcamera);

        // Add arena
        scene.objects.push_back(std::make_unique<Arena>());

        // Add player
        auto player = std::make_unique<Player>();
        auto player2 = std::make_unique<Player>();
        player2->position.x -= 12;
        scene.objects.push_back(move(player));
        scene.objects.push_back(move(player2));

        // Add ball to the scene
        auto ball = std::make_unique<Ball>();
        scene.objects.push_back(move(ball));


    }

    void showMenu(std::string image){
        scene.objects.clear();

        // Create a camera
        auto camera = make_unique<Camera>(60.0f, 1.0f, 0.1f, 100.0f);
        scene.camera = move(camera);

        // Add menu image
        auto canv = std::make_unique<Menu>(image);
        canv.get()->position = glm::vec3(0, 0, 0);
        scene.camera.get()->position = glm::vec3(0, 0, -3);
        scene.objects.push_back(move(canv));
    }

public:
    /*!
     * Construct custom game window
     */
    SceneWindow() : Window{"gl9_scene", SIZE, SIZE} {
        //hideCursor();
        glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

        // Initialize OpenGL state
        // Enable Z-buffer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Enable polygon culling
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        initScene();
    }

    void onKey(int key, int scanCode, int action, int mods) override {
        scene.keyboard[key] = action;

        if(!animation) {

            if (sceneN == 0) {
                // start game
                if (key == GLFW_KEY_S && action == GLFW_PRESS) {
                    makeGameScene();
                    sceneN = 1;
                }
                // exit game
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                    exit(EXIT_SUCCESS);
                }
            } else if (sceneN == 1) {
                // Reset
                if (key == GLFW_KEY_R && action == GLFW_PRESS) {
                    initScene();
                    sceneN = 0;
                }
                // Pause
                if (key == GLFW_KEY_P && action == GLFW_PRESS) {
                    animate = !animate;
                }
                // Z camera
                if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
                    zcamera = std::make_unique<Camera>(60.0f, 1.0f, 0.1f, 100.0f);
                    zcamera->position.z = -19.0f;
                    scene.camera = move(zcamera);
                }
                // Y camera
                if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
                    ycamera = std::make_unique<Camera>(60.0f, 1.0f, 0.1f, 100.0f);
                    ycamera->position.z = 0.0001f;
                    ycamera->position.y = -25.0f;
                    scene.camera = move(ycamera);
                }
                // X camera
                if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
                    xcamera = std::make_unique<Camera>(60.0f, 1.0f, 0.1f, 100.0f);
                    xcamera->position.x = -25.0f;
                    scene.camera = move(xcamera);
                }
                // rotate camera
                if (key == GLFW_KEY_Q) {
                    scene.camera->rotate(glm::vec3(0, 0, 0.05));
                }
                // rotate camera
                if (key == GLFW_KEY_E) {
                    scene.camera->rotate(glm::vec3(0, 0, -0.05));
                }
                // rotate camera to origin
                if (key == GLFW_KEY_W && action == GLFW_PRESS) {
                    scene.camera->rotateToOrigin();
                }

            } else if (sceneN == 2) {
                // exit game
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                    exit(EXIT_SUCCESS);
                }
                // menu
                if (key == GLFW_KEY_M && action == GLFW_PRESS) {
                    initScene();
                    sceneN = 0;
                }
            }
        }
    }

    void onIdle() override {
        // Track time
        static auto time = (float) glfwGetTime();

        // Compute time delta
        float dt = animate ? (float) glfwGetTime() - time : 0;

        time = (float) glfwGetTime();

        if(sceneN == 1){
            if(scene.score1 == 2){

                if(winTime > 0){
                    scene.winTime = true;
                    winTime -= dt;

                }
                else {
                    winTime = 8.0f;
                    scene.winTime = false;
                    showMenu("winp1.bmp");
                    sceneN = 2;
                }
            }
            else if(scene.score2 == 2){
                if(winTime > 0){
                    scene.winTime = true;
                    winTime -= dt;

                }
                else {
                    winTime = 8.0f;
                    scene.winTime = false;
                    showMenu("winp2.bmp");
                    sceneN = 2;
                }
            }
        }

        // Set gray background
        glClearColor(.5f, .5f, .5f, 0);
        // Clear depth and color buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update and render all objects
        scene.update(dt);
        scene.render();
    }

};

int main() {
    // Initialize our window
    SceneWindow window;

    // Main execution loop
    while (window.pollEvents()) {}

    return EXIT_SUCCESS;
}


