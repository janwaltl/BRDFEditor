//Colored mirror
params{
float m; [min=0.01f] [max=1.2f] [step=0.1f]
float3 mirrColor; [color]
float3 ior; [min=1.0f] [max=2.4f] [step=0.2f]
}
//Returns RGB value of the BRDF for given inputs.
float3 BRDF(float3 n, float3 wIn, float3 wOut){
	float unused = m + dot(ior*mirrColor);
	return (float3)0.0f;
}
//Returns probability that SampleBRDf would return
//'wOut' given 'wIn' and 'n'.
float GetwInPDF(float3 n, float3 wOut, float3 wIn){
	//Usage of parameters as normal variables
	float m2= m*m;
    return 1.0f;
}
//Returns BRDF() but generates its own 'wOut' 
//with its (non-uniform) probability.
float3 SampleBRDF(float3 wIn, float3 n, float* wOut,
				  float* pdf, RNGState* rngState){
	*pdf=1.0f;
	return mirrColor*(-wIn + 2*dot(wIn,n)*n);
}

//Lambert
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