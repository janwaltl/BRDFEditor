#include "ToneMappingKernelSource.hpp"

#include <src/Core/Exception.hpp>

#include "ToneMappingKernel.hpp"

namespace brdfEditor::clRenderer {

class ToneMappingKernel;
// Contains source code for a tone-mapping kernel.
ToneMappingKernelSource::ToneMappingKernelSource(
	std::string source, std::string name, makeKernel_fnc kernelGenerator) :
	source(std::move(source)),
	name(std::move(name)),
	kernelGenerator(std::move(kernelGenerator)) {

	auto fncNameBeginIT = getSource().find("TONE_MAPPING");
	if (fncNameBeginIT == getSource().npos)
		throw Exception("Tone mapping kernel '" + getName() +
						"' does not contain 'TONE_MAPPING' kernel function.");
}

std::string &
ToneMappingKernelSource::getSource() {
	return source;
}
const std::string &
ToneMappingKernelSource::getSource() const {
	return source;
}
const std::string &
ToneMappingKernelSource::getName() {
	return name;
}
const std::string &
ToneMappingKernelSource::getName() const {
	return name;
}

std::unique_ptr<ToneMappingKernel>
ToneMappingKernelSource::generateKernel(cl::Kernel kernel) const {
	return kernelGenerator(name, std::move(kernel));
}
namespace {
constexpr const static char *simpleSource = R"(
__kernel void TONE_MAPPING(int numSamples, global float4* inputImage, write_only image2d_t outImage,float beta)
{
	int tID= get_global_id(0) + get_global_size(0)*get_global_id(1);

	float4 pixel = inputImage[tID]/numSamples;

	float4 tonedCol = pixel/(pixel+beta);
	tonedCol.w=1.0f;
    write_imagef(outImage,(int2)(get_global_id(0),get_global_id(1)),tonedCol);	
}
)";

constexpr const static char *gammeCompressionSource = R"(
__kernel void TONE_MAPPING(int numSamples, global float4* inputImage, write_only image2d_t outImage, float alpha, float gamma)
{
	int tID= get_global_id(0) + get_global_size(0)*get_global_id(1);

	float4 pixel = inputImage[tID]/numSamples;

	float4 tonedCol = alpha*pow(pixel,gamma);
	tonedCol.w=1.0f;
    write_imagef(outImage,(int2)(get_global_id(0),get_global_id(1)),tonedCol);	
}
)";

} // namespace
ToneMappingKernelSource
makeSimpleTMKernelSource() {

	auto generator = [](std::string name, cl::Kernel kernel) {
		return std::make_unique<SimpleTMKernel>(std::move(name), kernel, 0, 1,
												2);
	};
	return ToneMappingKernelSource(simpleSource, "simpleToneMapping",
								   generator);
}

ToneMappingKernelSource
makeGammaCompressionTMKernelSource() {
	auto generator = [](std::string name, cl::Kernel kernel) {
		return std::make_unique<GammaCompressionTMKernel>(
			std::move(name), kernel, 0, 1, 2, 1.0f);
	};
	return ToneMappingKernelSource(gammeCompressionSource, "gammaCompressionTM",
								   generator);
}

}; // namespace brdfEditor::clRenderer