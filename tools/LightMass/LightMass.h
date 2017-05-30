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
#ifndef _H_LIGHT_MASS_H_
#define _H_LIGHT_MASS_H_

#include <ray/graphics_system.h>
#include <ray/graphics_device.h>
#include <ray/graphics_context.h>
#include <ray/graphics_swapchain.h>

#include "LightMassParams.h"
#include "LightMassListener.h"

_NAME_BEGIN

class LightMass final
{
public:
	LightMass() noexcept;
	LightMass(LightMassListenerPtr listener) noexcept;
	~LightMass() noexcept;

	bool open(const LightMassParams& params) noexcept;
	void close() noexcept;

	bool start() noexcept;
	void stop() noexcept;

	void setLightMapData(LightMapDataPtr data) noexcept;
	LightMapDataPtr getLightMapData() const noexcept;

	void setLightMassListener(LightMassListenerPtr pointer) noexcept;
	LightMassListenerPtr getLightMassListener() const noexcept;

private:
	bool _initialize;
	bool _isStopped;

	ray::GraphicsDevicePtr _graphicsDevice;
	ray::GraphicsContextPtr _graphicsContext;
	ray::GraphicsSwapchainPtr _graphicsSwapchain;

	LightMassBakingPtr _lightMass;
	LightMassListenerPtr _lightMassListener;
	LightMapDataPtr _lightMapData;
};

_NAME_END

#endif