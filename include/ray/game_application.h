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
#ifndef _H_GAME_APPLICATION_H_
#define _H_GAME_APPLICATION_H_

#include <ray/game_types.h>

_NAME_BEGIN

class EXPORT GameApplication
{
public:
	GameApplication() noexcept;
	virtual ~GameApplication() noexcept;

	bool open(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t dpi_w, std::uint32_t dpi_h) noexcept;
	void close() noexcept;

	bool start() noexcept;
	void stop() noexcept;

	void setGameListener(GameListenerPtr listener) noexcept;
	GameListenerPtr getGameListener() const noexcept;

	bool isQuitRequest() const noexcept;

	bool openScene(GameScenePtr& scene) noexcept;
	bool openScene(const util::string& name) noexcept;
	void closeScene(GameScenePtr& name) noexcept;
	void closeScene(const util::string& name) noexcept;
	GameScenePtr findScene(const util::string& name) noexcept;

	bool addFeatures(GameFeaturePtr& feature) noexcept;
	void removeFeatures(GameFeaturePtr& feature) noexcept;

	void setFileService(bool enable) noexcept;
	void setFileServicePath(const util::string& path) noexcept;
	void setFileServiceListener(bool enable) noexcept;
	void setResDownloadURL(const util::string& path) noexcept;

	bool sendMessage(const MessagePtr& message) noexcept;
	bool postMessage(const MessagePtr& message) noexcept;

	bool sendInputEvent(const InputEvent& event) noexcept;
	bool postInputEvent(const InputEvent& event) noexcept;

	void update() noexcept;

private:
	GameApplication(const GameApplication&) noexcept = delete;
	GameApplication& operator=(const GameApplication&) noexcept = delete;

private:

	bool _isInitialize;

	util::string _workDir;
	util::string _engineDir;
	util::string _resourceBaseDir;
	util::string _downloadURL;

	GameServer* _gameServer;
	GameListenerPtr _gameListener;

	IoServer* _ioServer;
	IoInterface* _ioInterface;
	IoListenerPtr _ioListener;

	delegate<void()> _onWindowSizeChange;

	GameFeaturePtr _inputFeature;
	GameFeaturePtr _gameBaseFeature;
	GameFeaturePtr _scriptFeature;
	GameFeaturePtr _physicFeature;
	GameFeaturePtr _soundFeature;
	GameFeaturePtr _renderFeature;
	GameFeaturePtr _guiFeature;
};

_NAME_END

#endif