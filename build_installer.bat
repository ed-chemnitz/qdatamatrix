mkdir b
cd b
set PATH=%PATH%;C:\Qt\2009.03\mingw\bin\
cmake .. -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=..\cmake\Win32Toolchain.cmake
mingw32-make package
move qdatamatrix*win32*exe ..

