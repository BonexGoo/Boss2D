rd ".\packages\com.boss2d.helloworld\data" /S /Q
md ".\packages\com.boss2d.helloworld\data"
xcopy "..\..\..\assets\*.*" ".\packages\com.boss2d.helloworld\data\assets" /F /R /Y /I /S
xcopy "..\..\..\bin_Release64\*.*" ".\packages\com.boss2d.helloworld\data\bin" /F /R /Y /I /S
"C:\Qt\QtIFW-3.0.6\bin\binarycreator.exe" --offline-only -c config/config.xml -p packages helloworld_1.0.0
