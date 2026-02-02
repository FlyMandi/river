param( $config = "release" )
&premake5 clean
&premake5 vs2022
.\src\win_compile_shaders.ps1
&"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" .\build\Cnake.sln -t:Rebuild -p:Configuration=$config
&".\bin\$config\Cnake.exe"
