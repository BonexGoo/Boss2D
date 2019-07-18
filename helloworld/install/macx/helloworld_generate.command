#!/bin/sh
cd /Users/cto/Dropbox/geegee/helloworld/install/macx
rm -rf generated
mkdir generated
cp -R ../../build-helloworld_macx_ios-Desktop_Qt_5_11_1_clang_64bit-Release/helloworld.app generated/helloworld.app
/Users/cto/Qt/5.11.1/clang_64/bin/macdeployqt generated/helloworld.app
