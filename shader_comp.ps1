$glslc = (Join-Path -PATH $env:VULKAN_SDK -ChildPath "\bin\glslc.exe")
$src = Join-Path $PSScriptRoot "\src\"
$shaders = Join-Path $src "\shaders\"
$bin = Join-Path $PSScriptRoot "\bin\"

function Out-Shader{
    param(
        $fileName
    )

    if(-Not(Test-Path $bin)){
        &mkdir $bin
    }

    $baseName = $fileName.baseName
    $ext = $fileName.extension
    $target = Join-Path $bin "$baseName$ext.spv"
    $param = $fileName, '-o', $target

    if(Test-Path $target)
    {
        Remove-Item $target
    }

    &$glslc $param

    if(-not (Test-Path $target))
    {
        Write-Host "ERROR: failed to compile $baseName" -ForegroundColor Red
        exit 99;
    }

    Write-Host "Compiled " -NoNewline
    Write-Host $fileName -ForegroundColor DarkCyan -NoNewline
    Write-Host " successfully to: $target"
}

foreach($shader in (Get-ChildItem -File $shaders))
{
    Out-Shader $shader
}
