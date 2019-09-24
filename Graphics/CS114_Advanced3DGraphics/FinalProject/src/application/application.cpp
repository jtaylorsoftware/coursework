/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "application/application.hpp"

#include "utility/model_loader.hpp"
#include "utility/model_types.hpp"

#include "engine/euler_camera.hpp"
#include "engine/timer.hpp"
#include "engine/model_library.hpp"

#include "engine/world_object.hpp"
#include "engine/mesh_renderer.hpp"
#include "engine/light_manager.hpp"

#include "engine/light.hpp"
#include "engine/material.hpp"
#include "engine/mesh.hpp"
#include "engine/renderer.hpp"

#include "opengl/shader.hpp"
#include "opengl/shader_program.hpp"
#include "opengl/texture.hpp"
#include "opengl/texture2d.hpp"
#include "opengl/cubemap.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <future>
#include <map> 
#include <sstream>
#include <vector>

// Get rid of WinAPI macros
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

// Useful global variables that manage some underlying OpenGL data.
// These would be in some uber-Renderer class object in a more polished version of the framework.
// For now they're global to make setup easier.

// Projection matrix and the default global camera
glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(1280) / 720, 0.1f, 100.f);
EulerCamera camera = EulerCamera(glm::vec3(0.f, 3.f, 6.f));

// OpenGL objects for geometry buffer
std::unique_ptr<Texture> geometryPosition;
std::unique_ptr<Texture> geometryNormal;
std::unique_ptr<Texture> geometryColor;
GLuint geomRenderBuffer;
GLuint geometryBuffer;

// OpenGL objects for light pass render target and shader
GLuint lightPassFbo;
GLuint lightPassRenderBuffer;
std::unique_ptr<Texture> lightPassColorTexture;
std::shared_ptr<ShaderProgram> lightPassShader;

// Vertex array object for fullscreen quad used for displaying the rendered scene after lighting
// and post-processing
GLuint screenQuadVao;
std::shared_ptr<ShaderProgram> postProcessProgram; // post-processing shader, only does gamma-correction for now

// Shader and handle to OpenGL texture for creating BRDF lookup texture as described by Epic Games
std::shared_ptr<ShaderProgram> brdfLutShader;
GLuint brdfLut = 0;


Application::Application() : isRunning(false), cursorVisible(false){
	WindowProperties properties;
	properties.width = 1280;
	properties.height = 720;
	properties.title = "OpenGL Project";
	window.reset(new Window(properties, this));

	int swapInterval = 1;
	window->SetSwapInterval(swapInterval);
	window->SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	timer.reset(new Timer(glfwGetTime));
}

Application::~Application(){
	window.release();
	glfwTerminate();
}

void Application::SetKeyboardState(int key, int scancode, int action, int mods){
	keys[key] = (unsigned short)action;
}

void Application::DoMouseInput(double mouseX, double mouseY){
	if (cursorVisible){
		return; // only move camera when the native mouse is hidden
	}

	WindowDimensions windowSize = window->GetSize();

	static float previousMouseX = windowSize.width / 2.0f;
	static float previousMouseY = windowSize.height / 2.0f;

	static bool isFirstMovement = true;
	if (isFirstMovement){
		previousMouseX = (float)mouseX;
		previousMouseY = (float)mouseY;
		isFirstMovement = false;
	}

	camera.OffsetYawFromCurrent(glm::radians(static_cast<float>(mouseX - previousMouseX)));
	camera.OffsetPitchFromCurrent(glm::radians(static_cast<float>(mouseY - previousMouseY)));
	previousMouseX = static_cast<float>(mouseX);
	previousMouseY = static_cast<float>(mouseY);

	camera.Rotate(camera.GetPitch(), camera.GetYaw());
}

void Application::DoFramebufferResize(int width, int height){ // Doesn't want to work properly so Window's resize is disabled
	glViewport(0, 0, width, height);
	projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 100.f);

	glTextureStorage2D(geometryPosition->GetId(), 1, GL_RGBA16F, width, height);
	glTextureStorage2D(geometryNormal->GetId(), 1, GL_RGBA16F, width, height);
	glTextureStorage2D(geometryColor->GetId(), 1, GL_RGBA16F, width, height);
	glNamedRenderbufferStorage(geomRenderBuffer, GL_DEPTH_COMPONENT, width, height);

	glTextureStorage2D(lightPassColorTexture->GetId(), 1, GL_RGBA16F, width, height);
	glNamedRenderbufferStorage(lightPassRenderBuffer, GL_DEPTH_COMPONENT, width, height);


	glNamedBufferSubData(Renderer::GetVPUniformBlock(), 0, sizeof(glm::mat4), glm::value_ptr(projection));
}

GLuint CreateQuad(){
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	GLuint vbo;
	glCreateBuffers(1, &vbo);
	glNamedBufferData(vbo, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	GLuint vao;
	glCreateVertexArrays(1, &vao);
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(float) * 4);

	glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 0, 0);

	glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2);
	glVertexArrayAttribBinding(vao, 1, 0);

	glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexArrayAttrib(vao, 1);

	return vao;
}

void RenderQuad(GLuint vao){
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

// Utility function to set up render to texture targets in an identical way every time
Texture* CreateTextureStorage(TextureStorageFormat format, GLint filter){
	Texture* texture = new Texture2d();

	texture->AllocateStorage(format);

	texture->SetParameter(GL_TEXTURE_MIN_FILTER, filter);
	texture->SetParameter(GL_TEXTURE_MAG_FILTER, filter);

	return texture;
}

// Utility function to set up renderbuffers in the same way every time
GLuint CreateFrameBufferRenderbuffer(int width, int height){
	GLuint renderBuffer;
	glCreateRenderbuffers(1, &renderBuffer);
	glNamedRenderbufferStorage(renderBuffer, GL_DEPTH_COMPONENT, width, height);
	return renderBuffer;
}

void Application::DoKeyboardInput(){
	if (keys[GLFW_KEY_W] == GLFW_PRESS || keys[GLFW_KEY_W] == GLFW_REPEAT){
		camera.MoveForward();
	}
	if (keys[GLFW_KEY_S] == GLFW_PRESS || keys[GLFW_KEY_S] == GLFW_REPEAT){
		camera.MoveBackward();
	}
	if (keys[GLFW_KEY_A] == GLFW_PRESS || keys[GLFW_KEY_A] == GLFW_REPEAT){
		camera.StrafeLeft();
	}
	if (keys[GLFW_KEY_D] == GLFW_PRESS || keys[GLFW_KEY_D] == GLFW_REPEAT){
		camera.StrafeRight();
	}
	if (keys[GLFW_KEY_ESCAPE] == GLFW_PRESS){
		window->FlagWindowToClose();
		isRunning = false;
	}
	if (keys[GLFW_KEY_TAB] == GLFW_PRESS){
		keys[GLFW_KEY_TAB] = 0; // clear state ("toggle" functionality)
		if (!cursorVisible){
			window->SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cursorVisible = true;
		} else{
			window->SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cursorVisible = false;
		}
	}
}

void Application::InitShaders(){
	// Preload shaders into the static ShaderProgram map member

	ShaderProgram::CreateFromFile("pbr_geometry_buffer");
	ShaderProgram::CreateFromFile("shadow_map");
	ShaderProgram::CreateFromFile("cubemap");
	ShaderProgram::CreateFromFile("convolve_cubemap");
	ShaderProgram::CreateFromFile("prefilter_cubemap");


	lightPassShader = ShaderProgram::CreateFromFile("pbr_light_pass");
	postProcessProgram = ShaderProgram::CreateFromFile("post_process");
	brdfLutShader = ShaderProgram::CreateFromFile("brdf_lut");
}

// Utility function to try loading models from the .model file, falling back to .fbx if necessary
void LoadModel(const std::string& name, const std::string& basePath){
	RawModel* model = ModelLoader::LoadModelFromBinaryFile(basePath + name + ".model");
	if (model){
		ModelLibrary::LoadModel(name, model);
	} else{
		model = ModelLoader::LoadModelFromExternalFileFormat(basePath + name + ".fbx");
		if (model){
			ModelLoader::SaveToBinaryFile(model, basePath + name + ".model");
			ModelLibrary::LoadModel(name, model);
		}
	}
}

void Application::InitModels(){
	// Preload models used in the scene
	LoadModel("ground_wet_mud", "../assets/models/ground/");
	LoadModel("SM_MiniAmp", "../assets/models/MiniAmp/");
	LoadModel("silver_sphere", "../assets/models/primitives/");
	LoadModel("gold_sphere", "../assets/models/primitives/");
	LoadModel("plastic_sphere", "../assets/models/primitives/");
	LoadModel("wood_sphere", "../assets/models/primitives/");
}

void Application::InitLights(){
	// Initialize the lights stored in the static LightManager class

	LightManager::Init();

	constexpr int numLights = 4;
	glm::vec4 positions[] =
	{
		glm::vec4(-7.f,  8.f,  -7.f, 1.f),
		glm::vec4(-7.f, 6.f, 7.0f, 1.f),
		glm::vec4(7.f,  4.f, -7.f, 1.f),
		glm::vec4(7.f,  7.f,  7.f, 1.f)
	};

	glm::vec4 colors[] =
	{
		glm::vec4(1.f, 0.f, 0.f, 1.f),
		glm::vec4(0.f, 1.f, 0.f, 1.f),
		glm::vec4(0.f, 0.f, 1.f, 1.f),
		glm::vec4(1.f, 1.f, 1.f, 1.f),
	};

	for (int i = 0; i < numLights; ++i){
		PointLight pointLight;
		pointLight.position = positions[i];
		pointLight.color = glm::pow(colors[i], glm::vec4(2.2f));
		LightManager::AddPointLight(pointLight);
	}

	{
		DirectionalLight sun;
		sun.position = glm::vec4(cos(glm::radians(75.f)) * 10.0f, 10.0f, -cos(glm::radians(15.f)) * 10.0f, 1.f);
		sun.direction = glm::normalize(glm::vec4(0.f, 0.f, 0.f, 1.f) - sun.position);
		sun.color = glm::pow(glm::vec4(1.f), glm::vec4(2.2f));
		LightManager::SetDirectionalLight(sun);
	}
	LightManager::UpdateLightBuffer();
}

void Application::InitWorldObjects(){
	// Initialize the WorldObjects by positioning them in the scene

	glm::vec3 groundTranslations[] = {
		{-5.f, -0.1f, -5.f},
		{0.f, -0.1f, 5.f},
		{-5.f, -0.1f, 5.f},

		{-5.f, -0.1f, 0.f},
		{0.f, -0.1f, 0.f},
		{5.f, -0.1f, 0.f},

		{5.f, -0.1f, -5.f},
		{0.f, -0.1f, -5.f},
		{5.f, -0.1f, 5.f},
	};

	// Make the ground
	{
		for (int i = 0; i < 9; ++i){
			WorldObject* ground = new WorldObject();
			ground->SetModel(ModelLibrary::GetModel("ground_wet_mud"));
			for (auto& mesh : ground->GetModel()->meshes){
				mesh->material.shaderProgram = ShaderProgram::CreateFromFile("pbr_geometry_buffer");
			}
			ground->Init();

			ground->Translate(groundTranslations[i]);
			ground->Scale(5.f);
			deferredRenderObjects.push_back(std::unique_ptr<WorldObject>(ground));
		}
	}

	// Add 4 spheres of the different materials
	glm::vec3 sphereTranslations[] = {
		{-6.f,  3.f,  -10.f},
		{ -3.f,  3.f,  -10.f},
		{ 3.f,   3.f,   -10.f}, 
		{ 6.f,  3.f,   -10.f} 
	};
	std::string sphereNames[] = {
		"silver_sphere", "gold_sphere", "wood_sphere", "plastic_sphere",
	};
	{
		for (int i = 0; i < 4; ++i){
			WorldObject* sphere = new WorldObject();
			sphere->SetModel(ModelLibrary::GetModel(sphereNames[i]));
			for (auto& mesh : sphere->GetModel()->meshes){
				mesh->material.shaderProgram = ShaderProgram::CreateFromFile("pbr_geometry_buffer");
			}
			sphere->Init();

			sphere->Translate(sphereTranslations[i]);
			sphere->Scale(1.5f);
			deferredRenderObjects.push_back(std::unique_ptr<WorldObject>(sphere));
		}
	}

	// Add the miniamp
	{
		WorldObject* miniamp = new WorldObject();
		miniamp->SetModel(ModelLibrary::GetModel("SM_MiniAmp"));
		for (auto& mesh : miniamp->GetModel()->meshes){
			mesh->material.shaderProgram = ShaderProgram::CreateFromFile("pbr_geometry_buffer");
		}
		miniamp->Init();

		miniamp->Scale(0.05f);
		deferredRenderObjects.push_back(std::unique_ptr<WorldObject>(miniamp));
	}

}

void Application::InitGeometryBuffer(){
	// Set up the render targets for the geometry framebuffer 

	glCreateFramebuffers(1, &geometryBuffer);

	auto windowSize = window->GetSize();
	TextureStorageFormat format;
	format.width = windowSize.width;
	format.height = windowSize.height;
	format.internalFormat = GL_RGBA16F;
	format.levels = 1;
	// need to use 4 component float values for position, normal
	geometryPosition.reset(CreateTextureStorage(format, GL_NEAREST));
	geometryNormal.reset(CreateTextureStorage(format, GL_NEAREST));
	format.internalFormat = GL_RGBA8; // use 24 bit rgba for color
	geometryColor.reset(CreateTextureStorage(format, GL_NEAREST));

	constexpr int numAttachments = 3;
	GLenum attachments[numAttachments] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glNamedFramebufferTexture(geometryBuffer, attachments[0], geometryPosition->GetId(), 0);
	glNamedFramebufferTexture(geometryBuffer, attachments[1], geometryNormal->GetId(), 0);
	glNamedFramebufferTexture(geometryBuffer, attachments[2], geometryColor->GetId(), 0);

	glNamedFramebufferDrawBuffers(geometryBuffer, numAttachments, attachments);

	geomRenderBuffer = CreateFrameBufferRenderbuffer(windowSize.width, windowSize.height);
	glNamedFramebufferRenderbuffer(geometryBuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, geomRenderBuffer);
	glNamedFramebufferReadBuffer(geometryBuffer, GL_NONE);

	GLenum fboStatus = glCheckNamedFramebufferStatus(geometryBuffer, GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "FBO incomplete\n";
		exit(-1);
	}
}

void Application::InitLightPassFramebuffer(){
	// Set up the render target for the light pass framebuffer and shader

	auto windowSize = window->GetSize();
	TextureStorageFormat fboFormat;
	fboFormat.width = windowSize.width;
	fboFormat.height = windowSize.height;
	fboFormat.internalFormat = GL_RGBA16F;
	fboFormat.levels = 1;
	lightPassColorTexture.reset(CreateTextureStorage(fboFormat, GL_LINEAR));

	lightPassRenderBuffer = CreateFrameBufferRenderbuffer(windowSize.width, windowSize.height);

	glCreateFramebuffers(1, &lightPassFbo);
	glNamedFramebufferTexture(lightPassFbo, GL_COLOR_ATTACHMENT0, lightPassColorTexture->GetId(), 0);
	glNamedFramebufferRenderbuffer(lightPassFbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, lightPassRenderBuffer); // won't need to sample depth later

	GLenum fboStatus = glCheckNamedFramebufferStatus(lightPassFbo, GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "FBO incomplete\n";
		exit(-1);
	}
}

void Application::InitSkybox(){
	// Initialize the static Cubemap class

	std::string rootPath = "../assets/cubemaps/hw_sahara/sahara_";
	std::vector<std::string> textures = {
		rootPath + "rt.tga",
		rootPath + "lf.tga",
		rootPath + "up.tga",
		rootPath + "dn.tga",
		rootPath + "bk.tga",
		rootPath + "ft.tga"
	};
	Cubemap::CreateFromTextures(textures);
}

void Application::InitBrdfLut(){
	// Initialize the render target and framebuffer for the BRDF lookup texture
	GLint viewportDims[4];
	glGetIntegerv(GL_VIEWPORT, viewportDims);

	GLuint fbo = 0;
	GLuint rbo = 0;
	glCreateFramebuffers(1, &fbo);
	glCreateRenderbuffers(1, &rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindRenderbuffer(GL_FRAMEBUFFER, rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glCreateTextures(GL_TEXTURE_2D, 1, &brdfLut);
	glBindTexture(GL_TEXTURE_2D, brdfLut);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

	glTextureParameteri(brdfLut, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(brdfLut, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(brdfLut, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(brdfLut, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLut, 0);

	GLenum fboStatus = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "FBO incomplete\n";
		exit(-1);
	}

	glViewport(0, 0, 512, 512);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Create the BRDF LUT by running the shader
	glUseProgram(brdfLutShader->GetId());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);


	RenderQuad(screenQuadVao);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);

	glViewport(viewportDims[0], viewportDims[1], viewportDims[2], viewportDims[3]);
}

void Application::RunDemo(){
	isRunning = true;

	// Set some desired OpenGL state
	auto windowSize = window->GetSize();
	glViewport(0, 0, windowSize.width, windowSize.height);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glClearColor(0.f, 0.f, 0.f, 1.f);

	std::cout << "Jeremy Taylor CS 114 Final Project Demo Application\n"
		"Controls:\n"
		"W,A,S,D control camera forward/sideways movement\n"
		"Mouse movement controls the camera forward direction\n" << std::endl;
	std::cout <<
		"Once the scene is loaded, you may need to left click once to ensure"
		" that the window is fully focused\n" << std::endl;

	std::cout << "Initializing scene data..." << std::endl;

	screenQuadVao = CreateQuad();

	std::cout << "Initializing shaders..." << std::endl;
	InitShaders();

	std::cout << "Done" << std::endl;
	std::cout << "Initializing models..." << std::endl;

	InitModels();

	std::cout << "Done" << std::endl;
	std::cout << "Initializing lights..." << std::endl;

	InitLights();

	std::cout << "Done" << std::endl;
	std::cout << "Initializing world objects..." << std::endl;

	InitWorldObjects();

	std::cout << "Done" << std::endl;
	std::cout << "Initializing framebuffer objects..." << std::endl;

	InitGeometryBuffer();
	InitLightPassFramebuffer();

	std::cout << "Done" << std::endl;
	std::cout << "Initializing skybox..." << std::endl;

	InitSkybox();

	std::cout << "Done" << std::endl;
	std::cout << "Initializing uniform buffers..." << std::endl;

	// Set the projection and view data
	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::GetVPUniformBlock());
	glNamedBufferSubData(Renderer::GetVPUniformBlock(), 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glNamedBufferSubData(Renderer::GetVPUniformBlock(), 64, sizeof(glm::mat4), glm::value_ptr(camera.GetView()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	std::cout << "Done" << std::endl;
	std::cout << "Initializing diffuse irradiance map..." << std::endl;

	Cubemap::Convolve();

	std::cout << "Done" << std::endl;
	std::cout << "Initializing specular irradiance map..." << std::endl;

	Cubemap::Prefilter();

	std::cout << "Done" << std::endl;
	std::cout << "Initializing BRDF lookup texture..." << std::endl;

	InitBrdfLut();

	std::cout << "Done" << std::endl;

	window->Show();

	// Used to help calculate FPS
	static double lastDisplay = timer->GetCurrentTime() * 1000.0;
	static int frames = 0;

	while (window->IsOpen()){
		// Process input
		glfwPollEvents();

		// Update
		timer->Tick();
		DoKeyboardInput();


		++frames;
		// Calculate seconds/frame
		if (timer->GetCurrentTime() * 1000.0 - lastDisplay * 1000.0 >= 1.0){
			lastDisplay += 1.0;
			double secondsPerFrame = 1.0 / static_cast<double>(frames);
			window->SetTitle(std::string("s/Frame: ") + std::to_string(secondsPerFrame));
			frames = 0;
		}


		// Update the Uniform Block data for View
		glBindBuffer(GL_UNIFORM_BUFFER, Renderer::GetVPUniformBlock());
		glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(glm::mat4), glm::value_ptr(camera.GetView()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		camera.Update(timer->GetDeltaTime());

		for (auto& object : deferredRenderObjects){
			object->Update(timer->GetDeltaTime());
		}
		for (auto& object : forwardRenderObjects){
			object->Update(timer->GetDeltaTime());
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//--- Do shadow mapping
		glCullFace(GL_FRONT);
		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, Renderer::shadowMapFramebuffer); // only shadow mapping from the directional light
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindTextureUnit(0, Renderer::shadowMapTexture->GetId());
		for (auto& object : deferredRenderObjects){
			object->RenderToShadowMap();
		}
		// disable shadow map
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCullFace(GL_BACK);
		

		windowSize = window->GetSize();
		glViewport(0, 0, windowSize.width, windowSize.height); // get back to scene viewport size

		// Render into geometry buffer
		glBindFramebuffer(GL_FRAMEBUFFER, geometryBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto& object : deferredRenderObjects){
			object->Render();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // disable geometry buffer

		glBindFramebuffer(GL_FRAMEBUFFER, lightPassFbo); // render into light pass FBO

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Do lighting pass
		glProgramUniform3fv(lightPassShader->GetId(), 0, 1, glm::value_ptr(camera.GetPosition())); // view position
		glBindTextureUnit(0, geometryPosition->GetId());
		glBindTextureUnit(1, geometryNormal->GetId());
		glBindTextureUnit(2, geometryColor->GetId());
		glBindTextureUnit(5, brdfLut);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Cubemap::GetConvolvedMap());
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Cubemap::GetPrefilteredMap());
		glUseProgram(lightPassShader->GetId());
		RenderQuad(screenQuadVao); // render deferred scene

		// Blit depth info to the light pass buffer
		glBlitNamedFramebuffer(geometryBuffer, lightPassFbo,
			0, 0, windowSize.width, windowSize.height,
			0, 0, windowSize.width, windowSize.height,
			GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		// Render objects in forward-pass, in this program only a Cubemap
		Cubemap::Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // disable light pass FBO


		// Post process step, draw to default framebuffer
		glUseProgram(postProcessProgram->GetId());
		glBindTextureUnit(0, lightPassColorTexture->GetId());
		RenderQuad(screenQuadVao); // render final image

		window->SwapBuffers();
	}
}

void Application::Run(){
	RunDemo();
}

bool Application::IsRunning(){
	return isRunning;
}
