#!/bin/bash

# Create AppDir and start populating
mkdir -p appdir/usr/bin
cp build/oamlStudio appdir/usr/bin/
cp oamlStudio.desktop appdir/
cp oamlStudio.svg appdir/

# Deploy to AppDir
wget -c https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
chmod a+x linuxdeployqt-continuous-x86_64.AppImage
./linuxdeployqt-continuous-x86_64.AppImage appdir/usr/bin/oamlStudio

# Generate AppImage
./linuxdeployqt-continuous-x86_64.AppImage appdir/usr/bin/oamlStudio -appimage -bundle-non-qt-libs
