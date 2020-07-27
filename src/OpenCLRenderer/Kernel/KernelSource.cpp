#include "KernelSource.hpp"
#include <regex>
#include <sstream>
#include <cctype>
#include <iostream>
#include <cassert>

#include "src/Core/Exception.hpp"
#include "BRDF.hpp"

namespace brdfEditor::clRenderer {
namespace {
std::map<std::string, int>
buildMatIds(const std::vector<BRDFsCHandle> &BRDFs) {
	std::map<std::string, int> matIDs;
	int i = 0;
	for (auto &&BRDFHandle : BRDFs) {
		auto &&[brdfName, brdf] = *BRDFHandle;
		matIDs.try_emplace(brdf.getBRDFFncName(), i++);
	}
	return matIDs;
}

constexpr const char *brdfEvalDecl = "float3 BRDFEval(MatInfo matInfo, float3 "
									 "normal, float3 wIn, float3 wOut, "
									 "__global Param* paramBuffer)";
constexpr const char *brdfSampleEvalDecl =
	"float3 BRDFSampleEval(MatInfo matInfo, float3 "
	"normal, float3 wIn, float3* "
	"wOut, RNGState* rngState, float* pdf, __global "
	"Param* paramBuffer)";
constexpr const char *getWInPDFDecl =
	"float GetwInPDF(MatInfo matInfo, float3 n, float3 wOut,float3 wIn, __global "
	"Param* paramBuffer)";
constexpr const char *beginSwitch = "\n{ \n\tswitch(matInfo.brdfID)\n\t{\n";
// Tag located in the kernel that marks where to put the declarations
constexpr const char *declTag = "//DECLHERE";
// Builds BRDFEval() function for the path tracer that uses switch(matID) to
// select material's BRDF() 	BRDFs - will be used in the switch 	matIDs -
// converts
// BRDFFncName() to the matID, must contain entries for the BRDFs 	declaration,
// definition - output parameters for the BRDFEval() function.
void
buildBRDFEvalFnc(const std::vector<BRDFsCHandle> &BRDFs,
				 const std::map<std::string, int> &matIDs,
				 std::string *declaration, std::string *definition) {
	*declaration = std::string(brdfEvalDecl) + ";\n";

	std::string def;
	def += brdfEvalDecl;
	def += beginSwitch;
	for (auto &&BRDFHandle : BRDFs) {
		auto &&[brdfName, brdf] = *BRDFHandle;
		// Will be found, because matIDs were built from BRDFs
		int matID = matIDs.at(brdf.getBRDFFncName());
		std::string matCase = "\tcase " + std::to_string(matID) + ":\n";
		def += matCase;
		def += "\t\treturn " + brdf.getBRDFFncName() +
			   "(normal, wIn, wOut,paramBuffer + matInfo.paramOff);\n";
	}
	def += "\tdefault:\n\t\treturn (float3)(0.0f);\n\t}\n}\n";
	*definition = std::move(def);
}
// Builds BRDFSampleEval() function for the path tracer that uses switch(matID)
// to select material's SampleBRDF(). If a BRDF does not support custom
// sampling, cosine-weighted sampling and material's BRDF() will be used. BRDFs
// - will be
// used in the switch 	matIDs - converts BRDFFncName() to the matID, must
// contain entries for the BRDFs 	declaration, definition - output parameters
// for the BRDFSampleEval() function.
void
builSampledBRDFEvalFnc(const std::vector<BRDFsCHandle> &BRDFs,
					   const std::map<std::string, int> &matIDs,
					   std::string *declaration, std::string *definition) {
	*declaration = std::string(brdfSampleEvalDecl) + ";\n";

	std::string def;
	def += brdfSampleEvalDecl;
	def += beginSwitch;
	for (auto &&BRDFHandle : BRDFs) {
		auto &&[brdfName, brdf] = *BRDFHandle;

		// Will be found, because matIDs were built from BRDFs
		int matID = matIDs.at(brdf.getBRDFFncName());
		std::string matCase = "\tcase " + std::to_string(matID) + ":\n";
		def += matCase;
		if (brdf.hasCustomSampling())
			def += "\t\treturn " + *brdf.getSampleBRDFFncName() +
				   "(normal,wIn,wOut,pdf,rngState,paramBuffer + "
				   "matInfo.paramOff);\n";
		else {
			def += "\t\t*wOut=genCosineDir(wIn, normal, rngState, pdf);\n";
			def += "\t\treturn " + brdf.getBRDFFncName() +
				   "(normal, wIn, *wOut,paramBuffer + matInfo.paramOff);\n";
		}
	}
	def += "\tdefault:\n\t\treturn (float3)(0.0f);\n\t}\n}\n";
	*definition = std::move(def);
}
} // namespace

void
buildGetwInPDF(const std::vector<BRDFsCHandle> &BRDFs,
			   const std::map<std::string, int> &matIDs,
			   std::string *declaration, std::string *definition) {
	*declaration = std::string(getWInPDFDecl) + ";\n";

	std::string def;
	def += getWInPDFDecl;
	def += beginSwitch;
	for (auto &&BRDFHandle : BRDFs) {
		auto &&[brdfName, brdf] = *BRDFHandle;

		// Will be found, because matIDs were built from BRDFs
		int matID = matIDs.at(brdf.getBRDFFncName());
		std::string matCase = "\tcase " + std::to_string(matID) + ":\n";
		def += matCase;
		if (brdf.hasCustomSampling())
			def += "\t\treturn " + *brdf.getGetwInPDFFncName() +
				   "(n,wOut,wIn,paramBuffer + "
				   "matInfo.paramOff);\n";
		else {
			// Cosine-weighted sampling has this PDF
			def += "\t\treturn dot(n,wIn)/PI;\n";
		}
	}
	def += "\tdefault:\n\t\treturn 0.0f;\n\t}\n}\n";
	*definition = std::move(def);
}
//
KernelMainSource::KernelMainSource(const std::string &mainSource,
								   std::string kernelFncName,
								   const std::vector<BRDFsCHandle> &BRDFs) :
	matIDs(buildMatIds(BRDFs)),
	kernelFncName(std::move(kernelFncName)) {
	std::string brdfDecls;
	for (auto &&BRDFHandle : BRDFs) {
		auto &&[brdfName, brdf] = *BRDFHandle;
		brdfDecls += brdf.getDeclarations();
		brdfDecls += '\n';
	}
	std::string brdfDefs{'\n'};
	for (auto &&BRDFHandle : BRDFs) {
		auto &&[brdfName, brdf] = *BRDFHandle;
		brdfDefs +=
			std::string("#line ") + "1 \"" + brdf.getBRDFFncName() + "\"\n";
		brdfDefs += brdf.getDefinitions();
		brdfDefs += '\n';
	}
	std::string brdfEvalDecl, brdfEvalDef, brdfSampleEvalDecl,
		brdfSampleEvalDef, getwInEvalDecl, getWInEvalDef;
	buildBRDFEvalFnc(BRDFs, matIDs, &brdfEvalDecl, &brdfEvalDef);
	builSampledBRDFEvalFnc(BRDFs, matIDs, &brdfSampleEvalDecl,
						   &brdfSampleEvalDef);
	buildGetwInPDF(BRDFs, matIDs, &getwInEvalDecl, &getWInEvalDef);
	source = mainSource;
	// Inserts declarations to predefined position in the kernel.

	source.replace(source.find(declTag), strlen(declTag),
				   brdfDecls + brdfEvalDecl + brdfSampleEvalDecl +
					   getwInEvalDecl);
	// Append all other definitions so they can leverage helper functions in the
	// kernel.
	source += brdfSampleEvalDef;
	source += brdfEvalDef;
	source += getWInEvalDef;
	source += brdfDefs;
}

const std::string &
KernelMainSource::getKernelFncName() const {
	return kernelFncName;
}
int
KernelMainSource::getMatID(const std::string &brdfFncName) const {
	auto IT = matIDs.find(brdfFncName);
	return IT != matIDs.end() ? IT->second : -1;
}
const std::string &
KernelMainSource::getSource() const {
	return source;
}

} // namespace brdfEditor::clRenderer
