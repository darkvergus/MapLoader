/*
 * Copyright (c) 2022, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-FileCopyrightText: Copyright (c) 2014-2021 NVIDIA CORPORATION
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef DH_TONEMAMP_H
#define DH_TONEMAMP_H 1

#ifdef __cplusplus
using uint = uint32_t;
#define INLINE inline
#else
#define INLINE
#endif


const int eTonemapFilmic    = 0;
const int eTonemapUncharted = 1;
const int eTonemapGamma     = 2;


// Tonemapper settings
struct Tonemapper
{
  int   method;
  int   isActive;
  float exposure;
  float brightness;
  float contrast;
  float saturation;
  float vignette;
  float gamma;

#ifdef __cplusplus
  // clang-format off
  Tonemapper(): method(0), isActive(1), exposure(1.0F), brightness(1.0F), contrast(1.0F), saturation(1.0F), vignette(0.0F), gamma(2.2F) {}
  // clang-format on
#endif
};

// Bindings
const int eTonemapperInput  = 0;
const int eTonemapperOutput = 1;

#ifdef __cplusplus
vec3 div(vec3 left, vec3 right)
{
    return left.Scale(1 / right.X, 1 / right.Y, 1 / right.Z);
}
#define nvec3(val) vec3(val, val, val)
#elif
#define nvec3 vec3
vec3 scale(vec3 left, vec3 right)
{
    return left * right;
}
vec3 div(vec3 left, vec3 right)
{
    return left / right;
}
#endif

// http://filmicworlds.com/blog/filmic-tonemapping-operators/
INLINE vec3 tonemapFilmic(vec3 color)
{
  vec3 temp   = max(nvec3(0.0F), color - nvec3(0.004F));
  vec3 result = (scale(temp, scale(nvec3(6.2F), temp) + nvec3(0.5F)));
  result = div(result, (scale(temp, scale(nvec3(6.2F), temp) + nvec3(1.7F)) + nvec3(0.06F)));
  return result;
}

// Gamma Correction in Computer Graphics
// see https://www.teamten.com/lawrence/graphics/gamma/
INLINE vec3 gammaCorrection(vec3 color, float gamma)
{
  return pow(color, vec3(1.0F / gamma));
}

// Uncharted 2 tone map
// see: http://filmicworlds.com/blog/filmic-tonemapping-operators/
INLINE vec3 tonemapUncharted2Impl(vec3 color)
{
  const float a = 0.15F;
  const float b = 0.50F;
  const float c = 0.10F;
  const float d = 0.20F;
  const float e = 0.02F;
  const float f = 0.30F;
  return ((color * (a * color + c * b) + d * e) / (color * (a * color + b) + d * f)) - e / f;
}

INLINE vec3 tonemapUncharted(vec3 color, float gamma)
{
  const float W             = 11.2F;
  const float exposure_bias = 2.0F;
  color                     = tonemapUncharted2Impl(color * exposure_bias);
  vec3 white_scale          = div(nvec3(1.0F), tonemapUncharted2Impl(nvec3(W)));
  return gammaCorrection(color * white_scale, gamma);
}

INLINE vec3 applyTonemap(Tonemapper tm, vec3 color, vec2 uv)
{
  // Exposure
  color *= tm.exposure;
  vec3 c;
  // Tonemap
  switch(tm.method)
  {
    case eTonemapFilmic:
      c = tonemapFilmic(color);
      break;
    case eTonemapUncharted:
      c = tonemapUncharted(color, tm.gamma);
      break;
    default:
      c = gammaCorrection(color, tm.gamma);
      break;
  }
  //contrast
  c = clamp(mix(nvec3(0.5F), c, nvec3(tm.contrast)), nvec3(0.F), nvec3(1.F));
  // brighness
  c = pow(c, vec3(1.0F / tm.brightness));
  // saturation
  vec3 i = vec3(dot(c, vec3(0.299F, 0.587F, 0.114F)));
  c      = mix(i, c, tm.saturation);
  // vignette
  vec2 center_uv = ((uv)-vec2(0.5F)) * vec2(2.0F);
  c *= 1.0F - dot(center_uv, center_uv) * tm.vignette;

  return c;
}

// http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
INLINE vec3 toSrgb(vec3 rgb)
{
  vec3 s1 = sqrt(rgb);
  vec3 s2 = sqrt(s1);
  vec3 s3 = sqrt(s2);
  return vec3(0.662002687F) * s1 + vec3(0.684122060F) * s2 - vec3(0.323583601F) * s3 - vec3(0.0225411470F) * rgb;
}

INLINE vec3 toLinear(vec3 srgb)
{
  return scale(srgb, (scale(srgb, (scale(srgb, nvec3(0.305306011F)) + nvec3(0.682171111F))) + nvec3(0.012522878F)));
}
#endif  // DH_TONEMAMP_H
