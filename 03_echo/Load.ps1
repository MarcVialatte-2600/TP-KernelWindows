# Create service
$local_path = (Get-Location).Path
sc.exe create Echo type= kernel start= demand error= normal binPath= $local_path\x64\Debug\Echo.sys DisplayName= Echo

# Start the service
sc.exe start Echo