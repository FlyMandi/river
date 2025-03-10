param( $config = "debug" )
&premake5 vs2022
.\src\win_compile_shaders.ps1
Push-Location ".\build\"
&"$env:ProgramFiles\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" .\Cnake.sln -t:Rebuild -p:Configuration=$config
Pop-Location
