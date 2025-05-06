# rpc
rpc：vscode and Ubuntu

bin: Executable files (compiled project files)
build: Project build files (files generated during compilation)
lib: Project library files (static or dynamic libraries)
src: Source code files (core logic of the project)
test: Test code (unit tests or integration tests to ensure code quality)
example: Example usage code (helps users understand how to use the framework or library)
CMakeLists.txt: Top-level CMake configuration file (defines build process, source files, library files, etc.)
README.md: Project documentation file (includes project overview, installation instructions, usage guidelines, contribution info, etc.)
autobuild.sh: Automated build script (simplifies the build and compilation process)

1.the translation for setting up the protobuf environment on Ubuntu:
  Download the project: You can either download it from the project’s official download link or from the GitHub source code repository: https://github.com/google/protobuf.
  In the source package, the src/README.md file contains detailed installation instructions. The installation process is as follows:
  Extract the compressed file:
  unzip protobuf-master.zip
  Enter the extracted directory:
  cd protobuf-master
  Install required tools:
  sudo apt-get install autoconf automake libtool curl make g++ unzip
  Automatically generate the configure file:
  ./autogen.sh
  Configure the environment:
  ./configure
  Compile the source code (this may take some time):
  make
  Install:
  sudo make install
  Refresh the dynamic library cache:
  sudo ldconfig
2.Zookeeper is widely used in distributed environments and offers many excellent features, such as global naming services, service registry, and global distributed locks in distributed environments.
  Zookeeper Data Organization - Znode Nodes
  syntax = "proto3";
  package fixbug;
  option cc_generic_services = true;
  
  message LoginRequest {
    string name = 1;
    string pwd = 2;
  }
  
  message RegRequest {
    string name = 1;
    string pwd = 2;
    int32 age = 3;
    enum SEX {
      MAN = 0;
      WOMAN = 1;
    }
    SEX sex = 4;
    string phone = 5;
  }
  
  message Response {
    int32 errorno = 1;
    string errormsg = 2;
    bool result = 3;
  }
  
  // Define RPC service interface and methods
  service UserServiceRpc {
    rpc login(LoginRequest) returns (Response);
    rpc reg(RegRequest) returns (Response);
  }

  Zookeeper's Watcher Mechanism
  
  1.Download the zookeeper-3.4.10.tar.gz file from the project documentation, and extract it.
  2.In the extracted directory:
  
     zookeeper-3.4.10$ cd conf
     zookeeper-3.4.10/conf$ mv zoo_sample.cfg zoo.cfg
  
  
  3.Go to the bin directory and start zkServer:
  
     ./zkServer.sh start
  
  
  4.You can use netstat to check the port of zkServer. In the bin directory, start zkClient.sh to connect to zkServer and familiarize yourself with how Zookeeper organizes nodes.
  
  Zookeeper Native Development API (C/C++ Interface)
  
  5.Go to the extracted directory src/c:
  
     ~/package/zookeeper-3.4.10/src/c$
  
  6.Zookeeper provides native C/C++ and Java API development interfaces. These need to be compiled from source. The process is as follows:
  
     sudo ./configure
     sudo make
     sudo make install
3.muduo and boost install
