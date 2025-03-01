Push-Location $PSScriptRoot
Get-ChildItem -Path ../Src -Include ('*.cpp','*.h','*.hpp') -File -Recurse -Exclude external | 
    Where-Object {$_.DirectoryName -notlike '*ThirdParty*'} |
    ForEach-Object -Parallel {
        & clang-format -i $_.FullName
    }
Pop-Location
