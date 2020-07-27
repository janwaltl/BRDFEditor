#ifndef BRDFEDITOR_OPENCL_KERNEL_BRDF_HEADER
#define BRDFEDITOR_OPENCL_KERNEL_BRDF_HEADER

#include <string>
#include <vector>

#include "src/OpenCLRenderer/Kernel/BRDFParams.h"
#include "src/Core/Exception.hpp"

namespace brdfEditor::clRenderer {
// Represents BRDFunction as parsed source.
class BRDF {
public:
	class FormatException : public Exception {
	public:
		using Exception::Exception;
	};
	using parameters_t = std::map<std::string, BRDFParam>;

	// Source must contain 'BRDF' function and optionally 'SampleBRDF' function.
	// --prefix: a string used to prefix all global names. That means 'BRDF',
	// 'SampleBRDF' and any other functions.
	// Throws: FormatException on invalid format.
	BRDF(std::string source, const std::string &prefix);

	// Returns definitions for BRDF() and possibly SampleBRDF()
	const std::string &
	getDefinitions() const;
	// String containing all declarations needed for this BRDF.
	const std::string &
	getDeclarations() const;
	// Whether the optional 'SampleBRDF' function was present in the source.
	bool
	hasCustomSampling() const;
	// Returns prefixed BRDF name.
	const std::string &
	getBRDFFncName() const;
	// Returns prefixed SampleBRDF name or nullptr if
	// hasCustomSampling()==false.
	const std::string *
	getSampleBRDFFncName() const;
	// Returns prefixed GetWInPDF name or nullptr if
	// hasCustomSampling()==false.
	const std::string *
		getGetwInPDFFncName() const;
	// Returns source that was used to generate this BRDF.
	const std::string &
	getSource() const;
	// See const variant.
	const std::string &
	getSource();

	friend bool
	operator==(const BRDF &l, const BRDF &r);
	// Assigns and parses new source.
	// Uses same prefix as given in the constructor.
	// May invalidate materials' parameters!
	// Throws: FormatException on invalid format.
	void
	changeSource(const std::string &newSource);

private:
	std::string prefix;
	std::string inputSource;
	std::string brdfFncName;
	std::string sampleBrdfFncName;
	std::string getWInPDFFncName;
	// Header declarations
	std::string declaration;
	// Definitions for BRDF() and possibly SampleBRDF()
	std::string definitions;

public:
	parameters_t parameters;
	// Names of the materials associated with this BRDF
	std::vector<std::string> materials;
};

using BRDFS_t = std::map<std::string, BRDF>;
// Small value type, cheap copy, move. random-access iterator.
// Valid until the source(or the library) exists.
using BRDFsCHandle = BRDFS_t::const_iterator;
// See BRDFsCHandle
using BRDFsHandle = BRDFS_t::iterator;
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_KERNEL_BRDF_HEADER
