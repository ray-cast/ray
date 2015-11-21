#ifndef _GUI_BUFFER_H_
#define _GUI_BUFFER_H_

#include "MyGUI_Prerequest.h"
#include "MyGUI_IVertexBuffer.h"
#include <ray/platform.h>

_NAME_BEGIN

class OpenGL3VertexBuffer : public MyGUI::IVertexBuffer
{
public:
	OpenGL3VertexBuffer();
	virtual ~OpenGL3VertexBuffer();

	virtual void setVertexCount(std::size_t _count);
	virtual std::size_t getVertexCount();

	virtual MyGUI::Vertex* lock();
	virtual void unlock();

	void destroy();
	void create();

	unsigned int getBufferID() const
	{
		return mVAOID;
	}

	private:
	unsigned int mVAOID;
	unsigned int mBufferID;
	size_t mNeedVertexCount;
	size_t mSizeInBytes;
};

_NAME_END

#endif