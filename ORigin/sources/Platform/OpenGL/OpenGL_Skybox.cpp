#include "pch.h"
#include "OpenGL_Skybox.h"
#include "Origin\Renderer\Renderer.h"

#include "stb_image.h"

namespace origin
{
	OpenGLSkybox::OpenGLSkybox(const std::string& filepath, const std::string& format)
		: m_Filepath(filepath), m_ImageFormat(format)
	{
		float vertices[] = {

			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
		};

		m_VertexArray = VertexArray::Create();
		m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		BufferLayout layout = {{ShaderDataType::Float3, "aPos"}};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t* indices = new uint32_t[36];

		uint32_t Offset = 0;
		for (uint32_t i = 0; i < 36; i += 6)
		{
			indices[i + 0] = Offset + 0;
			indices[i + 1] = Offset + 1;
			indices[i + 2] = Offset + 2;

			indices[i + 3] = Offset + 2;
			indices[i + 4] = Offset + 3;
			indices[i + 5] = Offset + 0;

			Offset += 4;
		}

		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, 36);
		m_VertexArray->SetIndexBuffer(indexBuffer);
		delete[] indices;

		LoadTexture();
		m_Shader = Renderer::GetGShader("Skybox");
		m_Shader->Enable();
	}

	void OpenGLSkybox::Draw(const glm::mat4& viewProjection)
	{
		// Disable depth testing
		glDisable(GL_DEPTH_TEST);

		m_Shader->Enable();
		m_Shader->SetMatrix("uViewProjection", viewProjection);
		m_Shader->SetInt("uSkybox", m_TextureID);
		m_Shader->SetFloat("blurFactor", m_BlurFactor);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
		RenderCommand::DrawIndexed(m_VertexArray, 36);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		m_Shader->Disable();

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLSkybox::Draw(const SceneCamera& camera)
	{
		Draw(camera.GetViewProjection());
	}

	void OpenGLSkybox::Draw(const EditorCamera& camera)
	{
		Draw(camera.GetViewProjection());
	}

	void OpenGLSkybox::SetBlur(float blur)
	{
		m_BlurFactor = blur;
	}

	void OpenGLSkybox::LoadTexture()
	{
		std::vector<std::string> path
		{
			m_Filepath + "right" + m_ImageFormat,
			m_Filepath + "left" + m_ImageFormat,
			m_Filepath + "top" + m_ImageFormat,
			m_Filepath + "bottom" + m_ImageFormat,
			m_Filepath + "front" + m_ImageFormat,
			m_Filepath + "back" + m_ImageFormat
		};

		int width, height, bpp;
		stbi_uc* data = nullptr;

		stbi_set_flip_vertically_on_load(false);
		GLenum dataFormat = 0, internalFormat = 0;

		// set the file path
		for (uint32_t i = 0; i < path.size(); i++)
		{
			data = stbi_load(path[i].c_str(), &height, &width, &bpp, 0);
			switch (bpp)
			{
			case 3:
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;

				break;

			case 4:
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			}

			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
				OGN_CORE_TRACE("Texture \"{0}\" Successfully Loaded", path[i]);
				stbi_image_free(data);
			}
			else
			{
				stbi_image_free(data);
				OGN_CORE_ASSERT(false, "Failed to load Texture: {0}", path[i]);
			}
		}

		OGN_CORE_TRACE("Bits Per Pixel : {0}", bpp);
		OGN_CORE_TRACE("Internal Format: {0}, Data Format: {1}", internalFormat, dataFormat);

		// generate texture
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}