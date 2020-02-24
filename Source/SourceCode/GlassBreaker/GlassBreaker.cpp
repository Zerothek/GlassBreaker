#include "GlassBreaker.h"

#include <vector>
#include <iostream>

#include <stb/stb_image.h>
#include <Core/Engine.h>


using namespace std;

int type = 1, got_hit = 0, wireframe = 0;
int glass_size_x = 10;
int glass_size_y = 20;
float ds = 0.05;
float movement_speed = 1.0f;
glm::vec3 object_pos = glm::vec3(ds * (float)glass_size_x / 2 + 0.05, ds * (float)glass_size_y / 2, 3);
float time_dilation = 1;

inline glm::mat4 RotateOX(float radians)
{
	return glm::transpose(
		glm::mat4(1, 0, 0, 0,
			0, cos(radians), -sin(radians), 0,
			0, sin(radians), cos(radians), 0,
			0, 0, 0, 1)
	);
}

float radians = 0;
glm::mat4 rotOXmat4 = RotateOX(radians);
GLfloat rotOX[16];

struct Particle
{
	glm::vec4 position;
	glm::vec4 speed;
	glm::vec4 initialPos;
	glm::vec4 initialSpeed;
	glm::vec4 type;
	//int type;

	Particle() {};

	Particle(const glm::vec4 &pos, const glm::vec4 &speed)
	{
		SetInitial(pos, speed);
	}

	void SetInitial(const glm::vec4 &pos, const glm::vec4 &speed)
	{
		position = pos;
		initialPos = pos;

		this->speed = speed;
		initialSpeed = speed;
	}

	void SetInitial(const glm::vec4 &pos, const glm::vec4 &speed, const  glm::vec4 &type)
	{
		position = pos;
		initialPos = pos;

		this->speed = speed;
		initialSpeed = speed;

		this->type = type;
	}
};

ParticleEffect<Particle> *particleEffect;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/World.cpp

GlassBreaker::GlassBreaker()
{
}

GlassBreaker::~GlassBreaker()
{
}

void GlassBreaker::ResetScene() 
{
	unsigned int nrParticles = glass_size_x * glass_size_y * 2;

	auto particleSSBO = particleEffect->GetParticleBuffer();
	Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

	float pOX = 0;
	float pOY = 0;

	for (unsigned int i = 0; i < glass_size_y; i++)
	{
		pOY += ds + 0.0001;
		for (unsigned int j = 0; j < glass_size_x; j++) {
			pOX += ds + 0.001;

			glm::vec4 pos(1);
			pos.x = pOX;
			pos.y = pOY;

			/*glm::vec4 pos(1);
			pos.x = (rand() % cubeSize - hSize) / 10.0f;
			pos.y = (rand() % cubeSize - hSize) / 10.0f;
			pos.z = (rand() % cubeSize - hSize) / 10.0f;*/

			glm::vec4 type1(1);
			glm::vec4 type2(-1);
			type1.z = rand() % 45;
			type1.y = type1.z;
			type2.z = rand() % 45;
			type2.y = type2.z;

			float magnitude = i, mag2 = j;
			if (i > glass_size_y / 2) {
				magnitude = glass_size_y - i - 1;
			}

			if (j > glass_size_x / 2) {
				mag2 = glass_size_x - j - 1;
			}

			magnitude += 1;
			magnitude /= 10;
			magnitude += (rand() % 100) / 1000;

			glm::vec4 speed(0);
			//speed.x = (rand() % 20 - 10) / 10.0f;
			//speed.z = -(rand() % 20) / 10.0f - magnitude;
			float rd = rand() % 100;
			speed.z = ((-magnitude - mag2) / 2 - rd / 100.0) / 2;

			//speed.y = rand() % 2 + 2.0f;

			data[i * 2 * glass_size_x + 2 * j].SetInitial(pos, speed, type1);

			rd = rand() % 100;
			speed.z = ((-magnitude - mag2) / 2 - rd / 100.0) / 2;
			pos.x += ds + 0.0005;
			//pos.z *= -1;

			data[i * 2 * glass_size_x + 2 * j + 1].SetInitial(pos, speed, type2);
		}

		pOX = 0;
	}
	particleSSBO->SetBufferData(data);

	got_hit = 0;
	wireframe = -1;
	movement_speed = 1.0f;
	object_pos = glm::vec3(ds * (float)glass_size_x / 2 + 0.05, ds * (float)glass_size_y / 2, 3);
	time_dilation = 1;
}

void GlassBreaker::Init()
{
	for (int i = 0; i < 16; i++) {
		rotOX[i] = rotOXmat4[i / 4][i % 4];
	}

	auto camera = GetSceneCamera();
	camera->SetPositionAndRotation(glm::vec3(ds * (float)glass_size_x / 2 + 0.05, ds * (float)glass_size_y / 2, -3), glm::quat(glm::vec3(0, 180 * TO_RADIANS, 0)));
	camera->Update();

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Load textures
	{
		TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES, "random.jpg");
	}

	LoadShader("Simple", false);
	LoadShader("Particle");

	unsigned int nrParticles = glass_size_x * glass_size_y * 2;

	particleEffect = new ParticleEffect<Particle>();
	particleEffect->Generate(nrParticles, true);

	auto particleSSBO = particleEffect->GetParticleBuffer();
	Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

	float pOX = 0;
	float pOY = 0;

	for (unsigned int i = 0; i < glass_size_y; i++)
	{
		pOY += ds + 0.0001;
		for (unsigned int j = 0; j < glass_size_x; j++) {
			pOX += ds + 0.001;

			glm::vec4 pos(1);
			pos.x = pOX;
			pos.y = pOY;

			/*glm::vec4 pos(1);
			pos.x = (rand() % cubeSize - hSize) / 10.0f;
			pos.y = (rand() % cubeSize - hSize) / 10.0f;
			pos.z = (rand() % cubeSize - hSize) / 10.0f;*/

			glm::vec4 type1(1);
			glm::vec4 type2(-1);
			type1.z = rand() % 45;
			type1.y = type1.z;
			type2.z = rand() % 45;
			type2.y = type2.z;

			float magnitude = i, mag2 = j;
			if (i > glass_size_y / 2) {
				magnitude = glass_size_y - i - 1;
			}

			if (j > glass_size_x / 2) {
				mag2 = glass_size_x - j - 1;
			}

			magnitude += 1;
			magnitude /= 10;
			magnitude += (rand() % 100) / 1000;

			glm::vec4 speed(0);
			//speed.x = (rand() % 20 - 10) / 10.0f;
			//speed.z = -(rand() % 20) / 10.0f - magnitude;
			float rd = rand() % 100;
			speed.z = ((-magnitude - mag2) / 2 - rd / 100.0) / 2;

			//speed.y = rand() % 2 + 2.0f;

			data[i * 2 * glass_size_x + 2 * j].SetInitial(pos, speed, type1);

			rd = rand() % 100;
			speed.z = ((-magnitude - mag2) / 2 - rd / 100.0) / 2;
			pos.x += ds + 0.0005;
			//pos.z *= -1;

			data[i * 2 * glass_size_x + 2 * j + 1].SetInitial(pos, speed, type2);
		}

		pOX = 0;
	}
	particleSSBO->SetBufferData(data);

	std::string texturePath = RESOURCE_PATH::TEXTURES + "Cube/";
	cubeMapTextureID = UploadCubeMapTexture(
		texturePath + "posx.png",
		texturePath + "posy.png",
		texturePath + "posz.png",
		texturePath + "negx.png",
		texturePath + "negy.png",
		texturePath + "negz.png"
	);

	std::string shaderPath = "Source/SourceCode/GlassBreaker/Shaders/";
	// Create a shader program for rendering to texture
	{
		Shader *shader = new Shader("ShaderNormal");
		shader->AddShader(shaderPath + "Normal.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Normal.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}


void GlassBreaker::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}


void GlassBreaker::Update(float deltaTimeSeconds)
{
	{
		object_pos -= time_dilation * movement_speed * glm::vec3(0, 0, deltaTimeSeconds / 4);
		glm::mat4 model = glm::translate(glm::mat4(1), object_pos);
		model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
		RenderMesh(meshes["box"], shaders["Simple"], model);

		if (object_pos.z <= 1) {
			got_hit = 1;
		}
	}

	auto camera = GetSceneCamera();
	// draw the cubemap
	{
		Shader *shader = shaders["ShaderNormal"];
		shader->Use();

		glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(30));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		int loc_texture = shader->GetUniformLocation("texture_cubemap");
		glUniform1i(loc_texture, 0);

		meshes["box"]->Render();
	}

	radians = deltaTimeSeconds / 2;
	rotOXmat4 = RotateOX(radians);
	for (int i = 0; i < 16; i++) {
		rotOX[i] = rotOXmat4[i / 4][i % 4];
	}

	glLineWidth(3);

	/*glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);*/

	if (wireframe == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	{
		auto shader = shaders["Particle"];
		if (shader->GetProgramID())
		{
			shader->Use();
			TextureManager::GetTexture("random.jpg")->BindToTextureUnit(GL_TEXTURE0);
			particleEffect->Render(GetSceneCamera(), shader);
		}

		int location = glGetUniformLocation(shader->GetProgramID(), "radians");
		glUniform1f(location, radians);

		location = glGetUniformLocation(shader->GetProgramID(), "got_hit");
		glUniform1f(location, got_hit);

		location = glGetUniformLocation(shader->GetProgramID(), "wireframe");
		glUniform1f(location, wireframe);

		location = glGetUniformLocation(shader->GetProgramID(), "time_dilation");
		glUniform1f(location, time_dilation);

		int loc_camera = shader->GetUniformLocation("camera_position");
		auto cameraPosition = camera->transform->GetWorldPosition();
		glUniform3f(loc_camera, cameraPosition.x, cameraPosition.y, cameraPosition.z);
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	if (wireframe == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GlassBreaker::FrameEnd()
{
	//DrawCoordinatSystem();
}

unsigned int GlassBreaker::UploadCubeMapTexture(const std::string &posx, const std::string &posy, const std::string &posz, const std::string& negx, const std::string& negy, const std::string& negz)
{
	int width, height, chn;

	unsigned char* data_posx = stbi_load(posx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posy = stbi_load(posy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posz = stbi_load(posz.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negx = stbi_load(negx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negy = stbi_load(negy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negz = stbi_load(negz.c_str(), &width, &height, &chn, 0);

	// TODO - create OpenGL texture
	unsigned int textureID = 0;
	glGenTextures(1, &textureID);

	// TODO - bind the texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	float maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// TODO - load texture information for each face
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posz);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negz);


	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// free memory
	SAFE_FREE(data_posx);
	SAFE_FREE(data_posy);
	SAFE_FREE(data_posz);
	SAFE_FREE(data_negx);
	SAFE_FREE(data_negy);
	SAFE_FREE(data_negz);

	return textureID;
}

void GlassBreaker::LoadShader(std::string name, bool hasGeomtery)
{
	static std::string shaderPath = "Source/SourceCode/GlassBreaker/Shaders/";

	// Create a shader program for particle system
	{
		Shader *shader = new Shader(name.c_str());
		shader->AddShader((shaderPath + name + ".VS.glsl").c_str(), GL_VERTEX_SHADER);
		shader->AddShader((shaderPath + name + ".FS.glsl").c_str(), GL_FRAGMENT_SHADER);
		if (hasGeomtery)
		{
			shader->AddShader((shaderPath + name + ".GS.glsl").c_str(), GL_GEOMETRY_SHADER);
		}

		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/Window/InputController.h

void GlassBreaker::OnInputUpdate(float deltaTime, int mods)
{
	// treat continuous update based on input
};

void GlassBreaker::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_SPACE) {
		wireframe = (wireframe + 1) % 3;
	}

	if (key == GLFW_KEY_P) {
		time_dilation += 0.1;
	}

	if (key == GLFW_KEY_O) {
		time_dilation -= 0.1;
	}

	if (key == GLFW_KEY_R) {
		ResetScene();
	}
};

void GlassBreaker::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void GlassBreaker::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void GlassBreaker::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void GlassBreaker::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void GlassBreaker::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void GlassBreaker::OnWindowResize(int width, int height)
{
	// treat window resize event
}
