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
#include <ray/render_system.h>
#include <ray/render_pipeline.h>

#if defined(_BUILD_OPENGL) || defined(_BUILD_OPENGL_ES)
#include <ray/ogl_state.h>
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
#include <ray/resource.h>

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

RenderWindowPtr
RenderFactory::createRenderWindow(WindHandle hwnd) except
{
	return std::make_shared<OGLCanvas>(hwnd);
}

RenderSystemPtr 
RenderFactory::createRenderSystem() noexcept
{
	return std::make_shared<DefaultRenderSystem>();
}

RenderPipelinePtr 
RenderFactory::createRenderPipeline() noexcept
{
	return std::make_shared<DefaultRenderPipeline>();
}

RenderStatePtr 
RenderFactory::createRenderState() noexcept
{
	return std::make_shared<OGLRenderState>();
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
	ResLoader<OGLTexture> loader;

	loader.load(name,
		[&](std::shared_ptr<OGLTexture> texture, const std::string& name) {
		MemoryStream stream;
		IoServer::instance()->openFile(name, stream);
		Image image;
		if (image.load(stream))
		{
			PixelFormat format = PixelFormat::R8G8B8A8;

			if (image.getImageType() == Image::dds1)
				format = PixelFormat::RGBA_DXT1;
			else if (image.getImageType() == Image::dds3)
				format = PixelFormat::RGBA_DXT3;
			else if (image.getImageType() == Image::dds5)
				format = PixelFormat::RGBA_DXT5;
			else if (image.getImageType() == Image::ati2)
				format = PixelFormat::RG_ATI2;
			else
			{
				if (image.bpp() == 24)
					format = PixelFormat::R8G8B8;
				else if (image.bpp() == 32)
					format = PixelFormat::R8G8B8A8;
				else
				{
					assert(false);
				}					
			}

			texture->setMipLevel(image.getMipLevel());
			texture->setMipSize(image.size());
			texture->setSize(image.width(), image.height());
			texture->setTexDim(TextureDim::DIM_2D);
			texture->setTexFormat(format);
			texture->setStream(image.data());
			texture->setup();
			return true;
		}

		return false;
	});

	return loader.data();
}

RenderTexturePtr
RenderFactory::createRenderTexture() noexcept
{
	return std::make_shared<OGLRenderTexture>();
}

MultiRenderTexturePtr
RenderFactory::createMultiRenderTexture() noexcept
{
	return std::make_shared<OGLMultiRenderTexture>();
}

VertexBufferDataPtr
RenderFactory::createVertexBuffer() noexcept
{
	return std::make_shared<VertexBufferData>();
}

IndexBufferDataPtr
RenderFactory::createIndexBuffer() noexcept
{
	return std::make_shared<IndexBufferData>();
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

	VertexComponents components;

	auto& vertices = mesh.getVertexArray();
	if (!vertices.empty())
	{
		if (vertices.size() == numVertex)
			components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_POSITION, VertexFormat::GPU_VERTEX_FLOAT3));
	}

	auto& colors = mesh.getColorArray();
	if (!colors.empty())
	{
		if (colors.size() == numVertex)
			components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_DIFFUSE, VertexFormat::GPU_VERTEX_FLOAT4));
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

	auto& tangent = mesh.getTangentArray();
	if (!tangent.empty())
	{
		if (tangent.size() == numVertex)
			components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_TANGENT, VertexFormat::GPU_VERTEX_FLOAT3));
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

		if (colors.size() == numVertex)
		{
			char* data = (char*)vb->data() + offset;
			for (auto& it : colors)
			{
				*(float4*)data = it;
				data += stride;
			}

			offset += sizeof(float4);
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

RenderBufferPtr 
RenderFactory::createRenderBuffer(const std::vector<MeshPropertyPtr> meshes) except
{
	auto buffer = std::make_shared<OGLRenderBuffer>();

	auto numVertex = 0;
	auto numIndex = 0;

	for (auto& it : meshes)
	{
		numVertex += it->getNumVertices();
		numIndex += it->getNumIndices();
	}

	std::vector<VertexComponent> components;

	if (!meshes.front()->getVertexArray().empty())
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_POSITION, VertexFormat::GPU_VERTEX_FLOAT3));

	if (!meshes.front()->getColorArray().empty())
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_DIFFUSE, VertexFormat::GPU_VERTEX_FLOAT4));

	if (!meshes.front()->getNormalArray().empty())
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_NORMAL, VertexFormat::GPU_VERTEX_FLOAT3));

	if (!meshes.front()->getTexcoordArray().empty())
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_TEXCOORD, VertexFormat::GPU_VERTEX_FLOAT2));

	if (!meshes.front()->getTangentArray().empty())
		components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_TANGENT, VertexFormat::GPU_VERTEX_FLOAT3));
	
	VertexBufferDataPtr vb;

	if (numVertex)
	{
		vb = RenderFactory::createVertexBuffer();
		vb->setVertexComponents(components);
		vb->setup(numVertex, VertexUsage::GPU_USAGE_STATIC);

		std::size_t offsetVertices = 0;
		std::size_t stride = vb->getVertexSize();

		for (auto& mesh : meshes)
		{
			auto& vertices = mesh->getVertexArray();
			auto& colors = mesh->getColorArray();
			auto& normals = mesh->getNormalArray();
			auto& tangents = mesh->getTangentArray();
			auto& texcoords = mesh->getTexcoordArray();

			std::size_t offset1 = 0;

			if (!vertices.empty())
			{
				char* data = (char*)vb->data() + offset1 + offsetVertices;
				for (auto& it : vertices)
				{
					*(float3*)data = it;
					data += stride;
				}

				offset1 += sizeof(float3);
			}

			if (!colors.empty())
			{
				char* data = (char*)vb->data() + offset1 + offsetVertices;
				for (auto& it : colors)
				{
					*(float4*)data = it;
					data += stride;
				}

				offset1 += sizeof(float4);
			}

			if (!normals.empty())
			{
				char* data = (char*)vb->data() + offset1 + offsetVertices;
				for (auto& it : normals)
				{
					*(float3*)data = it;
					data += stride;
				}

				offset1 += sizeof(float3);
			}

			if (!texcoords.empty())
			{
				char* data = (char*)vb->data() + offset1 + offsetVertices;
				for (auto& it : texcoords)
				{
					*(float2*)data = it;
					data += stride;
				}

				offset1 += sizeof(float2);
			}

			if (!tangents.empty())
			{
				char* data = (char*)vb->data() + offset1 + offsetVertices;
				for (auto& it : tangents)
				{
					*(float3*)data = it;
					data += stride;
				}

				offset1 += sizeof(float3);
			}

			offsetVertices += mesh->getNumVertices() * vb->getVertexSize();
		}
	}

	IndexBufferDataPtr ib;

	if (numIndex > 0)
	{
		ib = RenderFactory::createIndexBuffer();
		ib->setup(numIndex, IndexType::GPU_UINT32, VertexUsage::GPU_USAGE_STATIC);

		std::size_t offsetIndices = 0;

		for (auto& it : meshes)
		{
			auto& faces = it->getFaceArray();
			if (!faces.empty())
			{
				std::uint32_t* indices = (std::uint32_t*)ib->data() + offsetIndices;
				for (auto& face : faces)
				{
#if !defined(EGLAPI)
					*indices++ = face;
#else
					*indices++ = offsetIndices + face;
#endif
				}

				offsetIndices += faces.size();
			}
		}
	}

	buffer->setup(vb, ib);

	return buffer;
}

MaterialPtr
RenderFactory::createMaterial(const string& filename) except
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
			throw failure(__TEXT("The file has been damaged and can't be recovered, so I can't open it") + filename);
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
						case ShaderVariantType::SPT_FLOAT2:
						{
							param->assign(parseFloat2(arg.second));
						}
						break;
						case ShaderVariantType::SPT_FLOAT3:
						{
							param->assign(parseFloat3(arg.second));
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