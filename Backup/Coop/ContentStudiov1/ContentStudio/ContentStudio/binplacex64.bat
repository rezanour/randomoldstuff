ECHO Copying Effects, PixelShaders, and VertexShaders to output directory...
md ..\x64\Debug
md ..\x64\Release
copy res\BasicEffect.hlsl.vs ..\x64\Debug\BasicEffect.hlsl.vs
copy res\BasicEffect.hlsl.vs ..\x64\Release\BasicEffect.hlsl.vs
copy res\BasicEffect.hlsl.ps ..\x64\Debug\BasicEffect.hlsl.ps
copy res\BasicEffect.hlsl.ps ..\x64\Release\BasicEffect.hlsl.ps
copy res\BasicEffects.hlsl   ..\x64\Debug\BasicEffects.hlsl
copy res\BasicEffects.hlsl   ..\x64\Release\BasicEffects.hlsl
copy res\concrete.tc ..\x64\Debug\concrete.tc
copy res\concrete.tc ..\x64\Release\concrete.tc
copy res\proto_bricks_n.tc ..\x64\Debug\proto_bricks_n.tc
copy res\proto_bricks_n.tc ..\x64\Release\proto_bricks_n.tc
md ..\x64\Debug\UI
md ..\x64\Release\UI
copy res\font.dds ..\x64\Debug\UI
copy res\font.dds ..\x64\Release\UI
copy res\dxutcontrols.dds ..\x64\Debug\UI
copy res\dxutcontrols.dds ..\x64\Release\UI
copy res\DXUTShared.fx ..\x64\Debug\UI
copy res\DXUTShared.fx ..\x64\Release\UI
