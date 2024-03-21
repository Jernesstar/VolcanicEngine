#include <OpenGL/Shader.h>

#include <Saddle/Core/Application.h>
#include <Saddle/Events/EventSystem.h>
#include <Saddle/Renderer/Mesh.h>
#include <Saddle/Renderer/StereographicCamera.h>
#include <Saddle/Renderer/CameraController.h>
#include <Saddle/Renderer/Renderer.h>

using namespace VolcaniCore;

class ModelDemo : public Application {
public:
    ModelDemo();

    void OnUpdate(TimeStep ts) override;

private:
    struct Vertex {
        glm::vec3 Position;
    };

    Vertex vertices[8] = 
    {
        { { -0.5f,  0.5f,  0.5 } }, // 0 Front Top Left
        { {  0.5f,  0.5f,  0.5 } }, // 1 Front Top Right
        { { -0.5f, -0.5f,  0.5 } }, // 2 Front Bottom Left
        { {  0.5f, -0.5f,  0.5 } }, // 3 Front Bottom Right

        { { -0.5f,  0.5f, -0.5 } }, // 4 Back Top Left 
        { {  0.5f,  0.5f, -0.5 } }, // 5 Back Top Right
        { { -0.5f, -0.5f, -0.5 } }, // 6 Back Bottom Left
        { {  0.5f, -0.5f, -0.5 } }, // 7 Back Bottom Right
    };

    uint32_t indices[36] =
    {
        0, 2, 3,
        3, 1, 0,

        5, 7, 6,
        6, 4, 5,

        4, 6, 2,
        2, 0, 4,

        1, 3, 7,
        7, 5, 1,

        4, 0, 1,
        1, 5, 4,

        7, 3, 2,
        2, 6, 7,
    };

    BufferLayout l1 =
    {
        { "a_Position", BufferDataType::Vec3, true },
    };

    IndexBuffer* index_buffer = new IndexBuffer(indices);

    VertexBuffer* light_buffer = new VertexBuffer(vertices, l1);
    VertexArray* light_array = new VertexArray(light_buffer, index_buffer);

    Shader m_Shader{ { "Sandbox/assets/shaders/Model.glsl.vert", "Sandbox/assets/shaders/Model.glsl.frag" } };
    Shader light_shader{ { "Sandbox/assets/shaders/Light.glsl.frag", "Sandbox/assets/shaders/Light.glsl.vert" } };

    glm::vec3 light_pos = { 1.2f, 1.0f, 2.0f }, light_color = { 1.0f, 1.0f, 1.0f };
    glm::mat4 light_model{ 1.0f };

    StereographicCamera m_Camera{ 90.0f, 0.1f, 100.0f, 1600, 900 };
    CameraController m_Controller{ m_Camera };

    Mesh* m_Mesh;
};

ModelDemo::ModelDemo()
{
    EventSystem::RegisterEventListener<KeyPressedEvent>(
    [](const KeyPressedEvent& event)
    {
        if(event.Key == Key::Escape)
            Application::Close();
    });
    EventSystem::RegisterEventListener<WindowResizedEvent>(
    [this](const WindowResizedEvent& event)
    {
        this->m_Camera.Resize(event.Width, event.Height);
    });

    light_model = glm::translate(light_model, light_pos);
    light_model = glm::scale(light_model, glm::vec3(0.2f));

    m_Camera.SetPosition({ 0.0f, 0.0f, 5.0f });

    m_Shader.Bind();
    m_Shader.SetMat4("u_Model", glm::mat4(1.0f));
    m_Shader.SetVec3("u_LightPosition", light_pos);
    m_Shader.SetVec3("u_LightColor", light_color);

    light_shader.Bind();
    light_shader.SetVec3("u_LightColor", light_color);
    light_shader.SetMat4("u_Model", light_model);

    m_Mesh = new Mesh();
    m_Mesh->LoadMesh("Sandbox/assets/models/backpack/backpack.obj");
}

void ModelDemo::OnUpdate(TimeStep ts)
{
    Renderer::Clear();

    m_Controller.OnUpdate(ts);

    light_shader.Bind();
    light_shader.SetMat4("u_ViewProj", m_Camera.GetViewProjection());
    Renderer::DrawIndexed(light_array);

    m_Shader.Bind();
    m_Shader.SetMat4("u_ViewProj", m_Camera.GetViewProjection());
    m_Shader.SetVec3("u_CameraPosition", m_Camera.GetPosition());
    Renderer::RenderMesh(m_Mesh);
}