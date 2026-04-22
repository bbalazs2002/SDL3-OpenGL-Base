#include "../Headers/MyApp.h"
#include "../Utils/SDL_GLDebugMessageCallback.h"
#include "../Utils/ProgramBuilder.h"
#include "../Utils/Log.h"

#include <imgui.h>
#include <iostream>
#include <string>
#include <sstream>
#include <memory>

MyApp::MyApp()
{
}
MyApp::~MyApp()
{
}

/////////////////////
// --- SHADERS --- //
/////////////////////
void MyApp::InitShaders()
{

	// models
	m_programModelID = glCreateProgram();
	ProgramBuilder{ m_programModelID }
		.ShaderStage(GL_VERTEX_SHADER, "src/Shaders/Models/Vert_Model.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "src/Shaders/Models/Frag_Model.frag")
		.Link();

	InitAxesShader();
	InitSkyboxShader();
}
void MyApp::CleanShaders()
{
	glDeleteProgram(m_programModelID);
	m_programModelID = 0;

	CleanSkyboxShader();
	CleanAxesShader();
}
void MyApp::InitSkyboxShader() {
	m_programSkyboxID = glCreateProgram();
	ProgramBuilder{ m_programSkyboxID }
		.ShaderStage(GL_VERTEX_SHADER, "src/Shaders/Skybox/Vert_skybox.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "src/Shaders/Skybox/Frag_skybox_skeleton.frag")
		.Link();
}
void MyApp::CleanSkyboxShader() {
	glDeleteProgram(m_programSkyboxID);
	m_programSkyboxID = 0;
}
void MyApp::InitAxesShader()
{
	m_programAxesID = glCreateProgram();
	ProgramBuilder{ m_programAxesID }
		.ShaderStage(GL_VERTEX_SHADER, "src/Shaders/Axes/Vert_axes.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "src/Shaders/Axes/Frag_PosCol.frag")
		.Link();
}
void MyApp::CleanAxesShader()
{
	glDeleteProgram(m_programAxesID);
	m_programAxesID = 0;
}

//////////////////////
// --- GEOMETRY --- //
//////////////////////
void MyApp::InitGeometry()
{

	// Equinox from .obj file with .mtl
	m_model = std::make_unique<Model>(
		ModelParams{
			ShaderProgramCollection{
				m_programModelID
			},
			"Equinox",
			true
		}
	);
	m_model.get()->SetObjPath("Assets/Equinox-render/Equinox.obj");
	m_model.get()->AddTransform(glm::transpose(glm::mat4{
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 5 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 1 }
		}
	));

	InitSkyboxGeometry();
}
void MyApp::CleanGeometry()
{
	CleanSkyboxGeometry();
}
void MyApp::InitSkyboxGeometry() {
	// skybox geometry
	MeshObject<glm::vec3> skyboxCPU =
	{
		std::vector<glm::vec3>
		{
			// back
			glm::vec3(-1, -1, -1),
			glm::vec3(1, -1, -1),
			glm::vec3(1,  1, -1),
			glm::vec3(-1,  1, -1),
			// front
			glm::vec3(-1, -1, 1),
			glm::vec3(1, -1, 1),
			glm::vec3(1,  1, 1),
			glm::vec3(-1,  1, 1),
		},

		std::vector<GLuint>
		{
			// back
			0, 1, 2,
			2, 3, 0,
			// front
			4, 6, 5,
			6, 4, 7,
			// left
			0, 3, 4,
			4, 3, 7,
			// right
			1, 5, 2,
			5, 6, 2,
			// bottom
			1, 0, 4,
			1, 4, 5,
			// top
			3, 2, 6,
			3, 6, 7,
		}
	};

	m_SkyboxGPU = CreateGLObjectFromMesh(skyboxCPU, { { 0, offsetof(glm::vec3, x), 3, GL_FLOAT } });
}
void MyApp::CleanSkyboxGeometry()
{
	CleanOGLObject(m_SkyboxGPU);
}

//////////////////////
// --- TEXTURES --- //
//////////////////////
void MyApp::InitTexture() {
	InitSkyboxTexture();
}
void MyApp::CleanTexture() {
	CleanSkyboxTexture();
}
void MyApp::InitSkyboxTexture() {
	// skybox texture
	static const char* skyboxFiles[6] = {
		"Assets/xpos.png",
		"Assets/xneg.png",
		"Assets/ypos.png",
		"Assets/yneg.png",
		"Assets/zpos.png",
		"Assets/zneg.png",
	};

	ImageRGBA images[6];
	for (int i = 0; i < 6; ++i)
	{
		images[i] = ImageFromFile(skyboxFiles[i], false);
	}

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_skyboxTextureID);
	glTextureStorage2D(m_skyboxTextureID, 1, GL_RGBA8, images[0].width, images[0].height);

	for (int face = 0; face < 6; ++face)
	{
		glTextureSubImage3D(m_skyboxTextureID, 0, 0, 0, face, images[face].width, images[face].height, 1, GL_RGBA, GL_UNSIGNED_BYTE, images[face].data());
	}

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}
void MyApp::CleanSkyboxTexture() {
	glDeleteTextures(1, &m_skyboxTextureID);
	m_skyboxTextureID = 0;
}

////////////////////////////
// --- INITIALIZATION --- //
////////////////////////////
void MyApp::SetupDebugCallback()
{
	// Enable and set the debug callback function if we are in debug context
	GLint context_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_FALSE);
		glDebugMessageCallback(SDL_GLDebugMessageCallback, nullptr);
	}
}
bool MyApp::Init()
{
	SetupDebugCallback();

	// Set a bluish clear color
	// glClear() will use this for clearing the color buffer.
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	InitShaders();
	InitTexture();
	InitGeometry();

	//
	// Other
	//
	// glEnable(GL_CULL_FACE);	 // Enable discarding the back-facing faces.
	glCullFace(GL_BACK);     // GL_BACK: facets facing away from camera, GL_FRONT: facets facing towards the camera
	glEnable(GL_DEPTH_TEST); // Enable depth testing. (for overlapping geometry)
	glDepthFunc(GL_LESS);

	// Camera
	m_camera.SetView(
		glm::vec3(0, 20, 20),	// From where we look at the scene - eye
		glm::vec3(0, 4, 0),		// Which point of the scene we are looking at - at
		glm::vec3(0, 1, 0)		// Upwards direction - up
	);
	m_cameraManipulator.SetCamera(&m_camera);

	return true;
}

////////////////////
// --- UPDATE --- //
////////////////////
void MyApp::Update(const SUpdateInfo& updateInfo)
{
	m_cameraManipulator.Update(updateInfo.DeltaTimeInSec);
	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;
}

////////////////////
// --- RENDER --- //
////////////////////
void MyApp::RenderAxes()
{
	glUseProgram(m_programAxesID);

	glm::mat4 axisWorld = glm::translate(m_camera.GetAt());
	glProgramUniformMatrix4fv(m_programAxesID, ul(m_programAxesID, "viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));
	glProgramUniformMatrix4fv(m_programAxesID, ul(m_programAxesID, "world"), 1, GL_FALSE, glm::value_ptr(axisWorld));

	// We always want to see it, regardless of whether there is an object in front of it
	glDisable(GL_DEPTH_TEST);

	glDrawArrays(GL_LINES, 0, 6);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}
void MyApp::RenderSkybox() {
	glUseProgram(m_programSkyboxID);

	glProgramUniform1i(m_programSkyboxID, ul(m_programSkyboxID, "skyboxTexture"), 1);
	glProgramUniformMatrix4fv(m_programSkyboxID, ul(m_programSkyboxID, "viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));
	glProgramUniformMatrix4fv(m_programSkyboxID, ul(m_programSkyboxID, "world"), 1, GL_FALSE, glm::value_ptr(glm::translate(m_camera.GetEye())));

	// Save the last Z-test, namely the relation by which we update the pixel.
	GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);

	// Now we use less-then-or-equal, because we push everything to the far clipping plane
	glDepthFunc(GL_LEQUAL);

	glBindTextureUnit(1, m_skyboxTextureID);
	glBindVertexArray(m_SkyboxGPU.vaoID);

	glDrawElements(GL_TRIANGLES, m_SkyboxGPU.count, GL_UNSIGNED_INT, nullptr);

	glDepthFunc(prevDepthFnc);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindTextureUnit(0, 0);
}
void MyApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render model
	const RenderParams rp{
		1.f, m_camera.GetEye(), 0, glm::ivec2(m_width, m_height), m_camera.GetViewProj()
	};
	m_model.get()->Render(rp);

	RenderSkybox();
	if (m_showAxes) {
		RenderAxes();
	}
}
void MyApp::RenderGUI()
{
	ImGui::Begin("Options window");
	{
		ImGui::Text("Render resolution %dx%d", m_width, m_height);
		ImGui::Checkbox("Show axes", &m_showAxes);
	}
	ImGui::End();
}

///////////////////
// --- CLEAN --- //
///////////////////
void MyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
	CleanTexture();
}

////////////////////////////
// --- EVENT HANDLERS --- //
////////////////////////////
void MyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{
	if (key.repeat == 0) // Triggers only once when held
	{
		if (key.key == SDLK_F5 && key.mod & SDL_KMOD_CTRL) // CTRL + F5
		{
			CleanShaders();
			InitShaders();
		}
		if (key.key == SDLK_F1) // F1
		{
			GLint polygonModeFrontAndBack[2] = {};
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
			glGetIntegerv(GL_POLYGON_MODE, polygonModeFrontAndBack); // Query the current polygon mode. It gives the front and back modes separately.
			GLenum polygonMode = (polygonModeFrontAndBack[0] != GL_FILL ? GL_FILL : GL_LINE); // Switch between FILL and LINE
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
			glPolygonMode(GL_FRONT_AND_BACK, polygonMode); // Set the new polygon mode
		}
	}
	m_cameraManipulator.KeyboardDown(key);
}
void MyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
	m_cameraManipulator.KeyboardUp(key);
}
void MyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	m_cameraManipulator.MouseMove(mouse);
}
void MyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
}
void MyApp::MouseUp(const SDL_MouseButtonEvent& mouse)
{
}
void MyApp::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
	m_cameraManipulator.MouseWheel(wheel);
}
void MyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	m_camera.SetAspect(static_cast<float>(_w) / _h);
	m_width = _w;
	m_height = _h;
}
void MyApp::OtherEvent(const SDL_Event& ev)
{
}