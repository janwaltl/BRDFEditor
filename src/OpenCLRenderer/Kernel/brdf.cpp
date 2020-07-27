#include "BRDF.hpp"

#include <numeric>
#include <cassert>
#include <regex>
#include <sstream>
#include <cctype>
#include <map>
#include <iostream>

namespace brdfEditor::clRenderer {
bool
operator==(const BRDF &l, const BRDF &r) {
	return l.brdfFncName == r.brdfFncName &&
		   l.getDefinitions() == r.getDefinitions();
}
namespace {
constexpr const char *brdfEvalDecl = "float3 BRDFEval(MatInfo matInfo, float3 "
									 "normal, float3 wIn, float3 wOut, "
									 "__global Param* paramBuffer)";
constexpr const char *brdfSampleEvalDecl =
	"float3 BRDFSampleEval(MatInfo matInfo, float3 "
	"normal, float3 wIn, float3* "
	"wOut, RNGState* rngState, float* pdf, __global "
	"Param* paramBuffer)";
constexpr const char *beginSwitch = "\n{ \n\tswitch(matInfo.brdfID)\n\t{\n";

template <typename Number, typename Fnc>
std::vector<Number>
parseDefaultNumbers(const std::string &defaultStr, Fnc &&strToNumber) {
	std::regex defaultRegex{R"(([\-0-9\.f]+))"};
	auto defaultBegin = std::sregex_iterator(defaultStr.begin(),
											 defaultStr.end(), defaultRegex);
	auto defaultEnd = std::sregex_iterator();
	if (!defaultStr.empty() && defaultBegin == defaultEnd)
		throw BRDF::FormatException(
			"Default initializer does not contain any numbers.");

	std::vector<Number> numbers;

	while (defaultBegin != defaultEnd) {

		numbers.push_back(strToNumber(defaultBegin->str()));
		++defaultBegin;
	}
	return numbers;
}
BRDFParam
buildParam(const std::string &type, const std::string &minStr,
		   const std::string &maxStr, const std::string &stepStr,
		   const std::string &defaultStr, bool color) {
	try {
		// Shared parsing of cl_floatX - extracts min,max,step and default
		// numbers.
		auto &&toFloats = [&](auto &param, std::size_t reqNumNumbers) {
			param.min = minStr.empty() ? -100.0f : std::stof(minStr);
			param.max = maxStr.empty() ? +100.0f : std::stof(maxStr);
			param.step = stepStr.empty() ? 1.0f : std::stof(stepStr);

			if (defaultStr.empty()) {
				std::vector<cl_float> numbers(reqNumNumbers);
				std::fill(numbers.begin(), numbers.end(),
						  (param.max + param.min) / 2.0f);
				return numbers;
			}

			auto numbers = parseDefaultNumbers<cl_float>(
				defaultStr, [](auto n) { return std::stof(n); });
			if (numbers.size() != reqNumNumbers) {
				std::string reqNum = std::to_string(reqNumNumbers);
				throw BRDF::FormatException("Default initializer for float" +
											reqNum + " must contain exactly " +
											reqNum + " floats.");
			}
			return numbers;
		};

		if (type == "int") {
			BRDFIntParam p;
			p.min = minStr.empty() ? INT32_MIN : std::stoi(minStr);
			p.max = maxStr.empty() ? INT32_MIN : std::stoi(maxStr);
			p.step = stepStr.empty() ? 1 : std::stoi(stepStr);
			if (defaultStr.empty())
				p.value = (p.max + p.min) / 2;
			else {
				auto numbers = parseDefaultNumbers<cl_int>(
					defaultStr, [](auto n) { return std::stoi(n); });
				if (numbers.size() != 1)
					throw BRDF::FormatException(
						"Default initializer for int must contain exactly one "
						"integer.");
				else
					p.value = numbers[0];
			}
			return BRDFParam(std::move(p));
		} else if (type == "float") {
			BRDFFloatParam p;
			auto numbers = toFloats(p, 1);
			p.value = numbers[0];
			return BRDFParam(std::move(p));
		} else if (type == "float2") {
			BRDFFloat2Param p;
			auto numbers = toFloats(p, 2);
			assert(numbers.size() == 2);
			p.value = cl_float2{numbers[0], numbers[1]};
			return BRDFParam(std::move(p));
		} else if (type == "float3") {
			BRDFFloat3Param p;
			auto numbers = toFloats(p, 3);
			assert(numbers.size() == 3);
			p.value = cl_float3{numbers[0], numbers[1], numbers[2]};
			p.isColor = color;
			return BRDFParam(std::move(p));
		} else if (type == "float4") {
			BRDFFloat4Param p;
			auto numbers = toFloats(p, 4);
			assert(numbers.size() == 4);
			p.value = cl_float4{numbers[0], numbers[1], numbers[2], numbers[3]};
			return BRDFParam(std::move(p));
		} else
			throw BRDF::FormatException("Unrecognized parameter type \"" +
										type + "\"");
	} catch (const std::invalid_argument &e) {
		throw BRDF::FormatException("Invalid parameter option: " +
									std::string(e.what()));
	} catch (const std::out_of_range &e) {
		throw BRDF::FormatException("Parameter option out of range: " +
									std::string(e.what()));
	}
}

BRDF::parameters_t
buildParameters(const std::string &params) {
	const std::regex paramRegex{
		R"(^[ \t]*(float|float[234]|int)[ \t]+([a-zA-Z_0-9]+);[ \t]*)"};
	std::istringstream stream(params);
	std::string line;
	std::smatch match;

	BRDF::parameters_t parameters;

	while (std::getline(stream, line)) {
		if (line.empty() || std::all_of(line.begin(), line.end(),
										[](char c) { return std::isalpha(c); }))
			continue;
		if (!std::regex_search(line, match, paramRegex))
			throw BRDF::FormatException("Syntax error: Invalid parameter");

		std::string typeStr = match[1].str();
		std::string name = match[2].str();
		std::string opt = match.suffix().str();
		bool isColor = opt.find("[color]") != std::string::npos;
		std::smatch optMatch;
		std::string minStr, maxStr, stepStr, defaultStr;
		if (std::regex_search(opt, optMatch,
							  std::regex(R"(\[min=([\-0-9\.f]+)\])")))
			minStr = optMatch[1].str();
		if (std::regex_search(opt, optMatch,
							  std::regex(R"(\[max=([\-0-9\.f]+)\])")))
			maxStr = optMatch[1].str();
		if (std::regex_search(opt, optMatch,
							  std::regex(R"(\[step=([\-0-9\.f]+)\])")))
			stepStr = optMatch[1].str();
		if (std::regex_search(
				opt, optMatch,
				std::regex(
					R"(\[default[ \t]*=[ \t]*([\-0-9\.f]+|(\([\-0-9\.f, ]+\)))\])")))
			defaultStr = optMatch[1].str();

		// TODO extract default= and set .cl
		auto &&[IT, inserted] = parameters.try_emplace(
			std::move(name),
			buildParam(typeStr, minStr, maxStr, stepStr, defaultStr, isColor));
		if (!inserted)
			throw BRDF::FormatException("Parameter with the name \"" + name +
										"\" already exits.");
	}
	return parameters;
}
std::string
genParamInitialization(const BRDF::parameters_t &params) {
	std::ostringstream output;
	std::size_t i = 0;
	for (auto &&[paramName, param] : params) {
		param.visit([&](auto &&p) mutable {
			using ParamT = std::decay_t<decltype(p)>;
			const char *typeStr, *valueStr;
			if constexpr (std::is_same_v<ParamT, BRDFIntParam>) {
				typeStr = "int";
				valueStr = "i";
			} else if constexpr (std::is_same_v<ParamT, BRDFFloatParam>) {
				typeStr = "float";
				valueStr = "f1";
			} else if constexpr (std::is_same_v<ParamT, BRDFFloat2Param>) {
				typeStr = "float2";
				valueStr = "f2";
			} else if constexpr (std::is_same_v<ParamT, BRDFFloat3Param>) {
				typeStr = "float3";
				valueStr = "f3";
			} else if constexpr (std::is_same_v<ParamT, BRDFFloat4Param>) {
				typeStr = "float4";
				valueStr = "f4";
			} else
				static_assert(false, "Does not cover all variant types.");

			output << typeStr << ' ' << paramName << '=';
			output << std::string("paramBuffer[") << i++ << ']';
			output << ".value." << valueStr << ";\n";
		});
	}
	return output.str();
}
} // namespace

BRDF::BRDF(std::string source, const std::string &prefix) {
	this->prefix = prefix + '_';
	changeSource(source);
}

const std::string &
BRDF::getDeclarations() const {
	return declaration;
}

bool
BRDF::hasCustomSampling() const {
	return !sampleBrdfFncName.empty();
}

const std::string &
BRDF::getBRDFFncName() const {
	return brdfFncName;
}

const std::string *
BRDF::getSampleBRDFFncName() const {
	return hasCustomSampling() ? &sampleBrdfFncName : nullptr;
}
// Returns Definitions for BRDF() and possibly SampleBRDF()
const std::string &
BRDF::getDefinitions() const {
	return definitions;
}

const std::string *
BRDF::getGetwInPDFFncName() const {
	return hasCustomSampling() ? &getWInPDFFncName : nullptr;
}

const std::string &
BRDF::getSource() const {
	return inputSource;
}

const std::string &
BRDF::getSource() {
	return inputSource;
}

namespace {
void
replace_all(std::string &str, const std::string &from, const std::string &to) {
	// https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
	std::size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos +=
			to.length(); // Handles case where 'to' is a substring of 'from'
	}
}
} // namespace
void
BRDF::changeSource(const std::string &newSource) {
	std::string source = inputSource = newSource;
	brdfFncName.clear();
	sampleBrdfFncName.clear();
	getWInPDFFncName.clear();
	declaration.clear();
	definitions.clear();
	std::smatch match;

	// IMPROVE Make more flexible
	//	- add prefix BRDF calls from SampleBRDF and other functions

	std::regex paramsRegex(R"(params[ \t\n]*\{([^}]*)\})");
	std::string paramInit;

	if (std::regex_search(source, match, paramsRegex) && match.size() == 2) {
		parameters = buildParameters(match[1].str());
		paramInit = genParamInitialization(parameters);
		source = match.suffix().str();
	} else
		parameters.clear();

	std::regex declRegex{R"(float3[ ]+()(BRDF\([^\)]+\)))"};
	if (!std::regex_search(source, match, declRegex) || match.size() != 3)
		throw FormatException(
			"BRDF's source does not contain declaration for the BRDF");
	else {
		declaration = "float3 " + prefix + match[2].str() + ";\n";
		auto pos = declaration.find(')');
		declaration.replace(pos, 1, ", __global Param* paramBuffer)");

		std::size_t declOff = match[1].first - source.begin();
		source.replace(match[1].first, match[1].second, prefix);
		brdfFncName = prefix + "BRDF";

		pos = source.find('{', declOff);
		source.replace(pos + 1, 0, '\n' + paramInit);
		pos = source.find(')', declOff);
		source.replace(pos, 1, ", __global Param* paramBuffer)");
	}

	std::regex sampleDeclRegex{R"(float3[ ]+()(SampleBRDF\([^\)]+\)))"};
	// Search for optional SampleBRDF function
	if (std::regex_search(source, match, sampleDeclRegex) &&
		match.size() == 3) {
		auto d = "float3 " + prefix + match[2].str() + ";\n";
		auto pos = d.find(')');
		d.replace(pos, 1, ", __global Param* paramBuffer)");
		declaration += d;

		std::size_t declOff = match[1].first - source.begin();
		source.replace(match[1].first, match[1].second, prefix);
		sampleBrdfFncName = prefix + "SampleBRDF";
		pos = source.find('{', declOff);
		source.replace(pos + 1, 0, '\n' + paramInit);
		pos = source.find(')', declOff);
		source.replace(pos, 1, ", __global Param* paramBuffer)");
	}

	std::regex getPDFDeclRegex{R"(float[ ]+()(GetwInPDF\([^\)]+\)))"};
	// Search for optional SampleBRDF function
	if (std::regex_search(source, match, getPDFDeclRegex) &&
		match.size() == 3) {
		if (!hasCustomSampling()) {
			throw FormatException("Since 'GetwInPDF' has been defined, 'SampleBRDF' must be too.");
		}
		auto d = "float " + prefix + match[2].str() + ";\n";
		auto pos = d.find(')');
		d.replace(pos, 1, ", __global Param* paramBuffer)");
		declaration += d;

		std::size_t declOff = match[1].first - source.begin();
		source.replace(match[1].first, match[1].second, prefix);
		getWInPDFFncName = prefix + "GetwInPDF";
		pos = source.find('{', declOff);
		source.replace(pos + 1, 0, '\n' + paramInit);
		pos = source.find(')', declOff);
		source.replace(pos, 1, ", __global Param* paramBuffer)");
	}
	else if (hasCustomSampling()) {
		throw FormatException("Since 'SampleBRDF' has been defined, 'GetwInPDF' must be too.");
	}

	// Replaces all calls
	replace_all(source, " BRDF ", getBRDFFncName());
	replace_all(source, " BRDF(", getBRDFFncName());
	if (hasCustomSampling()) {
		replace_all(source, " SampleBRDF ", *getSampleBRDFFncName());
		replace_all(source, " SampleBRDF(", *getSampleBRDFFncName());
		replace_all(source, " GetwInPDF ", *getGetwInPDFFncName());
		replace_all(source, " GetwInPDF(", *getGetwInPDFFncName());
	}
	definitions = std::move(source);
}

} // namespace brdfEditor::clRenderer
