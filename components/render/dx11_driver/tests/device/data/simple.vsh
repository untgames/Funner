#pragma pack_matrix (row_major)

struct VS_INPUT
{
  float4 Position   : POSITION;
};

struct VS_OUTPUT
{
  float4 Position   : POSITION;
};

cbuffer MainBuffer: register(b0)
{
  float4x4 WorldViewProj : packoffset (c0);
}

VS_OUTPUT main (in VS_INPUT In)
{
  VS_OUTPUT Out;

  Out.Position = mul(In.Position, WorldViewProj);

  return Out;
}
