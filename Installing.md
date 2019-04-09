# NovusCore Installation (Windows)

#### Note: NovusCore is not ready for production use at the time of writing.

## Step 1: Install prerequisities.

[CMake 3.1 or newer](https://cmake.org/download/)
(Required for building from source)

[OpenSSL 1.0.2q](http://slproweb.com/products/Win32OpenSSL.html)

[MySQL C Connector 5.6 (libmysqlcclient) or newer](https://dev.mysql.com/downloads/connector/c/)

**Additionally to run the server you will need a MySQL Server.**

(This is where all the data about accounts, characters and the world is stored.)

[MySQL Community Server 8.0.15](https://dev.mysql.com/downloads/mysql/)

**To build the Server from source you will need a way to compile.**
I recommend getting [Visual Studio Community Edition](https://visualstudio.microsoft.com/) but any IDE supported by CMake should work so use whichever you are comfortable with.

## Step 2: Building the Server from source.

1.A) Clone the NovusCore repository from Github using git or a program like Github Desktop. (Recommended, easier to update in the future)
1.B) Download the NovusCore repository from Github as a zip file and extract it.

2. Open CMake.
  1. The Source code is in the NovusCore folder that you downloaded. (The folder you are looking for is the one that contains the README.md)
  2. Where to build the binaries is the folder where the generated project files will be.
  3. Press Configure.
