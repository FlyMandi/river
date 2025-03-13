$glslc = (Join-Path -PATH $env:VULKAN_SDK -ChildPath "\Bin\glslc.exe")
$src = Join-Path $PSScriptRoot "\src\"
$shaders = Join-Path $src "\shaders\" 
$bin = Join-Path $PSScriptRoot "\bin\" 

function Out-Shader{
    param(
        $fileName
    )
    $baseName = $fileName.baseName
    $target = Join-Path $bin "$baseName.spv"
    $param = $fileName, '-o', $target 
    
    &$glslc $param
    Write-Host "Compiled " -NoNewline
    Write-Host $fileName -ForegroundColor DarkCyan -NoNewline
    Write-Host " successfully to: $target"
}

foreach($shader in (Get-ChildItem -File $shaders)){
    Out-Shader $shader
}
