// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#include "modpmx.h"
#include <ray/fstream.h>

#include <glfw/glfw3.h>

bool domain_main(int argc, char** argv)
{
	std::string filepath = argv[1];
	if (filepath.empty())
		return false;

	ray::ifstream fileRead;
	if (!fileRead.open(filepath))
		return false;

	ray::PMXHandler model;
	if (model.doCanRead(fileRead))
	{
		model.doLoad(fileRead);
		model.computeLightmapPackByLightmapper(2048, 2048, 4, true, 1, 1);

		/*ray::ofstream fileWrite;
		if (!fileWrite.open(filepath + ".pmx"))
			return 0;

		model.doSave(fileWrite);*/
	}

	return true;
}

int main(int argc, char** argv)
{
	if (::glfwInit() == GL_FALSE)
		return 1;

#if defined(GLFW_EXPOSE_NATIVE_X11)
	::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
	::glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	::glfwWindowHint(GLFW_VISIBLE, false);
#endif
	::glfwSwapInterval(0);

	auto window = ::glfwCreateWindow(800, 600, "LightMap Pack", nullptr, nullptr);
	if (window)
	{
		::glfwMakeContextCurrent(window);
		
		int code = domain_main(argc, argv);

		::glfwDestroyWindow(window);
		::glfwTerminate();

		std::system("pause");

		return code;
	}

	return 1;
}