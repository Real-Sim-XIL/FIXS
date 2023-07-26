
"============================================================"
" Check RealSim Software Requirements (Experimental Feature) "
"============================================================"
# define RealSim version requirements
# $RealSimRequirements = [pscustomobject]@{
# vissim = '11.0'
# sumo = '1.13.0'
# carmaker = '10.0.1'
# matlab = '9.7'
# }
$RealSimRequirements=[pscustomobject](Get-Content requirements.txt -Raw | ConvertFrom-StringData)

$software32 = (Get-ItemProperty HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall\*) | sort-object -property DisplayName | Select-Object DisplayName, DisplayVersion

$software64 = (Get-ItemProperty HKLM:\Software\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\*) | sort-object -property DisplayName | Select-Object DisplayName, DisplayVersion

$softwareCheckList = $software32, $software64

# loop over all software that need to check
$RealSimSoftwareNames = $RealSimRequirements | Get-Member -MemberType Properties | Select-Object Name
$RealSimComplyList = [pscustomobject]@{
vissim = [bool] $false
sumo = [bool] $false
carmaker = [bool] $false
matlab = [bool] $false
}
for (($i=0); ($i -lt $RealSimSoftwareNames.Count); $i++)
{
	$curSoftwareName = $RealSimSoftwareNames.Name[$i]
	
	$isComply=[bool] $false
	for (($iS=0); ($iS -lt $softwareCheckList.Count); $iS++)
	{
		$curSoftwareFound=$softwareCheckList[$iS] -match $curSoftwareName
		for (($j=0); ($j -lt $curSoftwareFound.Count); $j++)
		{
			if ($curSoftwareName -eq "carmaker"){
				$curVer = ($curSoftwareFound.DisplayName[$j] -replace "CarMaker", "" )
			}
			else{
				if ($curSoftwareFound.Count -eq 1){
					$curVer = $curSoftwareFound.DisplayVersion -replace "-", "."
				}else{
					$curVer = $curSoftwareFound.DisplayVersion[$j] -replace "-", "."
				}
			}
			Write-Output "Found $($curSoftwareName) $($curVer)"
			if ([version]$curVer -ge [version]$RealSimRequirements.($curSoftwareName))
			{
				$isComply=[bool] $true
			}
		}
	}
	$RealSimComplyList.($curSoftwareName)= $isComply
}

# check default SUMO and Matlab
$sumoDefVer = (sumo -V)[0].Split()[-1]
Write-Output "Found default sumo $($sumoDefVer)"
if ([version]$sumoDefVer -ge [version]$RealSimRequirements.('sumo'))
{
	$RealSimComplyList.('sumo')=[bool] $true
}

"Finished finding software"
""

for (($i=0); ($i -lt $RealSimSoftwareNames.Count); $i++)		
{
	$curSoftwareName = $RealSimSoftwareNames.Name[$i]	
	if ($RealSimComplyList.($curSoftwareName) -eq $true)
	{
		Write-Output "Satisfied $($curSoftwareName) version >= $($RealSimRequirements.($curSoftwareName))"
	}
	else
	{
		Write-Output "Failed to find $($curSoftwareName) version >= $($RealSimRequirements.($curSoftwareName))"
	}
}

