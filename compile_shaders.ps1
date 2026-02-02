$glslc = (Join-Path -PATH $env:VULKAN_SDK -ChildPath "\Bin\glslc.exe")
$src = Join-Path $PSScriptRoot "\src\"
$shaders = Join-Path $src "\shaders\" 
$bin = Join-Path $PSScriptRoot "\bin\" 

function Out-Shader{
    param(
        $fileName
    )
    $source = Join-Path -Path $shaders -ChildPath $fileName
    $target = Join-Path $bin "$fileName.spv"
    $param = $source, '-o', $target 
    
    &$glslc $param
    Write-Host "Compiled " -NoNewline
    Write-Host $fileName -BackgroundColor DarkCyan -NoNewline
    Write-Host " successfully to: $target"
}

Out-Shader simple_shader.vert
Out-Shader simple_shader.frag
