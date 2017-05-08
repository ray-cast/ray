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
#ifndef _H_MACRO_H_
#define _H_MACRO_H_

#define JOIN(a, b) a##b

#define MAKE_PARAMS1_0(t)
#define MAKE_PARAMS1_1(t)    t##1
#define MAKE_PARAMS1_2(t)    t##1, t##2
#define MAKE_PARAMS1_3(t)    t##1, t##2, t##3
#define MAKE_PARAMS1_4(t)    t##1, t##2, t##3, t##4
#define MAKE_PARAMS1_5(t)    t##1, t##2, t##3, t##4, t##5
#define MAKE_PARAMS1_6(t)    t##1, t##2, t##3, t##4, t##5, t##6
#define MAKE_PARAMS1_7(t)    t##1, t##2, t##3, t##4, t##5, t##6,t##7
#define MAKE_PARAMS1_8(t)    t##1, t##2, t##3, t##4, t##5, t##6,t##7,t##8
#define MAKE_PARAMS1_9(t)    t##1, t##2, t##3, t##4, t##5, t##6,t##7,t##8,t##9
#define MAKE_PARAMS1_10(t)   t##1, t##2, t##3, t##4, t##5, t##6,t##7,t##8,t##9,t##10

#define MAKE_PARAMS2_0(t1, t2)
#define MAKE_PARAMS2_1(t1, t2)   t1##1 t2##1
#define MAKE_PARAMS2_2(t1, t2)   t1##1 t2##1, t1##2 t2##2
#define MAKE_PARAMS2_3(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3
#define MAKE_PARAMS2_4(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4
#define MAKE_PARAMS2_5(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4, t1##5 t2##5
#define MAKE_PARAMS2_6(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4, t1##5 t2##5, t1##6 t2##6
#define MAKE_PARAMS2_7(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4, t1##5 t2##5, t1##6 t2##6,t1##7 t2##7
#define MAKE_PARAMS2_8(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4, t1##5 t2##5, t1##6 t2##6,t1##7 t2##7,t1##8 t2##8
#define MAKE_PARAMS2_9(t1, t2)   t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4, t1##5 t2##5, t1##6 t2##6,t1##7 t2##7,t1##8 t2##8,t1##9 t2##9
#define MAKE_PARAMS2_10(t1, t2)  t1##1 t2##1, t1##2 t2##2, t1##3 t2##3, t1##4 t2##4, t1##5 t2##5, t1##6 t2##6,t1##7 t2##7,t1##8 t2##8,t1##9 t2##9,t1##10 t2##10

#define MAKE_PARAMS1(n, t)         JOIN(MAKE_PARAMS1_, n) (t)
#define MAKE_PARAMS2(n, t1, t2)    JOIN(MAKE_PARAMS2_, n) (t1, t2)

#endif