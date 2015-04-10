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
#ifndef _H_MODEL_DEF_H_
#define _H_MODEL_DEF_H_

#include <ray/modcfg.h>

_NAME_BEGIN

enum ModelType
{
    MT_UNKNOWN,
#if _BUILD_3DS_HANDLER
    MT_3DS,
#endif
#if _BUILD_ASE_HANDLER
    MT_ASE,
#endif
#if _BUILD_B3D_HANDLER
    MT_B3D,
#endif
#if _BUILD_BLEND_HANDLER
    MT_BLEND,
#endif
#if _BUILD_DXF_HANDLER
    MT_DXF,
#endif
#if _BUILD_HMP_HANDLER
    MT_HMP,
#endif
#if _BUILD_IFC_HANDLER
    MT_IFC,
#endif
#if _BUILD_IRR_HANDLER
    MT_IRR,
#endif
#if _BUILD_LWO_HANDLER
    MT_LWO,
#endif
#if _BUILD_LWS_HANDLER
    MT_LWS,
#endif
#if _BUILD_M3_HANDLER
    MT_M3,
#endif
#if _BUILD_MD2_HANDLER
    MT_MD2,
#endif
#if _BUILD_MD3_HANDLER
    MT_MD3,
#endif
#if _BUILD_MD4_HANDLER
    MT_MD4,
#endif
#if _BUILD_MD5_HANDLER
    MT_MD5,
#endif
#if _BUILD_MDL_HANDLER
    MT_MDL,
#endif
#if _BUILD_MESH_HANDLER
    MT_MESH,
#endif
#if _BUILD_NFF_HANDLER
    MT_NFF,
#endif
#if _BUILD_OBJ_HANDLER
    MT_OBJ,
#endif
#if _BUILD_OGRE_HANDLER
    MT_OGRE,
#endif
#if _BUILD_PK3_HANDLER
    MT_PK3,
#endif
#if _BUILD_PMD_HANDLER
    MT_PMD,
#endif
#if _BUILD_PMX_HANDLER
    MT_PMX,
#endif
#if _BUILD_X_HANDLER
    MT_XFILE,
#endif
};

_NAME_END

#endif
