#ifndef GLAD_GLAD_H_
#define GLAD_GLAD_H_

#ifdef __gl_h_
#error OpenGL header already included, remove previous include.
#endif
#define __gl_h_ 1

#ifdef __glext_h_
#error OpenGL extension header already included, remove previous include.
#endif
#define __glext_h_ 1

#ifdef __glcorearb_h_
#error OpenGL core header already included, remove previous include.
#endif
#define __glcorearb_h_ 1

#ifndef APIENTRY
#if defined(_WIN32) || defined(__CYGWIN__)
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif
#endif

#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GLAPI
#define GLAPI extern
#endif

#include <KHR/khrplatform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*GLADloadproc)(const char* name);

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef char GLchar;
typedef khronos_int8_t GLbyteARB;
typedef khronos_int16_t GLhalf;
typedef khronos_int32_t GLfixed;
typedef khronos_intptr_t GLintptr;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_int64_t GLint64;
typedef khronos_uint64_t GLuint64;

#define GL_FALSE 0
#define GL_TRUE 1

#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400

#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_VENDOR 0x1F00
#define GL_EXTENSIONS 0x1F03
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C

typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(
    GLfloat red,
    GLfloat green,
    GLfloat blue,
    GLfloat alpha
);
typedef const GLubyte* (APIENTRYP PFNGLGETSTRINGPROC)(GLenum name);
typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);

GLAPI PFNGLCLEARPROC glad_glClear;
GLAPI PFNGLCLEARCOLORPROC glad_glClearColor;
GLAPI PFNGLGETSTRINGPROC glad_glGetString;
GLAPI PFNGLVIEWPORTPROC glad_glViewport;

int gladLoadGLLoader(GLADloadproc load);

#define glClear glad_glClear
#define glClearColor glad_glClearColor
#define glGetString glad_glGetString
#define glViewport glad_glViewport

#ifdef __cplusplus
}
#endif

#endif
