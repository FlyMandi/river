param( $build = "debug" )
&premake5 vs2022
&"$env:ProgramFiles\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" .\build\Cnake.sln -t:Rebuild -p:Configuration=$build
.\win_compile_shaders.ps1
