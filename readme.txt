Maintained toolsets: msvc, msvc-x64, macosx-10.7, linux-x64, iphone-device-armv7, iphone-device-arm64, iphone-simulator-x86, iphone-simulator-x86-64, android-armv7 

ubuntu packages needed for compilation: libasound2-dev libx11-dev libegl1-mesa-dev libgles2-mesa-dev

macosx-10.7 toolsets can be built only using XCode 9.4.1 or older. You can specify XCode path using XCODE_PATH environment variable (i.e. export XCODE_PATH="/Applications/Xcode9.4.1.app")