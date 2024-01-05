# Create service
$local_path = (Get-Location).Path
sc.exe create Rootkit type= kernel start= demand error= normal binPath= $local_path\x64\Debug\Rootkit.sys DisplayName= Rootkit

# Start the service
sc.exe start Rootkit