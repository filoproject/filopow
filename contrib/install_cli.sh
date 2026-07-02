 #!/usr/bin/env bash

 # Execute this file to install the filopow cli tools into your path on OS X

 CURRENT_LOC="$( cd "$(dirname "$0")" ; pwd -P )"
 LOCATION=${CURRENT_LOC%Filopow-Qt.app*}

 # Ensure that the directory to symlink to exists
 sudo mkdir -p /usr/local/bin

 # Create symlinks to the cli tools
 sudo ln -s ${LOCATION}/Filopow-Qt.app/Contents/MacOS/filopowd /usr/local/bin/filopowd
 sudo ln -s ${LOCATION}/Filopow-Qt.app/Contents/MacOS/filopow-cli /usr/local/bin/filopow-cli
