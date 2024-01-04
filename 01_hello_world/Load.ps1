# Create service
$local_path = (Get-Location).Path
sc.exe create HelloWorld type= kernel start= demand error= normal binPath= $local_path\x64\Debug\HelloWorld.sys DisplayName= HelloWorld

# Start the service
sc.exe start HelloWorld