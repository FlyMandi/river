$Platforms = "Win64", "Unix"
$Configurations = "Debug", "Release"

foreach($platform in $Platforms){
    foreach($config in $Configurations){
        $objPath = ".\obj\$platform" + "_$config"
        $binPath = ".\bin\$platform" + "_$config"
        if(-Not(Test-Path $objPath)){
            &mkdir $objPath
        }
        if(-Not(Test-Path $binPath)){
            &mkdir $binPath
        }
    }
}

&premake5 gmake
&premake5 ecc
Push-Location ".\build\"
&make
Pop-Location
