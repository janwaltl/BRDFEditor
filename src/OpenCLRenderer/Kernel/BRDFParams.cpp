#include "BRDFParams.h"

namespace brdfEditor::clRenderer {
BRDFParam_cl
BRDFParam::getCL() const {
	return std::visit(
		[](auto &&p) -> BRDFParam_cl {
			using ParamT = std::decay_t<decltype(p)>;
			BRDFParam_cl param_cl;
			if constexpr (std::is_same_v<ParamT, BRDFIntParam>) {
				param_cl.integer = p.value;
			} else if constexpr (std::is_same_v<ParamT, BRDFFloatParam>) {
				param_cl.float1 = p.value;
			} else if constexpr (std::is_same_v<ParamT, BRDFFloat2Param>) {
				param_cl.float2 = p.value;
			} else if constexpr (std::is_same_v<ParamT, BRDFFloat3Param>) {
				param_cl.float3 = p.value;
			} else if constexpr (std::is_same_v<ParamT, BRDFFloat4Param>) {
				param_cl.float4 = p.value;
			} else
				static_assert(false, "Does not cover all variant types.");
			return param_cl;
		},
		param);
}
} // namespace brdfEditor::clRenderer
