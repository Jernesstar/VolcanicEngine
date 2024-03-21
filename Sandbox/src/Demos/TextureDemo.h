#pragma once

#include <Saddle/Core/Application.h>
#include <Saddle/Events/EventSystem.h>
#include <Saddle/Renderer/Renderer.h>
#include <Saddle/Renderer/OrthographicCamera.h>
#include <Saddle/Renderer/CameraController.h>
#include <Saddle/Renderer/Renderer2D.h>

using namespace VolcaniCore;

class TextureDemo : public Application {
public:
    TextureDemo();

    void OnUpdate(TimeStep ts);

    Entity entity;

    Texture2D texture1{ "Sandbox/assets/images/start_bg.png" };
    Texture2D texture2{ "Sandbox/assets/images/kick_drum.png" };

    OrthographicCamera camera{ 0.0f, 0.0f, 0.0f, 0.0f };
    CameraController controller{ camera };
};

TextureDemo::TextureDemo()
{
    EventSystem::RegisterEventListener<KeyPressedEvent>(
    [](const KeyPressedEvent& event)
    {
        if(event.Key == Key::Escape)
            Application::Close();
    });

    GetWindow().SetWindowIcon("Sandbox/assets/images/start_bg.png");
    controller.TranslationSpeed = 1.0f;

    entity.AddComponent<TextureComponent>("Sandbox/assets/images/apple.png");
    entity.AddComponent<TransformComponent>().Translation = glm::vec3(400.0f, 600.0f, -0.1f);
}

void TextureDemo::OnUpdate(TimeStep ts)
{
    Renderer::Clear({ 1, 1, 1, 1 });

    glm::vec2 vec = GetWindow().GetFrameBufferSize();
    camera.SetProjection(0.0f, vec.x, 0.0f, vec.y);

    Renderer2D::BeginScene(camera);
    {
        Renderer2D::DrawQuad(&texture1, glm::vec3(vec / 2.0f, 0.0f), vec);
        Renderer2D::DrawQuad(&texture2, glm::vec3(vec / 2.0f, -0.1f));
        Renderer2D::DrawEntity(entity);
    }
    Renderer2D::EndScene();
}