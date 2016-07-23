// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include "ogl_types.h"

#if defined(_BUILD_PLATFORM_LINUX) || defined(_BUILD_PLATFORM_APPLE) && defined(_BUILD_OPENGL_ES)
#	include <dlfcn.h>
#endif

_NAME_BEGIN

#ifdef GLEW_MX
#	ifdef _WIN32
WGLEWContext _wglewctx;
#		define wglewGetContext() (&_wglewctx)
#	elif !defined(__APPLE__) || defined(GLEW_APPLE_GLX)
GLXEWContext _glxewctx;
#	define glxewGetContext() (&_glxewctx)
#	endif
#endif

#if defined(_BUILD_OPENGL_ES)

PFNGLACCUM __glAccum;
PFNGLALPHAFUNC __glAlphaFunc;
PFNGLARETEXTURESRESIDENT __glAreTexturesResident;
PFNGLARRAYELEMENT __glArrayElement;
PFNGLBEGIN __glBegin;
PFNGLBINDTEXTURE __glBindTexture;
PFNGLBITMAP __glBitmap;
PFNGLBLENDFUNC __glBlendFunc;
PFNGLCALLLIST __glCallList;
PFNGLCALLLISTS __glCallLists;
PFNGLCLEAR __glClear;
PFNGLCLEARACCUM __glClearAccum;
PFNGLCLEARCOLOR __glClearColor;
PFNGLCLEARDEPTH __glClearDepth;
PFNGLCLEARINDEX __glClearIndex;
PFNGLCLEARSTENCIL __glClearStencil;
PFNGLCLIPPLANE __glClipPlane;
PFNGLCOLOR3B __glColor3b;
PFNGLCOLOR3BV __glColor3bv;
PFNGLCOLOR3D __glColor3d;
PFNGLCOLOR3DV __glColor3dv;
PFNGLCOLOR3F __glColor3f;
PFNGLCOLOR3FV __glColor3fv;
PFNGLCOLOR3I __glColor3i;
PFNGLCOLOR3IV __glColor3iv;
PFNGLCOLOR3S __glColor3s;
PFNGLCOLOR3SV __glColor3sv;
PFNGLCOLOR3UB __glColor3ub;
PFNGLCOLOR3UBV __glColor3ubv;
PFNGLCOLOR3UI __glColor3ui;
PFNGLCOLOR3UIV __glColor3uiv;
PFNGLCOLOR3US __glColor3us;
PFNGLCOLOR3USV __glColor3usv;
PFNGLCOLOR4B __glColor4b;
PFNGLCOLOR4BV __glColor4bv;
PFNGLCOLOR4D __glColor4d;
PFNGLCOLOR4DV __glColor4dv;
PFNGLCOLOR4F __glColor4f;
PFNGLCOLOR4FV __glColor4fv;
PFNGLCOLOR4I __glColor4i;
PFNGLCOLOR4IV __glColor4iv;
PFNGLCOLOR4S __glColor4s;
PFNGLCOLOR4SV __glColor4sv;
PFNGLCOLOR4UB __glColor4ub;
PFNGLCOLOR4UBV __glColor4ubv;
PFNGLCOLOR4UI __glColor4ui;
PFNGLCOLOR4UIV __glColor4uiv;
PFNGLCOLOR4US __glColor4us;
PFNGLCOLOR4USV __glColor4usv;
PFNGLCOLORMASK __glColorMask;
PFNGLCOLORMATERIAL __glColorMaterial;
PFNGLCOLORPOINTER __glColorPointer;
PFNGLCOPYPIXELS __glCopyPixels;
PFNGLCOPYTEXIMAGE1D __glCopyTexImage1D;
PFNGLCOPYTEXIMAGE2D __glCopyTexImage2D;
PFNGLCOPYTEXSUBIMAGE1D __glCopyTexSubImage1D;
PFNGLCOPYTEXSUBIMAGE2D __glCopyTexSubImage2D;
PFNGLCULLFACE __glCullFace;
PFNGLDELETELISTS __glDeleteLists;
PFNGLDELETETEXTURES __glDeleteTextures;
PFNGLDEPTHFUNC __glDepthFunc;
PFNGLDEPTHMASK __glDepthMask;
PFNGLDEPTHRANGE __glDepthRange;
PFNGLDISABLE __glDisable;
PFNGLDISABLECLIENTSTATE __glDisableClientState;
PFNGLDRAWARRAYS __glDrawArrays;
PFNGLDRAWBUFFER __glDrawBuffer;
PFNGLDRAWELEMENTS __glDrawElements;
PFNGLDRAWPIXELS __glDrawPixels;
PFNGLEDGEFLAG __glEdgeFlag;
PFNGLEDGEFLAGPOINTER __glEdgeFlagPointer;
PFNGLEDGEFLAGV __glEdgeFlagv;
PFNGLENABLE __glEnable;
PFNGLENABLECLIENTSTATE __glEnableClientState;
PFNGLEND __glEnd;
PFNGLENDLIST __glEndList;
PFNGLEVALCOORD1D __glEvalCoord1d;
PFNGLEVALCOORD1DV __glEvalCoord1dv;
PFNGLEVALCOORD1F __glEvalCoord1f;
PFNGLEVALCOORD1FV __glEvalCoord1fv;
PFNGLEVALCOORD2D __glEvalCoord2d;
PFNGLEVALCOORD2DV __glEvalCoord2dv;
PFNGLEVALCOORD2F __glEvalCoord2f;
PFNGLEVALCOORD2FV __glEvalCoord2fv;
PFNGLEVALMESH1 __glEvalMesh1;
PFNGLEVALMESH2 __glEvalMesh2;
PFNGLEVALPOINT1 __glEvalPoint1;
PFNGLEVALPOINT2 __glEvalPoint2;
PFNGLFEEDBACKBUFFER __glFeedbackBuffer;
PFNGLFINISH __glFinish;
PFNGLFLUSH __glFlush;
PFNGLFOGF __glFogf;
PFNGLFOGFV __glFogfv;
PFNGLFOGI __glFogi;
PFNGLFOGIV __glFogiv;
PFNGLFRONTFACE __glFrontFace;
PFNGLFRUSTUM __glFrustum;
PFNGLGENLISTS __glGenLists;
PFNGLGENTEXTURES __glGenTextures;
PFNGLGETBOOLEANV __glGetBooleanv;
PFNGLGETCLIPPLANE __glGetClipPlane;
PFNGLGETDOUBLEV __glGetDoublev;
PFNGLGETERROR __glGetError;
PFNGLGETFLOATV __glGetFloatv;
PFNGLGETINTEGERV __glGetIntegerv;
PFNGLGETLIGHTFV __glGetLightfv;
PFNGLGETLIGHTIV __glGetLightiv;
PFNGLGETMAPDV __glGetMapdv;
PFNGLGETMAPFV __glGetMapfv;
PFNGLGETMAPIV __glGetMapiv;
PFNGLGETMATERIALFV __glGetMaterialfv;
PFNGLGETMATERIALIV __glGetMaterialiv;
PFNGLGETPIXELMAPFV __glGetPixelMapfv;
PFNGLGETPIXELMAPUIV __glGetPixelMapuiv;
PFNGLGETPIXELMAPUSV __glGetPixelMapusv;
PFNGLGETPOINTERV __glGetPointerv;
PFNGLGETPOLYGONSTIPPLE __glGetPolygonStipple;
PFNGLGETSTRING __glGetString;
PFNGLGETTEXENVFV __glGetTexEnvfv;
PFNGLGETTEXENVIV __glGetTexEnviv;
PFNGLGETTEXGENDV __glGetTexGendv;
PFNGLGETTEXGENFV __glGetTexGenfv;
PFNGLGETTEXGENIV __glGetTexGeniv;
PFNGLGETTEXIMAGE __glGetTexImage;
PFNGLGETTEXLEVELPARAMETERFV __glGetTexLevelParameterfv;
PFNGLGETTEXLEVELPARAMETERIV __glGetTexLevelParameteriv;
PFNGLGETTEXPARAMETERFV __glGetTexParameterfv;
PFNGLGETTEXPARAMETERIV __glGetTexParameteriv;
PFNGLHINT __glHint;
PFNGLINDEXMASK __glIndexMask;
PFNGLINDEXPOINTER __glIndexPointer;
PFNGLINDEXD __glIndexd;
PFNGLINDEXDV __glIndexdv;
PFNGLINDEXF __glIndexf;
PFNGLINDEXFV __glIndexfv;
PFNGLINDEXI __glIndexi;
PFNGLINDEXIV __glIndexiv;
PFNGLINDEXS __glIndexs;
PFNGLINDEXSV __glIndexsv;
PFNGLINDEXUB __glIndexub;
PFNGLINDEXUBV __glIndexubv;
PFNGLINITNAMES __glInitNames;
PFNGLINTERLEAVEDARRAYS __glInterleavedArrays;
PFNGLISENABLED __glIsEnabled;
PFNGLISLIST __glIsList;
PFNGLISTEXTURE __glIsTexture;
PFNGLLIGHTMODELF __glLightModelf;
PFNGLLIGHTMODELFV __glLightModelfv;
PFNGLLIGHTMODELI __glLightModeli;
PFNGLLIGHTMODELIV __glLightModeliv;
PFNGLLIGHTF __glLightf;
PFNGLLIGHTFV __glLightfv;
PFNGLLIGHTI __glLighti;
PFNGLLIGHTIV __glLightiv;
PFNGLLINESTIPPLE __glLineStipple;
PFNGLLINEWIDTH __glLineWidth;
PFNGLLISTBASE __glListBase;
PFNGLLOADIDENTITY __glLoadIdentity;
PFNGLLOADMATRIXD __glLoadMatrixd;
PFNGLLOADMATRIXF __glLoadMatrixf;
PFNGLLOADNAME __glLoadName;
PFNGLLOGICOP __glLogicOp;
PFNGLMAP1D __glMap1d;
PFNGLMAP1F __glMap1f;
PFNGLMAP2D __glMap2d;
PFNGLMAP2F __glMap2f;
PFNGLMAPGRID1D __glMapGrid1d;
PFNGLMAPGRID1F __glMapGrid1f;
PFNGLMAPGRID2D __glMapGrid2d;
PFNGLMAPGRID2F __glMapGrid2f;
PFNGLMATERIALF __glMaterialf;
PFNGLMATERIALFV __glMaterialfv;
PFNGLMATERIALI __glMateriali;
PFNGLMATERIALIV __glMaterialiv;
PFNGLMATRIXMODE __glMatrixMode;
PFNGLMULTMATRIXD __glMultMatrixd;
PFNGLMULTMATRIXF __glMultMatrixf;
PFNGLNEWLIST __glNewList;
PFNGLNORMAL3B __glNormal3b;
PFNGLNORMAL3BV __glNormal3bv;
PFNGLNORMAL3D __glNormal3d;
PFNGLNORMAL3DV __glNormal3dv;
PFNGLNORMAL3F __glNormal3f;
PFNGLNORMAL3FV __glNormal3fv;
PFNGLNORMAL3I __glNormal3i;
PFNGLNORMAL3IV __glNormal3iv;
PFNGLNORMAL3S __glNormal3s;
PFNGLNORMAL3SV __glNormal3sv;
PFNGLNORMALPOINTER __glNormalPointer;
PFNGLORTHO __glOrtho;
PFNGLPASSTHROUGH __glPassThrough;
PFNGLPIXELMAPFV __glPixelMapfv;
PFNGLPIXELMAPUIV __glPixelMapuiv;
PFNGLPIXELMAPUSV __glPixelMapusv;
PFNGLPIXELSTOREF __glPixelStoref;
PFNGLPIXELSTOREI __glPixelStorei;
PFNGLPIXELTRANSFERF __glPixelTransferf;
PFNGLPIXELTRANSFERI __glPixelTransferi;
PFNGLPIXELZOOM __glPixelZoom;
PFNGLPOINTSIZE __glPointSize;
PFNGLPOLYGONMODE __glPolygonMode;
PFNGLPOLYGONOFFSET __glPolygonOffset;
PFNGLPOLYGONSTIPPLE __glPolygonStipple;
PFNGLPOPATTRIB __glPopAttrib;
PFNGLPOPCLIENTATTRIB __glPopClientAttrib;
PFNGLPOPMATRIX __glPopMatrix;
PFNGLPOPNAME __glPopName;
PFNGLPRIORITIZETEXTURES __glPrioritizeTextures;
PFNGLPUSHATTRIB __glPushAttrib;
PFNGLPUSHCLIENTATTRIB __glPushClientAttrib;
PFNGLPUSHMATRIX __glPushMatrix;
PFNGLPUSHNAME __glPushName;
PFNGLRASTERPOS2D __glRasterPos2d;
PFNGLRASTERPOS2DV __glRasterPos2dv;
PFNGLRASTERPOS2F __glRasterPos2f;
PFNGLRASTERPOS2FV __glRasterPos2fv;
PFNGLRASTERPOS2I __glRasterPos2i;
PFNGLRASTERPOS2IV __glRasterPos2iv;
PFNGLRASTERPOS2S __glRasterPos2s;
PFNGLRASTERPOS2SV __glRasterPos2sv;
PFNGLRASTERPOS3D __glRasterPos3d;
PFNGLRASTERPOS3DV __glRasterPos3dv;
PFNGLRASTERPOS3F __glRasterPos3f;
PFNGLRASTERPOS3FV __glRasterPos3fv;
PFNGLRASTERPOS3I __glRasterPos3i;
PFNGLRASTERPOS3IV __glRasterPos3iv;
PFNGLRASTERPOS3S __glRasterPos3s;
PFNGLRASTERPOS3SV __glRasterPos3sv;
PFNGLRASTERPOS4D __glRasterPos4d;
PFNGLRASTERPOS4DV __glRasterPos4dv;
PFNGLRASTERPOS4F __glRasterPos4f;
PFNGLRASTERPOS4FV __glRasterPos4fv;
PFNGLRASTERPOS4I __glRasterPos4i;
PFNGLRASTERPOS4IV __glRasterPos4iv;
PFNGLRASTERPOS4S __glRasterPos4s;
PFNGLRASTERPOS4SV __glRasterPos4sv;
PFNGLREADBUFFER __glReadBuffer;
PFNGLREADPIXELS __glReadPixels;
PFNGLRECTD __glRectd;
PFNGLRECTDV __glRectdv;
PFNGLRECTF __glRectf;
PFNGLRECTFV __glRectfv;
PFNGLRECTI __glRecti;
PFNGLRECTIV __glRectiv;
PFNGLRECTS __glRects;
PFNGLRECTSV __glRectsv;
PFNGLRENDERMODE __glRenderMode;
PFNGLROTATED __glRotated;
PFNGLROTATEF __glRotatef;
PFNGLSCALED __glScaled;
PFNGLSCALEF __glScalef;
PFNGLSCISSOR __glScissor;
PFNGLSELECTBUFFER __glSelectBuffer;
PFNGLSHADEMODEL __glShadeModel;
PFNGLSTENCILFUNC __glStencilFunc;
PFNGLSTENCILMASK __glStencilMask;
PFNGLSTENCILOP __glStencilOp;
PFNGLTEXCOORD1D __glTexCoord1d;
PFNGLTEXCOORD1DV __glTexCoord1dv;
PFNGLTEXCOORD1F __glTexCoord1f;
PFNGLTEXCOORD1FV __glTexCoord1fv;
PFNGLTEXCOORD1I __glTexCoord1i;
PFNGLTEXCOORD1IV __glTexCoord1iv;
PFNGLTEXCOORD1S __glTexCoord1s;
PFNGLTEXCOORD1SV __glTexCoord1sv;
PFNGLTEXCOORD2D __glTexCoord2d;
PFNGLTEXCOORD2DV __glTexCoord2dv;
PFNGLTEXCOORD2F __glTexCoord2f;
PFNGLTEXCOORD2FV __glTexCoord2fv;
PFNGLTEXCOORD2I __glTexCoord2i;
PFNGLTEXCOORD2IV __glTexCoord2iv;
PFNGLTEXCOORD2S __glTexCoord2s;
PFNGLTEXCOORD2SV __glTexCoord2sv;
PFNGLTEXCOORD3D __glTexCoord3d;
PFNGLTEXCOORD3DV __glTexCoord3dv;
PFNGLTEXCOORD3F __glTexCoord3f;
PFNGLTEXCOORD3FV __glTexCoord3fv;
PFNGLTEXCOORD3I __glTexCoord3i;
PFNGLTEXCOORD3IV __glTexCoord3iv;
PFNGLTEXCOORD3S __glTexCoord3s;
PFNGLTEXCOORD3SV __glTexCoord3sv;
PFNGLTEXCOORD4D __glTexCoord4d;
PFNGLTEXCOORD4DV __glTexCoord4dv;
PFNGLTEXCOORD4F __glTexCoord4f;
PFNGLTEXCOORD4FV __glTexCoord4fv;
PFNGLTEXCOORD4I __glTexCoord4i;
PFNGLTEXCOORD4IV __glTexCoord4iv;
PFNGLTEXCOORD4S __glTexCoord4s;
PFNGLTEXCOORD4SV __glTexCoord4sv;
PFNGLTEXCOORDPOINTER __glTexCoordPointer;
PFNGLTEXENVF __glTexEnvf;
PFNGLTEXENVFV __glTexEnvfv;
PFNGLTEXENVI __glTexEnvi;
PFNGLTEXENVIV __glTexEnviv;
PFNGLTEXGEND __glTexGend;
PFNGLTEXGENDV __glTexGendv;
PFNGLTEXGENF __glTexGenf;
PFNGLTEXGENFV __glTexGenfv;
PFNGLTEXGENI __glTexGeni;
PFNGLTEXGENIV __glTexGeniv;
PFNGLTEXIMAGE1D __glTexImage1D;
PFNGLTEXIMAGE2D __glTexImage2D;
PFNGLTEXPARAMETERF __glTexParameterf;
PFNGLTEXPARAMETERFV __glTexParameterfv;
PFNGLTEXPARAMETERI __glTexParameteri;
PFNGLTEXPARAMETERIV __glTexParameteriv;
PFNGLTEXSUBIMAGE1D __glTexSubImage1D;
PFNGLTEXSUBIMAGE2D __glTexSubImage2D;
PFNGLTRANSLATED __glTranslated;
PFNGLTRANSLATEF __glTranslatef;
PFNGLVERTEX2D __glVertex2d;
PFNGLVERTEX2DV __glVertex2dv;
PFNGLVERTEX2F __glVertex2f;
PFNGLVERTEX2FV __glVertex2fv;
PFNGLVERTEX2I __glVertex2i;
PFNGLVERTEX2IV __glVertex2iv;
PFNGLVERTEX2S __glVertex2s;
PFNGLVERTEX2SV __glVertex2sv;
PFNGLVERTEX3D __glVertex3d;
PFNGLVERTEX3DV __glVertex3dv;
PFNGLVERTEX3F __glVertex3f;
PFNGLVERTEX3FV __glVertex3fv;
PFNGLVERTEX3I __glVertex3i;
PFNGLVERTEX3IV __glVertex3iv;
PFNGLVERTEX3S __glVertex3s;
PFNGLVERTEX3SV __glVertex3sv;
PFNGLVERTEX4D __glVertex4d;
PFNGLVERTEX4DV __glVertex4dv;
PFNGLVERTEX4F __glVertex4f;
PFNGLVERTEX4FV __glVertex4fv;
PFNGLVERTEX4I __glVertex4i;
PFNGLVERTEX4IV __glVertex4iv;
PFNGLVERTEX4S __glVertex4s;
PFNGLVERTEX4SV __glVertex4sv;
PFNGLVERTEXPOINTER __glVertexPointer;
PFNGLVIEWPORT __glViewport;

#endif

#if defined(_BUILD_PLATFORM_WINDOWS)
PFNWGLSWAPINTERVALEXTPROC __wglSwapIntervalEXT;
PFNWGLCREATECONTEXTATTRIBSARBPROC   __wglCreateContextAttribsARB;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC __wglGetPixelFormatAttribivARB;
#elif defined(_BUILD_OPENGL_ES)
#	define GetProcAddress dlsym
#endif

bool initGLExtention = false;

bool initGLExtenstion() noexcept
{
	if (initGLExtention)
		return true;

#if	defined(_BUILD_PLATFORM_APPLE)
	glewExperimental = true;
#endif
	if (glewInit() != GLEW_OK)
	{
		GL_PLATFORM_LOG("glewInit() failed.");
		return false;
	}
	
#if defined(GLEW_MX)
#	if	defined(_BUILD_PLATFORM_WINDOWS)
	if (wglewInit() != GLEW_OK)
		return false;
#	elif defined(_BUILD_PLATFORM_LINUX)
	if (glxewInit() != GLEW_OK)
		return false;
#	endif
#endif

#if defined(_BUILD_PLATFORM_WINDOWS) && defined(_BUILD_OPENGL_ES)
	HMODULE module = ::LoadLibrary("OpenGL32");
	if (!module)
	{
		GL_PLATFORM_LOG("OpenGL dynamic library is not found.");
		return false;
	}
#elif defined(_BUILD_PLATFORM_LINUX) && defined(_BUILD_OPENGL_ES)
	void* module = ::dlopen("/usr/lib/x86_64-linux-gnu/libGL.so.1", RTLD_NOW);
	if (!module)
	{
		GL_PLATFORM_LOG("OpenGL dynamic library is not found.");
		return false;
	}
#elif defined(_BUILD_PLATFORM_APPLE) && defined(_BUILD_OPENGL_ES)
	void* module = ::dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_NOW);
	if (!module)
	{
		GL_PLATFORM_LOG("OpenGL dynamic library is not found.");
		return false;
	}
#endif

#if defined(_BUILD_OPENGL_ES)
	__glAccum = (PFNGLACCUM)::GetProcAddress(module, "glAccum");
	__glAlphaFunc = (PFNGLALPHAFUNC)::GetProcAddress(module, "glAlphaFunc");
	__glAreTexturesResident = (PFNGLARETEXTURESRESIDENT)::GetProcAddress(module, "glAreTexturesResident");
	__glArrayElement = (PFNGLARRAYELEMENT)::GetProcAddress(module, "glArrayElement");
	__glBegin = (PFNGLBEGIN)::GetProcAddress(module, "glBegin");
	__glBindTexture = (PFNGLBINDTEXTURE)::GetProcAddress(module, "glBindTexture");
	__glBitmap = (PFNGLBITMAP)::GetProcAddress(module, "glBitmap");
	__glBlendFunc = (PFNGLBLENDFUNC)::GetProcAddress(module, "glBlendFunc");
	__glCallList = (PFNGLCALLLIST)::GetProcAddress(module, "glCallList");
	__glCallLists = (PFNGLCALLLISTS)::GetProcAddress(module, "glCallLists");
	__glClear = (PFNGLCLEAR)::GetProcAddress(module, "glClear");
	__glClearAccum = (PFNGLCLEARACCUM)::GetProcAddress(module, "glClearAccum");
	__glClearColor = (PFNGLCLEARCOLOR)::GetProcAddress(module, "glClearColor");
	__glClearDepth = (PFNGLCLEARDEPTH)::GetProcAddress(module, "glClearDepth");
	__glClearIndex = (PFNGLCLEARINDEX)::GetProcAddress(module, "glClearIndex");
	__glClearStencil = (PFNGLCLEARSTENCIL)::GetProcAddress(module, "glClearStencil");
	__glClipPlane = (PFNGLCLIPPLANE)::GetProcAddress(module, "glClipPlane");
	__glColor3b = (PFNGLCOLOR3B)::GetProcAddress(module, "glColor3b");
	__glColor3bv = (PFNGLCOLOR3BV)::GetProcAddress(module, "glColor3bv");
	__glColor3d = (PFNGLCOLOR3D)::GetProcAddress(module, "glColor3d");
	__glColor3dv = (PFNGLCOLOR3DV)::GetProcAddress(module, "glColor3dv");
	__glColor3f = (PFNGLCOLOR3F)::GetProcAddress(module, "glColor3f");
	__glColor3fv = (PFNGLCOLOR3FV)::GetProcAddress(module, "glColor3fv");
	__glColor3i = (PFNGLCOLOR3I)::GetProcAddress(module, "glColor3i");
	__glColor3iv = (PFNGLCOLOR3IV)::GetProcAddress(module, "glColor3iv");
	__glColor3s = (PFNGLCOLOR3S)::GetProcAddress(module, "glColor3s");
	__glColor3sv = (PFNGLCOLOR3SV)::GetProcAddress(module, "glColor3sv");
	__glColor3ub = (PFNGLCOLOR3UB)::GetProcAddress(module, "glColor3ub");
	__glColor3ubv = (PFNGLCOLOR3UBV)::GetProcAddress(module, "glColor3ubv");
	__glColor3ui = (PFNGLCOLOR3UI)::GetProcAddress(module, "glColor3ui");
	__glColor3uiv = (PFNGLCOLOR3UIV)::GetProcAddress(module, "glColor3uiv");
	__glColor3us = (PFNGLCOLOR3US)::GetProcAddress(module, "glColor3us");
	__glColor3usv = (PFNGLCOLOR3USV)::GetProcAddress(module, "glColor3usv");
	__glColor4b = (PFNGLCOLOR4B)::GetProcAddress(module, "glColor4b");
	__glColor4bv = (PFNGLCOLOR4BV)::GetProcAddress(module, "glColor4bv");
	__glColor4d = (PFNGLCOLOR4D)::GetProcAddress(module, "glColor4d");
	__glColor4dv = (PFNGLCOLOR4DV)::GetProcAddress(module, "glColor4dv");
	__glColor4f = (PFNGLCOLOR4F)::GetProcAddress(module, "glColor4f");
	__glColor4fv = (PFNGLCOLOR4FV)::GetProcAddress(module, "glColor4fv");
	__glColor4i = (PFNGLCOLOR4I)::GetProcAddress(module, "glColor4i");
	__glColor4iv = (PFNGLCOLOR4IV)::GetProcAddress(module, "glColor4iv");
	__glColor4s = (PFNGLCOLOR4S)::GetProcAddress(module, "glColor4s");
	__glColor4sv = (PFNGLCOLOR4SV)::GetProcAddress(module, "glColor4sv");
	__glColor4ub = (PFNGLCOLOR4UB)::GetProcAddress(module, "glColor4ub");
	__glColor4ubv = (PFNGLCOLOR4UBV)::GetProcAddress(module, "glColor4ubv");
	__glColor4ui = (PFNGLCOLOR4UI)::GetProcAddress(module, "glColor4ui");
	__glColor4uiv = (PFNGLCOLOR4UIV)::GetProcAddress(module, "glColor4uiv");
	__glColor4us = (PFNGLCOLOR4US)::GetProcAddress(module, "glColor4us");
	__glColor4usv = (PFNGLCOLOR4USV)::GetProcAddress(module, "glColor4usv");
	__glColorMask = (PFNGLCOLORMASK)::GetProcAddress(module, "glColorMask");
	__glColorMaterial = (PFNGLCOLORMATERIAL)::GetProcAddress(module, "glColorMaterial");
	__glColorPointer = (PFNGLCOLORPOINTER)::GetProcAddress(module, "glColorPointer");
	__glCopyPixels = (PFNGLCOPYPIXELS)::GetProcAddress(module, "glCopyPixels");
	__glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1D)::GetProcAddress(module, "glCopyTexImage1D");
	__glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2D)::GetProcAddress(module, "glCopyTexImage2D");
	__glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1D)::GetProcAddress(module, "glCopyTexSubImage1D");
	__glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2D)::GetProcAddress(module, "glCopyTexSubImage2D");
	__glCullFace = (PFNGLCULLFACE)::GetProcAddress(module, "glCullFace");
	__glDeleteLists = (PFNGLDELETELISTS)::GetProcAddress(module, "glDeleteLists");
	__glDeleteTextures = (PFNGLDELETETEXTURES)::GetProcAddress(module, "glDeleteTextures");
	__glDepthFunc = (PFNGLDEPTHFUNC)::GetProcAddress(module, "glDepthFunc");
	__glDepthMask = (PFNGLDEPTHMASK)::GetProcAddress(module, "glDepthMask");
	__glDepthRange = (PFNGLDEPTHRANGE)::GetProcAddress(module, "glDepthRange");
	__glDisable = (PFNGLDISABLE)::GetProcAddress(module, "glDisable");
	__glDisableClientState = (PFNGLDISABLECLIENTSTATE)::GetProcAddress(module, "glDisableClientState");
	__glDrawArrays = (PFNGLDRAWARRAYS)::GetProcAddress(module, "glDrawArrays");
	__glDrawBuffer = (PFNGLDRAWBUFFER)::GetProcAddress(module, "glDrawBuffer");
	__glDrawElements = (PFNGLDRAWELEMENTS)::GetProcAddress(module, "glDrawElements");
	__glDrawPixels = (PFNGLDRAWPIXELS)::GetProcAddress(module, "glDrawPixels");
	__glEdgeFlag = (PFNGLEDGEFLAG)::GetProcAddress(module, "glEdgeFlag");
	__glEdgeFlagPointer = (PFNGLEDGEFLAGPOINTER)::GetProcAddress(module, "glEdgeFlagPointer");
	__glEdgeFlagv = (PFNGLEDGEFLAGV)::GetProcAddress(module, "glEdgeFlagv");
	__glEnable = (PFNGLENABLE)::GetProcAddress(module, "glEnable");
	__glEnableClientState = (PFNGLENABLECLIENTSTATE)::GetProcAddress(module, "glEnableClientState");
	__glEnd = (PFNGLEND)::GetProcAddress(module, "glEnd");
	__glEndList = (PFNGLENDLIST)::GetProcAddress(module, "glEndList");
	__glEvalCoord1d = (PFNGLEVALCOORD1D)::GetProcAddress(module, "glEvalCoord1d");
	__glEvalCoord1dv = (PFNGLEVALCOORD1DV)::GetProcAddress(module, "glEvalCoord1dv");
	__glEvalCoord1f = (PFNGLEVALCOORD1F)::GetProcAddress(module, "glEvalCoord1f");
	__glEvalCoord1fv = (PFNGLEVALCOORD1FV)::GetProcAddress(module, "glEvalCoord1fv");
	__glEvalCoord2d = (PFNGLEVALCOORD2D)::GetProcAddress(module, "glEvalCoord2d");
	__glEvalCoord2dv = (PFNGLEVALCOORD2DV)::GetProcAddress(module, "glEvalCoord2dv");
	__glEvalCoord2f = (PFNGLEVALCOORD2F)::GetProcAddress(module, "glEvalCoord2f");
	__glEvalCoord2fv = (PFNGLEVALCOORD2FV)::GetProcAddress(module, "glEvalCoord2fv");
	__glEvalMesh1 = (PFNGLEVALMESH1)::GetProcAddress(module, "glEvalMesh1");
	__glEvalMesh2 = (PFNGLEVALMESH2)::GetProcAddress(module, "glEvalMesh2");
	__glEvalPoint1 = (PFNGLEVALPOINT1)::GetProcAddress(module, "glEvalPoint1");
	__glEvalPoint2 = (PFNGLEVALPOINT2)::GetProcAddress(module, "glEvalPoint2");
	__glFeedbackBuffer = (PFNGLFEEDBACKBUFFER)::GetProcAddress(module, "glFeedbackBuffer");
	__glFinish = (PFNGLFINISH)::GetProcAddress(module, "glFinish");
	__glFlush = (PFNGLFLUSH)::GetProcAddress(module, "glFlush");
	__glFogf = (PFNGLFOGF)::GetProcAddress(module, "glFogf");
	__glFogfv = (PFNGLFOGFV)::GetProcAddress(module, "glFogfv");
	__glFogi = (PFNGLFOGI)::GetProcAddress(module, "glFogi");
	__glFogiv = (PFNGLFOGIV)::GetProcAddress(module, "glFogiv");
	__glFrontFace = (PFNGLFRONTFACE)::GetProcAddress(module, "glFrontFace");
	__glFrustum = (PFNGLFRUSTUM)::GetProcAddress(module, "glFrustum");
	__glGenLists = (PFNGLGENLISTS)::GetProcAddress(module, "glGenLists");
	__glGenTextures = (PFNGLGENTEXTURES)::GetProcAddress(module, "glGenTextures");
	__glGetBooleanv = (PFNGLGETBOOLEANV)::GetProcAddress(module, "glGetBooleanv");
	__glGetClipPlane = (PFNGLGETCLIPPLANE)::GetProcAddress(module, "glGetClipPlane");
	__glGetDoublev = (PFNGLGETDOUBLEV)::GetProcAddress(module, "glGetDoublev");
	__glGetError = (PFNGLGETERROR)::GetProcAddress(module, "glGetError");
	__glGetFloatv = (PFNGLGETFLOATV)::GetProcAddress(module, "glGetFloatv");
	__glGetIntegerv = (PFNGLGETINTEGERV)::GetProcAddress(module, "glGetIntegerv");
	__glGetLightfv = (PFNGLGETLIGHTFV)::GetProcAddress(module, "glGetLightfv");
	__glGetLightiv = (PFNGLGETLIGHTIV)::GetProcAddress(module, "glGetLightiv");
	__glGetMapdv = (PFNGLGETMAPDV)::GetProcAddress(module, "glGetMapdv");
	__glGetMapfv = (PFNGLGETMAPFV)::GetProcAddress(module, "glGetMapfv");
	__glGetMapiv = (PFNGLGETMAPIV)::GetProcAddress(module, "glGetMapiv");
	__glGetMaterialfv = (PFNGLGETMATERIALFV)::GetProcAddress(module, "glGetMaterialfv");
	__glGetMaterialiv = (PFNGLGETMATERIALIV)::GetProcAddress(module, "glGetMaterialiv");
	__glGetPixelMapfv = (PFNGLGETPIXELMAPFV)::GetProcAddress(module, "glGetPixelMapfv");
	__glGetPixelMapuiv = (PFNGLGETPIXELMAPUIV)::GetProcAddress(module, "glGetPixelMapuiv");
	__glGetPixelMapusv = (PFNGLGETPIXELMAPUSV)::GetProcAddress(module, "glGetPixelMapusv");
	__glGetPointerv = (PFNGLGETPOINTERV)::GetProcAddress(module, "glGetPointerv");
	__glGetPolygonStipple = (PFNGLGETPOLYGONSTIPPLE)::GetProcAddress(module, "glGetPolygonStipple");
	__glGetString = (PFNGLGETSTRING)::GetProcAddress(module, "glGetString");
	__glGetTexEnvfv = (PFNGLGETTEXENVFV)::GetProcAddress(module, "glGetTexEnvfv");
	__glGetTexEnviv = (PFNGLGETTEXENVIV)::GetProcAddress(module, "glGetTexEnviv");
	__glGetTexGendv = (PFNGLGETTEXGENDV)::GetProcAddress(module, "glGetTexGendv");
	__glGetTexGenfv = (PFNGLGETTEXGENFV)::GetProcAddress(module, "glGetTexGenfv");
	__glGetTexGeniv = (PFNGLGETTEXGENIV)::GetProcAddress(module, "glGetTexGeniv");
	__glGetTexImage = (PFNGLGETTEXIMAGE)::GetProcAddress(module, "glGetTexImage");
	__glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFV)::GetProcAddress(module, "glGetTexLevelParameterfv");
	__glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIV)::GetProcAddress(module, "glGetTexLevelParameteriv");
	__glGetTexParameterfv = (PFNGLGETTEXPARAMETERFV)::GetProcAddress(module, "glGetTexParameterfv");
	__glGetTexParameteriv = (PFNGLGETTEXPARAMETERIV)::GetProcAddress(module, "glGetTexParameteriv");
	__glHint = (PFNGLHINT)::GetProcAddress(module, "glHint");
	__glIndexMask = (PFNGLINDEXMASK)::GetProcAddress(module, "glIndexMask");
	__glIndexPointer = (PFNGLINDEXPOINTER)::GetProcAddress(module, "glIndexPointer");
	__glIndexd = (PFNGLINDEXD)::GetProcAddress(module, "glIndexd");
	__glIndexdv = (PFNGLINDEXDV)::GetProcAddress(module, "glIndexdv");
	__glIndexf = (PFNGLINDEXF)::GetProcAddress(module, "glIndexf");
	__glIndexfv = (PFNGLINDEXFV)::GetProcAddress(module, "glIndexfv");
	__glIndexi = (PFNGLINDEXI)::GetProcAddress(module, "glIndexi");
	__glIndexiv = (PFNGLINDEXIV)::GetProcAddress(module, "glIndexiv");
	__glIndexs = (PFNGLINDEXS)::GetProcAddress(module, "glIndexs");
	__glIndexsv = (PFNGLINDEXSV)::GetProcAddress(module, "glIndexsv");
	__glIndexub = (PFNGLINDEXUB)::GetProcAddress(module, "glIndexub");
	__glIndexubv = (PFNGLINDEXUBV)::GetProcAddress(module, "glIndexubv");
	__glInitNames = (PFNGLINITNAMES)::GetProcAddress(module, "glInitNames");
	__glInterleavedArrays = (PFNGLINTERLEAVEDARRAYS)::GetProcAddress(module, "glInterleavedArrays");
	__glIsEnabled = (PFNGLISENABLED)::GetProcAddress(module, "glIsEnabled");
	__glIsList = (PFNGLISLIST)::GetProcAddress(module, "glIsList");
	__glIsTexture = (PFNGLISTEXTURE)::GetProcAddress(module, "glIsTexture");
	__glLightModelf = (PFNGLLIGHTMODELF)::GetProcAddress(module, "glLightModelf");
	__glLightModelfv = (PFNGLLIGHTMODELFV)::GetProcAddress(module, "glLightModelfv");
	__glLightModeli = (PFNGLLIGHTMODELI)::GetProcAddress(module, "glLightModeli");
	__glLightModeliv = (PFNGLLIGHTMODELIV)::GetProcAddress(module, "glLightModeliv");
	__glLightf = (PFNGLLIGHTF)::GetProcAddress(module, "glLightf");
	__glLightfv = (PFNGLLIGHTFV)::GetProcAddress(module, "glLightfv");
	__glLighti = (PFNGLLIGHTI)::GetProcAddress(module, "glLighti");
	__glLightiv = (PFNGLLIGHTIV)::GetProcAddress(module, "glLightiv");
	__glLineStipple = (PFNGLLINESTIPPLE)::GetProcAddress(module, "glLineStipple");
	__glLineWidth = (PFNGLLINEWIDTH)::GetProcAddress(module, "glLineWidth");
	__glListBase = (PFNGLLISTBASE)::GetProcAddress(module, "glListBase");
	__glLoadIdentity = (PFNGLLOADIDENTITY)::GetProcAddress(module, "glLoadIdentity");
	__glLoadMatrixd = (PFNGLLOADMATRIXD)::GetProcAddress(module, "glLoadMatrixd");
	__glLoadMatrixf = (PFNGLLOADMATRIXF)::GetProcAddress(module, "glLoadMatrixf");
	__glLoadName = (PFNGLLOADNAME)::GetProcAddress(module, "glLoadName");
	__glLogicOp = (PFNGLLOGICOP)::GetProcAddress(module, "glLogicOp");
	__glMap1d = (PFNGLMAP1D)::GetProcAddress(module, "glMap1d");
	__glMap1f = (PFNGLMAP1F)::GetProcAddress(module, "glMap1f");
	__glMap2d = (PFNGLMAP2D)::GetProcAddress(module, "glMap2d");
	__glMap2f = (PFNGLMAP2F)::GetProcAddress(module, "glMap2f");
	__glMapGrid1d = (PFNGLMAPGRID1D)::GetProcAddress(module, "glMapGrid1d");
	__glMapGrid1f = (PFNGLMAPGRID1F)::GetProcAddress(module, "glMapGrid1f");
	__glMapGrid2d = (PFNGLMAPGRID2D)::GetProcAddress(module, "glMapGrid2d");
	__glMapGrid2f = (PFNGLMAPGRID2F)::GetProcAddress(module, "glMapGrid2f");
	__glMaterialf = (PFNGLMATERIALF)::GetProcAddress(module, "glMaterialf");
	__glMaterialfv = (PFNGLMATERIALFV)::GetProcAddress(module, "glMaterialfv");
	__glMateriali = (PFNGLMATERIALI)::GetProcAddress(module, "glMateriali");
	__glMaterialiv = (PFNGLMATERIALIV)::GetProcAddress(module, "glMaterialiv");
	__glMatrixMode = (PFNGLMATRIXMODE)::GetProcAddress(module, "glMatrixMode");
	__glMultMatrixd = (PFNGLMULTMATRIXD)::GetProcAddress(module, "glMultMatrixd");
	__glMultMatrixf = (PFNGLMULTMATRIXF)::GetProcAddress(module, "glMultMatrixf");
	__glNewList = (PFNGLNEWLIST)::GetProcAddress(module, "glNewList");
	__glNormal3b = (PFNGLNORMAL3B)::GetProcAddress(module, "glNormal3b");
	__glNormal3bv = (PFNGLNORMAL3BV)::GetProcAddress(module, "glNormal3bv");
	__glNormal3d = (PFNGLNORMAL3D)::GetProcAddress(module, "glNormal3d");
	__glNormal3dv = (PFNGLNORMAL3DV)::GetProcAddress(module, "glNormal3dv");
	__glNormal3f = (PFNGLNORMAL3F)::GetProcAddress(module, "glNormal3f");
	__glNormal3fv = (PFNGLNORMAL3FV)::GetProcAddress(module, "glNormal3fv");
	__glNormal3i = (PFNGLNORMAL3I)::GetProcAddress(module, "glNormal3i");
	__glNormal3iv = (PFNGLNORMAL3IV)::GetProcAddress(module, "glNormal3iv");
	__glNormal3s = (PFNGLNORMAL3S)::GetProcAddress(module, "glNormal3s");
	__glNormal3sv = (PFNGLNORMAL3SV)::GetProcAddress(module, "glNormal3sv");
	__glNormalPointer = (PFNGLNORMALPOINTER)::GetProcAddress(module, "glNormalPointer");
	__glOrtho = (PFNGLORTHO)::GetProcAddress(module, "glOrtho");
	__glPassThrough = (PFNGLPASSTHROUGH)::GetProcAddress(module, "glPassThrough");
	__glPixelMapfv = (PFNGLPIXELMAPFV)::GetProcAddress(module, "glPixelMapfv");
	__glPixelMapuiv = (PFNGLPIXELMAPUIV)::GetProcAddress(module, "glPixelMapuiv");
	__glPixelMapusv = (PFNGLPIXELMAPUSV)::GetProcAddress(module, "glPixelMapusv");
	__glPixelStoref = (PFNGLPIXELSTOREF)::GetProcAddress(module, "glPixelStoref");
	__glPixelStorei = (PFNGLPIXELSTOREI)::GetProcAddress(module, "glPixelStorei");
	__glPixelTransferf = (PFNGLPIXELTRANSFERF)::GetProcAddress(module, "glPixelTransferf");
	__glPixelTransferi = (PFNGLPIXELTRANSFERI)::GetProcAddress(module, "glPixelTransferi");
	__glPixelZoom = (PFNGLPIXELZOOM)::GetProcAddress(module, "glPixelZoom");
	__glPointSize = (PFNGLPOINTSIZE)::GetProcAddress(module, "glPointSize");
	__glPolygonMode = (PFNGLPOLYGONMODE)::GetProcAddress(module, "glPolygonMode");
	__glPolygonOffset = (PFNGLPOLYGONOFFSET)::GetProcAddress(module, "glPolygonOffset");
	__glPolygonStipple = (PFNGLPOLYGONSTIPPLE)::GetProcAddress(module, "glPolygonStipple");
	__glPopAttrib = (PFNGLPOPATTRIB)::GetProcAddress(module, "glPopAttrib");
	__glPopClientAttrib = (PFNGLPOPCLIENTATTRIB)::GetProcAddress(module, "glPopClientAttrib");
	__glPopMatrix = (PFNGLPOPMATRIX)::GetProcAddress(module, "glPopMatrix");
	__glPopName = (PFNGLPOPNAME)::GetProcAddress(module, "glPopName");
	__glPrioritizeTextures = (PFNGLPRIORITIZETEXTURES)::GetProcAddress(module, "glPrioritizeTextures");
	__glPushAttrib = (PFNGLPUSHATTRIB)::GetProcAddress(module, "glPushAttrib");
	__glPushClientAttrib = (PFNGLPUSHCLIENTATTRIB)::GetProcAddress(module, "glPushClientAttrib");
	__glPushMatrix = (PFNGLPUSHMATRIX)::GetProcAddress(module, "glPushMatrix");
	__glPushName = (PFNGLPUSHNAME)::GetProcAddress(module, "glPushName");
	__glRasterPos2d = (PFNGLRASTERPOS2D)::GetProcAddress(module, "glRasterPos2d");
	__glRasterPos2dv = (PFNGLRASTERPOS2DV)::GetProcAddress(module, "glRasterPos2dv");
	__glRasterPos2f = (PFNGLRASTERPOS2F)::GetProcAddress(module, "glRasterPos2f");
	__glRasterPos2fv = (PFNGLRASTERPOS2FV)::GetProcAddress(module, "glRasterPos2fv");
	__glRasterPos2i = (PFNGLRASTERPOS2I)::GetProcAddress(module, "glRasterPos2i");
	__glRasterPos2iv = (PFNGLRASTERPOS2IV)::GetProcAddress(module, "glRasterPos2iv");
	__glRasterPos2s = (PFNGLRASTERPOS2S)::GetProcAddress(module, "glRasterPos2s");
	__glRasterPos2sv = (PFNGLRASTERPOS2SV)::GetProcAddress(module, "glRasterPos2sv");
	__glRasterPos3d = (PFNGLRASTERPOS3D)::GetProcAddress(module, "glRasterPos3d");
	__glRasterPos3dv = (PFNGLRASTERPOS3DV)::GetProcAddress(module, "glRasterPos3dv");
	__glRasterPos3f = (PFNGLRASTERPOS3F)::GetProcAddress(module, "glRasterPos3f");
	__glRasterPos3fv = (PFNGLRASTERPOS3FV)::GetProcAddress(module, "glRasterPos3fv");
	__glRasterPos3i = (PFNGLRASTERPOS3I)::GetProcAddress(module, "glRasterPos3i");
	__glRasterPos3iv = (PFNGLRASTERPOS3IV)::GetProcAddress(module, "glRasterPos3iv");
	__glRasterPos3s = (PFNGLRASTERPOS3S)::GetProcAddress(module, "glRasterPos3s");
	__glRasterPos3sv = (PFNGLRASTERPOS3SV)::GetProcAddress(module, "glRasterPos3sv");
	__glRasterPos4d = (PFNGLRASTERPOS4D)::GetProcAddress(module, "glRasterPos4d");
	__glRasterPos4dv = (PFNGLRASTERPOS4DV)::GetProcAddress(module, "glRasterPos4dv");
	__glRasterPos4f = (PFNGLRASTERPOS4F)::GetProcAddress(module, "glRasterPos4f");
	__glRasterPos4fv = (PFNGLRASTERPOS4FV)::GetProcAddress(module, "glRasterPos4fv");
	__glRasterPos4i = (PFNGLRASTERPOS4I)::GetProcAddress(module, "glRasterPos4i");
	__glRasterPos4iv = (PFNGLRASTERPOS4IV)::GetProcAddress(module, "glRasterPos4iv");
	__glRasterPos4s = (PFNGLRASTERPOS4S)::GetProcAddress(module, "glRasterPos4s");
	__glRasterPos4sv = (PFNGLRASTERPOS4SV)::GetProcAddress(module, "glRasterPos4sv");
	__glReadBuffer = (PFNGLREADBUFFER)::GetProcAddress(module, "glReadBuffer");
	__glReadPixels = (PFNGLREADPIXELS)::GetProcAddress(module, "glReadPixels");
	__glRectd = (PFNGLRECTD)::GetProcAddress(module, "glRectd");
	__glRectdv = (PFNGLRECTDV)::GetProcAddress(module, "glRectdv");
	__glRectf = (PFNGLRECTF)::GetProcAddress(module, "glRectf");
	__glRectfv = (PFNGLRECTFV)::GetProcAddress(module, "glRectfv");
	__glRecti = (PFNGLRECTI)::GetProcAddress(module, "glRecti");
	__glRectiv = (PFNGLRECTIV)::GetProcAddress(module, "glRectiv");
	__glRects = (PFNGLRECTS)::GetProcAddress(module, "glRects");
	__glRectsv = (PFNGLRECTSV)::GetProcAddress(module, "glRectsv");
	__glRenderMode = (PFNGLRENDERMODE)::GetProcAddress(module, "glRenderMode");
	__glRotated = (PFNGLROTATED)::GetProcAddress(module, "glRotated");
	__glRotatef = (PFNGLROTATEF)::GetProcAddress(module, "glRotatef");
	__glScaled = (PFNGLSCALED)::GetProcAddress(module, "glScaled");
	__glScalef = (PFNGLSCALEF)::GetProcAddress(module, "glScalef");
	__glScissor = (PFNGLSCISSOR)::GetProcAddress(module, "glScissor");
	__glSelectBuffer = (PFNGLSELECTBUFFER)::GetProcAddress(module, "glSelectBuffer");
	__glShadeModel = (PFNGLSHADEMODEL)::GetProcAddress(module, "glShadeModel");
	__glStencilFunc = (PFNGLSTENCILFUNC)::GetProcAddress(module, "glStencilFunc");
	__glStencilMask = (PFNGLSTENCILMASK)::GetProcAddress(module, "glStencilMask");
	__glStencilOp = (PFNGLSTENCILOP)::GetProcAddress(module, "glStencilOp");
	__glTexCoord1d = (PFNGLTEXCOORD1D)::GetProcAddress(module, "glTexCoord1d");
	__glTexCoord1dv = (PFNGLTEXCOORD1DV)::GetProcAddress(module, "glTexCoord1dv");
	__glTexCoord1f = (PFNGLTEXCOORD1F)::GetProcAddress(module, "glTexCoord1f");
	__glTexCoord1fv = (PFNGLTEXCOORD1FV)::GetProcAddress(module, "glTexCoord1fv");
	__glTexCoord1i = (PFNGLTEXCOORD1I)::GetProcAddress(module, "glTexCoord1i");
	__glTexCoord1iv = (PFNGLTEXCOORD1IV)::GetProcAddress(module, "glTexCoord1iv");
	__glTexCoord1s = (PFNGLTEXCOORD1S)::GetProcAddress(module, "glTexCoord1s");
	__glTexCoord1sv = (PFNGLTEXCOORD1SV)::GetProcAddress(module, "glTexCoord1sv");
	__glTexCoord2d = (PFNGLTEXCOORD2D)::GetProcAddress(module, "glTexCoord2d");
	__glTexCoord2dv = (PFNGLTEXCOORD2DV)::GetProcAddress(module, "glTexCoord2dv");
	__glTexCoord2f = (PFNGLTEXCOORD2F)::GetProcAddress(module, "glTexCoord2f");
	__glTexCoord2fv = (PFNGLTEXCOORD2FV)::GetProcAddress(module, "glTexCoord2fv");
	__glTexCoord2i = (PFNGLTEXCOORD2I)::GetProcAddress(module, "glTexCoord2i");
	__glTexCoord2iv = (PFNGLTEXCOORD2IV)::GetProcAddress(module, "glTexCoord2iv");
	__glTexCoord2s = (PFNGLTEXCOORD2S)::GetProcAddress(module, "glTexCoord2s");
	__glTexCoord2sv = (PFNGLTEXCOORD2SV)::GetProcAddress(module, "glTexCoord2sv");
	__glTexCoord3d = (PFNGLTEXCOORD3D)::GetProcAddress(module, "glTexCoord3d");
	__glTexCoord3dv = (PFNGLTEXCOORD3DV)::GetProcAddress(module, "glTexCoord3dv");
	__glTexCoord3f = (PFNGLTEXCOORD3F)::GetProcAddress(module, "glTexCoord3f");
	__glTexCoord3fv = (PFNGLTEXCOORD3FV)::GetProcAddress(module, "glTexCoord3fv");
	__glTexCoord3i = (PFNGLTEXCOORD3I)::GetProcAddress(module, "glTexCoord3i");
	__glTexCoord3iv = (PFNGLTEXCOORD3IV)::GetProcAddress(module, "glTexCoord3iv");
	__glTexCoord3s = (PFNGLTEXCOORD3S)::GetProcAddress(module, "glTexCoord3s");
	__glTexCoord3sv = (PFNGLTEXCOORD3SV)::GetProcAddress(module, "glTexCoord3sv");
	__glTexCoord4d = (PFNGLTEXCOORD4D)::GetProcAddress(module, "glTexCoord4d");
	__glTexCoord4dv = (PFNGLTEXCOORD4DV)::GetProcAddress(module, "glTexCoord4dv");
	__glTexCoord4f = (PFNGLTEXCOORD4F)::GetProcAddress(module, "glTexCoord4f");
	__glTexCoord4fv = (PFNGLTEXCOORD4FV)::GetProcAddress(module, "glTexCoord4fv");
	__glTexCoord4i = (PFNGLTEXCOORD4I)::GetProcAddress(module, "glTexCoord4i");
	__glTexCoord4iv = (PFNGLTEXCOORD4IV)::GetProcAddress(module, "glTexCoord4iv");
	__glTexCoord4s = (PFNGLTEXCOORD4S)::GetProcAddress(module, "glTexCoord4s");
	__glTexCoord4sv = (PFNGLTEXCOORD4SV)::GetProcAddress(module, "glTexCoord4sv");
	__glTexCoordPointer = (PFNGLTEXCOORDPOINTER)::GetProcAddress(module, "glTexCoordPointer");
	__glTexEnvf = (PFNGLTEXENVF)::GetProcAddress(module, "glTexEnvf");
	__glTexEnvfv = (PFNGLTEXENVFV)::GetProcAddress(module, "glTexEnvfv");
	__glTexEnvi = (PFNGLTEXENVI)::GetProcAddress(module, "glTexEnvi");
	__glTexEnviv = (PFNGLTEXENVIV)::GetProcAddress(module, "glTexEnviv");
	__glTexGend = (PFNGLTEXGEND)::GetProcAddress(module, "glTexGend");
	__glTexGendv = (PFNGLTEXGENDV)::GetProcAddress(module, "glTexGendv");
	__glTexGenf = (PFNGLTEXGENF)::GetProcAddress(module, "glTexGenf");
	__glTexGenfv = (PFNGLTEXGENFV)::GetProcAddress(module, "glTexGenfv");
	__glTexGeni = (PFNGLTEXGENI)::GetProcAddress(module, "glTexGeni");
	__glTexGeniv = (PFNGLTEXGENIV)::GetProcAddress(module, "glTexGeniv");
	__glTexImage1D = (PFNGLTEXIMAGE1D)::GetProcAddress(module, "glTexImage1D");
	__glTexImage2D = (PFNGLTEXIMAGE2D)::GetProcAddress(module, "glTexImage2D");
	__glTexParameterf = (PFNGLTEXPARAMETERF)::GetProcAddress(module, "glTexParameterf");
	__glTexParameterfv = (PFNGLTEXPARAMETERFV)::GetProcAddress(module, "glTexParameterfv");
	__glTexParameteri = (PFNGLTEXPARAMETERI)::GetProcAddress(module, "glTexParameteri");
	__glTexParameteriv = (PFNGLTEXPARAMETERIV)::GetProcAddress(module, "glTexParameteriv");
	__glTexSubImage1D = (PFNGLTEXSUBIMAGE1D)::GetProcAddress(module, "glTexSubImage1D");
	__glTexSubImage2D = (PFNGLTEXSUBIMAGE2D)::GetProcAddress(module, "glTexSubImage2D");
	__glTranslated = (PFNGLTRANSLATED)::GetProcAddress(module, "glTranslated");
	__glTranslatef = (PFNGLTRANSLATEF)::GetProcAddress(module, "glTranslatef");
	__glVertex2d = (PFNGLVERTEX2D)::GetProcAddress(module, "glVertex2d");
	__glVertex2dv = (PFNGLVERTEX2DV)::GetProcAddress(module, "glVertex2dv");
	__glVertex2f = (PFNGLVERTEX2F)::GetProcAddress(module, "glVertex2f");
	__glVertex2fv = (PFNGLVERTEX2FV)::GetProcAddress(module, "glVertex2fv");
	__glVertex2i = (PFNGLVERTEX2I)::GetProcAddress(module, "glVertex2i");
	__glVertex2iv = (PFNGLVERTEX2IV)::GetProcAddress(module, "glVertex2iv");
	__glVertex2s = (PFNGLVERTEX2S)::GetProcAddress(module, "glVertex2s");
	__glVertex2sv = (PFNGLVERTEX2SV)::GetProcAddress(module, "glVertex2sv");
	__glVertex3d = (PFNGLVERTEX3D)::GetProcAddress(module, "glVertex3d");
	__glVertex3dv = (PFNGLVERTEX3DV)::GetProcAddress(module, "glVertex3dv");
	__glVertex3f = (PFNGLVERTEX3F)::GetProcAddress(module, "glVertex3f");
	__glVertex3fv = (PFNGLVERTEX3FV)::GetProcAddress(module, "glVertex3fv");
	__glVertex3i = (PFNGLVERTEX3I)::GetProcAddress(module, "glVertex3i");
	__glVertex3iv = (PFNGLVERTEX3IV)::GetProcAddress(module, "glVertex3iv");
	__glVertex3s = (PFNGLVERTEX3S)::GetProcAddress(module, "glVertex3s");
	__glVertex3sv = (PFNGLVERTEX3SV)::GetProcAddress(module, "glVertex3sv");
	__glVertex4d = (PFNGLVERTEX4D)::GetProcAddress(module, "glVertex4d");
	__glVertex4dv = (PFNGLVERTEX4DV)::GetProcAddress(module, "glVertex4dv");
	__glVertex4f = (PFNGLVERTEX4F)::GetProcAddress(module, "glVertex4f");
	__glVertex4fv = (PFNGLVERTEX4FV)::GetProcAddress(module, "glVertex4fv");
	__glVertex4i = (PFNGLVERTEX4I)::GetProcAddress(module, "glVertex4i");
	__glVertex4iv = (PFNGLVERTEX4IV)::GetProcAddress(module, "glVertex4iv");
	__glVertex4s = (PFNGLVERTEX4S)::GetProcAddress(module, "glVertex4s");
	__glVertex4sv = (PFNGLVERTEX4SV)::GetProcAddress(module, "glVertex4sv");
	__glVertexPointer = (PFNGLVERTEXPOINTER)::GetProcAddress(module, "glVertexPointer");
	__glViewport = (PFNGLVIEWPORT)::GetProcAddress(module, "glViewport");
#endif

#if defined(_BUILD_PLATFORM_WINDOWS)
	__wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)::wglGetProcAddress("wglSwapIntervalEXT");
	__wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)::wglGetProcAddress("wglGetPixelFormatAttribivARB");
	__wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)::wglGetProcAddress("wglCreateContextAttribsARB");

	initGLExtention = __wglCreateContextAttribsARB ? true : false;
#else
	initGLExtention = true;
#endif

	return initGLExtention;
}

_NAME_END