$glslc = (Join-Path -PATH $env:VULKAN_SDK -ChildPath "\Bin\glslc.exe")

function Out-Shader{
    param(
        $fileName
    )
    $source = (Join-Path -PATH $PSScriptRoot -ChildPath "\shaders\$fileName")
    $target = "$source.spv"
    $param = $source, '-o', $target 
    
    &$glslc $param
    Write-Host "        Compiled " -NoNewline
    Write-Host $fileName -BackgroundColor DarkCyan -NoNewline
    Write-Host " successfully to: $target"
}

Out-Shader simple_shader.vert
Out-Shader simple_shader.frag
