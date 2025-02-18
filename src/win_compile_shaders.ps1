$glslc = (Join-Path -PATH $env:VULKAN_SDK -ChildPath "\Bin\glslc.exe")
$shaders =  (Join-Path -PATH $PSScriptRoot -ChildPath "\shaders\")

if(-Not(Test-Path $shaders)){ &mkdir $shaders }

function Out-Shader{
    param(
        $fileName
    )
    $source = Join-Path -Path $shaders -ChildPath $fileName
    $target = "$source.spv"
    $param = $source, '-o', $target 
    
    &$glslc $param
    Write-Host "Compiled " -NoNewline
    Write-Host $fileName -BackgroundColor DarkCyan -NoNewline
    Write-Host " successfully to: $target"
}

Out-Shader simple_shader.vert
Out-Shader simple_shader.frag
