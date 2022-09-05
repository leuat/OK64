rm -rf work
mkdir work

ORG=C:/Users/leuat/Documents/GitHub/OK64/Release/release 
QT=c:/Qt/6.4.0/msvc2019_64
WRK=C:/Users/leuat/Documents/GitHub/OK64/publish/work
cd $ORG
$QT/bin/windeployqt .

cd $WRK
ls
cp -r $ORG/* .
rm *.obj
rm *.cpp
rm *.h
ls

#"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"  x86_amd64
#c:/Qt/6.4.0/msvc2019_64/bin/qmake ../../OK64/OK64.pro
#make -j8 -f Makefile
#ls

cp C:/Users/leuat/Documents/SDL2-2.24.0/lib/x64/SDL2.dll .
cd ..
mv work OK64
rm ok64_win64.zip
"c:\Program Files\7-Zip\7z.exe" a ok64_win64.zip OK64
#mv ok64_macos_arm.tar.gz .. 
#cd ..
scp ok64_win64.zip leuat@www.irio.co.uk:www.irio.co.uk/ok64

echo "DONE"
#read -p ""

