// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
#include <ray/gui_renderer.h>
#include <ray/gui_texture.h>
#include <ray/gui_buffer.h>
#include <ray/gui_assert.h>
#include "MyGUI_VertexData.h"
#include "MyGUI_Gui.h"
#include "MyGUI_Timer.h"

#include "GL/glew.h"

_NAME_BEGIN

using namespace Gui;

GuiRenderer& GuiRenderer::getInstance()
{
	return *getInstancePtr();
}

GuiRenderer* GuiRenderer::getInstancePtr()
{
	return static_cast<GuiRenderer*>(RenderManager::getInstancePtr());
}

GuiRenderer::GuiRenderer() 
	: _update(false)
	, _imageLoader(nullptr)
	, _isSupportedPbo(false)
	, _isInitialise(false)
{
}

GLuint buildShader(const char* text, GLenum type)
{
	GLuint id = glCreateShader(type);
	glShaderSource(id, 1, &text, 0);
	glCompileShader(id);

	GLint success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		GLint len = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);

		GLchar* buffer = new GLchar[len];
		glGetShaderInfoLog(id, len, NULL, buffer);
		std::string infoLog = buffer;
		delete[] buffer;

		MYGUI_PLATFORM_EXCEPT(infoLog);
	}

	return id;
}

GLuint GuiRenderer::createShaderProgram(void)
{
const char vertexShader[] =
    "#version 130\n" // GLSL 1.30 = OpenGL 3.0
    "out vec4 Color;\n"
    "out vec2 TexCoord;\n"
    "in vec3 VertexPosition;\n"
    "in vec4 VertexColor;\n"
    "in vec2 VertexTexCoord;\n"
    "uniform float YScale;\n"
    "void main()\n"
    "{\n"
    "  TexCoord = VertexTexCoord;\n"
    "  Color = VertexColor;\n"
    "  vec4 vpos = vec4(VertexPosition,1.0);\n"
    "  vpos.y *= YScale;\n"
    "  gl_Position = vpos;\n"
    "}\n"
    ;

const char fragmentShader[] =
    "#version 130\n"
    "in vec4 Color; \n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D Texture;\n"
    "void main(void)\n"
    "{\n"
    "  FragColor = texture2D(Texture, TexCoord) * Color;\n"
    "}\n"
    ;

GLuint vsID = buildShader(vertexShader, GL_VERTEX_SHADER);
GLuint fsID = buildShader(fragmentShader, GL_FRAGMENT_SHADER);

GLuint progID = glCreateProgram();
glAttachShader(progID, vsID);
glAttachShader(progID, fsID);

// setup vertex attribute positions for vertex buffer
glBindAttribLocation(progID, 0, "VertexPosition");
glBindAttribLocation(progID, 1, "VertexColor");
glBindAttribLocation(progID, 2, "VertexTexCoord");
glBindFragDataLocation(progID, 0, "FragColor");

glLinkProgram(progID);

GLint success;
glGetProgramiv(progID, GL_LINK_STATUS, &success);

if (success == GL_FALSE) {
    GLint len = 0;
    glGetProgramiv(progID, GL_INFO_LOG_LENGTH, &len);

    GLchar* buffer = new GLchar[len];
    glGetProgramInfoLog(progID, len, NULL, buffer);
    std::string infoLog = buffer;
    delete[] buffer;

    MYGUI_PLATFORM_EXCEPT(infoLog);
}
glDeleteShader(vsID); // flag for deletion on call to glDeleteProgram
glDeleteShader(fsID);

int textureUniLoc = glGetUniformLocation(progID, "Texture");
if (textureUniLoc == -1) {
    MYGUI_PLATFORM_EXCEPT("Unable to retrieve uniform variable location");
}
mYScaleUniformLocation = glGetUniformLocation(progID, "YScale");
if (mYScaleUniformLocation == -1) {
    MYGUI_PLATFORM_EXCEPT("Unable to retrieve YScale variable location");
}
glUseProgram(progID);
glUniform1i(textureUniLoc, 0); // set active sampler for 'Texture' to GL_TEXTURE0
glUniform1f(mYScaleUniformLocation, 1.0f);
glUseProgram(0);

return progID;
}

void GuiRenderer::initialise()
{
	MYGUI_PLATFORM_ASSERT(!_isInitialise, getClassTypeName() << " initialised twice");
	MYGUI_PLATFORM_LOG(Info, "* Initialise: " << getClassTypeName());

	mVertexFormat = MyGUI::VertexColourType::ColourABGR;

	_update = false;

	mReferenceCount = 0;

	if (!(GLEW_VERSION_3_0)) 
	{
		const char *version = (const char *) glGetString(GL_VERSION);
		MYGUI_PLATFORM_EXCEPT(std::string("OpenGL 3.0 or newer not available, current version is ") + version);
	}

	_isSupportedPbo = glewIsExtensionSupported("GL_EXT_pixel_buffer_object") != 0;
	mProgramID = createShaderProgram();

	MYGUI_PLATFORM_LOG(Info, getClassTypeName() << " successfully initialized");
	_isInitialise = true;
}

void 
GuiRenderer::shutdown()
{
	MYGUI_PLATFORM_ASSERT(_isInitialise, getClassTypeName() << " is not initialised");
	MYGUI_PLATFORM_LOG(Info, "* Shutdown: " << getClassTypeName());

	destroyAllResources();

	MYGUI_PLATFORM_LOG(Info, getClassTypeName() << " successfully shutdown");
	_isInitialise = false;
}

void
GuiRenderer::setImageLoader(Gui::GuiImageLoaderPtr loader) noexcept
{
	_imageLoader = loader;
}

Gui::GuiImageLoaderPtr
GuiRenderer::getImageLoader() const noexcept
{
	return _imageLoader;
}

MyGUI::IVertexBuffer* 
GuiRenderer::createVertexBuffer()
{
	return new GuiVertexBuffer();
}

void GuiRenderer::destroyVertexBuffer(MyGUI::IVertexBuffer* _buffer)
{
	delete _buffer;
}

void GuiRenderer::doRenderRTT(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count)
{
	glUniform1f(mYScaleUniformLocation, -1.0f);
	doRender(_buffer, _texture, _count);
	glUniform1f(mYScaleUniformLocation, 1.0f);
}

void GuiRenderer::doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count)
{
	GuiVertexBuffer* buffer = static_cast<GuiVertexBuffer*>(_buffer);
	unsigned int buffer_id = buffer->getBufferID();
	MYGUI_PLATFORM_ASSERT(buffer_id, "Vertex buffer is not created");

	unsigned int texture_id = 0;
	if (_texture)
	{
		GuiTexture* texture = static_cast<GuiTexture*>(_texture);
		texture_id = texture->getTextureID();
		//MYGUI_PLATFORM_ASSERT(texture_id, "Texture is not created");
	}

	glBindTexture(GL_TEXTURE_2D, texture_id);

	glBindVertexArray(buffer_id);
	glDrawArrays(GL_TRIANGLES, 0, _count);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void GuiRenderer::begin()
{
	++mReferenceCount;

	glUseProgram(mProgramID);
	glActiveTexture(GL_TEXTURE0);
  
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GuiRenderer::end()
{
	if (--mReferenceCount == 0) 
	{
			glDisable(GL_BLEND);
			glUseProgram(0);
	}
}

const MyGUI::RenderTargetInfo& 
GuiRenderer::getInfo()
{
	return mInfo;
}

const MyGUI::IntSize& 
GuiRenderer::getViewSize() const
{
	return mViewSize;
}

MyGUI::VertexColourType 
GuiRenderer::getVertexFormat()
{
	return mVertexFormat;
}

bool 
GuiRenderer::isFormatSupported(MyGUI::PixelFormat _format, MyGUI::TextureUsage _usage)
{
	if (_format == MyGUI::PixelFormat::R8G8B8 ||
		_format == MyGUI::PixelFormat::R8G8B8A8)
		return true;

	return false;
}

void GuiRenderer::drawOneFrame()
{
	MyGUI::Gui* gui = MyGUI::Gui::getInstancePtr();
	if (gui == nullptr)
		return;

	static MyGUI::Timer timer;
	static unsigned long last_time = timer.getMilliseconds();
	unsigned long now_time = timer.getMilliseconds();
	unsigned long time = now_time - last_time;

	onFrameEvent((float)((double)(time) / (double)1000));

	last_time = now_time;

	begin();
	onRenderToTarget(this, _update);
	end();

	_update = false;
}

void GuiRenderer::setViewSize(int _width, int _height)
{
	if (_height == 0)
		_height = 1;
	if (_width == 0)
		_width = 1;

	mViewSize.set(_width, _height);

	mInfo.maximumDepth = 1;
	mInfo.hOffset = 0;
	mInfo.vOffset = 0;
	mInfo.aspectCoef = float(mViewSize.height) / float(mViewSize.width);
	mInfo.pixScaleX = 1.0f / float(mViewSize.width);
	mInfo.pixScaleY = 1.0f / float(mViewSize.height);

	onResizeView(mViewSize);
	_update = true;
}

bool GuiRenderer::isPixelBufferObjectSupported() const
{
	return _isSupportedPbo;
}

MyGUI::ITexture* 
GuiRenderer::createTexture(const std::string& _name)
{
	MapTexture::const_iterator item = mTextures.find(_name);
	MYGUI_PLATFORM_ASSERT(item == mTextures.end(), "Texture '" << _name << "' already exist");

	GuiTexture* texture = new GuiTexture(_name, _imageLoader);
	mTextures[_name] = texture;
	return texture;
}

void
GuiRenderer::destroyTexture(MyGUI::ITexture* _texture)
{
	if (_texture == nullptr)
		return;

	MapTexture::iterator item = mTextures.find(_texture->getName());
	MYGUI_PLATFORM_ASSERT(item != mTextures.end(), "Texture '" << _texture->getName() << "' not found");

	mTextures.erase(item);
	delete _texture;
}

MyGUI::ITexture*
GuiRenderer::getTexture(const std::string& _name)
{
	MapTexture::const_iterator item = mTextures.find(_name);
	if (item == mTextures.end())
		return nullptr;
	return item->second;
}

void 
GuiRenderer::destroyAllResources()
{
	for (MapTexture::const_iterator item = mTextures.begin(); item != mTextures.end(); ++item)
		delete item->second;

	mTextures.clear();

	if (mProgramID) 
	{
		glDeleteProgram(mProgramID);
		mProgramID = 0;
	}
}

_NAME_END
