#pragma pack_matrix (row_major)

struct VS_INPUT
{
  float4 Position : POSITION;
};

struct VS_OUTPUT
{
  float4 Position : SV_POSITION;
  float4 Color    : COLOR;
};

cbuffer MainBuffer: register(b0)
{
  float4x4 myProjMatrix;
  float4x4 myViewMatrix;
  float4x4 myObjectMatrix;
  float3   lightPos;  
  float3   lightDir;
}

VS_OUTPUT main (in VS_INPUT In)
{
  VS_OUTPUT Out;

//  Out.Position = mul (In.Position, myProjMatrix * myViewMatrix * myObjectMatrix);
//  Out.Position = mul (myProjMatrix * myViewMatrix * myObjectMatrix, In.Position);
//  Out.Position = mul (myObjectMatrix * myViewMatrix * myProjMatrix, In.Position);
  Out.Position = In.Position;
  Out.Color    = float4 (1.0, 1.0, 1.0, 1.0);

  return Out;
}
