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
#if defined(_BUILD_MYGUI)
#include <ray/mygui_manager.h>

_NAME_BEGIN

class DataStream : public MyGUI::IDataStream
{
public:
	DataStream()
	{
	}

	virtual ~DataStream()
	{
	}

	bool open(const std::string& path)
	{
		return IoServer::instance()->openFile(_stream, path);
	}

	virtual bool eof()
	{
		assert(_stream);
		return _stream->eof();
	}

	virtual size_t size()
	{
		assert(_stream);
		return _stream->size();
	}

	virtual void readline(std::string& _source, MyGUI::Char _delim)
	{
		assert(_stream);

		_source.clear();

		for (;;)
		{
			char buffer;
			if (!_stream->read(&buffer, 1))
				break;

			if (_stream->eof())
				break;

			if (util::isLineEnd(buffer))
				break;

			_source.push_back(buffer);
		}
	}

	virtual size_t read(void* _buf, size_t _count)
	{
		assert(_stream);
		_stream->read((char*)_buf, _count);
		return _stream->gcount();
	}

private:
	StreamReaderPtr _stream;
};

MyGuiResManager::MyGuiResManager() noexcept
	: _isInitialise(false)
{
}

void
MyGuiResManager::open() noexcept
{
	assert(!_isInitialise);
	_isInitialise = true;
}

void 
MyGuiResManager::close() noexcept
{
	assert(_isInitialise);
	_isInitialise = false;
}

MyGUI::IDataStream*
MyGuiResManager::getData(const std::string& _name)
{
	auto stream = make_scope<DataStream>();
	if (stream->open(_name))
	{
		return stream.dismiss();
	}

	return nullptr;
}

void 
MyGuiResManager::freeData(MyGUI::IDataStream* _data)
{
	delete _data;
}

bool
MyGuiResManager::isDataExist(const std::string& _name)
{
	return IoServer::instance()->existsFile(_name);
}

const MyGUI::VectorString&
MyGuiResManager::getDataListNames(const std::string& _pattern)
{
	assert(false);
	MyGUI::VectorString v;
	return v;
}

const std::string& 
MyGuiResManager::getDataPath(const std::string& _name)
{
	IoServer::instance()->getResolveAssign(_name, _resolvePath);
	return _resolvePath;
}

_NAME_END
#endif