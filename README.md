# Online Board

This is a board that you can share with others. Everything you draw or change will be visible to other users of the application. This program is a client so it needs a server to be run. You can visit the server repository by [this link](https://github.com/egor79k/blackboard_server).

## Build and deploy

The application was written in Qt Creator 4.11.0 (based on Qt 5.12.8, GCC 9.3.0 64 bit) so you need it to be installed to be able to build the application. Once you opened the project in Qt Creator, set up the needed build configuration and build the application. After this copy the "Data" folder and "server_config.json" into build directory.

After you built the program, you can deploy it. First delete all unnecessary file in the build directory leaving only the executable and files copied previously. The following process for different platforms is described below.

### Windows

In command prompt run the following commands:

```
<Qt_installation_directory>/bin/windeployqt
cd <path_to_build_directory>/windeployqt --quick --no-translations .
```

After this you can run the .exe file.

### Ubuntu

First you need to download and install [CQtDeployer](https://github.com/QuasarApp/CQtDeployer/releases). Then you should run the following command:

```
cqtdeployer -bin <path_to_built_executable> deploySystem
```

After this the "DistributionKit" directory will be created inside the build directory. There will be a file with the .sh extension inside this directory which is the executable of the application. You can now run it.
