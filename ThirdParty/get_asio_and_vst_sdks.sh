#!/bin/bash

# Due to licensing restrictions, the VST/ASIO SDK source code 
# can't be redistributed in this project. However, you can easily
# download the zip files and extract them here with the following script:

echo Downloading SDKs..

wget -w 1 -r -np -nd -nv http://www.steinberg.net/sdk_downloads/vstsdk367_03_03_2017_build_352.zip
wget -w 1 -r -np -nd -nv http://www.steinberg.net/sdk_downloads/asiosdk2.3.zip
unzip ./vstsdk367_03_03_2017_build_352.zip
unzip ./asiosdk2.3.zip
pushd VST_SDK
./copy_vst2_to_vst3_sdk.sh 
popd
mv ASIOSDK2.3 ASIO
rm ./vstsdk367_03_03_2017_build_352.zip
rm ./asiosdk2.3.zip

echo Done
