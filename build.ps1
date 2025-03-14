param( 
    [Parameter(position=0,Mandatory=$false)]
    $build = "release",
    [Parameter(position=1,Mandatory=$false)]
    [switch] $nAMD = $false 
)

&premake5 vs2022

$VS = Join-Path $env:PROGRAMFILES "\Microsoft Visual Studio\2022\Community\"

if($nAMD){ $MSBuild = Join-Path $VS "\MSBuild\Current\bin\" }
else{ $MSBuild = Join-Path $VS "\MSBuild\Current\bin\amd64\" }

&"$MSBuild\MSBuild.exe" .\build\Cnake.sln -p:Configuration=$build
.\shader_comp.ps1
