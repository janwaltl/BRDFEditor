params{
float3 color; [color]
float roughness;[min=0.0f][default=1.0f]
}

float3 BRDF(float3 n, float3 wIn, float3 wOut)
{
	float K = max(0.0f,dot(n,wIn));
	float V = max(0.0f,dot(n,wOut));
	float P = max(0.0f,dot(normalize(wIn - K*n),normalize(wOut - V*n)));
	float sig = roughness*roughness;
	float A = 1.0f- 0.5f* sig/(sig + 0.33f);
	float B = 0.45f*sig/(sig + 0.09f);
	return color / PI * (A + P*B*sqrt((1-V*V)*(1-K*K))/max(K,V));
}