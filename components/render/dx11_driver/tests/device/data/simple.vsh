struct VS_INPUT
{
  float4 Position   : POSITION;
  float2 Texture    : TEXCOORD0;
};

struct VS_OUTPUT
{
  float4 Position   : POSITION;
  float2 Texture    : TEXCOORD0;
};

cbuffer MainBuffer: register(b0)
{
  float4x4 WorldViewProj : packoffset (c0);
}

VS_OUTPUT main (in VS_INPUT In)
{
  VS_OUTPUT Out;

  Out.Position = mul(In.Position, WorldViewProj);
  Out.Texture  = In.Texture;

  return Out;
}
