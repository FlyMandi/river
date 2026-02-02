param(
    [Parameter(position=0,Mandatory=$false)]
    $build = "Debug",
    [Parameter(position=1,Mandatory=$false)]
    $OS = "Win64",
    [Parameter(position=2,Mandatory=$false)]
    $compiler = "",
    [Parameter(position=3,Mandatory=$false)]
    [switch]$nAMD = $false
)
if($compiler -eq "" -and $IsWindows)
{
    $compiler = "MSVC"
}
elseIf($compiler -eq "" -and $IsLinux)
{
    $compiler = "g++"
}

$Platforms = "Win64", "Unix"
$Configurations = "Debug", "Release"

foreach($platform in $Platforms)
{
    foreach($config in $Configurations)
    {
        $objPath = ".\obj\$platform" + "_$config"
        $binPath = ".\bin\$platform" + "_$config"

        if(-Not(Test-Path $objPath))
        {
            &mkdir $objPath
        }

        if(-Not(Test-Path $binPath))
        {
            &mkdir $binPath
        }
    }
}

if(-Not(Test-Path ".\build\"))
{
    &mkdir ".\build\"
}

&premake5 ecc

$target = ".\bin\$OS" + "_$build\River.exe"

$sourceFiles = Get-ChildItem ".\src\" -File
$sourceFiles += Get-ChildItem ".\include\" -File

foreach($file in $sourceFiles)
{
    $sourceFilePaths += " "
    $sourceFilePaths += $file.FullName
}
#TODO: flags
#TODO: switch over to the linux version of the Vulkan SDK and the MSVC C++ stl, rid myself of MSVC

if("MSVC" -eq $compiler)
{
    &premake5 vs2022

    $VS = Join-Path $env:PROGRAMFILES "\Microsoft Visual Studio\2022\Community\"

    if($nAMD)
    {
        $MSBuild = Join-Path $VS "\MSBuild\Current\bin\"
    }
    else
    {
        $MSBuild = Join-Path $VS "\MSBuild\Current\bin\amd64\"
    }

    &"$MSBuild\MSBuild.exe" .\build\River.sln -p:Configuration=$build

}
elseIf("clang" -eq $compiler)
{
    #it don't want
    &clang $sourceFilePaths
}
elseIf("g++" -eq $compiler)
{
    &premake5 gmake

    Push-Location ".\build\"
    &make
    Pop-Location
}

if(0 -eq $LASTEXITCODE)
{
    Write-Host "`ncompiled successfully!" -ForegroundColor Green
    Write-Host "compiling shaders..."
    .\shader_comp.ps1
}

if(0 -eq $LASTEXITCODE)
{
    Write-Host "running $target..."
    &$target
}
