# Create service
$local_path = (Get-Location).Path
sc.exe create ProcTrace type= kernel start= demand error= normal binPath= $local_path\x64\Debug\ProcTrace.sys DisplayName= ProcTrace

# Start the service
sc.exe start ProcTrace