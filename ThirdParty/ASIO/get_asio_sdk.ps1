$asioSdkSource = "https://download.steinberg.net/sdk_downloads/asiosdk2.3.zip"
$asioSdkDestination = ".\asiosdk2.3.zip"

[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
Invoke-WebRequest $asioSdkSource -OutFile $asioSdkDestination

Expand-Archive $asioSdkDestination
Remove-Item $asioSdkDestination

Move-Item .\asiosdk2.3\ASIOSDK2.3\* .\ -Force
Remove-Item .\asiosdk2.3 -Recurse
