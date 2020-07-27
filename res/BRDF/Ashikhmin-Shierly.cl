params{
    float3 specularCol; [color] [default=(1.0f,1.0f,1.0f)]
    float3 diffuseCol; [color] [default]=(0.8f,0.2f,0.1f)]
    float nu;
    float nv;
}

float3 BRDF(float3 n, float3 wIn, float3 wOut)
{
    float3 h = normalize(wIn+wOut);
    
    float nh= dot(n,h);
    float hwIn=dot(h,wIn);
    float nwIn=dot(n,wIn);
    float nwOut=dot(n,wOut);

    float3 u,v;
    genBasis(n,&u,&v);
    float hu = dot(h,u);
    float hv = dot(h,v);

    float s = (1-hwIn);
    float s2 = s*s;
    float3 F = specularCol + (1-specularCol)* s2*s2*s;
    float3 beta = (nu*hu*hu + nv*hv*hv )/ (1-nh*nh);
    float3 specular = sqrt((nu+1.0f)*(nv+1.0f))/(8*PI) * pow(nh,beta) * F/ (hwIn * max(nwIn,nwOut));

    float b = 1-nwIn/2.0f;
    float b2 = b*b;
    float c = 1-nwOut/2.0f;
    float c2 = c*c;

    float3 diffuse = 28.0f/23.0f / PI *(1-specularCol) * (1-b2*b2*b)*(1-c2*c2*c);
	return specular + diffuse;
}
