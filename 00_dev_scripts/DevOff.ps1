bcdedit /set testsigning off
bcdedit /set nointegritychecks off
bcdedit /set debug off

if ($?) {
    Write-Host "Success! Reboot to disable everything."
} else {
    Write-Host "Failed to disable everything. Are you running as administrator?"
}

Write-Host "Reboot now? (y/n)"
$answer = Read-Host
if ($answer -eq "y") {
    Restart-Computer
}