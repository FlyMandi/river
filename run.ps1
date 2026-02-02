param( 
    [Parameter(position=0,Mandatory=$false)]
    $build = "Debug",
    [Parameter(position=1,Mandatory=$false)]
    $OS = "Win64",
    [Parameter(position=2,Mandatory=$false)]
    [switch]$nAMD = $false,
    [Parameter(position=3,Mandatory=$false)]
    $compiler = "MSVC"
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

&premake5 ecc
.\shader_comp.ps1
$target = ".\bin\$OS" + "_$build\River.exe"
Remove-Item $target

if("MSVC" -eq $compiler){
    &premake5 vs2022

    $VS = Join-Path $env:PROGRAMFILES "\Microsoft Visual Studio\2022\Community\"

    if($nAMD){ 
        $MSBuild = Join-Path $VS "\MSBuild\Current\bin\" 
    }else{ 
        $MSBuild = Join-Path $VS "\MSBuild\Current\bin\amd64\" 
    }

    &"$MSBuild\MSBuild.exe" .\build\River.sln -p:Configuration=$build

}elseIf("g++" -eq $compiler){
    &premake5 gmake

    Push-Location ".\build\"
    &make
    Pop-Location
}

&$target
