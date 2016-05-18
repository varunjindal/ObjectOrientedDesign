@echo off
echo.
echo ***********************************************************
echo           Dependency-Based Remote Code Repository, v1.0 
echo ***********************************************************
echo.

@echo off
start "TestExecutive" Debug\TestExecutive.exe
start "Server" Debug\Server.exe 7080
start "Client" Debug\Client.exe 8081 7080