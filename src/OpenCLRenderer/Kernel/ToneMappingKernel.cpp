#include "ToneMappingKernel.hpp"

namespace brdfEditor::clRenderer {
ToneMappingKernel::ToneMappingKernel(std::string name, cl::Kernel kernel,
									 std::size_t numSamplesArgIndex,
									 std::size_t inImageArgIndex,
									 std::size_t outImageArgIndex) :
	kernel(std::move(kernel)),
	name(std::move(name)),
	numSamplesArgIndex(numSamplesArgIndex),
	inImageArgIndex(inImageArgIndex),
	outImageArgIndex(outImageArgIndex) {}

void
ToneMappingKernel::setArguments(std::size_t numSamples, cl::Buffer &inImage,
								const cl::Image &outImage,
								cl::NDRange imageSize) {
	this->imageSize = imageSize;
	kernel.setArg(numSamplesArgIndex, static_cast<int>(numSamples));
	kernel.setArg(inImageArgIndex, inImage);
	kernel.setArg(outImageArgIndex, outImage);
}

cl::Kernel
ToneMappingKernel::getKernel() const{
	return kernel;
}

const cl::NDRange
ToneMappingKernel::getImageSize() const {
	return imageSize;
}

void
ToneMappingKernel::doExecute(const cl::CommandQueue &queue) {
	queue.enqueueNDRangeKernel(getKernel(), 0, getImageSize());
}

void
ToneMappingKernel::execute(const cl::CommandQueue &queue) {
	doExecute(queue);
}

const std::string &
ToneMappingKernel::getName() const {
	return name;
}

GammaCompressionTMKernel::GammaCompressionTMKernel(
	std::string name, cl::Kernel kernel, std::size_t numSamplesArgIndex,
	std::size_t inImageArgIndex, std::size_t outImageArgIndex, float gamma) :
	ToneMappingKernel(std::move(name), std::move(kernel), numSamplesArgIndex,
					  inImageArgIndex, outImageArgIndex),
	gamma(gamma) {}

void
GammaCompressionTMKernel::setGamma(float gamma) {
	this->gamma = gamma;
}

float
GammaCompressionTMKernel::getGamma() const {
	return gamma;
}

void GammaCompressionTMKernel::setAlpha(float alpha)
{
	this->alpha = alpha;
}

float GammaCompressionTMKernel::getAlpha() const
{
	return alpha;
}

void GammaCompressionTMKernel::acceptVisit(ToneMappingKArgsVisitor &v)
{
	v.visit(*this);
}

void
GammaCompressionTMKernel::doExecute(const cl::CommandQueue &queue) {
	getKernel().setArg(3, alpha);
	getKernel().setArg(4, gamma);
	queue.enqueueNDRangeKernel(getKernel(), 0, getImageSize());
}

void SimpleTMKernel::setBeta(float beta)
{
	this->beta = beta;
}

float SimpleTMKernel::getBeta() const
{
	return beta;
}

void SimpleTMKernel::acceptVisit(ToneMappingKArgsVisitor & v)
{
	v.visit(*this);
}

void SimpleTMKernel::doExecute(const cl::CommandQueue & queue)
{
	getKernel().setArg(3, beta);
	queue.enqueueNDRangeKernel(getKernel(), 0, getImageSize());
}

} // namespace brdfEditor::clRenderer
