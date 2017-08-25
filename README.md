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
    
    https://www.qt.io/download-open-source/ or
    
    Ubuntu/Debian
    
    ``` sudo apt-get install qt5-default qtdeclarative5-dev qml-module-qtquick-controls qml-module-qtquick-controls2 qml-module-qtquick-templates2 qml-module-qtquick-dialogs```

## Build

1. Download and install the APIs mentioned above
2. If you have Qt Creator installed, you can open the project from Qt Creator and run it. If you want to build the code manually, follow these instructions:

*Note:* The name of the printer comes from the other part of the dialog(built by Anshul), so in my part, the value is hardcoded in main.qml and advanced.qml files as a property printer_name. To test my part of the dialog, please change the value "Xerox_Placeholder" to one that matches the printer you want to test with.

```
# Get the code

someDirectory$ git clone https://github.com/rithvikp1998/cpd.git
someDirectory$ cd cpd
someDirectory/cpd$ git submodule init; git submodule update

#Build backend library

someDirectory/cpd$ cd backends/cups
someDirectory/cpd/backends/cups$ git submodule init; git submodule update
someDirectory/cpd/backends/cups$ make lib; make release

# Build project

someDirectory/cpd/backends/cups$ cd ../..
someDirectory/cpd$ mkdir build; cd build
someDirectory/cpd/build$ qmake ../cpd.pro; make
someDirectory/cpd/build$ sudo cp ../libs/libcpd.so.1 /usr/lib/

# Get the test application
someDirectory/cpd/build$ cd ../..
someDirectory$ git clone https://github.com/rithvikp1998/printTest; cd printTest
someDirectory/printTest$ mkdir build; cd build
someDirectory/printTest/build$ qmake ../printTest.pro; make

# Run the test application which uses the CPD library
someDirectory/printTest/build$ ./printTest

```

## TODO

1. The CUPS API doesn't get the more advanced options like Contrast, Brightness, Color Balance etc. The dialog has a tab name "Advanced" which has static dropdowns to indicate these options but only the "Resolution" dropdown has backend support while the rest don't. These dropdowns will get the backend support as soon as it is provided by the CUPS API

2. Qt uses a QPrinter class for all the printing operations. To integrate this dialog with the Qt library itself, it is preferable that the dialog use a similar approach too, so that the existing developers and applications need not modify much of their code.
