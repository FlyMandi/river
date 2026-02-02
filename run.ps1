param( 
    [Parameter(position=0,Mandatory=$false)]
    $OS = "Win64",
    [Parameter(position=1,Mandatory=$false)]
    $build = "Debug",
    [Parameter(position=2,Mandatory=$false)]
    [switch]$nAMD = $false 
)
$Platforms = "Win64", "Unix"
$Configurations = "Debug", "Release"

foreach($platform in $Platforms){
    foreach($config in $Configurations){
        $objPath = ".\obj\$platform" + "_$config"
        $binPath = ".\bin\$platform" + "_$config"
        if(-Not(Test-Path $objPath)){
            &mkdir $objPath
        }
        if(-Not(Test-Path $binPath)){
            &mkdir $binPath
        }
    }
}

&premake5 vs2022
&premake5 ecc
.\shader_comp.ps1

$VS = Join-Path $env:PROGRAMFILES "\Microsoft Visual Studio\2022\Community\"

if($nAMD){ 
    $MSBuild = Join-Path $VS "\MSBuild\Current\bin\" 
}else{ 
    $MSBuild = Join-Path $VS "\MSBuild\Current\bin\amd64\" 
}

&"$MSBuild\MSBuild.exe" .\build\River.sln -p:Configuration=$build

$target = ".\bin\$OS" + "_$build\River.exe"

&$target
