#include "ReShade.fxh"
uniform float Sharpness < ui_type = "slider"; ui_min = 0.0; ui_max = 0.6; > = 0.18;
uniform float Saturation < ui_type = "slider"; ui_min = 0.8; ui_max = 1.2; > = 1.03;
uniform float Contrast < ui_type = "slider"; ui_min = 0.9; ui_max = 1.15; > = 1.02;
float4 LMF_PS(float4 pos : SV_Position, float2 uv : TEXCOORD) : SV_Target {
 float3 c = tex2D(ReShade::BackBuffer,uv).rgb;
 float2 px = BUFFER_PIXEL_SIZE;
 float3 avg = (tex2D(ReShade::BackBuffer,uv+float2(px.x,0)).rgb + tex2D(ReShade::BackBuffer,uv-float2(px.x,0)).rgb + tex2D(ReShade::BackBuffer,uv+float2(0,px.y)).rgb + tex2D(ReShade::BackBuffer,uv-float2(0,px.y)).rgb)*0.25;
 float3 color = saturate(c+(c-avg)*Sharpness);
 color=lerp(dot(color,float3(0.2126,0.7152,0.0722)),color,Saturation);
 return float4(saturate((color-0.5)*Contrast+0.5),1);
}
technique LMF_Subtle { pass { VertexShader = PostProcessVS; PixelShader = LMF_PS; } }
