#TODO: install VulkanSDK if not present
#curl -O https://sdk.lunarg.com/sdk/download/latest/windows/vulkan_sdk.exe

if(-Not(Get-Command premake5.exe -ErrorAction SilentlyContinue)){
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
