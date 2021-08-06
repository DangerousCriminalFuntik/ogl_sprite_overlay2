#pragma once

#include <sys/timeb.h>

#include <glad/glad.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <array>
#include <vector>
#include <fstream>
#include <string>
#include <string_view>

struct Vertex
{
	glm::vec2 position;
	glm::vec2 texcoord;
};

struct Transform
{
	glm::mat4 MVP;
};

class sprite
{
public:
    sprite();
    virtual ~sprite();

    void render();

	glm::vec2 getPosition() const { return position; }
	void setPosition(glm::vec2 val) { position = val; }

	glm::vec2 getSize() const { return size; }
	void setSize(glm::vec2 val) { size = val; }

    void  setRotation(float fRotation) { m_fRotation = m_fRotation; }
    float getRotation() { return m_fRotation; }

    void  setNumFrameColumns(int nNumFrameColumns) { m_nNumFrameColumns = nNumFrameColumns; }
    int   getNumFrameColumns() { return m_nNumFrameColumns; }

    void  setNumFrameRows(int nNumFrameRows) { m_nNumFrameRows = nNumFrameRows; }
    int   getNumFrameRows() { return m_nNumFrameRows; }

    void  setTotalFrames(int nTotalFrames) { m_nTotalFrames = nTotalFrames; }
    int   getTotalFrames() { return m_nTotalFrames; }

    void  setFrameDelay( float fFrameDelay ) { m_fFrameDelay = fFrameDelay; }
    float getFrameDelay() { return m_fFrameDelay; }

    void  setTextureAnimeInfo( int nTextureWidth, int nTextureHeight, 
                               int nFrameWidth,   int nFrameHeight,
                               int nNumFrameColumns, int nNumFrameRows, 
                               int nTotalFrames,
                               int nOffsetX = 0, int nOffsetY = 0 ) 
    {
        m_textureSize = glm::ivec2(nTextureWidth, nTextureHeight);
        m_frameSize = glm::ivec2(nFrameWidth, nFrameHeight);
        m_nNumFrameColumns = nNumFrameColumns;
        m_nNumFrameRows    = nNumFrameRows;
        m_nTotalFrames     = nTotalFrames;
        m_offset = glm::ivec2(nOffsetX, nOffsetY);
    }

	GLuint loadTexture(const std::string& filename, GLenum minFilter = GL_LINEAR, GLenum magFilter = GL_LINEAR, GLenum wrapMode = GL_REPEAT);

private:
	void checkShader(GLuint shader);
	void checkProgram(GLuint program);
	GLuint loadShader(std::string_view filename, GLenum shaderType);
	GLuint createProgram(const std::vector<GLuint>& shaders);

    GLuint g_spriteTextureID{};
	GLuint m_program{};
    GLuint pipeline{};
    GLint blockSize{};
	GLuint vao{};
	GLuint vbo{};
    GLuint ubo{};
	
    // Sprite Controls...
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 size = glm::vec2(1.0f, 1.0f);
    float  m_fRotation{0.0f};
    
    // Texture Animation Settings...
    float  m_fFrameDelay{0.0f};
    int    m_nNumFrameColumns{1};
    int    m_nNumFrameRows{1};
    int    m_nTotalFrames{1};

    glm::ivec2 m_textureSize = glm::ivec2(256, 256);
    glm::ivec2 m_frameSize = glm::ivec2(256, 256);
    glm::ivec2 m_offset = glm::ivec2(0, 0);

    // Frame tracking variables...
    int    m_nFrameNumber{0};
    int    m_nCurrentRow{0};
    int    m_nCurrentColumn{0};
	timeb  m_lastTime;
    bool   m_bFirstRendering{true};
};