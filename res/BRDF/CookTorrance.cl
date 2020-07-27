params{
float m; [min=0.01f] [max=1.2f] [step=0.1f]
float3 diffColor; [color]
float3 ior; [min=1.0f] [max=2.4f] [step=0.2f]
}

float3 BRDF(float3 n, float3 wOut, float3 wIn)
{
	
	float m2 = m*m;
	float3 h = normalize(wOut+wIn);
	float nh = dot(n,h);
	float nIn = dot(n,wIn);
	float nOut = dot(n,wOut);
	float hIn = dot(h,wIn);
	float hOut = dot(h,wOut);
	float nh2 = nh*nh;
	float nh4 = nh2 * nh2;
	//Beckmann
	float D = 1.0f/(PI*m2*nh4) * exp((nh2 - 1.0f)/(nh2*m2));
	//Fresnel, Shlick
	float3 F0 = (1.0f-ior)/(1.0f+ior);
	F0=F0*F0;
	float3 F = F0 + (1.0f - F0) * pow(1.0f - nIn,5.0f);
	//Geometry term
	float G = min(1.0f, min(2.0f*nh*nOut/hOut, 2.0f*nh*nIn/hOut));
	
	return (1.0f-F)*diffColor/PI+ F*G*D/(4*nIn*nOut);
}

float GetwInPDF(float3 n, float3 wOut, float3 wIn){
	float3 h = normalize(wOut+wIn);
	float m2= m*m;
	float nh = dot(n,h);
	float nh2 = nh*nh;
	float nh4 = nh2 * nh2;

    return 1.0f/(PI*m2*nh4) * exp((1.0f-1.0f/nh2)/m2) / dot(wOut,h) / 4.0f;
}
float3 SampleBRDF(float3 n, float3 wOut, float3* wIn, float* pdf, RNGState* rngState)
{
	n=normalize(n);
	float e2=uniformRNG(rngState);
	float e1=uniformRNG(rngState);
	float m2= m*m;

	float loge1 = log(e1);
	loge1 = isinf(loge1)? 0.0f : loge1;
	float tan2th = -m2*loge1;
	float costh2 = 1.0f / (1.0f+tan2th);
	float costh = sqrt(costh2);
	float sinth = sqrt(1.0f-costh2);
	float phi = TWOPI * e2;
	
	//Tangent space
	float3 tn1,tn2;
	genBasis(n,&tn1,&tn2);
	
	float3 tangentOff = cos(phi)*tn1 + sin(phi)*tn2;
	float3 h = normalize(sinth*tangentOff + costh*n);
	*wIn = normalize(-wOut + 2.0f*dot(h,wOut)*h);
	if(dot(*wIn,n)<=0.0f)
	{
		*pdf=0.0f;
		return (float3)0.0f;
	}

	float nh = dot(n,h);
	float nIn = dot(n,*wIn);
	float nOut = dot(n,wOut);
	float hIn = dot(h,*wIn);
	float hOut = dot(h,wOut);
	float nh2 = nh*nh;
	float nh4 = nh2 * nh2;
	//Beckmann
	float D = 1.0f/(PI*m2*nh4) * exp((nh2 - 1.0f)/(nh2*m2));
	*pdf = D/ dot(wOut,h) / 4.0f;
	//Fresnel, Shlick
	float3 F0 = (1.0f-ior)/(1.0f+ior);
	F0=F0*F0;
	float3 F = F0 + (1.0f - F0) * pow(1.0f - nIn,5.0f);
	//Geometry term
	float G = min(1.0f, min(nh*nOut/hOut, nh*nIn/hOut));
	
	return (1.0f-F)*diffColor/PI+ F*G*D/(4*nIn*nOut);
}
