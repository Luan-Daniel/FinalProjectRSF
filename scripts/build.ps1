#!/usr/bin/env pwsh

### Preset Values ###

$DOTENV_PATH = ".env"
$SOURCE_DIR = "src"
$BUILD_DIR = "build"
$GEN_DIR = "gen"

### Functions ###

function LoadEnviroment{
  param([Parameter(Mandatory=$true)][string]$dotenv_path)
  # Do not put comments on the end of lines. Like: `VAR="VALUE" # Comment`.
  # Do not use quotation marks in variable names or values.
  # Variable names and values will be trimmed.

  if (-not (Test-Path -Path $dotenv_path -PathType Leaf)) {
    Write-Output "`e[31m[!]`e[0m ``$dotenv_path`` file not found. 0 Variables were loaded."
    return
  }

  $count = 0
  $env_vars = Get-Content -Path $dotenv_path
  foreach ($line in $env_vars) {
    $trimmed_line = $line.Trim()
    if ($trimmed_line -eq "" -or $line.StartsWith("#")) {continue}

    $parts = $line -split '=', 2
    $name = $parts[0].Trim()
    $value = $parts[1].Trim()

    Set-Item -Path "env:$name" -Value $value
    $count += 1
  }
  Write-Output "[i] $count variable(s) loaded from ``$dotenv_path``."
}

function BuildWithPicoSdk{
  param(
    [Parameter(Mandatory=$true)]
    [string]$name,
    [Parameter(Mandatory=$true)]
    [object]$instructions
  )

  $target_arch = $instructions.target.arch.ToString()
  $executable = $instructions.toolchain.param.executable.ToString()

  $project_src_directory = (Join-Path $SOURCE_DIR $name)
  $project_gen_directory = (Join-Path $GEN_DIR $name)
  $project_build_directory = (Join-Path $BUILD_DIR $name)
  $hex_gen_path = (Join-Path $project_gen_directory "$($executable).uf2")
  $hex_build_path = (Join-Path $project_build_directory "$($executable).uf2")

  cmake -G "$env:CMAKE_GENERATOR" -H"$project_directory" -B"$project_gen_directory" -DPICO_BOARD="$target_arch"
  & $env:MAKE_TOOL --directory="$project_gen_directory" "$executable"

  if (Test-Path $hex_gen_path -PathType Leaf) {
    if (-not (Test-Path -Path $hex_build_path -PathType Container)) {
      New-Item -ItemType Directory -Path $project_build_directory -Force > $null
    }
    Move-Item -Path $hex_gen_path -Destination $project_build_directory -Force > $null
    Write-Output "[i] Final binary moved to ``$project_build_directory``directory."
  }
  else {
    Write-Output "[i] Could not find final binary. Look for it inside ``$project_gen_directory``."
  }
}

### Script ###

LoadEnviroment($DOTENV_PATH)
$nArgs = $args.Count

if ($nArgs -ne 1) {
  Write-Output "`e[31m[!]`e[0m Script expect a single argument with the project name under the src directory ``$SOURCE_PATH``, given: $nArgs. Eg: ``> scripts/build.ps1 blink_example``"
  return
}

$project_name = $args[0].ToString()
$project_directory = (Join-Path $SOURCE_DIR $project_name)
$instructions_path = (Join-Path $project_directory ".json")

if (-not (Test-Path -Path "$project_directory" -PathType Container)){
  Write-Output "`e[31m[!]`e[0m ``$project_directory`` directory not found."
  return
}

if (-not (Test-Path -Path $instructions_path -PathType Leaf)){
  Write-Output "`e[31m[!]`e[0m ``$project_name`` build instructions not found. Expected to be at: ``$instructions_path``"
  return
}

$instructions = ConvertFrom-Json -InputObject (Get-Content -Path $instructions_path -Raw)

foreach ($key in $instructions.env.Keys) {Set-Item -Path "env:$key" -Value instructions.env[$key]}

if ($instructions.toolchain.buildtool -eq "pico_sdk") {
  BuildWithPicoSdk -name $project_name -instructions $instructions
}