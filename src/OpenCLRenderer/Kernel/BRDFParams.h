#ifndef BRDFEDITOR_OPENCL_KERNEL_BRDF_PARAM_HEADER
#define BRDFEDITOR_OPENCL_KERNEL_BRDF_PARAM_HEADER

#include <map>
#include <string>
#include <utility>
#include <type_traits>
#include <variant>

#include <src/OpenCLRenderer/CLHeaders.hpp>

namespace brdfEditor::clRenderer {

// OpenCL struct for a BRDF parameter.
struct BRDFParam_cl {
	union {
		cl_int integer;
		cl_float float1;
		cl_float2 float2;
		cl_float3 float3;
		cl_float4 float4;
	};
};

namespace impl {
// ID is needed because cl_float3 is defined as cl_float4 thus the
// BRDFFloat3Param and BRDFFloat4Param are the same types which messes up std::variant.
// indexing in BRDFParam class.
template <typename ValueType, typename BoundType, int ID>
struct BRDFParam_impl {
	ValueType value;
	BoundType min, max, step;
};
template <>
struct BRDFParam_impl<cl_float3, float,3> {
	cl_float3 value;
	float min, max, step;
	bool isColor;
};
} // namespace impl

using BRDFIntParam = impl::BRDFParam_impl<cl_int, int, 0>;
using BRDFFloatParam = impl::BRDFParam_impl<cl_float, float, 1>;
using BRDFFloat2Param = impl::BRDFParam_impl<cl_float2, float, 2>;
using BRDFFloat3Param = impl::BRDFParam_impl<cl_float3, float, 3>;
using BRDFFloat4Param = impl::BRDFParam_impl<cl_float4, float, 4>;

// A parameter for a BRDF material.
class BRDFParam {
public:
	template <typename T>
	BRDFParam(T &&parameter) : param(std::forward<T>(parameter)) {}

	template <typename T>
	constexpr bool
	holdsType() const {
		return std::holds_alternative<T>(param);
	}
	constexpr bool isSameTypeAs(const BRDFParam& other)
	{
		return other.param.index() == this->param.index();
	}
	template <typename T>
	T &
	get() {
		return std::get<T>(param);
	}

	template <typename T>
	const T &
	get() const {
		return std::get<T>(param);
	}
	template <typename T>
	void
	set(T &&parameter) {
		param = parameter;
	}
	BRDFParam_cl
	getCL() const;
	template <typename Fn>
	auto
	visit(Fn &&fnc) {
		return std::visit(std::forward<Fn>(fnc), param);
	}
	template <typename Fn>
	auto
	visit(Fn &&fnc) const {
		return std::visit(std::forward<Fn>(fnc), param);
	}

private:
	std::variant<BRDFIntParam, BRDFFloatParam, BRDFFloat2Param, BRDFFloat3Param,
				 BRDFFloat4Param>
		param;
};

using BRDFParams_t = std::map<std::string, BRDFParam>;
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_KERNEL_BRDF_PARAMS_HEADER
