#ifndef BRDFEDITOR_OPENCL_TONE_MAPPING_KERNEL_HEADER
#define BRDFEDITOR_OPENCL_TONE_MAPPING_KERNEL_HEADER

#include <string>

#include <src/OpenCLRenderer/CLHeaders.hpp>

namespace brdfEditor::clRenderer {

	class SimpleTMKernel;
	class GammaCompressionTMKernel;
	struct ToneMappingKArgsVisitor {
		virtual void visit(SimpleTMKernel&) = 0;
		virtual void visit(GammaCompressionTMKernel&) = 0;
};
// Kernel representing a tonemapping function that will be applied on the
// rendered image.
class ToneMappingKernel {
public:
	ToneMappingKernel(std::string name, cl::Kernel kernel,
					  std::size_t numSamplesArgIndex,
					  std::size_t inImageArgIndex,
					  std::size_t outImageArgIndex);
	virtual ~ToneMappingKernel() = default;

	void
	setArguments(std::size_t numSamples, cl::Buffer &inImage,
				 const cl::Image &outImage, cl::NDRange imageSize);

	void
	execute(const cl::CommandQueue &queue);
	const std::string &
	getName() const;

	virtual void acceptVisit(ToneMappingKArgsVisitor&) = 0;
protected:
	cl::Kernel
	getKernel() const;
	const cl::NDRange
	getImageSize() const;

private:
	// Enqueues the kernel into the command queue.
	// Default implementation calls 2D kernel without local groups.
	virtual void
	doExecute(const cl::CommandQueue &queue);

	cl::Kernel kernel;
	cl::NDRange imageSize;
	std::string name;
	std::size_t numSamplesArgIndex;
	std::size_t inImageArgIndex;
	std::size_t outImageArgIndex;
};

// Tonemapping: result =  LI/(Beta+LI)
class SimpleTMKernel : public ToneMappingKernel {
public:
	using ToneMappingKernel::ToneMappingKernel;
	void
	setBeta(float beta);
	float
	getBeta() const;
	virtual void acceptVisit(ToneMappingKArgsVisitor&v) override final;
private:
	virtual void
	doExecute(const cl::CommandQueue &queue) override final;

	float beta = 1.0f;
};

// Tonemapping: result =  alpha* LI^Gamma
class GammaCompressionTMKernel : public ToneMappingKernel {
public:
	GammaCompressionTMKernel(std::string name, cl::Kernel kernel,
							 std::size_t numSamplesArgIndex,
							 std::size_t inImageArgIndex,
							 std::size_t outImageArgIndex, float gamma);

	void
	setGamma(float gamma);
	float
	getGamma() const;
	void
	setAlpha(float alpha);
	float
	getAlpha() const;

	virtual void acceptVisit(ToneMappingKArgsVisitor&v) override final;
private:
	virtual void
	doExecute(const cl::CommandQueue &queue) override final;

	float alpha, gamma;
};
} // namespace brdfEditor::clRenderer
#endif
