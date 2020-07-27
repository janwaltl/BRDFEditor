params{
float3 color;[default=(1.0f,0.5f,1.0f)] [color]
}

float3 BRDF(float3 n, float3 wIn, float3 wOut)
{
	return (float3)(color) / PI;
}
float3 SampleBRDF(float3 n, float3 wIn, float3* wOut, float* pdf, RNGState* rngState)
{
	*wOut=genCosineDir(wIn,n,rngState,pdf);
	return (float3)(color) / PI;
}
float GetwInPDF(float3 normal, float3 wOut, float3 wIn){
	return dot(normal,wIn)/PI;
}