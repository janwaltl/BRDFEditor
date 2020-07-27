#ifndef BRDFEDITOR_OPENCL_RENDERER_ENV_MAP_HEADER
#define BRDFEDITOR_OPENCL_RENDERER_ENV_MAP_HEADER

#include <string>
#include <vector>

#include "CLHeaders.hpp"

namespace brdfEditor::clRenderer {

	struct EnvMap_cl
	{
		//Float3
		std::vector<cl_float4> envMarginalRowHist;
		//Float3
		std::vector<cl_float4> envRowHists;
		std::vector<cl_float4> envMap;
		std::size_t width;
		std::size_t height;
	};
class EnvMap {
public:
	EnvMap(const std::string &filename, float intensity);
	
	EnvMap_cl&
	getCL();
	const EnvMap_cl& getCL()const;

	static EnvMap makeBlack(std::size_t width, std::size_t height);
	
	float intensity;
private:
	EnvMap(EnvMap_cl&& clMap,float intensity);
	EnvMap_cl clMap;
};
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_RENDERER_ENV_MAP_HEADER
