#include "engine/iso/IsoGridRenderer.h"

#include "engine/iso/IsoMath.h"

#include <glad/glad.h>

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace IsoForge
{
namespace
{
struct LineVertex
{
    float x;
    float y;
    float r;
    float g;
    float b;
    float a;
};

constexpr float GridColorR = 0.35f;
constexpr float GridColorG = 0.45f;
constexpr float GridColorB = 0.55f;
constexpr float GridColorA = 1.0f;

constexpr float HighlightColorR = 0.9f;
constexpr float HighlightColorG = 0.85f;
constexpr float HighlightColorB = 0.25f;
constexpr float HighlightColorA = 1.0f;

constexpr const char* VertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec4 a_Color;

out vec4 v_Color;

void main()
{
    v_Color = a_Color;
    gl_Position = vec4(a_Position, 0.0, 1.0);
}
)";

constexpr const char* FragmentShaderSource = R"(
#version 330 core
in vec4 v_Color;

out vec4 FragColor;

void main()
{
    FragColor = v_Color;
}
)";

GLuint CompileShader(GLenum shaderType, const char* source, const char* debugName)
{
    const GLuint shader = glCreateShader(shaderType);
    if (shader == 0)
    {
        std::cerr << "Failed to create " << debugName << ".\n";
        return 0;
    }

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE)
    {
        GLint infoLogLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::string infoLog(static_cast<size_t>(infoLogLength > 0 ? infoLogLength : 1), '\0');
        glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.data());

        std::cerr << "Failed to compile " << debugName << ": " << infoLog << '\n';
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint CreateShaderProgram()
{
    const GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, VertexShaderSource, "iso grid vertex shader");
    if (vertexShader == 0)
    {
        return 0;
    }

    const GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, FragmentShaderSource, "iso grid fragment shader");
    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        return 0;
    }

    const GLuint program = glCreateProgram();
    if (program == 0)
    {
        std::cerr << "Failed to create iso grid shader program.\n";
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE)
    {
        GLint infoLogLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::string infoLog(static_cast<size_t>(infoLogLength > 0 ? infoLogLength : 1), '\0');
        glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());

        std::cerr << "Failed to link iso grid shader program: " << infoLog << '\n';
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

LineVertex MakeVertex(
    float screenX,
    float screenY,
    float viewportWidth,
    float viewportHeight,
    float r,
    float g,
    float b,
    float a
)
{
    const float ndcX = (screenX / viewportWidth) * 2.0f - 1.0f;
    const float ndcY = 1.0f - (screenY / viewportHeight) * 2.0f;

    return {ndcX, ndcY, r, g, b, a};
}

void AppendLine(
    std::vector<LineVertex>& vertices,
    const Vec2& start,
    const Vec2& end,
    float originX,
    float originY,
    float viewportWidth,
    float viewportHeight,
    float r,
    float g,
    float b,
    float a
)
{
    vertices.push_back(MakeVertex(originX + start.x, originY + start.y, viewportWidth, viewportHeight, r, g, b, a));
    vertices.push_back(MakeVertex(originX + end.x, originY + end.y, viewportWidth, viewportHeight, r, g, b, a));
}

void AppendDiamondOutline(
    std::vector<LineVertex>& vertices,
    int gridX,
    int gridY,
    float tileWidth,
    float tileHeight,
    float originX,
    float originY,
    float viewportWidth,
    float viewportHeight,
    float r,
    float g,
    float b,
    float a
)
{
    const Vec2 top = IsoMath::GridToWorld(gridX, gridY, tileWidth, tileHeight);
    const Vec2 right = IsoMath::GridToWorld(gridX + 1, gridY, tileWidth, tileHeight);
    const Vec2 bottom = IsoMath::GridToWorld(gridX + 1, gridY + 1, tileWidth, tileHeight);
    const Vec2 left = IsoMath::GridToWorld(gridX, gridY + 1, tileWidth, tileHeight);

    AppendLine(vertices, top, right, originX, originY, viewportWidth, viewportHeight, r, g, b, a);
    AppendLine(vertices, right, bottom, originX, originY, viewportWidth, viewportHeight, r, g, b, a);
    AppendLine(vertices, bottom, left, originX, originY, viewportWidth, viewportHeight, r, g, b, a);
    AppendLine(vertices, left, top, originX, originY, viewportWidth, viewportHeight, r, g, b, a);
}
}

IsoGridRenderer::IsoGridRenderer() = default;

IsoGridRenderer::~IsoGridRenderer()
{
    Release();
}

IsoGridRenderer::IsoGridRenderer(IsoGridRenderer&& other) noexcept
    : m_ShaderProgram(other.m_ShaderProgram)
    , m_Vao(other.m_Vao)
    , m_Vbo(other.m_Vbo)
{
    other.m_ShaderProgram = 0;
    other.m_Vao = 0;
    other.m_Vbo = 0;
}

IsoGridRenderer& IsoGridRenderer::operator=(IsoGridRenderer&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    Release();

    m_ShaderProgram = other.m_ShaderProgram;
    m_Vao = other.m_Vao;
    m_Vbo = other.m_Vbo;

    other.m_ShaderProgram = 0;
    other.m_Vao = 0;
    other.m_Vbo = 0;

    return *this;
}

void IsoGridRenderer::DrawGrid(
    int columns,
    int rows,
    float tileWidth,
    float tileHeight,
    float originX,
    float originY,
    float viewportWidth,
    float viewportHeight
)
{
    if (columns <= 0 || rows <= 0 || tileWidth <= 0.0f || tileHeight <= 0.0f || viewportWidth <= 0.0f || viewportHeight <= 0.0f)
    {
        return;
    }

    EnsureInitialized();
    if (m_ShaderProgram == 0 || m_Vao == 0 || m_Vbo == 0)
    {
        return;
    }

    std::vector<LineVertex> vertices;
    vertices.reserve(static_cast<size_t>(columns * rows * 8));

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < columns; ++x)
        {
            AppendDiamondOutline(
                vertices,
                x,
                y,
                tileWidth,
                tileHeight,
                originX,
                originY,
                viewportWidth,
                viewportHeight,
                GridColorR,
                GridColorG,
                GridColorB,
                GridColorA
            );
        }
    }

    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(LineVertex)),
        vertices.data(),
        GL_DYNAMIC_DRAW
    );

    glUseProgram(m_ShaderProgram);
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void IsoGridRenderer::DrawTileHighlight(
    int gridX,
    int gridY,
    float tileWidth,
    float tileHeight,
    float originX,
    float originY,
    float viewportWidth,
    float viewportHeight
)
{
    if (gridX < 0 || gridY < 0 || tileWidth <= 0.0f || tileHeight <= 0.0f || viewportWidth <= 0.0f || viewportHeight <= 0.0f)
    {
        return;
    }

    EnsureInitialized();
    if (m_ShaderProgram == 0 || m_Vao == 0 || m_Vbo == 0)
    {
        return;
    }

    std::vector<LineVertex> vertices;
    vertices.reserve(8);
    AppendDiamondOutline(
        vertices,
        gridX,
        gridY,
        tileWidth,
        tileHeight,
        originX,
        originY,
        viewportWidth,
        viewportHeight,
        HighlightColorR,
        HighlightColorG,
        HighlightColorB,
        HighlightColorA
    );

    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(LineVertex)),
        vertices.data(),
        GL_DYNAMIC_DRAW
    );

    glUseProgram(m_ShaderProgram);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void IsoGridRenderer::EnsureInitialized()
{
    if (m_ShaderProgram != 0 && m_Vao != 0 && m_Vbo != 0)
    {
        return;
    }

    Release();

    m_ShaderProgram = CreateShaderProgram();
    if (m_ShaderProgram == 0)
    {
        return;
    }

    glGenVertexArrays(1, &m_Vao);
    glGenBuffers(1, &m_Vbo);

    if (m_Vao == 0 || m_Vbo == 0)
    {
        std::cerr << "Failed to create iso grid OpenGL buffers.\n";
        Release();
        return;
    }

    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        static_cast<GLsizei>(sizeof(LineVertex)),
        reinterpret_cast<const void*>(offsetof(LineVertex, x))
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        4,
        GL_FLOAT,
        GL_FALSE,
        static_cast<GLsizei>(sizeof(LineVertex)),
        reinterpret_cast<const void*>(offsetof(LineVertex, r))
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void IsoGridRenderer::Release()
{
    if (m_Vbo != 0)
    {
        glDeleteBuffers(1, &m_Vbo);
        m_Vbo = 0;
    }

    if (m_Vao != 0)
    {
        glDeleteVertexArrays(1, &m_Vao);
        m_Vao = 0;
    }

    if (m_ShaderProgram != 0)
    {
        glDeleteProgram(m_ShaderProgram);
        m_ShaderProgram = 0;
    }
}
}
