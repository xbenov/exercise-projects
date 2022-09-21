
#include <ppgso/ppgso.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

#include "scene.h"
#include "object.h"

using namespace std;
using namespace glm;

class Menu final : public Object
{
private :
    // Static resources (Shared between instances)
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;
public:
    Menu(std::string image);
    bool update(Scene &scene, float dt) override;
    void render(Scene &scene) override;
    void setImage(std::string image);
};