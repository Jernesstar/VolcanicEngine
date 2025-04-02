#pragma once

#include <glad/gl.h>

#include <VolcaniCore/Core/Log.h>

#ifdef _DEBUG
	#define GL_CHECK(statement) \
			statement; \
			CheckOpenGLError(#statement, __FILE__, __LINE__);
#else
	#define GL_CHECK(statement) statement
#endif

namespace VolcaniCore::OpenGL {

void CheckOpenGLError(const char* statement, const char* fname, int line)
{
	GLenum err = glGetError();
	while(err != GL_NO_ERROR)
	{
		VOLCANICORE_LOG_ERROR("[OpenGL]: File: %s, Line: %i\n", err, fname, line, statement);
		err = glGetError();
	}
}

}