[System.Version]$installVersion = "1.4.309.0"

# TODO: write bash version of setup script and cleanup this mess

function Install-VulkanSDK
{
    Write-Host "Installing Vulkan SDK version $installVersion`:`n"

    if($IsWindows)
    {
        &curl -O https://sdk.lunarg.com/sdk/download/$installVersion/windows/VulkanSDK-$installVersion-Installer.exe
        Write-Host "`nExpect a UAC prompt."
        &.\VulkanSDK-$installVersion-Installer.exe
        Write-Host "`nMake sure the vulkan SDK is added under the VULKAN_SDK environment variable."
    }
    elseif($isLinux)
    {
        &curl -O https://sdk.lunarg.com/sdk/download/$installVersion/linux/vulkansdk-linux-x86_64-$installVersion.tar.xz
        Write-Host "`nVulkanSDK version $installVersion has been downloaded, but not installed."
        Write-Host "Now install the SDK the rest of the way."
        Write-Host "`nIf you need help, read the article below:"
        Write-Host "https://www.amd.com/en/resources/support-articles/faqs/GPU-636.html"

    }
    elseif($IsMacOS)
    {
        &curl -O https://sdk.lunarg.com/sdk/download/$installVersion/mac/vulkansdk-macos-$installVersion.zip
    }

}

if([string]::IsNullOrEmpty($env:VULKAN_SDK))
{
    Install-VulkanSDK
}
elseIf(Test-Path $env:VULKAN_SDK)
{
    $vkVersion = [System.Version](Get-Item $env:VULKAN_SDK).Name

    if($vkVersion -lt $installVersion)
    {
        Write-Host "Found older Vulkan SDK installation, version $vkVersion"
        Write-Host "Do you want to install version $installVersion now? (y/n)"

        $answer = Read-Host
        if($answer -eq "y" -or $input -eq "yes")
        {
            Install-VulkanSDK
        }
        else
        {
            Write-Host "not updating automatically, please do so manually."
        }
    }
    else
    {
        Write-Host "Found valid Vulkan SDK installation, version $vkVersion"
    }
}
else
{
    throw "ERROR: the vulkan SDK environment variable ($env:VULKAN_SDK) does not point to a valid directory."
}

if(-Not(Get-Command premake5 -ErrorAction SilentlyContinue))
{
    Write-Host "Could not find premake5."
    Write-Host "If you wish to utilize run.ps1, you will have to have it installed and added to PATH."
    Write-Host ""
}
else
{
    Write-Host "Found premake5."
}

if($isWindows -and -Not(Get-Command MSBuild -ErrorAction SilentlyContinue))
{
    Write-Host "Could not find MSBuild."
    Write-Host "If you wish to utilize run.ps1, you will have to have it installed and added to PATH."
    Write-Host ""
}
else
{
    Write-Host "Found MSBuild."
}
