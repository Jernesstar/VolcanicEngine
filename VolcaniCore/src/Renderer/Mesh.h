#pragma once

namespace VolcaniCore {

class Mesh {
// public:
//     Mesh(const std::string& path = "");
//     ~Mesh();

//     void LoadMesh(const std::string& path);

//     uint32_t GetSubMeshCount() const { return m_SubMeshes.size(); }
//     const std::string& GetPath() const { return m_Path; }

// private:
//     enum BufferIndex {
//         Position,
//         TextureCoordinate,
//         Normal
//     };
//     struct SubMesh {
//         uint32_t BaseVertex = 0;
//         uint32_t BaseIndex = 0;
//         uint32_t IndexCount = 0;
//         uint32_t MaterialIndex = 0;
//     };

//     struct Material {
//         std::unique_ptr<Texture2D> Diffuse;
//         std::unique_ptr<Texture2D> Specular;

//         void Bind()
//         {
//             if(Diffuse) Diffuse->Bind(0);
//             if(Specular) Specular->Bind(1);
//         }
//     };

//     std::string m_Path;

//     std::vector<SubMesh> m_SubMeshes;
//     std::vector<Material> m_Materials;

//     std::vector<glm::vec3> m_Positions;
//     std::vector<glm::vec3> m_Normals;
//     std::vector<glm::vec2> m_TextureCoords;
//     std::vector<uint32_t> m_Indices;

//     std::unique_ptr<VertexBuffer> m_Buffers[3];
//     std::unique_ptr<IndexBuffer> m_IndexBuffer;
//     std::unique_ptr<VertexArray> m_VertexArray;

//     void Clear();
//     void LoadSubMesh(const aiMesh* mesh);
//     void LoadMaterial(const aiMaterial* material, const std::string& path, uint32_t index);
//     Texture2D* LoadTexture(const aiMaterial* material, const std::string& dir, aiTextureType type);

//     friend class Renderer;
};

}