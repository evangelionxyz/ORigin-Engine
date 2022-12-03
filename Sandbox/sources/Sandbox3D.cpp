// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

//// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
//#include "Sandbox3D.h"
//#include "Origin\EntryPoint.h"
//#include <glm\gtc\matrix_transform.hpp>
//
//namespace Origin
//{
//	void Sandbox3D::OnAttach()
//	{
//		float vertices[]
//		{
//		 // position
//    -0.5f, -0.5f,  0.5f,  // 0 [FRONT] bottom left
//     0.5f, -0.5f,  0.5f,  // 1 [FRONT] bottom right
//     0.5f,  0.5f,  0.5f,  // 2 [FRONT] top right
//    -0.5f,  0.5f,  0.5f, 	// 3 [FRONT] top left
//
//    -0.5f, -0.5f, -0.5f,  // 4 [BACK] bottom left
//     0.5f, -0.5f, -0.5f,  // 5 [BACK] bottom right
//     0.5f,  0.5f, -0.5f,  // 6 [BACK] top right
//    -0.5f,  0.5f, -0.5f,  // 7 [BACK] top left
//
//    -0.5f,  0.5f,  0.5f,  // 8 [LEFT] top left
//    -0.5f,  0.5f, -0.5f,  // 9 [LEFT] top right
//    -0.5f, -0.5f, -0.5f,  //10 [LEFT] bottom right
//    -0.5f, -0.5f,  0.5f,  //11 [LEFT] bottom left
//
//     0.5f,  0.5f,  0.5f,  //12 [RIGHT] top left
//     0.5f,  0.5f, -0.5f,  //13 [RIGHT] top right
//     0.5f, -0.5f, -0.5f,  //14 [RIGHT] bottom right
//     0.5f, -0.5f,  0.5f,  //15 [RIGHT] bottom left
//
//		-0.5f,  0.5f,  0.5f,  //16 [TOP] top left
//     0.5f,  0.5f,  0.5f,  //17 [TOP] top right
//     0.5f,  0.5f, -0.5f,  //18 [TOP] bottom right
//    -0.5f,  0.5f, -0.5f, 	//19 [TOP] bottom left
//
//		-0.5f, -0.5f,  0.5f,  //20 [BOTTOM] top left
//     0.5f, -0.5f,  0.5f,  //21 [BOTTOM] top right
//     0.5f, -0.5f, -0.5f,  //22 [BOTTOM] bottom right
//    -0.5f, -0.5f, -0.5f, 	//23 [BOTTOM] bottom left
//		};
//
//		va = VertexArray::Create();
//		vb = VertexBuffer::Create(vertices, sizeof(vertices));
//
//		BufferLayout layout = {
//			{ ShaderDataType::Float3, "aPos"},
//		};
//
//		vb->SetLayout(layout);
//		va->AddVertexBuffer(vb);
//
//		uint32_t indices[]
//		{
//			0, 1, 2,
//			2, 3, 0,
//
//			4, 5, 6,
//			6, 7, 4,
//
//			8, 9, 10,
//			10, 11, 8,
//
//			12, 13, 14,
//			14, 15, 12,
//
//			16, 17, 18,
//			18, 19, 16,
//
//			20, 21, 22,
//			22, 23, 20
//		};
//
//		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, 6 * 6);
//		va->SetIndexBuffer(indexBuffer);
//
//		shader = Shader::Create("assets/shaders/Sandbox3DShader.glsl");
//	}
//
//	void Sandbox3D::OnUpdate(Timestep ts)
//	{
//		RenderCommand::ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//		RenderCommand::Clear();
//
//		shader->Bind();
//		va->Bind();
//
//		glDrawElements(GL_TRIANGLES, va->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
//
//		transform = glm::translate(glm::mat4(1.0f), glm::vec3(position))
//			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(1, 1, 1))
//			* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
//
//		shader->SetMatrix("uTransform", transform);
//
//		va->Unbind();
//		shader->Unbind();
//
//		rotation += increment * ts;
//	}
//}