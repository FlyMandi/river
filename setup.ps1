if(-Not(Get-Command premake5 -ErrorAction SilentlyContinue)){
   if(Get-Command scoop -ErrorAction SilentlyContinue){ 
        &scoop install premake
   }else{
        $sourceRepo = "premake/premake-core"
        $namePattern = "*windows.zip"
        $sourceURI = ((Invoke-RestMethod -Method GET -Uri "https://api.github.com/repos/$sourceRepo/releases/latest").assets | Where-Object name -like $namePattern).browser_download_url
        $zipFolderName = $(Split-Path -Path $sourceURI -Leaf)
        $tempZIP = Join-Path -Path $([System.IO.Path]::GetTempPath()) -ChildPath $zipFolderName 
        Invoke-WebRequest -Uri $sourceURI -Out $tempZIP

        Expand-Archive -Path $tempZIP -DestinationPath $PSScriptRoot -Force
        Remove-Item $tempZIP -Force
   }
}

[System.Version]$installVersion = "1.4.309.0"

function Install-VulkanSDK{
    Write-Host "Installing Vulkan SDK version $installVersion`:`n"

    if($IsWindows){ 
        &curl -O https://sdk.lunarg.com/sdk/download/$installVersion/windows/VulkanSDK-$installVersion-Installer.exe
        Write-Host "`nExpect a UAC prompt." 
        &.\VulkanSDK-$installVersion-Installer.exe
    }elseif($isLinux){
        if(Get-Command apt-get -ErrorAction SilentlyContinue){
            Write-Host "Expect a sudo prompt. If you don't like that or it fails, manually install via:"
            Write-Host "`nsudo apt-get install vulkan-sdk=$installVersion"
            &sudo apt-get install vulkan-sdk=$installVersion
        }else{
            Write-Host "You don't have apt-get, so install via the package manager of your choice or proceed below."
            &curl -O https://sdk.lunarg.com/sdk/download/$installVersion/linux/vulkansdk-linux-x86_64-$installVersion.tar.xz
            Write-Host "`nVulkanSDK version $installVersion has been downloaded, but not installed."
            Write-Host "Now install the SDK the rest of the way." 
            Write-Host "`nIf you need help, read the article below:"
            Write-Host "https://www.amd.com/en/resources/support-articles/faqs/GPU-636.html"
        }

    }elseif($IsMacOS){
        #TODO: add mac install steps... eh.
        &curl -O https://sdk.lunarg.com/sdk/download/$installVersion/mac/vulkansdk-macos-$installVersion.zip
    }

}

if([string]::IsNullOrEmpty($env:VULKAN_SDK)){
    Install-VulkanSDK
}elseIf(Test-Path $env:VULKAN_SDK){
    $vkVersion = [System.Version](Get-Item $env:VULKAN_SDK).Name

    if($vkVersion -lt $installVersion) {
        Write-Host "Found older Vulkan SDK installation, version $vkVersion"
        Install-VulkanSDK
    }else{
        Write-Host "Found valid Vulkan SDK installation, version $vkVersion"
    }
}else{
    throw "ERROR: $env:VULKAN_SDK does not point to a valid directory."
}

