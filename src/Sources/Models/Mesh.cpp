#include <vector>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

// GLEW
#include <GL/glew.h>

#include "../../Utils/GLUtils.hpp"
#include "../../Utils/Log.h"
#include "../../Headers/Material/Material.h"
#include "../../Headers/Model/Mesh.h"
#include "../../Headers/Types.h"

void Mesh::Build(std::vector<Vertex> verteces, std::vector<GLuint> indeces) {
	MeshObject<Vertex> mesh{verteces, indeces};
	m_GPU = CreateGLObjectFromMesh(mesh, vertexAttribList);
}

void Mesh::Render(const MeshRenderParams& p) {
	// -- Check if the model can be rendered --
	if (GetMaterial() == nullptr) {
		LOG("Corrupted material found");
		exit(1);
	}

	// -- Set shader input data --
	// Layout for model
	glBindVertexArray(GetVAO());
	// Material module
	Material::UploadMaterialToShader(p.progID, GetMaterial());

	// -- Draw call --
	glDrawElements(p.drawMode, GetVertexCount(), GL_UNSIGNED_INT, nullptr);

	// -- Restore initial OGL state --
	glBindVertexArray(0);
	Material::ClearMaterialFromShader();
}