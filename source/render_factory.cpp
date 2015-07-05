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
#include <ray/render_factory.h>

#if _BUILD_OPENGL
#include <ray/ogl_shader.h>
#include <ray/ogl_texture.h>
#include <ray/ogl_buffer.h>
#include <ray/ogl_renderer.h>
#endif

#include <ray/image.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <ray/material_maker.h>
#include <ray/xmlreader.h>
#include <ray/parse.h>

_NAME_BEGIN

RenderDevicePtr
RenderFactory::createRenderDevice() noexcept
{
	return std::make_shared<OGLRenderer>();
}

RenderWindowPtr
RenderFactory::createRenderWindow() noexcept
{
	return std::make_shared<OGLCanvas>();
}

ShaderPtr
RenderFactory::createShader() noexcept
{
	return std::make_shared<OGLShader>();
}

ShaderObjectPtr
RenderFactory::createShaderObject() noexcept
{
	return std::make_shared<OGLShaderObject>();
}

TexturePtr
RenderFactory::createTexture() noexcept
{
	return std::make_shared<OGLTexture>();
}

TexturePtr
RenderFactory::createTexture(const std::string& name) except
{
	MemoryStream stream;
	IoServer::instance()->openFile(name, stream);

	if (stream)
	{
		Image image;
		if (image.load(stream))
		{
			PixelFormat format = PixelFormat::R8G8B8A8;

			if (image.bpp() == 24)
				format = PixelFormat::R8G8B8;
			else if (image.bpp() == 32)
				format = PixelFormat::R8G8B8A8;

			auto texture = RenderFactory::createTexture();
			texture->setSize(image.width(), image.height());
			texture->setTexDim(TextureDim::DIM_2D);
			texture->setTexFormat(format);
			texture->setStream(image.data());
			texture->setup();

			return texture;
		}
	}
	else
	{
		throw failure("fail to open : " + name);
	}

	return nullptr;
}

RenderTargetPtr
RenderFactory::createRenderTarget() noexcept
{
	return std::make_shared<OGLRenderTarget>();
}

MultiRenderTargetPtr
RenderFactory::createMultiRenderTarget() noexcept
{
	return std::make_shared<OGLMultiRenderTarget>();
}

VertexBufferDataPtr
RenderFactory::createVertexBuffer() noexcept
{
	return std::make_shared<OGLVertexBuffer>();
}

IndexBufferDataPtr
RenderFactory::createIndexBuffer() noexcept
{
	return std::make_shared<OGLIndexBuffer>();
}

RenderBufferPtr
RenderFactory::createRenderBuffer() noexcept
{
	return std::make_shared<OGLRenderBuffer>();
}

RenderBufferPtr
RenderFactory::createRenderBuffer(const MeshProperty& mesh) except
{
	auto buffer = std::make_shared<OGLRenderBuffer>();

	auto numVertex = mesh.getNumVertices();
	auto numIndex = mesh.getNumIndices();

	std::vector<VertexComponent> components;

	auto& vertices = mesh.getVertexArray();
	if (!vertices.empty())
	{
		if (vertices.size() == numVertex)
			components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_POSITION, VertexFormat::GPU_VERTEX_FLOAT3));
	}

	auto& normals = mesh.getNormalArray();
	if (!normals.empty())
	{
		if (normals.size() == numVertex)
			components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_NORMAL, VertexFormat::GPU_VERTEX_FLOAT3));
	}

	auto& texcoords = mesh.getTexcoordArray();
	if (!texcoords.empty())
	{
		if (texcoords.size() == numVertex)
			components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_TEXCOORD, VertexFormat::GPU_VERTEX_FLOAT2));
	}

	VertexBufferDataPtr vb;

	if (numVertex)
	{
		vb = RenderFactory::createVertexBuffer();
		vb->setVertexComponents(components);
		vb->setup(numVertex, VertexUsage::GPU_USAGE_STATIC);

		std::size_t offset = 0;
		std::size_t stride = vb->getVertexSize();
		if (vertices.size() == numVertex)
		{
			char* data = (char*)vb->data();
			for (auto& it : vertices)
			{
				*(float3*)data = it;
				data += stride;
			}

			offset += sizeof(float3);
		}

		if (normals.size() == numVertex)
		{
			char* data = (char*)vb->data() + offset;
			for (auto& it : normals)
			{
				*(float3*)data = it;
				data += stride;
			}

			offset += sizeof(float3);
		}

		if (texcoords.size() == numVertex)
		{
			char* data = (char*)vb->data() + offset;
			for (auto& it : texcoords)
			{
				*(float2*)data = it;
				data += stride;
			}

			offset += sizeof(float2);
		}
	}

	IndexBufferDataPtr ib;

	auto& faces = mesh.getFaceArray();
	if (numIndex > 0)
	{
		ib = RenderFactory::createIndexBuffer();
		ib->setup(numIndex, IndexType::GPU_UINT32, VertexUsage::GPU_USAGE_STATIC);

		if (!faces.empty())
		{
			std::uint32_t* indices = (std::uint32_t*)ib->data();
			for (auto& face : faces)
			{
				*indices++ = face;
			}
		}
	}

	buffer->setup(vb, ib);

	return buffer;
}

MaterialPtr
RenderFactory::createMaterial(const std::string& filename) except
{
	MemoryStream stream;

	IoServer::instance()->openFile(filename, stream);
	if (!stream.is_open())
		return false;

	XMLReader xml;
	if (xml.open(stream))
	{
		std::string name;
		std::map<std::string, std::string> args;

		if (!xml.setToFirstChild())
		{
			throw failure("The file has been damaged and can't be recovered, so I can't open it" + filename);
		}

		do
		{
			auto key = xml.getCurrentNodeName();
			if (key == "attribute")
			{
				auto attributes = xml.getAttrs();
				for (auto& it : attributes)
				{
					if (it == "shader")
					{
						name = xml.getString(it);
					}
					else
					{
						args[it] = xml.getString(it);
					}
				}
			}
		} while (xml.setToNextChild());

		if (!name.empty())
		{
			MaterialMaker maker;
			auto material = maker.load(name);
			if (material)
			{
				for (auto& arg : args)
				{
					auto param = material->getParameter(arg.first);
					if (param)
					{
						auto type = param->getType();
						switch (type)
						{
						case ShaderVariantType::SPT_FLOAT:
						{
							param->assign(parseFloat<Float>(arg.second));
						}
						break;
						case ShaderVariantType::SPT_FLOAT4:
						{
							param->assign(parseFloat4(arg.second));
						}
						break;
						case ShaderVariantType::SPT_TEXTURE:
						{
							param->assign(RenderFactory::createTexture(arg.second));
						}
						break;
						}
					}
				}

				return material;
			}
		}
	}

	return nullptr;
}

_NAME_END