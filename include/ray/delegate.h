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
#ifndef _H_DELEGATE_H_
#define _H_DELEGATE_H_

#include <vector>
#include <assert.h>
#include <ray/trait.h>
#include <ray/macro.h>
#include <ray/except.h>

_NAME_BEGIN

template<typename T>
class binder;

template<typename _Tx, typename _Ty>
class delegate_bind;

template<typename T>
class delegate;

#define DELEGATE_MAX_ARGS 0
#include "delegate_impl.h"
#undef DELEGATE_MAX_ARGS

#define DELEGATE_MAX_ARGS 1
#include "delegate_impl.h"
#undef DELEGATE_MAX_ARGS

#define DELEGATE_MAX_ARGS 2
#include "delegate_impl.h"
#undef DELEGATE_MAX_ARGS

#define DELEGATE_MAX_ARGS 3
#include "delegate_impl.h"
#undef DELEGATE_MAX_ARGS

#define DELEGATE_MAX_ARGS 4
#include "delegate_impl.h"
#undef DELEGATE_MAX_ARGS

#define DELEGATE_MAX_ARGS 5
#include "delegate_impl.h"
#undef DELEGATE_MAX_ARGS

#define DELEGATE_MAX_ARGS 6
#include "delegate_impl.h"
#undef DELEGATE_MAX_ARGS

#define DELEGATE_MAX_ARGS 7
#include "delegate_impl.h"
#undef DELEGATE_MAX_ARGS

#define DELEGATE_MAX_ARGS 8
#include "delegate_impl.h"
#undef DELEGATE_MAX_ARGS

template<typename _Functor, typename _This>
inline delegate_bind<typename trait::functor<_Functor>::result_type(), std::pair<_Functor, _This>> make_binder(_Functor t1, _This t2)
{
	return delegate_bind<typename trait::functor<_Functor>::result_type(), std::pair<_Functor, _This>>(std::make_pair(t1, t2));
}

template<typename _Functor, typename _This>
inline delegate_bind<typename trait::functor<_Functor>::result_type(typename trait::functor<_Functor>::arg_type), std::pair<_Functor, _This>> make_binder(_Functor t1, _This t2)
{
	return delegate_bind<typename trait::functor<_Functor>::result_type(typename trait::functor<_Functor>::arg_type), std::pair<_Functor, _This>>(std::make_pair(t1, t2));
}

_NAME_END

#endif