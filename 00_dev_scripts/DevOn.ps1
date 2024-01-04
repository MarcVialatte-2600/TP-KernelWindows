bcdedit /set testsigning on
bcdedit /set nointegritychecks on
bcdedit /set debug on

if ($?) {
    Write-Host "Success! Reboot to enable everything."
} else {
    Write-Host "Failed to enable everything. Are you running as administrator?"
}

Write-Host "Reboot now? (y/n)"
$answer = Read-Host
if ($answer -eq "y") {
    Restart-Computer
}