#include "CustomKernel.hpp"

namespace brdfEditor::clRenderer {
CustomKernel::CustomKernel(PassKey<Renderer>&& key,cl::Context context, cl::CommandQueue queue,
						   cl::Kernel kernel, cl::Program program) :
	context(std::move(context)),
	queue(std::move(queue)),
	kernel(std::move(kernel)),
	program(std::move(program)),
	validity(true) {
	(void)key;
}

cl::Context &
CustomKernel::getContext() {
	return context;
}

cl::Kernel &
CustomKernel::getKernel() {
	return kernel;
}

cl::CommandQueue & CustomKernel::getQueue()
{
	return queue;
}

void
CustomKernel::run(cl::NDRange globalRange, cl::NDRange localRange) {
	queue.enqueueNDRangeKernel(kernel, 0, globalRange, localRange);
}

bool
CustomKernel::isValid() {
	return true;
}
void
CustomKernel::setValidity(PassKey<Renderer> &&key, bool validity) {
	(void)key;
	this->validity = validity;
}
} // namespace brdfEditor::clRenderer
