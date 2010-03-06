@set BADA_STDOUT_FILE_NAME=D:\work\UNT\funner\components\system\sources\platform\bada\startup\test.txt
@copy  D:\work\unt\funner\tmp\bada-simulator\SYSTEMLIB.TESTS\tests\wrappers\application_exit.exe %BADA_SDK_HOME%\Rsrc\bada_WVGA\Win32FS\Osp\Applications\93bt1p123e\Bin\firstapp.exe
@%BADA_SDK_HOME%\Lib\WinSgpp\simulator --app-id=93bt1p123e --app-file=firstapp.exe -s %BADA_SDK_HOME%\Lib\WinSgpp\PhoneShell.dll -d S8000_Generic.dbi
