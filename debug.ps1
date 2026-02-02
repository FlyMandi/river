if(-Not(Test-Path ".\bin\Win64_Debug\River.exe") -And -Not(Test-Path ".\bin\Unix_Debug\River.exe"))
{
    Write-Host "no executable to debug."
    return;
}

if($IsWindows)
{
    #find out useful command line flags maybe?
    &raddbg
}
#add linux debugger in the future
