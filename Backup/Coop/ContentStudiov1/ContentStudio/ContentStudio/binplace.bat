ECHO Copying Effects, PixelShaders, and VertexShaders to output directory...
md ..\Debug
md ..\Release
copy res\BasicEffect.hlsl.vs ..\Debug\BasicEffect.hlsl.vs
copy res\BasicEffect.hlsl.vs ..\Release\BasicEffect.hlsl.vs
copy res\BasicEffect.hlsl.ps ..\Debug\BasicEffect.hlsl.ps
copy res\BasicEffect.hlsl.ps ..\Release\BasicEffect.hlsl.ps
copy res\BasicEffects.hlsl   ..\Debug\BasicEffects.hlsl
copy res\BasicEffects.hlsl   ..\Release\BasicEffects.hlsl
copy res\concrete.tc ..\Debug\concrete.tc
copy res\concrete.tc ..\Release\concrete.tc
copy res\proto_bricks_n.tc ..\Debug\proto_bricks_n.tc
copy res\proto_bricks_n.tc ..\Release\proto_bricks_n.tc
md ..\Debug\UI
md ..\Release\UI
copy res\font.dds ..\Debug\UI
copy res\font.dds ..\Release\UI
copy res\dxutcontrols.dds ..\Debug\UI
copy res\dxutcontrols.dds ..\Release\UI
copy res\DXUTShared.fx ..\Debug\UI
copy res\DXUTShared.fx ..\Release\UI
