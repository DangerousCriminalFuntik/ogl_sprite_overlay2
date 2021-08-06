#include <iostream>
#include <chrono>
#include "sprite.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

sprite::sprite()
{
	auto vs = loadShader("sprite.vert", GL_VERTEX_SHADER);
	auto fs = loadShader("sprite.frag", GL_FRAGMENT_SHADER);
	m_program = createProgram({ vs, fs });

	glCreateProgramPipelines(1, &pipeline);
	glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, m_program);

	glCreateVertexArrays(1, &vao);

	GLint UniformBufferOffset{};
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
	blockSize = glm::max(GLint(sizeof(Transform)), UniformBufferOffset);

	glCreateBuffers(1, &ubo);
	glNamedBufferStorage(ubo, blockSize, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	g_spriteTextureID = loadTexture("sprites.png");
}

sprite::~sprite()
{
	glDeleteProgram(m_program);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteTextures(1, &g_spriteTextureID);
}

void sprite::render() 
{
	timeb currentTime;
    float  fElapsed = 0.0f;

    if (m_bFirstRendering == true)
    {
		ftime(&m_lastTime);
        currentTime = m_lastTime;
        m_bFirstRendering = false;
    }
    else
    {
		ftime(&currentTime);
		
        // Ёто с точностью до одной секунды
        fElapsed  = (float)(currentTime.time - m_lastTime.time);
        // Ёто сокращaет до одной мс 
        fElapsed += (float)((currentTime.millitm - m_lastTime.millitm) / 1000.0f);
    }

	{
		auto transform = static_cast<Transform*>(glMapNamedBufferRange(ubo, 0,
			blockSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

		glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
		glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));
		Model = glm::rotate(Model, glm::radians(m_fRotation), glm::vec3(0.0f, 0.0f, 1.0f));
		transform->MVP = Projection * View * Model;

		glUnmapNamedBuffer(ubo);
	}

	glBindProgramPipeline(pipeline);

	glBindTextureUnit(0, g_spriteTextureID);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo);
		
    glDisable( GL_DEPTH_TEST );
    
    
	
	//-------------------------------------------------------------------------
	// –асположение текстурных координат дл€ простого квада
	//
	// (¬ерхний левый)  0.0, 1.0             1.0, 1.0  (¬ерхний правый)
	//                        +---------+
	//                        |         |
	//                        |         |
	//                        |         |
	//                        |         |
	//                        +---------+
	// (Ќижний левый)  0.0, 0.0             1.0, 0.0  (Ќижний правый)
	//-------------------------------------------------------------------------

	// ≈сли кадры анимации не заполн€ют всю текстуру целиком, нам необходимо
	// найти поддиапазон в пределах стандартного диапазона 
	// текстур (от 0.0 до 1.0), в котором существуют кадры.
    glm::vec2 subRange = glm::vec2( (1.0f / m_textureSize.x ) * (m_frameSize.x  * m_nNumFrameColumns),
									(1.0f / m_textureSize.y) * (m_frameSize.y * m_nNumFrameRows) );

	// Ќе преобразовывать ширину и высоту кадра в значени€ текстуры ... 
    glm::vec2 frame = glm::vec2( subRange.s / m_nNumFrameColumns,
								subRange.t / m_nNumFrameRows);

	// “еперь вычислим новые значени€ текстуры s/t дл€ четырех углов квадрата 
	// на основе текущего столбца и строки анимации
    glm::vec2 lowerLeft  = glm::vec2( m_nCurrentColumn * frame.s,
									1.0f - (m_nCurrentRow * frame.t) - frame.t );

	glm::vec2 lowerRight = glm::vec2( (m_nCurrentColumn * frame.s) + frame.s,
									1.0f - (m_nCurrentRow * frame.t) - frame.t );

	glm::vec2 upperRight = glm::vec2( (m_nCurrentColumn * frame.s) + frame.s,
									1.0f - (m_nCurrentRow * frame.t) );

	glm::vec2 upperLeft = glm::vec2( m_nCurrentColumn * frame.s,
									1.0f - (m_nCurrentRow * frame.t) );

	// ѕримените смещение к текстуре, если первый кадр не расположен 
	// в верхнем левом углу текстуры, как обычно.

    if (m_offset.x > 0 || m_offset.y > 0)
    {
        glm::vec2 offset = glm::vec2((1.0f / m_textureSize.x) * m_offset.x,
			(1.0f / m_textureSize.y) * m_offset.y);

        lowerLeft.s  += offset.s;
        lowerLeft.t  -= offset.t;

	    lowerRight.s += offset.s;
        lowerRight.t -= offset.t;

	    upperRight.s += offset.s;
        upperRight.t -= offset.t;

	    upperLeft.s  += offset.s;
        upperLeft.t  -= offset.t;
    }

	// ћы знаем, где находитс€ центр спрайта, теперь половина его ширины и высоты,
	// чтобы найти, где должны быть расположены его четыре угла дл€ создани€ квадрата.
    glm::vec2 relative = glm::vec2(size.x  / 2.0f, size.y / 2.0f);

	std::array<Vertex, 4> vertices 
	{
		Vertex{glm::vec2(-relative.x,-relative.y), lowerLeft}, // нижний левый
		Vertex{glm::vec2(relative.x,-relative.y), lowerRight}, // нижний правый			
		Vertex{glm::vec2(relative.x, relative.y), upperRight}, // верхний правый		
		Vertex{glm::vec2(-relative.x, relative.y), upperLeft}  // верхний левый
	};

	
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, vertices.size() * sizeof(Vertex), vertices.data(), 0);

	glBindVertexArray(vao);
	
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 0);

	glVertexArrayAttribBinding(vao, 1, 0);
	glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2));
	glEnableVertexArrayAttrib(vao, 1);

	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
	
   
	// ≈сли задержка кадра прошла, идЄм к следующему кадру анимации ... 
	if( fElapsed >= m_fFrameDelay )
    {
        m_lastTime = currentTime;
        
        ++m_nCurrentColumn;
        ++m_nFrameNumber;

        if( m_nCurrentColumn >= m_nNumFrameColumns )
        {
            m_nCurrentColumn = 0;
            ++m_nCurrentRow;
        }

        if( m_nCurrentRow >= m_nNumFrameRows )
        {
            m_nCurrentRow    = 0;
            m_nCurrentColumn = 0;
        }

        if( m_nFrameNumber >= m_nTotalFrames )
        {
            m_nCurrentRow    = 0;
            m_nCurrentColumn = 0;
            m_nFrameNumber   = 0;
        }
    }
}

GLuint sprite::loadTexture(const std::string& filename, GLenum minFilter /*= GL_LINEAR*/, GLenum magFilter /*= GL_LINEAR*/, GLenum wrapMode /*= GL_REPEAT*/)
{
	GLuint textureId{};
	glCreateTextures(GL_TEXTURE_2D, 1, &textureId);

	// set the texture wrapping parameters
	glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, wrapMode);
	glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, wrapMode);
	// set texture filtering parameters
	glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, minFilter);
	glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, magFilter);
	
	// load image, create texture and generate mipmaps
	stbi_set_flip_vertically_on_load(true);
	int w, h, n;
	const auto data = stbi_load(filename.c_str(), &w, &h, &n, 0);
	if (data)
	{
		glTextureStorage2D(textureId, 1, GL_RGBA8, w, h);
		glTextureSubImage2D(textureId, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateTextureMipmap(textureId);
	}
	else
	{
		std::cout << "Failed to load texture!\n";
	}
	stbi_image_free(data);

	return textureId;
}

GLuint sprite::loadShader(std::string_view filename, GLenum shaderType)
{
	auto source = [filename] {
		std::string result;
		std::ifstream stream(filename.data());

		if (!stream.is_open())
		{
			std::cout << "Failed to open file: " << std::string(filename) << '\n';
			return result;
		}

		stream.seekg(0, std::ios::end);
		result.reserve((size_t)stream.tellg());
		stream.seekg(0, std::ios::beg);

		result.assign(std::istreambuf_iterator<char>{stream},
			std::istreambuf_iterator<char>{});

		return result;
	}();

	auto pSource = source.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &pSource, nullptr);
	glCompileShader(shader);
	checkShader(shader);

	return shader;
}

GLuint sprite::createProgram(const std::vector<GLuint>& shaders)
{
	GLuint program_object = glCreateProgram();
	glProgramParameteri(program_object, GL_PROGRAM_SEPARABLE, GL_TRUE);

	for (const auto& shader : shaders)
	{
		glAttachShader(program_object, shader);
	}

	glLinkProgram(program_object);
	checkProgram(program_object);

	for (const auto& shader : shaders)
	{
		glDetachShader(program_object, shader);
		glDeleteShader(shader);
	}

	return program_object;
}

void sprite::checkShader(GLuint shader)
{
	GLint isCompiled = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

	GLint maxLength{};
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	if (maxLength > 0
#ifdef NDEBUG
		&& isCompiled == GL_FALSE
#endif // NDEBUG
		)
	{
		std::vector<char> buffer(maxLength);
		glGetShaderInfoLog(shader, maxLength, nullptr, buffer.data());
		glDeleteShader(shader);

		std::cout << "Error compiled:\n" << buffer.data() << '\n';
	}
}

void sprite::checkProgram(GLuint program)
{
	GLint isLinked = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

	GLint maxLength{};
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

	if (maxLength > 0
#ifdef NDEBUG
		&& isLinked == GL_FALSE
#endif // NDEBUG
		)
	{
		std::vector<char> buffer(maxLength);
		glGetProgramInfoLog(program, maxLength, nullptr, buffer.data());
		glDeleteProgram(program);

		std::cout << "Error linked:\n" << buffer.data() << '\n';
	}
}
