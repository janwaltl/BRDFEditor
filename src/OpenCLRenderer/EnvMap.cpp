#include "EnvMap.hpp"

#include <algorithm>
#include <map>
#include <memory>
#include <random>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <src/Core/Exception.hpp>

#include <chrono>
#include <iostream>
#include <numeric>
#include <cassert>
#include <set>

namespace brdfEditor::clRenderer {

namespace {
float
rgbToBrightness(const cl_float4 &pixel) {
	return 0.2126 * pixel.x + 0.7152 * pixel.y + 0.0722 * pixel.z;
}

// From
// https://stackoverflow.com/questions/10330002/sum-of-small-double-numbers-c
struct KahanAccumulation {
	double sum = 0.0;
	double correction = 0.0;

	operator double() { return sum; }
};

KahanAccumulation
KahanSum(KahanAccumulation accumulation, double value) {
	KahanAccumulation result;
	double y = value - accumulation.correction;
	double t = accumulation.sum + y;
	result.correction = (t - accumulation.sum) - y;
	result.sum = t;
	return result;
}

struct H {
	std::vector<double> k;
	std::vector<double> v;
};
H
buildSquareHistogram(const std::vector<double> &pdf) {

	double avg = 1.0 / pdf.size();

	std::vector<double> hist{pdf.begin(), pdf.end()};

	std::vector<double> k(pdf.size());
	std::vector<double> v(pdf.size());
	for (std::size_t i = 0; i < pdf.size(); ++i) {
		k[i] = i;
		v[i] = (i + 1) * avg;
	}
	for (std::size_t i = 1; i <= pdf.size() - 1; ++i) {
		const auto [minIT, maxIT] =
			std::minmax_element(hist.begin(), hist.end());
		std::size_t min = minIT - hist.begin();
		std::size_t max = maxIT - hist.begin();

		k[min] = max;
		double shiftAmount = (avg - hist[min]);
		v[min] = min * avg + hist[min];
		// v[min] = shiftAmount / avg;
		hist[max] -= shiftAmount;
		hist[min] = avg;
	}
	return H{k, v};
}

using Pdf = std::vector<double>;

struct ProcessedMap {
	std::pair<H, Pdf> marginal;
	std::vector<std::pair<H, Pdf>> rows;
	std::vector<cl_float4> values;
};
ProcessedMap
processMap(std::vector<cl_float4> &&values, std::size_t width,
		   std::size_t height) {

	// H margHist = buildSquareHistogram(mColPDF);

	Pdf marginalRowPdf(height);
	std::vector<std::pair<H, Pdf>> rows;
	KahanAccumulation marginalAcc{};
	for (std::size_t row = 0; row < height; ++row) {
		Pdf rowPDF(width);
		KahanAccumulation rowAcc{};
		for (std::size_t i = 0; i < width; ++i) {
			rowPDF[i] = rgbToBrightness(values[i + row * width]);
			rowAcc = KahanSum(rowAcc, rowPDF[i]);
		}
		marginalRowPdf[row] = rowAcc.sum;
		marginalAcc = KahanSum(marginalAcc, rowAcc.sum);
		// Normalize PDF
		for (std::size_t i = 0; i < width; ++i)
			rowPDF[i] /= rowAcc.sum;
		auto &&rowH = buildSquareHistogram(rowPDF);
		rows.emplace_back(rowH, rowPDF);
	}
	for (std::size_t i = 0; i < height; ++i)
		marginalRowPdf[i] /= marginalAcc.sum;

	std::pair<H, Pdf> marginal{buildSquareHistogram(marginalRowPdf),
							   marginalRowPdf};
	// file:///D:/Stahov%C3%A1n%C3%AD/v11i03.pdf
	/* TESTING
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	auto pick = [&dis, &gen](std::pair<H, Pdf> &d) {
		std::size_t N = d.second.size();
		double u = dis(gen);
		double v = dis(gen);
		std::size_t j = u * N;
		std::size_t picked = v < d.first.v[j] ?  d.first.k[j]: j;
		return std::make_pair(picked, d.second[picked]);
	};
	auto pickOrig = [&dis, &gen](std::pair<H, Pdf> &d) {
		std::size_t N = d.second.size();
		double u = dis(gen);
		std::size_t j = u * N;
		std::size_t picked = u < d.first.v[j] ?  j : d.first.k[j];
		return std::make_pair(picked, d.second[picked]);
	};

	auto counters = std::vector<float>(width * height);
	using namespace std::chrono;
	std::cout << "generating\n";
	auto now = std::chrono::high_resolution_clock::now();
	std::size_t N = 100'000'000;
	for (std::size_t i = 0; i < N; ++i) {
		auto [row, pdfRow] = pickOrig(marginal);

		auto [col, pdfColRow] = pickOrig(rows[row]);
		double pxy = pdfRow * pdfColRow;
		counters[row * width + col]+=1;
	}

	stbi_write_hdr("another_test_pixels.hdr", width, height, 1,
				   counters.data());
	std::terminate();*/
	return ProcessedMap{std::move(marginal), std::move(rows),
						std::move(values)};
}

EnvMap_cl
processedToCL(ProcessedMap &&processedMap) {
	EnvMap_cl clMap;
	clMap.envMap = std::move(processedMap.values);
	clMap.height = processedMap.marginal.second.size();
	clMap.width = processedMap.rows[0].second.size();
	clMap.envMarginalRowHist.resize(clMap.height);
	for (std::size_t i = 0; i < clMap.envMarginalRowHist.size(); ++i) {
		clMap.envMarginalRowHist[i] = cl_float4{
			static_cast<float>(processedMap.marginal.first.k[i]),
			static_cast<float>(processedMap.marginal.first.v[i]),
			static_cast<float>(processedMap.marginal.second[i]),
		};
	}

	clMap.envRowHists.resize(clMap.width * clMap.height);
	for (std::size_t y = 0; y < clMap.height; ++y)
		for (std::size_t x = 0; x < clMap.width; ++x)
			clMap.envRowHists[y * clMap.width + x] = cl_float4{
				static_cast<float>(processedMap.rows[y].first.k[x]),
				static_cast<float>(processedMap.rows[y].first.v[x]),
				static_cast<float>(processedMap.rows[y].second[x]),
			};
	return clMap;
}
} // namespace
EnvMap::EnvMap(const std::string &filename,float intensity):
intensity(intensity){
	int numChannels;
	int iWidth, iHeight;
	std::unique_ptr<float, void (*)(void *)> loaded_pixels(
		stbi_loadf(filename.c_str(), &iWidth, &iHeight, &numChannels, 4),
		stbi_image_free);
	if (!loaded_pixels)
		throw Exception("Could not load a HDR map from '" + filename + "'.");

	auto width = static_cast<std::size_t>(iWidth);
	auto height = static_cast<std::size_t>(iHeight);
	std::vector<cl_float4> values(width * height * 4);
	memcpy(values.data(), loaded_pixels.get(), sizeof(float) * values.size());

	clMap = processedToCL(processMap(std::move(values), width, height));
}
EnvMap_cl &
EnvMap::getCL() {
	return clMap;
}
const EnvMap_cl &
EnvMap::getCL() const {
	return clMap;
}
EnvMap
EnvMap::makeBlack(std::size_t width, std::size_t height) {

	EnvMap_cl clMap;

	std::vector<cl_float4> values(width * height * 4,
								  cl_float4{0.0f, 0.0f, 0.0f, 0.0f});
	clMap = processedToCL(processMap(std::move(values), width, height));

	return EnvMap(std::move(clMap),0.0f);
}
EnvMap::EnvMap(EnvMap_cl &&clMap,float intensity) : clMap(clMap),intensity(intensity) {}
} // namespace brdfEditor::clRenderer