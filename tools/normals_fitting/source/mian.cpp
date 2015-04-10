// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray\vector3.h>

float quantize255(float c)
{
    int r = static_cast<int>(ray::clamp(c * 0.5f + 0.5f, 0.0f, 1.0f) * 255 + 0.5f);
    float v = r / 255.0f * 2 - 1;
    return v;
}

float find_minimum_quantization_error(ray::Vector3 normal)
{
    normal /= std::max(abs(normal.x), std::max(abs(normal.y), abs(normal.z)));
    float min_error = 1e10f;
    float ret = 1;
    for (int step = 1; step < 128; ++step)
    {
        float t = (step + 0.5f) / 127.5f;

        // compute the probe
        ray::Vector3 p = normal * t;

        // quantize the probe
        ray::Vector3 quantized_p = ray::Vector3(quantize255(p.x), quantize255(p.y), quantize255(p.z));

        // error computation for the probe
        ray::Vector3 diff = (quantized_p - p) / t;
        float error = std::max(abs(diff.x), std::max(abs(diff.y), abs(diff.z)));

        // find the minimum
        if (error < min_error)
        {
            min_error = error;
            ret = t;
        }
    }
    return ret;
}

void gen_normals_fitting(std::vector<uint8_t>& fitting_map, int dim)
{
    fitting_map.resize(dim * dim);
    ray::Vector3 normal;
    normal.z = 1;
    for (int x = 0; x < dim; ++x)
    {
        normal.x = (x + 0.5f) / dim;
        for (int y = 0; y < dim; ++y)
        {
            normal.y = (y + 0.5f) / dim * (x + 1.0f) / dim;

            float quantized_length = find_minimum_quantization_error(~normal);
            fitting_map[y * dim + x] = static_cast<uint8_t>(ray::clamp(static_cast<int>(quantized_length * 255 + 0.5f), 0, 255));
        }
    }
}

int main()
{
    int const HALF_DIM = 1024;

    std::vector<std::vector<uint8_t> > fitting_map;
    fitting_map.push_back(std::vector<uint8_t>());
    gen_normals_fitting(fitting_map.back(), HALF_DIM);

    int dim = HALF_DIM / 2;
    int mipmap = 1;
    while (dim > 1)
    {
        fitting_map.push_back(std::vector<uint8_t>());

        std::vector<uint8_t> const & last_level = fitting_map[fitting_map.size() - 2];
        std::vector<uint8_t>& this_level = fitting_map.back();

        this_level.resize(dim * dim);
        for (int y = 0; y < dim; ++y)
        {
            for (int x = 0; x < dim; ++x)
            {
                this_level[y * dim + x] = static_cast<uint8_t>((last_level[(y * 2 + 0) * dim * 2 + (x * 2 + 0)]
                    + last_level[(y * 2 + 0) * dim * 2 + (x * 2 + 1)]
                    + last_level[(y * 2 + 1) * dim * 2 + (x * 2 + 0)]
                    + last_level[(y * 2 + 1) * dim * 2 + (x * 2 + 1)]) / 4.0f + 0.5f);
            }
        }

        dim /= 2;
        ++mipmap;
    }

    dim = HALF_DIM;

    return 0;
}
