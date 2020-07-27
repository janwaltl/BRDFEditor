params{
	float roughness; [default=64.0f]
}

float3 BRDF(float3 n, float3 wIn, float3 wOut)
{
	//Those would be the parameters of this BRDF
	float3 diffColor = (float3)(1.0f);//Colors of the surface
	float3 specColor = (float3)(1.0f);//Should be the light's color, that means white in BRDF evaluation
	
	float3 diffuse = 1.0f/PI;
	float3 halfVec = normalize(wIn+wOut);
	float3 spec = pow(dot(halfVec,n),roughness);
	return diffuse+spec;
}
