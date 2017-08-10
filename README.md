# Common Print Dialog
This repository was created as a part of my participation in the Google Summer of Code 2017 for the OpenPrinting division of The Linux Foundation.

## Goal
The goal of the project is to build a Print Dialog in Qt in order to make the process of printing simple and uniform across multiple platforms and devices. The project idea can be found here: https://wiki.linuxfoundation.org/gsoc/google-summer-code-2017-openprinting-projects

## Project Structure
The project consists of two major parts:
1. Frontend : This team builds an UI using Qt and integrates the backend APIs with the frontend. The dialog started as a separate application, however, it is planned to replace the existing [QPrintDialog](http://doc.qt.io/qt-5/qprintdialog.html) with the Common Print Dialog being built.

    Students :
    * Anshul Gupta - [Git repo](https://github.com/anshulgupta0803/common-print-dialog)
    * Me
    
    Mentor :
    * Dongxu Li - dongxuli2011@gmail.com


2. Backend APIs (CUPS, IPP, Google Cloud Print) : This team builds APIs for the dialog using [CUPS](https://www.cups.org/doc/api-cups.html) and [Google Cloud Print](https://www.google.com/cloudprint/#printers). Their APIs can be found [here](https://github.com/NilanjanaLodh/PrintDialog_Backend) and [here](https://github.com/dracarys09/gcp-backend).

## Dependencies
* poppler-qt5
    
    Ubuntu/Debian
    
    ``` sudo apt-get install libpoppler-qt5-dev ```
* CUPS
    
    Ubuntu/Debian
    
    ``` sudo apt-get install cups libcups2-dev```
* libglib
    
    Ubuntu/Debian
    
    ``` sudo apt-get install libglib2.0-dev ```
* Qt 5.7
    
    https://www.qt.io/download-open-source/
    
    or Ubuntu/Debian
    
    ``` sudo apt-get install qt5-default qtdeclarative5-dev ```

## Build

1. Download and install the APIs mentioned above
2. If you have Qt Creator installed, you can open the project from Qt Creator and run it. If you want to build the code manually, follow these instructions:

```
# Get the code
git clone https://github.com/rithvikp1998/cpd.git
cd cpd
git submodule init
git submodule update

# Build project
mkdir build; cd build
qmake ../cpd.pro
make

# Install CUPS Backend
cd ../backends/cups/
make gen
make
sudo ./install.sh

# Run the dialog
./cpd

```
