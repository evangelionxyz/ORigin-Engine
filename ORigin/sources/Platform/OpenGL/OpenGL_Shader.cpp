// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGL_Shader.h"
#include "Origin/Utils/Time.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Origin {

  namespace Utils{
    static GLenum ShaderTypeFromString(const std::string& type, const std::string& filepath = std::string())
    {
      if (type == "vertex") return GL_VERTEX_SHADER;
      if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

      if (!filepath.empty())
        OGN_CORE_ERROR("Unkown Shader Type!: {}", filepath);
      else
        OGN_CORE_ERROR("Unkown Shader Type!");

      OGN_CORE_ASSERT(false, "");
      return 0;
    }

    static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
    {
      switch (stage)
      {
        case GL_VERTEX_SHADER: return shaderc_glsl_vertex_shader;
        case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
      }
      OGN_CORE_ASSERT(false);
      return (shaderc_shader_kind)0;
    }

    static const char* GLShaderStageToString(GLenum stage)
    {
      switch (stage)
      {
        case GL_VERTEX_SHADER:  return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
      }
      OGN_CORE_ASSERT(false);

      return nullptr;
    }

    static const char* GetCacheDirectory()
    {
      return "Resources/Cache/Shaders/OpenGL";
    }

    static void CreateCachedDirectoryIfNeeded()
    {
      std::string cachedDirectory = GetCacheDirectory();
      if (!std::filesystem::exists(cachedDirectory))
        std::filesystem::create_directories(cachedDirectory);
    }

    static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
    {
      switch(stage)
      {
        case GL_VERTEX_SHADER: return ".cached_opengl.vert";
        case GL_FRAGMENT_SHADER: return ".cached_opengl.frag";
      }

      OGN_CORE_ASSERT(false);
      return nullptr;
    }

    static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
    {
      switch (stage)
      {
        case GL_VERTEX_SHADER: return ".cached_vulkan.vert";
        case GL_FRAGMENT_SHADER: return ".cached_vulkan.frag";
      }

      OGN_CORE_ASSERT(false);
      return nullptr;
    }

    static const char* ShaderDataTypeToString(GLenum type)
    {
      switch (type)
      {
        case GL_VERTEX_SHADER: return "Vertex";
        case GL_FRAGMENT_SHADER:  return "Fragment";
      }
      return "";
    }

    static const char* ShaderDataTypeToString(ShaderType type)
    {
      switch (type)
      {
        case ShaderType::VERTEX: return "VERTEX";
        case ShaderType::FRAGMENT:  return "FRAGMENT";
      }
      return "";
    }
  }

  OpenGLShader::OpenGLShader(const std::string& filepath, bool enableSpirv, bool recompileSpirv)
    : m_Filepath(filepath), m_RendererID(0), m_EnableSpirv(enableSpirv), m_RecompileSPIRV(recompileSpirv)
  {
    OGN_CORE_TRACE("Trying to load Shader : {}", m_Filepath);

    if (enableSpirv)
    {
      Utils::CreateCachedDirectoryIfNeeded();

      std::string source = ReadFile(filepath);
      auto shaderSources = PreProcess(source);
      {
        Timer timer;
        CompileOrGetVulkanBinaries(shaderSources);
        CompileOrGetOpenGLBinaries();
        CreateSpirvProgram();
        OGN_CORE_TRACE("Shader Creation took {0} ms", timer.ElapsedMillis());
      }
    }
    else
    {
      m_ShaderSource = ParseShader(filepath);
      m_RendererID = CreateProgram(m_ShaderSource.VertexSources, m_ShaderSource.FragmentSources);
    }

    // extract shader file to string name
    size_t lastSlash = filepath.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    auto lastDot = filepath.rfind(".");
    auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
    m_Name = filepath.substr(lastSlash, count);
  }

  OpenGLShader::OpenGLShader(const std::string& name, const std::string& filepath)
    : m_Name(name), m_Filepath(filepath), m_RendererID(0)
  {
    m_ShaderSource = ParseShader(filepath);
    m_RendererID = CreateProgram(m_ShaderSource.VertexSources, m_ShaderSource.FragmentSources);
  }

  OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    : m_Name(name), m_RendererID(0)
  {
    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSrc;
    sources[GL_FRAGMENT_SHADER] = fragmentSrc;

    CompileOrGetVulkanBinaries(sources);
    CompileOrGetOpenGLBinaries();
    CreateSpirvProgram();
  }

  OpenGLShader::~OpenGLShader()
  {
    glDeleteProgram(m_RendererID);
  }

  void OpenGLShader::CreateSpirvProgram()
  {
    GLuint program = glCreateProgram();

    std::vector<GLuint> shaderIDs;
    for (auto&& [stage, spirv] : m_OpenGLSPIRV)
    {
      GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
      glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
      glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
      glAttachShader(program, shaderID);
      OGN_CORE_WARN("{0} Shader Attached", Utils::ShaderDataTypeToString(stage));
    }

    glLinkProgram(program);
    glValidateProgram(program);

    GLint isLinked;

    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    OGN_CORE_TRACE("Shader Linked Status : {0}", isLinked);

    if (isLinked == GL_FALSE)
    {
      GLint maxLength;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
      std::vector<GLchar> infolog(maxLength);
      glGetProgramInfoLog(program, maxLength, &maxLength, infolog.data());

      glDeleteProgram(program);

      for (auto id : shaderIDs) glDeleteShader(id);
    }

    for (auto id : shaderIDs)
    {
      glDetachShader(program, id);
      glDeleteShader(id);
    }

    m_RendererID = program;
  }

  std::string OpenGLShader::ReadFile(const std::string& filepath)
  {
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (in)
    {
      in.seekg(0, std::ios::end);
      size_t size = in.tellg();
      if (size != -1)
      {
        result.resize(size);
        in.seekg(0, std::ios::beg);
        in.read(&result[0], size);
      }
      else OGN_CORE_ERROR("Could not read from file '{0}'", filepath);
    }
    else OGN_CORE_ERROR("Could not open file");
    return result;
  }

  std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
  {
    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "// type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);

    while (pos != std::string::npos)
    {
      size_t eol = source.find_first_of("\r\n", pos);
			OGN_CORE_ASSERT(eol != std::string::npos, "Syntax error")
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			OGN_CORE_ASSERT(Utils::ShaderTypeFromString(type, m_Filepath), "Invalid shader type specified")

      size_t nextLinePos = source.find_first_of("\r\n", eol);
      OGN_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax Error")
      pos = source.find(typeToken, nextLinePos);
      shaderSources[Utils::ShaderTypeFromString(type, m_Filepath)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
    }
    return shaderSources;
  }

  void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
  {
    GLuint program = glCreateProgram();
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
    const bool optimize = true;
    if (optimize)
      options.SetOptimizationLevel(shaderc_optimization_level_performance);
    std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

    auto& shaderData = m_VulkanSPIRV;
    shaderData.clear();
    for (auto&& [stage, source] : shaderSources)
    {
      std::filesystem::path shaderFilepath = m_Filepath;
      std::filesystem::path cachedPath = cacheDirectory / (shaderFilepath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

      std::ifstream infile(cachedPath, std::ios::in | std::ios::binary);
      if (infile.is_open())
      {
        OGN_CORE_WARN("Get Vulkan {0} Shader Binaries", Utils::ShaderDataTypeToString(stage));
        infile.seekg(0, std::ios::end);
        auto size = infile.tellg();
        infile.seekg(0, std::ios::beg);
        auto& data = shaderData[stage];
        data.resize(size / sizeof(uint32_t));
        infile.read((char*)data.data(), size);
      }
      else
      {
        OGN_CORE_WARN("Compile Vulkan {0} Shader To Binaries", Utils::ShaderDataTypeToString(stage));
        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_Filepath.c_str());
        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
          auto& message = module.GetErrorMessage();
          OGN_CORE_ASSERT(false, message);
        }

        shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

        std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
        if (out.is_open())
        {
          auto& data = shaderData[stage];
          out.write((char*)data.data(), data.size() * sizeof(uint32_t));
          out.flush();
          out.close();
        }
      }
    }
    for (auto&& [stage, data] : shaderData)
      Reflect(stage, data);
  }

  void OpenGLShader::CompileOrGetOpenGLBinaries()
  {
    auto& shaderData = m_OpenGLSPIRV;
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    const bool optimize = true;
    if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_performance);
    std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();
    shaderData.clear();

    m_OpenGLSourceCode.clear();
    for (auto&& [stage, spirv] : m_VulkanSPIRV)
    {
      std::filesystem::path shaderFilepath = m_Filepath;
      std::filesystem::path cachedPath = cacheDirectory / (shaderFilepath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

      std::ifstream infile(cachedPath, std::ios::in | std::ios::binary);
      if (infile.is_open() && !m_RecompileSPIRV)
      {
        OGN_CORE_WARN("Get OpenGL {0} Shader Binaries", Utils::ShaderDataTypeToString(stage));
        infile.seekg(0, std::ios::end);
        auto size = infile.tellg();
        infile.seekg(0, std::ios::beg);

        auto& data = shaderData[stage];
        data.resize(size / sizeof(uint32_t));
        infile.read((char*)data.data(), size);
      }
      else
      {
        OGN_CORE_WARN("Compile OpenGL {0} Shader To Binaries", Utils::ShaderDataTypeToString(stage));
        spirv_cross::CompilerGLSL glslCompiler(spirv);
        m_OpenGLSourceCode[stage] = glslCompiler.compile();
        auto& source = m_OpenGLSourceCode[stage];

        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_Filepath.c_str());
        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
          OGN_CORE_ERROR(module.GetErrorMessage());
          OGN_CORE_ASSERT(false, "")
        }

        shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
        std::ofstream outfile(cachedPath, std::ios::out | std::ios::binary);
        if(outfile.is_open())
        {
          auto& data = shaderData[stage];
          outfile.write((char*)data.data(), data.size() * sizeof(uint32_t));
          outfile.flush();
          outfile.close();
        }
      }
    }
  }

  void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
  {
    spirv_cross::Compiler compiler(shaderData);
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    OGN_CORE_TRACE("OpenGLShader::Reflect - {0}", Utils::GLShaderStageToString(stage));
    OGN_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
    OGN_CORE_TRACE("    {0} resources", resources.sampled_images.size());

    if (resources.uniform_buffers.size())
    {
      OGN_CORE_TRACE("Uniform buffers:");
      for (const auto& resource : resources.uniform_buffers)
      {
        const auto& bufferType = compiler.get_type(resource.base_type_id);
        uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
        uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        int memberCount = bufferType.member_types.size();
        OGN_CORE_WARN ("     Name = {0}", resource.name);
        OGN_CORE_TRACE("     Size = {0}", bufferSize);
        OGN_CORE_TRACE("  Binding = {0}", binding);
        OGN_CORE_TRACE("  Members = {0}", memberCount);
      }
    }
  }

  ShaderProgramSources OpenGLShader::ParseShader(const std::string& filepath)
  {
    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
      if (line.find("//type ") != std::string::npos
        || line.find("// type ") != std::string::npos
        || line.find("#type ") != std::string::npos
        || line.find("# type ") != std::string::npos)
      {
        if (line.find("vertex") != std::string::npos
          || line.find("Vertex") != std::string::npos)
        {
          type = ShaderType::VERTEX;
          Utils::ShaderDataTypeToString(type);
        }
        else if (line.find("fragment") != std::string::npos
          || line.find("Fragment") != std::string::npos
          || line.find("pixel") != std::string::npos
          || line.find("Pixel") != std::string::npos)
        {
          type = ShaderType::FRAGMENT;
          Utils::ShaderDataTypeToString(type);
        }
      }
      else
      {
        ss[(int)type] << line << "\n";
      }
    }

    return { ss[0].str(), ss[1].str() };
  }

  GLuint OpenGLShader::CompileShader(GLuint type, const std::string& source)
  {
    GLuint shaderID = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);

    int success;
    char infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
      OGN_CORE_WARN("Failed to Compile \"{0}\" Shader Location: {1}",
        type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT", m_Filepath);

      glDeleteShader(shaderID);

      return 0;
    }

    OGN_CORE_TRACE("{} {} Succesfully Compiled", m_Name,
      type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
    return shaderID;
  }

  GLuint OpenGLShader::CreateProgram(const std::string& vertexShader, std::string& fragmentShader)
  {
    // Create Program
    GLuint shaderProgram = glCreateProgram();
    GLuint vShader = CompileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // Attach and Link Shader->Program
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);

    glValidateProgram(shaderProgram);

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    return shaderProgram;
  }

  void OpenGLShader::Bind() const
  {
    glUseProgram(m_RendererID);
  }
  void OpenGLShader::Unbind() const
  {
    glUseProgram(0);
  }

	void OpenGLShader::SetBool(const std::string& name, bool boolean)
	{
    SetUniformBool(name, boolean);
	}

	void OpenGLShader::SetFloat(const std::string& name, float v0)
	{
    SetUniformFloat(name, v0);
	}

	void OpenGLShader::SetFloat(const std::string& name, float v0, float v1)
	{
    SetUniformFloat(name, v0, v1);
	}

	void OpenGLShader::SetFloat(const std::string& name, float v0, float v1, float v2)
	{
    SetUniformFloat(name, v0, v1, v2);
	}

	void OpenGLShader::SetFloat(const std::string& name, float v0, float v1, float v2, float v3)
	{
    SetUniformFloat(name, v0, v1, v2, v3);
	}

	void OpenGLShader::SetInt(const std::string& name, int v0)
	{
    SetUniformInt(name, v0);
	}

	void OpenGLShader::SetInt(const std::string& name, int v0, int v1)
	{
    SetUniformInt(name, v0, v1);
	}

	void OpenGLShader::SetInt(const std::string& name, int v0, int v1, int v2)
	{
    SetUniformInt(name, v0, v1, v2);
	}

	void OpenGLShader::SetInt(const std::string& name, int v0, int v1, int v2, int v3)
	{
    SetUniformInt(name, v0, v1, v2, v3);
	}

  void  OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
  {
    SetUniformIntArray(name, values, count);
  }

	void OpenGLShader::SetVector(const std::string& name, const glm::vec2& vector2)
	{
    SetUniformVector(name, vector2);
	}

	void OpenGLShader::SetVector(const std::string& name, const glm::vec3& vector3)
	{
    SetUniformVector(name, vector3);
	}

	void OpenGLShader::SetVector(const std::string& name, const glm::vec4& vector4)
	{
    SetUniformVector(name, vector4);
	}

	void OpenGLShader::SetMatrix(const std::string& name, const glm::mat3& matrix3)
	{
    SetUniformMatrix(name, matrix3);
	}

	void OpenGLShader::SetMatrix(const std::string& name, const glm::mat4& matrix4)
	{
    SetUniformMatrix(name, matrix4);
	}


  // private
	// BOOLEAN UNIFORM
  void OpenGLShader::SetUniformBool(const std::string& name, bool boolean) {
    glUniform1i(GetUniformLocation(name), boolean);
  }

  // FLOAT UNIFORM
  void OpenGLShader::SetUniformFloat(const std::string& name, float v0) {
    glUniform1f(GetUniformLocation(name), v0);
  }
  void OpenGLShader::SetUniformFloat(const std::string& name, float v0, float v1) {
    glUniform2f(GetUniformLocation(name), v0, v1);
  }
  void OpenGLShader::SetUniformFloat(const std::string& name, float v0, float v1, float v2) {
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
  }
  void OpenGLShader::SetUniformFloat(const std::string& name, float v0, float v1, float v2, float v3) {
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
  }
  // INT UNIFORM
  void OpenGLShader::SetUniformInt(const std::string& name, int v0) {
    glUniform1i(GetUniformLocation(name), v0);
  }

  void OpenGLShader::SetUniformInt(const std::string& name, int v0, int v1) {
    glUniform2i(GetUniformLocation(name), v0, v1);
  }

  void OpenGLShader::SetUniformInt(const std::string& name, int v0, int v1, int v2) {
    glUniform3i(GetUniformLocation(name), v0, v1, v2);
  }

  void OpenGLShader::SetUniformInt(const std::string& name, int v0, int v1, int v2, int v3) {
    glUniform4i(GetUniformLocation(name), v0, v1, v2, v3);
  }

  void OpenGLShader::SetUniformIntArray(const std::string& name, int* values, uint32_t count)
  {
    glUniform1iv(GetUniformLocation(name), count, values);
  }

  // VECTOR UNIFORM
  void OpenGLShader::SetUniformVector(const std::string& name, const glm::vec2& vector2) {
    glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(vector2));
  }

  void OpenGLShader::SetUniformVector(const std::string& name, const glm::vec3& vector3) {
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(vector3));
  }

  void OpenGLShader::SetUniformVector(const std::string& name, const glm::vec4& vector4) {
    glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(vector4));
  }

  // MATRIX UNIFORM
  void OpenGLShader::SetUniformMatrix(const std::string& name, const glm::mat3& MATRIX) {
    glUniformMatrix3fv(GetUniformLocation(name), 1, false, glm::value_ptr(MATRIX));
  }

  void OpenGLShader::SetUniformMatrix(const std::string& name, const glm::mat4& MATRIX) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(MATRIX));
  }

  int OpenGLShader::GetUniformLocation(const std::string& name)
  {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
      return m_UniformLocationCache[name];

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
      OGN_CORE_WARN("{0} : WARNING UNIFORM \"{1}\" does not exists or uninitialized", m_Name, name);

    m_UniformLocationCache[name] = location;
    return location;
  }
}