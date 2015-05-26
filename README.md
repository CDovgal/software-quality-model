# software-quality-model
Assess quality of source code using Bayesian network

How to build 'Software quality model' client
- Build Qt5.5 from sources (minimal package is needed)
- $ git clone git://gitorious.org/qt/qt5.git qt5
- $ cd qt5
- $ perl ./init-repository --module-subset=qttools,qtbase
- $ configure -developer-build -opensource -confirm-license -mp -nomake examples -nomake tests -debug-and-release -c++11 -no-warnings-are-errors
- $ make
- Install CMake 3.0 or higher
- Create bin directory in the root of a project and run (example contains mock for path to qt):
- $ cmake .. -DCMAKE_PREFIX_PATH="~/third_party/qt5/qtbase"                 # for Mac OS X, Linux
- $ cmake .. -DCMAKE_PREFIX_PATH="D:/qt5/qtbase" -G"Visual Studio 12 Win64" # for Windows
- Open quality-software-model.sln;
- Select Release configuration for the whole solution;
- Run INSTALL project.
- Find quality-software-model binary file and qt dynamic libraries in C:\Program Files\mtrust-demo\bin (for Windows) or /usr/local/bin (for MAc OS X, Linux)

