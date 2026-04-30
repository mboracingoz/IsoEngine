#include <glad/glad.h>

PFNGLCLEARPROC glad_glClear = 0;
PFNGLCLEARCOLORPROC glad_glClearColor = 0;
PFNGLVIEWPORTPROC glad_glViewport = 0;
PFNGLGETSTRINGPROC glad_glGetString = 0;

int gladLoadGLLoader(GLADloadproc load)
{
    if (load == 0)
    {
        return 0;
    }

    glad_glClear = (PFNGLCLEARPROC)load("glClear");
    glad_glClearColor = (PFNGLCLEARCOLORPROC)load("glClearColor");
    glad_glViewport = (PFNGLVIEWPORTPROC)load("glViewport");
    glad_glGetString = (PFNGLGETSTRINGPROC)load("glGetString");

    return glad_glClear != 0 &&
        glad_glClearColor != 0 &&
        glad_glViewport != 0 &&
        glad_glGetString != 0;
}
