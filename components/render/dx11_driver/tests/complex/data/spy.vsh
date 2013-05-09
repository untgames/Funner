#pragma pack_matrix (row_major)

struct VS_INPUT
{
  float4 Position : POSITION;
  float3 Normal   : NORMAL;
};

struct VS_OUTPUT
{
  float4 Position : SV_POSITION;
  float4 Color    : COLOR;
};

cbuffer MainBuffer: register(b0)
{
  float4x4 myObjectMatrix;
  float4x4 myViewMatrix;
  float4x4 myProjMatrix;
  float3   lightPos;  
};

VS_OUTPUT main (in VS_INPUT In)
{
  VS_OUTPUT Out;

  float4x4 wvp = mul (myProjMatrix, mul (myViewMatrix, myObjectMatrix));

  Out.Position = mul (wvp, In.Position);

  float3 worldPos       = mul (myObjectMatrix, In.Position).xyz;
  float3 lightDir       = normalize (lightPos - worldPos);
  float3 normal         = normalize (mul (myObjectMatrix, float4 (In.Normal, 0.0)).xyz);
  float  diffuseFactor  = clamp (dot (normal, lightDir), 0.0, 1.0);

  const float4 fvDiffuse = float4 (1.0, 0.0, 0.0, 1.0);

  Out.Color = fvDiffuse * diffuseFactor;

  return Out;
}
