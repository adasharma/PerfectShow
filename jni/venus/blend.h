#ifndef VENUS_BLEND_H_
#define VENUS_BLEND_H_

#include <stdint.h>
#include <algorithm>

#include <opencv2/core.hpp>

#include "venus/compiler.h"
#include "venus/scalar.h"

/* 
	some useful links:
	https://en.wikipedia.org/wiki/Blend_modes and other valuable references at the bottom page.
	https://en.wikipedia.org/wiki/Alpha_compositing

	http://stackoverflow.com/questions/5919663/how-does-photoshop-blend-two-images-together
	http://www.deepskycolors.com/archive/2010/04/21/formulas-for-Photoshop-blending-modes.html
	https://helpx.adobe.com/photoshop/using/blending-modes.html
*/

namespace venus {

template <typename T, int N = 4>
cv::Vec<T, N> mix(const cv::Vec<T, N>& dst, const cv::Vec<T, N>& src, float amount)
{
	cv::Vec<T, N> result;
	for(int i = 0; i < N; ++i)
		result[i] = lerp<T>(dst[i], src[i], amount);
	return result;
}

template<> cv::Vec3f mix<float,   3>(const cv::Vec3f& dst, const cv::Vec3f& src, float amount);
template<> cv::Vec4b mix<uint8_t, 4>(const cv::Vec4b& dst, const cv::Vec4b& src, float amount);
template<> cv::Vec4f mix<float,   4>(const cv::Vec4f& dst, const cv::Vec4f& src, float amount);


/**
 * It's like lerp but use uint32_t for 4 channels.
 */
uint32_t mix(const uint32_t& dst, const uint32_t& src, float amount);



// gimp/app/actions/layers-commands.c
// int mode range [0, 255], float mode range [0.0, 1.0]



// blend mode for uint8_t version
inline uint8_t blendNormal     (uint8_t A, uint8_t B)  { return A; }
inline uint8_t blendLighten    (uint8_t A, uint8_t B)  { return std::max(A, B); }
inline uint8_t blendDarken     (uint8_t A, uint8_t B)  { return std::min(A, B); }
inline uint8_t blendMultiply   (uint8_t A, uint8_t B)  { return (A * B + 127) / 255; }
inline uint8_t blendAverage    (uint8_t A, uint8_t B)  { return (A + B + 1) / 2; }
inline uint8_t blendAdd        (uint8_t A, uint8_t B)  { return std::min(255, A + B); }
inline uint8_t blendSubtract   (uint8_t A, uint8_t B)  { return std::max(0, A + B - 255); }
inline uint8_t blendDifference (uint8_t A, uint8_t B)  { return std::abs(A - B); }
inline uint8_t blendNegation   (uint8_t A, uint8_t B)  { return 255 - std::abs(255 - A - B); }
inline uint8_t blendScreen     (uint8_t A, uint8_t B)  { return 255 - ((255 - A) * (255 - B) + 127) / 255; }
inline uint8_t blendExclusion  (uint8_t A, uint8_t B)  { return A + B - (2 * A * B + 127)/ 255; }
inline uint8_t blendOverlay    (uint8_t A, uint8_t B)  { return (uint8_t)((B < 128 ? 2*A*B : (255 - 2*(255 - A)*(255 - B)) + 127)/ 255); }
inline uint8_t blendSoftLight  (uint8_t A, uint8_t B)  { return (B < 128) ? (2*((A>>1)+64))*(B/255.0F) : (255-(2*(255-((A>>1)+64))*(255-B)/255.0F)); }
inline uint8_t blendHardLight  (uint8_t A, uint8_t B)  { return blendOverlay(B, A); }
inline uint8_t blendColorDodge (uint8_t A, uint8_t B)  { return (B == 255) ? 255 : std::min(255, A * 255 / (255 - B)); }
inline uint8_t blendColorBurn  (uint8_t A, uint8_t B)  { return (B ==   0) ?   0 : std::max(  0, 255 - (255 - A) * 255 / B); }
inline uint8_t blendLinearDodge(uint8_t A, uint8_t B)  { return blendAdd     (A, B); }
inline uint8_t blendLinearBurn (uint8_t A, uint8_t B)  { return blendSubtract(A, B); }
inline uint8_t blendLinearLight(uint8_t A, uint8_t B)  { return (B < 128) ? blendLinearBurn(A, 2 * B) : blendLinearDodge(A, 2 * (B - 128)); }
inline uint8_t blendVividLight (uint8_t A, uint8_t B)  { return (B < 128) ? blendColorBurn (A, 2 * B) : blendColorDodge (A, 2 * (B - 128)); }
inline uint8_t blendPinLight   (uint8_t A, uint8_t B)  { return (B < 128) ? blendDarken    (A, 2 * B) : blendLighten    (A, 2 * (B - 128)); }
inline uint8_t blendHardMix    (uint8_t A, uint8_t B)  { return blendVividLight(A, B) < 128 ? 0:255; }
inline uint8_t blendReflect    (uint8_t A, uint8_t B)  { return (B == 255) ? 255 : std::min(255, A*A / (255 - B)); }
inline uint8_t blendGlow       (uint8_t A, uint8_t B)  { return blendReflect(B, A); }
inline uint8_t blendPhoenix    (uint8_t A, uint8_t B)  { return std::min(A, B) - std::max(A, B) + 255; }
inline uint8_t blendAlpha      (uint8_t A, uint8_t B, uint8_t O)  { return lerp(B, A, O);/*O * A + (1 - O) * B*/ }
inline uint8_t blendAlphaF     (uint8_t A, uint8_t B, uint8_t (*F)(uint8_t, uint8_t), uint8_t O)  { return blendAlpha(F(A, B), A, O); }

// blend mode for float version
inline float blendNormal     (float A, float B)  { return A; }
inline float blendLighten    (float A, float B)  { return std::max(A, B); }
inline float blendDarken     (float A, float B)  { return std::min(A, B); }
inline float blendMultiply   (float A, float B)  { return A * B; }
inline float blendAverage    (float A, float B)  { return (A + B) / 2; }
inline float blendAdd        (float A, float B)  { return std::min(1.0F, A + B); }
inline float blendSubtract   (float A, float B)  { return std::max(0.0F, A + B - 1.0F); }
inline float blendDifference (float A, float B)  { return std::abs(A - B); }
inline float blendNegation   (float A, float B)  { return 1.0F - std::abs(1.0F - A - B); }
inline float blendScreen     (float A, float B)  { return 1.0F - (1.0F - A) * (1.0F - B); }
inline float blendExclusion  (float A, float B)  { return A + B - 2*A*B; }
inline float blendOverlay    (float A, float B)  { return (B <= 0.5F) ? (2*A*B) : (1.0F - 2*(1.0F - A)*(1.0F - B)); }
inline float blendSoftLight  (float A, float B)  { return (B <= 0.5F) ? (A + 0.5F)*B : (1.0F - (1.5F - A)*(1.0F - B)); }
inline float blendHardLight  (float A, float B)  { return blendOverlay(B, A); }
inline float blendColorDodge (float A, float B)  { return fuzzyEqual(B, 1.0F) ? B : std::min(1.0F, A * 1.0F / (1.0F - B)); }
inline float blendColorBurn  (float A, float B)  { return fuzzyEqual(B, 0.0F) ? B : std::max(0.0F, 1.0F - (1.0F - A) * 1.0F / B); }
inline float blendLinearDodge(float A, float B)  { return blendAdd     (A, B); }
inline float blendLinearBurn (float A, float B)  { return blendSubtract(A, B); }
inline float blendLinearLight(float A, float B)  { return (B <= 0.5F) ? blendLinearBurn(A, 2 * B) : blendLinearDodge(A, 2 * (B - 0.5F)); }
inline float blendVividLight (float A, float B)  { return (B <= 0.5F) ? blendColorBurn (A, 2 * B) : blendColorDodge (A, 2 * (B - 0.5F)); }
inline float blendPinLight   (float A, float B)  { return (B <= 0.5F) ? blendDarken    (A, 2 * B) : blendLighten    (A, 2 * (B - 0.5F)); }
inline float blendHardMix    (float A, float B)  { return blendVividLight(A, B) <= 0.5F ? 0:1.0F; }
inline float blendReflect    (float A, float B)  { return fuzzyEqual(B, 1.0F) ? B : std::min(1.0F, A*A / (1.0F - B)); }
inline float blendGlow       (float A, float B)  { return blendReflect(B, A); }
inline float blendPhoenix    (float A, float B)  { return std::min(A, B) - std::max(A, B) + 1.0F; }
inline float blendAlpha      (float A, float B, float O)  { return lerp(B, A, O);/*O * A + (1 - O) * B*/ }
inline float blendAlphaF     (float A, float B, float (*F)(float, float), float O)  { return blendAlpha(F(A, B), A, O); }

void blendHue       (float* T, const float* A, const float* B);
void blendSaturation(float* T, const float* A, const float* B);
void blendLuminosity(float* T, const float* A, const float* B);
void blendColor     (float* T, const float* A, const float* B);




} /* namespace venus */
#endif /* VENUS_BLEND_H_ */