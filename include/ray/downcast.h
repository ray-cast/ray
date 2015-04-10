#ifndef _H_DOWN_CAST_H_
#define _H_DOWN_CAST_H_

#include <ray/platform.h>

_NAME_BEGIN

template<typename T>
class down_cast
{
public:
    typedef T* pointer;
    typedef const T* const_pointer;

public:
    pointer downcast() noexcept
    {
        return static_cast<pointer>(this);
    };

    const_pointer downcast() const noexcept
    {
        return static_cast<const_pointer>(this);
    };
};

_NAME_END

#endif