param ( $config = "release" )
Push-Location ".\bin\$config"
&".\Cnake.exe"
Pop-Location
