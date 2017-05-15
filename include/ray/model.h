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
#ifndef _H_MODEL_H_
#define _H_MODEL_H_

#include <ray/modhelp.h>

_NAME_BEGIN

class Model;
class EXPORT ModelHandler
{
public:
	virtual ~ModelHandler() noexcept {};

	virtual bool doCanRead(StreamReader& stream) const noexcept = 0;
	virtual bool doCanRead(const char* type) const noexcept = 0;

	virtual bool doLoad(StreamReader& stream, Model& model) noexcept = 0;
	virtual bool doSave(StreamWrite& stream, const Model& model) noexcept = 0;
};

class EXPORT Model final
{
public:
	typedef std::shared_ptr<ModelHandler> _Myhandler;

	typedef std::vector<MeshPropertyPtr>      MeshList;
	typedef std::vector<BonePtr>              BoneList;
	typedef std::vector<IKAttrPtr>            IKList;
	typedef std::vector<RigidbodyPropertyPtr> RigidbodyList;
	typedef std::vector<JointPropertyPtr>     JointList;
	typedef std::vector<MaterialPropertyPtr>  MaterialList;
	typedef std::vector<TexturePropertyPtr>   TextureList;
	typedef std::vector<AnimationPropertyPtr> AnimList;
	typedef std::vector<LightPropertyPtr>     LightList;
	typedef std::vector<CameraPropertyPtr>    CameraList;

public:
	Model() noexcept;
	~Model() noexcept;

	void addMesh(MeshPropertyPtr& mesh)           noexcept;
	void addBone(BonePtr& bone)					  noexcept;
	void addIK(IKAttrPtr& ik)                     noexcept;
	void addRigidbody(RigidbodyPropertyPtr& body) noexcept;
	void addJoint(JointPropertyPtr& joint)        noexcept;
	void addTexture(TexturePropertyPtr& texture)  noexcept;
	void addMaterial(MaterialPropertyPtr& mat)    noexcept;
	void addAnimtion(AnimationPropertyPtr& anim)  noexcept;
	void addLight(LightPropertyPtr& light)        noexcept;
	void addCamera(CameraPropertyPtr& camera)     noexcept;

	void addMesh(MeshPropertyPtr&& mesh)           noexcept;
	void addBone(BonePtr&& bone)				   noexcept;
	void addIK(IKAttrPtr&& ik)                     noexcept;
	void addRigidbody(RigidbodyPropertyPtr&& body) noexcept;
	void addJoint(JointPropertyPtr&& joint)        noexcept;
	void addTexture(TexturePropertyPtr&& texture)  noexcept;
	void addMaterial(MaterialPropertyPtr&& mat)    noexcept;
	void addAnimtion(AnimationPropertyPtr&& anim)  noexcept;
	void addLight(LightPropertyPtr&& light)        noexcept;
	void addCamera(CameraPropertyPtr&& camera)     noexcept;

	MeshList&      getMeshsList()     noexcept;
	BoneList&      getBonesList()     noexcept;
	IKList&        getIKList()        noexcept;
	RigidbodyList& getRigidbodyList() noexcept;
	JointList&     getJointList()     noexcept;
	TextureList&   getTexturesList()  noexcept;
	MaterialList&  getMaterialsList() noexcept;
	AnimList&      getAnimationList() noexcept;
	LightList&     getLightList()     noexcept;
	CameraList&    getCameraList()    noexcept;

	void setDirectory(const std::string& name) noexcept;
	const std::string&  getDirectory() const noexcept;

	const std::string&   getName()          const noexcept;
	const MeshList&      getMeshsList()     const noexcept;
	const BoneList&      getBonesList()     const noexcept;
	const IKList&        getIKList()        const noexcept;
	const RigidbodyList& getRigidbodyList() const noexcept;
	const JointList&     getJointList()     const noexcept;
	const TextureList&   getTexturesList()  const noexcept;
	const MaterialList&  getMaterialsList() const noexcept;
	const AnimList&      getAnimationList() const noexcept;
	const LightList&     getLightList()     const noexcept;
	const CameraList&    getCameraList()    const noexcept;

	bool hasMeshes()     const noexcept;
	bool hasBones()      const noexcept;
	bool hasIKs()        const noexcept;
	bool hasMaterials()  const noexcept;
	bool hasLights()     const noexcept;
	bool hasTextures()   const noexcept;
	bool hasCameras()    const noexcept;
	bool hasAnimations() const noexcept;

	void applyProcess(int flags) noexcept;

	void clear() noexcept;

	bool load(StreamReader& stream, const char* type) noexcept;
	bool save(StreamWrite& stream, const char* type) noexcept;

	bool emptyHandler() const noexcept;
	bool addHandler(_Myhandler handler) noexcept;
	bool removeHandler(_Myhandler handler) noexcept;
	bool find(const char* type, _Myhandler& handler) const noexcept;
	bool find(StreamReader& stream, _Myhandler& handler) const noexcept;
	bool find(StreamReader& stream, const char* type, _Myhandler& handler) const noexcept;

private:
	Model& operator=(const Model&) noexcept = delete;
	Model(const Model&) noexcept = delete;

private:

	std::string _name;

	std::string _directory;

	MeshList _meshes;

	BoneList _bones;

	IKList _iks;

	RigidbodyList _rigidbodys;

	JointList _joints;

	TextureList _textures;

	MaterialList _materials;

	AnimList _animations;

	LightList _lights;

	CameraList _cameras;

	std::vector<_Myhandler> _handlers;
};

_NAME_END

#endif