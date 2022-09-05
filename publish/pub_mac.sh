rm -rf work
mkdir work
cd work
 ~/Qt/6.4.0/macos/bin/qmake ../../OK64/OK64.pro
make -j8 -f Makefile
cd OK64.app
# ~/Qt/6.4.0/macos/bin/macdeployqt .
 cd ..
tar -czvf ok64_macos_arm.tar.gz OK64.app
mv ok64_macos_arm.tar.gz .. 
cd ..
scp ok64_macos_arm.tar.gz leuat@www.irio.co.uk:www.irio.co.uk/ok64