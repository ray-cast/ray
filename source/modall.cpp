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
#include <ray/modall.h>
#include <ray/modcfg.h>

#if _BUILD_OBJ_HANDLER
#   include <ray/modobj.h>
#endif

#if _BUILD_PMD_HANDLER
#   include <ray/modpmd.h>
#endif

#if _BUILD_PMX_HANDLER
#   include <ray/modpmx.h>
#endif

#if _BUILD_VMD_HANDLER
#   include <ray/modvmd.h>
#endif

#if _BUILD_SDKMESH_HANDLER
#   include <ray/modsdkmesh.h>
#endif

// #include "ray/modelx.h"
// #include "ray/model3ds.h"
// #include "ray/modelmd3.h"
// #include "ray/modelmdl.h"
// #include "ray/modelmd2.h"
// #include "ray/modelply.h"
// #include "ray/modelase.h"
// #include "ray/modelhmp.h"
// #include "ray/modelsmd.h"
// #include "ray/modelmdc.h"
// #include "ray/modelmd5.h"
// #include "ray/modelstl.h"
// #include "ray/modellwo.h"
// #include "ray/modeldxf.h"
// #include "ray/modelnff.h"
// #include "ray/modelraw.h"
// #include "ray/modeloff.h"
// #include "ray/modelac.h"
// #include "ray/modelbvh.h"
// #include "ray/modelirr.h"
// #include "ray/modelirrmesh.h"
// #include "ray/modelq3d.h"
// #include "ray/modelb3d.h"
// #include "ray/modelcollada.h"
// #include "ray/modelterragen.h"
// #include "ray/modelcsm.h"
// #include "ray/modelunreal.h"
// #include "ray/modellws.h"
// #include "ray/modelogre.h"
// #include "ray/modelms3d.h"
// #include "ray/modelcob.h"
// #include "ray/modelblender.h"
// #include "ray/modelq3bsp.h"
// #include "ray/modelndo.h"
// #include "ray/IFCLoader.h"
// #include "ray/modelm3.h"
// #include "ray/modelxgl.h"

_NAME_BEGIN

#if _BUILD_OBJ_HANDLER
std::shared_ptr<ModelHandler> objHandler = std::make_shared<ObjHandler>();
#endif

#if _BUILD_PMD_HANDLER
std::shared_ptr<ModelHandler> pmdHandler = std::make_shared<PMDHandler>();
#endif

#if _BUILD_PMX_HANDLER
std::shared_ptr<ModelHandler> pmxHandler = std::make_shared<PMXHandler>();
#endif

#if _BUILD_VMD_HANDLER
std::shared_ptr<ModelHandler> vmdHandler = std::make_shared<VMDHandler>();
#endif

#if _BUILD_SDKMESH_HANDLER
std::shared_ptr<ModelHandler> sdkmeshHandler = std::make_shared<SDKMeshHandler>();
#endif

void GetModelInstanceList(Model& model)
{
#if _BUILD_OBJ_HANDLER
	model.addHandler(objHandler);
#endif

#if _BUILD_PMD_HANDLER
	model.addHandler(pmdHandler);
#endif

#if _BUILD_PMX_HANDLER
	model.addHandler(pmxHandler);
#endif

#if _BUILD_VMD_HANDLER
	model.addHandler(vmdHandler);
#endif

#if _BUILD_SDKMESH_HANDLER
	model.addHandler(sdkmeshHandler);
#endif
	//     #if (!defined _BUILD_NO_X_HANDLER)
	//         out.push_back(new XFileLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_3DS_HANDLER)
	//         out.push_back(new Discreet3DSImporter());
	//     #endif
	//     #if (!defined _BUILD_NO_MD3_HANDLER)
	//         out.push_back(new MD3Loader());
	//     #endif
	//     #if (!defined _BUILD_NO_MD2_HANDLER)
	//         out.push_back(new MD2Loader());
	//     #endif
	//     #if (!defined _BUILD_NO_PLY_HANDLER)
	//         out.push_back(new PLYLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_MDL_HANDLER)
	//         out.push_back(new MDLLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_ASE_HANDLER)
	//         out.push_back(new ASELoader());
	//     #endif
	//     #if (!defined _BUILD_NO_HMP_HANDLER)
	//         out.push_back(new HMPLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_SMD_HANDLER)
	//         out.push_back(new SDMLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_MDC_HANDLER)
	//         out.push_back(new MDCLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_MD5_HANDLER)
	//         out.push_back(new MD5Loader());
	//     #endif
	//     #if (!defined _BUILD_NO_STL_HANDLER)
	//         out.push_back(new STLLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_LWO_HANDLER)
	//         out.push_back(new LWOLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_DXF_HANDLER)
	//         out.push_back(new DXFLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_NFF_HANDLER)
	//         out.push_back(new NFFLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_RAW_HANDLER)
	//         out.push_back(new RAWLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_OFF_HANDLER)
	//         out.push_back(new OFFLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_AC_HANDLER)
	//         out.push_back(new AC3DLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_BVH_HANDLER)
	//         out.push_back(new BVHLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_IRRMESH_HANDLER)
	//         out.push_back(new IRRMeshLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_IRR_HANDLER)
	//         out.push_back(new IRRLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_Q3D_HANDLER)
	//         out.push_back(new Q3DLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_B3D_HANDLER)
	//         out.push_back(new B3DLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_COLLADA_HANDLER)
	//      out.push_back(new ColladaLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_TERRAGEN_HANDLER)
	//         out.push_back(new TerragenLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_CSM_HANDLER)
	//         out.push_back(new CSMLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_3D_HANDLER)
	//         out.push_back(new UnrealLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_LWS_HANDLER)
	//         out.push_back(new LWSHandler());
	//     #endif
	//     #if (!defined _BUILD_NO_OGRE_HANDLER)
	//         out.push_back(new Ogre::OgreLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_MS3D_HANDLER)
	//         out.push_back(new MS3DLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_COB_HANDLER)
	//         out.push_back(new COBLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_BLEND_HANDLER)
	//         out.push_back(new BlenderLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_Q3BSP_HANDLER)
	//         out.push_back(new Q3BSPFileLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_NDO_HANDLER)
	//         out.push_back(new NDOLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_IFC_HANDLER)
	//         out.push_back(new IFCLoader());
	//     #endif
	//     #if (!defined _BUILD_NO_M3_HANDLER)
	//         out.push_back(new M3::M3Loader());
	//     #endif
	//     #if (!defined _BUILD_NO_XGL_HANDLER)
	//         out.push_back(new XGLLoader());
	//     #endif
}

_NAME_END