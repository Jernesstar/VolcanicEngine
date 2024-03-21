#include <OpenGL/FrameBuffer.h>

#include <Saddle/Core/Application.h>
#include <Saddle/Renderer/Renderer.h>
#include <Saddle/Renderer/Renderer2D.h>
#include <Saddle/Renderer/OrthographicCamera.h>
#include <Saddle/Renderer/StereographicCamera.h>
#include <Saddle/Renderer/CameraController.h>
#include <Saddle/Text/Font.h>
#include <Saddle/Text/Text.h>

using namespace VolcaniCore;

class ShadowDemo : public Application {
public:
    ShadowDemo();

    void OnUpdate(TimeStep ts) override;

private:
    Shader depth_shader{ { "Sandbox/assets/shaders/Depth.glsl.vert", "Sandbox/assets/shaders/Depth.glsl.frag" } };
    Shader shadow_shader{ { "Sandbox/assets/shaders/Shadow.glsl.vert", "Sandbox/assets/shaders/Shadow.glsl.frag" } };
    FrameBuffer* depth_map;

    Texture2D wood{ "Sandbox/assets/images/wood.png" };

    StereographicCamera camera{ 90.0f, 0.1f, 100.0f, 1600, 900 };
    OrthographicCamera light_transform;

    CameraController controller{ camera };

    float near_plane = 1.0f, far_plane = 7.5f;
    glm::mat4 mat;
};

ShadowDemo::ShadowDemo()
{
    EventSystem::RegisterEventListener<KeyPressedEvent>(
    [](const KeyPressedEvent& event)
    {
        if(event.Key == Key::Escape)
            Application::Close();
    });

    glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 light_view = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
                                    glm::vec3(0.0f, 0.0f,  0.0f),
                                    glm::vec3(0.0f, 1.0f,  0.0f));
    mat = light_projection * light_view;

    depth_map = new FrameBuffer({
        800, 600, AttachmentType::None, AttachmentType::Texture, AttachmentType::None
    });

    camera.SetPosition({ 0.0f, 0.0f, 1.0f });
    controller.TranslationSpeed = 0.5f;
}

void ShadowDemo::OnUpdate(TimeStep ts)
{
    controller.OnUpdate(ts);

    depth_shader.Bind();
    depth_shader.SetMat4("u_LightSpaceMatrix", mat);

    glViewport(0, 0, 800, 600);
    depth_map->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    wood.Bind();
    // renderScene(simpleDepthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport
    glViewport(0, 0, 800, 600);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Renderer::Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
}