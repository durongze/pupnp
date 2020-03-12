call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"
rem mkdir lib
rem tlbimp excel.exe /out:lib\excel.dll 
mkdir dyzbuild
pushd dyzbuild
for /f %%i in ('dir /s /b "*.dll"') do (del %%i )
cmake ..
MSBuild device.vcxproj
popd

for /f %%i in ('dir /s /b "*.dll"') do (copy %%i .\)
tasklist  | findstr /i "device"
taskkill /f /im device.exe
pause