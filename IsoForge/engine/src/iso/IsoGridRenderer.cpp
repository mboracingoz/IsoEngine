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
struct ColorVertex
{
    float x;
    float y;
    float r;
    float g;
    float b;
    float a;
};

struct TextureVertex
{
    float x;
    float y;
    float u;
    float v;
};

constexpr float GridColorR = 0.35f;
constexpr float GridColorG = 0.45f;
constexpr float GridColorB = 0.55f;
constexpr float GridColorA = 1.0f;

constexpr float HighlightColorR = 0.9f;
constexpr float HighlightColorG = 0.85f;
constexpr float HighlightColorB = 0.25f;
constexpr float HighlightColorA = 1.0f;

constexpr const char* ColorVertexShaderSource = R"(
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

constexpr const char* ColorFragmentShaderSource = R"(
#version 330 core
in vec4 v_Color;

out vec4 FragColor;

void main()
{
    FragColor = v_Color;
}
)";

constexpr const char* TextureVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_UV;

out vec2 v_UV;

void main()
{
    v_UV = a_UV;
    gl_Position = vec4(a_Position, 0.0, 1.0);
}
)";

constexpr const char* TextureFragmentShaderSource = R"(
#version 330 core
in vec2 v_UV;

out vec4 FragColor;

uniform sampler2D u_Texture;

void main()
{
    FragColor = texture(u_Texture, v_UV);
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

GLuint CreateShaderProgram(
    const char* vertexSource,
    const char* fragmentSource,
    const char* vertexDebugName,
    const char* fragmentDebugName,
    const char* programDebugName
)
{
    const GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource, vertexDebugName);
    if (vertexShader == 0)
    {
        return 0;
    }

    const GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentDebugName);
    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        return 0;
    }

    const GLuint program = glCreateProgram();
    if (program == 0)
    {
        std::cerr << "Failed to create " << programDebugName << ".\n";
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

        std::cerr << "Failed to link " << programDebugName << ": " << infoLog << '\n';
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

float ToNdcX(float screenX, float viewportWidth)
{
    return (screenX / viewportWidth) * 2.0f - 1.0f;
}

float ToNdcY(float screenY, float viewportHeight)
{
    return 1.0f - (screenY / viewportHeight) * 2.0f;
}

ColorVertex MakeColorVertex(
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
    return {
        ToNdcX(screenX, viewportWidth),
        ToNdcY(screenY, viewportHeight),
        r,
        g,
        b,
        a
    };
}

TextureVertex MakeTextureVertex(
    float screenX,
    float screenY,
    float viewportWidth,
    float viewportHeight,
    float u,
    float v
)
{
    return {
        ToNdcX(screenX, viewportWidth),
        ToNdcY(screenY, viewportHeight),
        u,
        v
    };
}

void AppendLine(
    std::vector<ColorVertex>& vertices,
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
    vertices.push_back(
        MakeColorVertex(originX + start.x, originY + start.y, viewportWidth, viewportHeight, r, g, b, a)
    );
    vertices.push_back(
        MakeColorVertex(originX + end.x, originY + end.y, viewportWidth, viewportHeight, r, g, b, a)
    );
}

void AppendDiamondOutline(
    std::vector<ColorVertex>& vertices,
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

void AppendFilledDiamond(
    std::vector<ColorVertex>& vertices,
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

    vertices.push_back(MakeColorVertex(originX + top.x, originY + top.y, viewportWidth, viewportHeight, r, g, b, a));
    vertices.push_back(
        MakeColorVertex(originX + right.x, originY + right.y, viewportWidth, viewportHeight, r, g, b, a)
    );
    vertices.push_back(
        MakeColorVertex(originX + bottom.x, originY + bottom.y, viewportWidth, viewportHeight, r, g, b, a)
    );

    vertices.push_back(
        MakeColorVertex(originX + top.x, originY + top.y, viewportWidth, viewportHeight, r, g, b, a)
    );
    vertices.push_back(
        MakeColorVertex(originX + bottom.x, originY + bottom.y, viewportWidth, viewportHeight, r, g, b, a)
    );
    vertices.push_back(MakeColorVertex(originX + left.x, originY + left.y, viewportWidth, viewportHeight, r, g, b, a));
}

void AppendTexturedDiamond(
    std::vector<TextureVertex>& vertices,
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
    const Vec2 top = IsoMath::GridToWorld(gridX, gridY, tileWidth, tileHeight);
    const Vec2 right = IsoMath::GridToWorld(gridX + 1, gridY, tileWidth, tileHeight);
    const Vec2 bottom = IsoMath::GridToWorld(gridX + 1, gridY + 1, tileWidth, tileHeight);
    const Vec2 left = IsoMath::GridToWorld(gridX, gridY + 1, tileWidth, tileHeight);

    vertices.push_back(MakeTextureVertex(originX + top.x, originY + top.y, viewportWidth, viewportHeight, 0.5f, 0.0f));
    vertices.push_back(
        MakeTextureVertex(originX + right.x, originY + right.y, viewportWidth, viewportHeight, 1.0f, 0.5f)
    );
    vertices.push_back(
        MakeTextureVertex(originX + bottom.x, originY + bottom.y, viewportWidth, viewportHeight, 0.5f, 1.0f)
    );

    vertices.push_back(MakeTextureVertex(originX + top.x, originY + top.y, viewportWidth, viewportHeight, 0.5f, 0.0f));
    vertices.push_back(
        MakeTextureVertex(originX + bottom.x, originY + bottom.y, viewportWidth, viewportHeight, 0.5f, 1.0f)
    );
    vertices.push_back(MakeTextureVertex(originX + left.x, originY + left.y, viewportWidth, viewportHeight, 0.0f, 0.5f));
}
}

IsoGridRenderer::IsoGridRenderer() = default;

IsoGridRenderer::~IsoGridRenderer()
{
    Release();
}

IsoGridRenderer::IsoGridRenderer(IsoGridRenderer&& other) noexcept
    : m_ColorShaderProgram(other.m_ColorShaderProgram)
    , m_ColorVao(other.m_ColorVao)
    , m_ColorVbo(other.m_ColorVbo)
    , m_TextureShaderProgram(other.m_TextureShaderProgram)
    , m_TextureVao(other.m_TextureVao)
    , m_TextureVbo(other.m_TextureVbo)
{
    other.m_ColorShaderProgram = 0;
    other.m_ColorVao = 0;
    other.m_ColorVbo = 0;
    other.m_TextureShaderProgram = 0;
    other.m_TextureVao = 0;
    other.m_TextureVbo = 0;
}

IsoGridRenderer& IsoGridRenderer::operator=(IsoGridRenderer&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    Release();

    m_ColorShaderProgram = other.m_ColorShaderProgram;
    m_ColorVao = other.m_ColorVao;
    m_ColorVbo = other.m_ColorVbo;
    m_TextureShaderProgram = other.m_TextureShaderProgram;
    m_TextureVao = other.m_TextureVao;
    m_TextureVbo = other.m_TextureVbo;

    other.m_ColorShaderProgram = 0;
    other.m_ColorVao = 0;
    other.m_ColorVbo = 0;
    other.m_TextureShaderProgram = 0;
    other.m_TextureVao = 0;
    other.m_TextureVbo = 0;

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
    if (columns <= 0 || rows <= 0 || tileWidth <= 0.0f || tileHeight <= 0.0f || viewportWidth <= 0.0f ||
        viewportHeight <= 0.0f)
    {
        return;
    }

    EnsureInitialized();
    if (m_ColorShaderProgram == 0 || m_ColorVao == 0 || m_ColorVbo == 0)
    {
        return;
    }

    std::vector<ColorVertex> vertices;
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

    glBindVertexArray(m_ColorVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_ColorVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(ColorVertex)),
        vertices.data(),
        GL_DYNAMIC_DRAW
    );

    glUseProgram(m_ColorShaderProgram);
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
    if (gridX < 0 || gridY < 0 || tileWidth <= 0.0f || tileHeight <= 0.0f || viewportWidth <= 0.0f ||
        viewportHeight <= 0.0f)
    {
        return;
    }

    EnsureInitialized();
    if (m_ColorShaderProgram == 0 || m_ColorVao == 0 || m_ColorVbo == 0)
    {
        return;
    }

    std::vector<ColorVertex> vertices;
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

    glBindVertexArray(m_ColorVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_ColorVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(ColorVertex)),
        vertices.data(),
        GL_DYNAMIC_DRAW
    );

    glUseProgram(m_ColorShaderProgram);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void IsoGridRenderer::DrawFilledTile(
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
    if (gridX < 0 || gridY < 0 || tileWidth <= 0.0f || tileHeight <= 0.0f || viewportWidth <= 0.0f ||
        viewportHeight <= 0.0f)
    {
        return;
    }

    EnsureInitialized();
    if (m_ColorShaderProgram == 0 || m_ColorVao == 0 || m_ColorVbo == 0)
    {
        return;
    }

    std::vector<ColorVertex> vertices;
    vertices.reserve(6);
    AppendFilledDiamond(
        vertices,
        gridX,
        gridY,
        tileWidth,
        tileHeight,
        originX,
        originY,
        viewportWidth,
        viewportHeight,
        r,
        g,
        b,
        a
    );

    const GLboolean wasBlendEnabled = glIsEnabled(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(m_ColorVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_ColorVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(ColorVertex)),
        vertices.data(),
        GL_DYNAMIC_DRAW
    );

    glUseProgram(m_ColorShaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    if (wasBlendEnabled == GL_FALSE)
    {
        glDisable(GL_BLEND);
    }
}

void IsoGridRenderer::DrawTexturedTile(
    int gridX,
    int gridY,
    float tileWidth,
    float tileHeight,
    float originX,
    float originY,
    float viewportWidth,
    float viewportHeight,
    uint32_t textureID
)
{
    if (gridX < 0 || gridY < 0 || tileWidth <= 0.0f || tileHeight <= 0.0f || viewportWidth <= 0.0f ||
        viewportHeight <= 0.0f || textureID == 0)
    {
        return;
    }

    EnsureTextureInitialized();
    if (m_TextureShaderProgram == 0 || m_TextureVao == 0 || m_TextureVbo == 0)
    {
        return;
    }

    std::vector<TextureVertex> vertices;
    vertices.reserve(6);
    AppendTexturedDiamond(vertices, gridX, gridY, tileWidth, tileHeight, originX, originY, viewportWidth, viewportHeight);

    const GLboolean wasBlendEnabled = glIsEnabled(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBindVertexArray(m_TextureVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_TextureVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(TextureVertex)),
        vertices.data(),
        GL_DYNAMIC_DRAW
    );

    glUseProgram(m_TextureShaderProgram);
    const GLint textureUniform = glGetUniformLocation(m_TextureShaderProgram, "u_Texture");
    glUniform1i(textureUniform, 0);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    if (wasBlendEnabled == GL_FALSE)
    {
        glDisable(GL_BLEND);
    }
}

void IsoGridRenderer::EnsureInitialized()
{
    if (m_ColorShaderProgram != 0 && m_ColorVao != 0 && m_ColorVbo != 0)
    {
        return;
    }

    if (m_ColorShaderProgram != 0 || m_ColorVao != 0 || m_ColorVbo != 0)
    {
        if (m_ColorVbo != 0)
        {
            glDeleteBuffers(1, &m_ColorVbo);
            m_ColorVbo = 0;
        }
        if (m_ColorVao != 0)
        {
            glDeleteVertexArrays(1, &m_ColorVao);
            m_ColorVao = 0;
        }
        if (m_ColorShaderProgram != 0)
        {
            glDeleteProgram(m_ColorShaderProgram);
            m_ColorShaderProgram = 0;
        }
    }

    m_ColorShaderProgram = CreateShaderProgram(
        ColorVertexShaderSource,
        ColorFragmentShaderSource,
        "iso color vertex shader",
        "iso color fragment shader",
        "iso color shader program"
    );
    if (m_ColorShaderProgram == 0)
    {
        return;
    }

    glGenVertexArrays(1, &m_ColorVao);
    glGenBuffers(1, &m_ColorVbo);

    if (m_ColorVao == 0 || m_ColorVbo == 0)
    {
        std::cerr << "Failed to create iso color OpenGL buffers.\n";
        if (m_ColorVbo != 0)
        {
            glDeleteBuffers(1, &m_ColorVbo);
            m_ColorVbo = 0;
        }
        if (m_ColorVao != 0)
        {
            glDeleteVertexArrays(1, &m_ColorVao);
            m_ColorVao = 0;
        }
        glDeleteProgram(m_ColorShaderProgram);
        m_ColorShaderProgram = 0;
        return;
    }

    glBindVertexArray(m_ColorVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_ColorVbo);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        static_cast<GLsizei>(sizeof(ColorVertex)),
        reinterpret_cast<const void*>(offsetof(ColorVertex, x))
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        4,
        GL_FLOAT,
        GL_FALSE,
        static_cast<GLsizei>(sizeof(ColorVertex)),
        reinterpret_cast<const void*>(offsetof(ColorVertex, r))
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void IsoGridRenderer::EnsureTextureInitialized()
{
    if (m_TextureShaderProgram != 0 && m_TextureVao != 0 && m_TextureVbo != 0)
    {
        return;
    }

    if (m_TextureShaderProgram != 0 || m_TextureVao != 0 || m_TextureVbo != 0)
    {
        if (m_TextureVbo != 0)
        {
            glDeleteBuffers(1, &m_TextureVbo);
            m_TextureVbo = 0;
        }
        if (m_TextureVao != 0)
        {
            glDeleteVertexArrays(1, &m_TextureVao);
            m_TextureVao = 0;
        }
        if (m_TextureShaderProgram != 0)
        {
            glDeleteProgram(m_TextureShaderProgram);
            m_TextureShaderProgram = 0;
        }
    }

    m_TextureShaderProgram = CreateShaderProgram(
        TextureVertexShaderSource,
        TextureFragmentShaderSource,
        "iso texture vertex shader",
        "iso texture fragment shader",
        "iso texture shader program"
    );
    if (m_TextureShaderProgram == 0)
    {
        return;
    }

    glGenVertexArrays(1, &m_TextureVao);
    glGenBuffers(1, &m_TextureVbo);

    if (m_TextureVao == 0 || m_TextureVbo == 0)
    {
        std::cerr << "Failed to create iso texture OpenGL buffers.\n";
        if (m_TextureVbo != 0)
        {
            glDeleteBuffers(1, &m_TextureVbo);
            m_TextureVbo = 0;
        }
        if (m_TextureVao != 0)
        {
            glDeleteVertexArrays(1, &m_TextureVao);
            m_TextureVao = 0;
        }
        glDeleteProgram(m_TextureShaderProgram);
        m_TextureShaderProgram = 0;
        return;
    }

    glBindVertexArray(m_TextureVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_TextureVbo);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        static_cast<GLsizei>(sizeof(TextureVertex)),
        reinterpret_cast<const void*>(offsetof(TextureVertex, x))
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        static_cast<GLsizei>(sizeof(TextureVertex)),
        reinterpret_cast<const void*>(offsetof(TextureVertex, u))
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void IsoGridRenderer::Release()
{
    if (m_TextureVbo != 0)
    {
        glDeleteBuffers(1, &m_TextureVbo);
        m_TextureVbo = 0;
    }

    if (m_TextureVao != 0)
    {
        glDeleteVertexArrays(1, &m_TextureVao);
        m_TextureVao = 0;
    }

    if (m_TextureShaderProgram != 0)
    {
        glDeleteProgram(m_TextureShaderProgram);
        m_TextureShaderProgram = 0;
    }

    if (m_ColorVbo != 0)
    {
        glDeleteBuffers(1, &m_ColorVbo);
        m_ColorVbo = 0;
    }

    if (m_ColorVao != 0)
    {
        glDeleteVertexArrays(1, &m_ColorVao);
        m_ColorVao = 0;
    }

    if (m_ColorShaderProgram != 0)
    {
        glDeleteProgram(m_ColorShaderProgram);
        m_ColorShaderProgram = 0;
    }
}
}
