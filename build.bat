@echo off
chcp 65001 >nul
title LightSwitch Build

echo ============================================
echo  LightSwitch Standalone — 构建脚本
echo ============================================
echo.

REM 检查 MSBuild 是否可用
where msbuild >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到 MSBuild。
    echo 请确保已安装 Visual Studio 2022 Build Tools，
    echo 并从 "开发者命令提示符" 运行此脚本。
    echo.
    echo 安装: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
    echo.
    pause
    exit /b 1
)

echo [1/2] 配置: Release x64
echo.

if "%Platform%"=="" set Platform=x64
if "%Configuration%"=="" set Configuration=Release

echo 平台: %Platform%
echo 配置: %Configuration%
echo.

msbuild LightSwitch.vcxproj /p:Configuration=%Configuration% /p:Platform=%Platform% /m

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [错误] 编译失败，错误码: %ERRORLEVEL%
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [2/2] 编译成功！
echo.
echo 输出: %Platform%\%Configuration%\LightSwitch.exe
dir "%Platform%\%Configuration%\LightSwitch.exe" 2>nul
echo.
pause
