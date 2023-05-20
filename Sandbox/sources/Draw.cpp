#include "Draw.h"

#include <glm\gtc\matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\quaternion.hpp>

struct Data
{
	std::shared_ptr<VertexArray> cubeVA;
	std::shared_ptr<VertexBuffer> cubeVB;
	std::shared_ptr<Shader> cubeShader;

	std::shared_ptr<VertexArray> lightVA;
	std::shared_ptr<VertexBuffer> lightVB;
	std::shared_ptr<Shader> lightShader;
};
static Data data;

void Draw::Init()
{
	float vertices[]
	{
		// position                 normal              texcoord
		-0.5f, -0.5f,  0.5f,   -1.0f, -1.0f,  1.0f,    0.0f, 0.0f, // 0 [FRONT] bottom left
		 0.5f, -0.5f,  0.5f,    1.0f, -1.0f,  1.0f,    1.0f, 0.0f, // 1 [FRONT] bottom right
		 0.5f,  0.5f,  0.5f,    1.0f,  1.0f,  1.0f,    1.0f, 1.0f, // 2 [FRONT] top right
		-0.5f,  0.5f,  0.5f,   -1.0f,  1.0f,  1.0f,    0.0f, 1.0f,	// 3 [FRONT] top left

		-0.5f, -0.5f, -0.5f,    -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, // 4 [BACK] bottom left
		 0.5f, -0.5f, -0.5f,     1.0f, -1.0f, -1.0f,    1.0f, 0.0f, // 5 [BACK] bottom right
		 0.5f,  0.5f, -0.5f,     1.0f,  1.0f, -1.0f,    1.0f, 1.0f, // 6 [BACK] top right
		-0.5f,  0.5f, -0.5f,    -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, // 7 [BACK] top left

		-0.5f,  0.5f,  0.5f,     -1.0f,  1.0f,  1.0f,    1.0f, 0.0f, // 8 [LEFT] top left	
		-0.5f,  0.5f, -0.5f,     -1.0f,  1.0f, -1.0f,    1.0f, 1.0f, // 9 [LEFT] top right
		-0.5f, -0.5f, -0.5f,     -1.0f, -1.0f, -1.0f,    0.0f, 1.0f, //10 [LEFT] bottom right
		-0.5f, -0.5f,  0.5f,     -1.0f, -1.0f,  1.0f,    0.0f, 0.0f, //11 [LEFT] bottom left

		 0.5f,  0.5f,  0.5f,      1.0f,  1.0f,  1.0f,    1.0f, 0.0f, //12 [RIGHT] top left	
		 0.5f,  0.5f, -0.5f,      1.0f,  1.0f, -1.0f,    1.0f, 1.0f, //13 [RIGHT] top right
		 0.5f, -0.5f, -0.5f,      1.0f, -1.0f, -1.0f,    0.0f, 1.0f, //14 [RIGHT] bottom right
		 0.5f, -0.5f,  0.5f,      1.0f, -1.0f,  1.0f,    0.0f, 0.0f, //15 [RIGHT] bottom left

		-0.5f,  0.5f,  0.5f,     -1.0f,  1.0f,  1.0f,     1.0f, 0.0f, //16 [TOP] top left
		 0.5f,  0.5f,  0.5f,      1.0f,  1.0f,  1.0f,     1.0f, 1.0f, //17 [TOP] top right
		 0.5f,  0.5f, -0.5f,      1.0f,  1.0f, -1.0f,     0.0f, 1.0f, //18 [TOP] bottom right
		-0.5f,  0.5f, -0.5f,     -1.0f,  1.0f, -1.0f,     0.0f, 0.0f,	//19 [TOP] bottom left

		-0.5f, -0.5f,  0.5f,     -1.0f, -1.0f,  1.0f,    1.0f, 0.0f, //20 [BOTTOM] top left
		 0.5f, -0.5f,  0.5f,      1.0f, -1.0f,  1.0f,    1.0f, 1.0f, //21 [BOTTOM] top right
		 0.5f, -0.5f, -0.5f,      1.0f, -1.0f, -1.0f,    0.0f, 1.0f, //22 [BOTTOM] bottom right
		-0.5f, -0.5f, -0.5f,     -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,	//23 [BOTTOM] bottom left
	};

	uint32_t indices[]
	{
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		22, 23, 20
	};

	// cube
	data.cubeVA = VertexArray::Create();
	data.cubeVB = VertexBuffer::Create(vertices, sizeof(vertices));
	data.cubeVB->SetLayout({
		{ShaderDataType::Float3, "aPosition"},
		{ShaderDataType::Float3, "aNormal"},
		{ShaderDataType::Float2, "aTexCoord"},
	});
	data.cubeVA->AddVertexBuffer(data.cubeVB);
	std::shared_ptr<IndexBuffer> cubeIB = IndexBuffer::Create(indices, sizeof(indices));
	data.cubeVA->SetIndexBuffer(cubeIB);
	data.cubeShader = Shader::Create("Resources/Shaders/Sandbox/default.glsl");
	data.cubeShader->Bind();


	float lightVertices[]
	{
		// position          
		-0.5f, -0.5f,  0.5f, // 0 [FRONT] bottom left
		 0.5f, -0.5f,  0.5f, // 1 [FRONT] bottom right
		 0.5f,  0.5f,  0.5f, // 2 [FRONT] top right
		-0.5f,  0.5f,  0.5f, // 3 [FRONT] top left
												 
		-0.5f, -0.5f, -0.5f, // 4 [BACK] bottom left
		 0.5f, -0.5f, -0.5f, // 5 [BACK] bottom right
		 0.5f,  0.5f, -0.5f, // 6 [BACK] top right
		-0.5f,  0.5f, -0.5f, // 7 [BACK] top left
												 
		-0.5f,  0.5f,  0.5f, // 8 [LEFT] top left	
		-0.5f,  0.5f, -0.5f, // 9 [LEFT] top right
		-0.5f, -0.5f, -0.5f, //10 [LEFT] bottom right
		-0.5f, -0.5f,  0.5f, //11 [LEFT] bottom left
												 
		 0.5f,  0.5f,  0.5f, //12 [RIGHT] top left	
		 0.5f,  0.5f, -0.5f, //13 [RIGHT] top right
		 0.5f, -0.5f, -0.5f, //14 [RIGHT] bottom right
		 0.5f, -0.5f,  0.5f, //15 [RIGHT] bottom left
												 
		-0.5f,  0.5f,  0.5f, //16 [TOP] top left
		 0.5f,  0.5f,  0.5f, //17 [TOP] top right
		 0.5f,  0.5f, -0.5f, //18 [TOP] bottom right
		-0.5f,  0.5f, -0.5f, //19 [TOP] bottom left
												 
		-0.5f, -0.5f,  0.5f, //20 [BOTTOM] top left
		 0.5f, -0.5f,  0.5f, //21 [BOTTOM] top right
		 0.5f, -0.5f, -0.5f, //22 [BOTTOM] bottom right
		-0.5f, -0.5f, -0.5f  //23 [BOTTOM] bottom left
	};

	// Light
	data.lightVA = VertexArray::Create();
	data.lightVB = VertexBuffer::Create(lightVertices, sizeof(lightVertices));
	data.lightVB->SetLayout({
		{ShaderDataType::Float3, "aPosition"},
		});
	data.lightVA->AddVertexBuffer(data.lightVB);
	data.lightVA->SetIndexBuffer(cubeIB);
	data.lightShader = Shader::Create("Resources/Shaders/Sandbox/lighting.glsl");
	data.lightShader->Bind();

	shaderMap["cube"] = data.cubeShader;
	shaderMap["light"] = data.lightShader;
}

void Draw::RenderCube()
{
	data.cubeVA->Bind();
	RenderCommand::DrawIndexed(data.cubeVA);
	data.cubeVA->Unbind();
}

void Draw::RenderLight()
{
	data.lightVA->Bind();
	RenderCommand::DrawIndexed(data.lightVA);
	data.lightVA->Unbind();
}

std::shared_ptr<Origin::Shader>& Draw::GetShader(std::string name)
{
	if (shaderMap.find(name) != shaderMap.end())
		return shaderMap.at(name);

	OGN_CORE_ASSERT(false);
}
