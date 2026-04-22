#pragma once

#include <memory>
#include <vector>
#include <filesystem>

#include "../../Utils/Log.h"
#include "../Types.h"
#include "../Material/Material.h"
#include "../Model/Mesh.h"

#include <tiny_obj_loader.h>

class ModelLoader {
public:
    static inline std::filesystem::path ResolveMTLSearchPath(const std::filesystem::path& objPath, const std::filesystem::path& mtlSearchPath)
    {
        // If mtlSearchPath is absolute, return
        if (mtlSearchPath.is_absolute())
            return mtlSearchPath;

        // Compute based on objPath
        std::filesystem::path objDir = objPath.parent_path();
        std::filesystem::path resolved = std::filesystem::weakly_canonical(objDir / mtlSearchPath);

        return resolved;
    }
    static inline std::filesystem::path ResolveTexturePath(const std::string& texPath, const std::filesystem::path& mtlSearchPath) {
        // normal texture path
        std::string pathName = texPath;
        std::replace(pathName.begin(), pathName.end(), '/', '\\');
        std::filesystem::path path = pathName;
        if (!path.is_absolute()) {
            path = mtlSearchPath / pathName;
        }
        return path;
    }

    static ModelLoaderReturn LoadFromOBJ(const std::filesystem::path& objPath,
        const std::filesystem::path& mtlSearchPath = "./") {

        ModelLoaderReturn retVal;

        // --- Generate and check .obj file path ---
        std::filesystem::path objFile = objPath.is_absolute() ?
            objPath :
            std::filesystem::path(PROJECT_ROOT) / objPath;
        if (!std::filesystem::exists(objFile)) {
            return retVal;
        }

        // --- Get .mtl search path ---
        std::filesystem::path absMtlSearchPath = ResolveMTLSearchPath(objFile, mtlSearchPath);

        // Configure tinyObjLoader
        tinyobj::ObjReaderConfig config;
        config.triangulate = true;
        config.mtl_search_path = absMtlSearchPath.string();

        // --- Read model from file ---
        LOG("Reading .obj file: ", objFile.generic_string());
        tinyobj::ObjReader reader;
        if (!reader.ParseFromFile(objFile.generic_string(), config)) {
            LOG_ERROR("TinyObjLoader error: ", reader.Error());
            throw std::runtime_error("Failed to load OBJ file.");
        }
        const auto& attrib = reader.GetAttrib();
        const auto& shapes = reader.GetShapes();
        const auto& materials = reader.GetMaterials();

        // --- Load materials ---
        LOG("Materials found: ", materials.size());

        if (materials.empty()) {
            retVal.materials.push_back(std::make_shared<Material>());
        }

        for (const auto& mat : materials) {
            auto material = std::make_shared<Material>();
            material->SetName(mat.name);
            material->SetDiffuseColor(glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]));
            material->SetSpecularColor(glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]));
            material->SetAmbientColor(glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]));
            material->SetShininess(mat.shininess);

            if (!mat.diffuse_texname.empty())
                material->SetDiffuseTex(Material::LoadTexture(ResolveTexturePath(mat.diffuse_texname, absMtlSearchPath)));
            if (!mat.specular_texname.empty())
                material->SetSpecularTex(Material::LoadTexture(ResolveTexturePath(mat.specular_texname, absMtlSearchPath)));
            if (!mat.emissive_texname.empty())
                material->SetEmissionTex(Material::LoadTexture(ResolveTexturePath(mat.emissive_texname, absMtlSearchPath)));
            if (!mat.normal_texname.empty())
                material->SetNormalTex(Material::LoadTexture(ResolveTexturePath(mat.normal_texname, absMtlSearchPath)));

            retVal.materials.push_back(material);
        }

        // --- Create Meshes ---
        for (const auto& shape : shapes) {
            std::unordered_map<int, std::vector<Vertex>> verticesPerMaterial;
            std::unordered_map<int, std::vector<GLuint>> indicesPerMaterial;

            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                int fv = shape.mesh.num_face_vertices[f];
                int mat_id = shape.mesh.material_ids.empty() ? -1 : shape.mesh.material_ids[f];

                if (mat_id < 0 || mat_id >= (int)retVal.materials.size()) mat_id = 0;

                for (size_t v = 0; v < fv; v++) {
                    Vertex vert{};
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    
                    // position
                    vert.position = glm::vec3(
                        attrib.vertices[3 * idx.vertex_index + 0],
                        attrib.vertices[3 * idx.vertex_index + 1],
                        attrib.vertices[3 * idx.vertex_index + 2]
                    );

                    // normal
                    if (idx.normal_index >= 0) {
                        vert.normal = glm::vec3(
                            attrib.normals[3 * idx.normal_index + 0],
                            attrib.normals[3 * idx.normal_index + 1],
                            attrib.normals[3 * idx.normal_index + 2]
                        );
                    }

                    // texture coordinates
                    if (idx.texcoord_index >= 0) {
                        vert.texcoord = glm::vec2(
                            attrib.texcoords[2 * idx.texcoord_index + 0],
                            attrib.texcoords[2 * idx.texcoord_index + 1]
                        );
                    }

                    auto& vList = verticesPerMaterial[mat_id];
                    vList.push_back(vert);
                    indicesPerMaterial[mat_id].push_back(static_cast<GLuint>(vList.size() - 1));
                }
                index_offset += fv;
            }

            for (auto& [mat_id, verts] : verticesPerMaterial) {
                auto& inds = indicesPerMaterial[mat_id];

                // Create a unique_ptr for the mesh
                auto mesh = std::make_unique<Mesh>();

                // shared_ptr assignment - multiple meshes can share the same material pointer
                mesh->SetMaterial(retVal.materials[mat_id]);

                mesh->Build(std::move(verts), std::move(inds));

                // Move the unique_ptr into the vector
                retVal.meshes.push_back(std::move(mesh));
            }
        }

        return retVal;
    }
};