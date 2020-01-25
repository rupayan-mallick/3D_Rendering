
# Acquisition and Reconstruction Project: Advanced Features

This is a 3D rendering system that support numerous 3D file formats and allow users to interact with the 3D models. 

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 

### Prerequisites

```
Linux opertaing system (e.g Ubuntu, Debian, Fedora, etc...)
C++ compiler 
```

### Installing


For the successful installation of all the required environments for this project, one can open and follow all steps from provided link below

```
https://wiki.qt.io/Install_Qt_5_on_Ubuntu
```
Or follow steps detatiled below.

#### Download Qt

open terminal and type

```
wget http://download.qt.io/official_releases/qt/5.7/5.7.0/qt-opensource-linux-x64-5.7.0.run
```

Adjust permission, run the installer and follow the instruction to complete the installation.

```
chmod +x qt-opensource-linux-x64-5.7.0.run
./qt-opensource-linux-x64-5.7.0.run
```
Install g++
Open a terminal and execute the following command to install g++:
```
sudo apt-get install build-essential
```
Install generic font configuration library - runtime
Open a terminal and execute the following command to install the full runtime files for the generic font configuration library:
```
sudo apt-get install libfontconfig1
```
Configure a compiler
Launch Qt Creator. Go to Tools > Options. Click Build & Run and select tab Kit. Configure a compiler if it is not automatically detected.

Install OpenGL libraries
Execute the following command to install OpenGL libraries:
```
sudo apt-get install mesa-common-dev
```
Note: Just installing the above-mentioned mesa-common-dev kit is not sufficient for more recent Ubuntu versions. 
For an additional package needs installation. Execute following command:

```
sudo apt-get install libglu1-mesa-dev -y
```

Set file association with pro files
When installing from the on-line source the file association is not done automatically. 
It also not show up when you try to associate it with file explorer. Create a file named “Qt-Creator.desktop” and fill the file with the following.
```
[Desktop Entry]
Version=1.0
Encoding=UTF-8
Type=Application
Name=QtCreator
Comment=QtCreator
NoDsiplay=true
Exec=(Install folder of QT)/Tools/QtCreator/bin/qtcreator %f
Icon=(Install folder of QT)/5.4/Src/qtdoc/doc/images/landing/icon_QtCreator_78x78px.png
Name[en_US]=Qt-Creator
```

Place this file in home .local/share/applications .

Edit a file named “defaults.list” in the same directory . Add the following line.

```
text/qtcreator=Qt-Creator.desktop;
```
open file mimeapps.list and check if the following line is present.
```
application/vnd.nokia.qt.qmakeprofile=qtcreator.desktop
```
if not add it under [added Associations].

Run the following command.
```
sudo update-mime-database /usr/share/mime
```
now Qt has been added to the list of file associations.
```
sudo apt-get install build-essential
sudo apt-get install mesa-common-dev
```

## Details of the project

### Files Includes With This Project:
      face.cpp     viewer_widget.cpp	    glwidget.cpp     face_viewer.cpp  
      face.h	   viewer_widget.h          glwidget.h       face_viewer.h		       


### Core Functionalities :
3D graphics programs allow objects to be created on an X-Y-Z scale (width, height, depth). As 3D entities, they can be rotated and viewed from all angles as well as be scaled larger or smaller. Thus, the rendering system support the different linear transformations to be applied on the 3D objects while rendering in real-time.
Functionalities included:
     
	  Rotation			Translation	    	Zoom     			Speedup  
      Transparency		Resetting View      Drawing Edges       Face Highlighting
	  Volume wise coloration				Face Sorting & Filtering	       

### Supported File Formats 
We have implemented our project to support PGM3D files and various other 3D file formats such as JSON, OBJ, STL etc. for the 3D rendering.  Thus, our project is limited to the above listed 3D file formats only.  

## Running the tests
### Building 
Open a terminal in the project directory and type:  

```mkdir build && cd build && qmake -qt=qt5 .. && make ```

This should successfully build the project 

### Running 
To run the system, you can either run it from the QtCreator or you can type this in your terminal:

``` ./faces_viewer [path_to_file] ```
If you didn't provide the [path_to_file], there is a button with a label **'load file'** so that you can click and choose a 3D file you want to render from the file chooser dialog.   

## Deployment

Add additional notes about how to deploy this on a live system

## Built With

* OpenGL
* QtCreator

## Authors

* **GEBREHIWOT Awet**  **KINFU Kaleab** **MALLICK Rupayan**  **ZHAO Shanshan**  

## Acknowledgments
* We would like to extend our sincere gratitude to Professor Ludovic Hofer for introducing us to this fascinating subject and push us to work hard and learn more about it.   
