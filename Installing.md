# NovusCore Installation (Windows)

#### Note: NovusCore is not ready for production use at the time of writing.

## Step 1: Install prerequisities.

[CMake 3.1 or newer](https://cmake.org/download/)
(Required for building from source)

[OpenSSL 1.0.2q/r](http://slproweb.com/products/Win32OpenSSL.html)
(1.1 is not supported)

[MySQL C Connector 6.0 (libmysqlcclient) or newer](https://dev.mysql.com/downloads/connector/c/)

**Additionally to run the server you will need a MySQL Server. Any version compatible with the "MySQl C Connector 6.0" should work.**

(This is where all the data about accounts, characters and the world is stored.)

[MySQL Community Server 8.0.15](https://dev.mysql.com/downloads/mysql/)

**To build the Server from source you will need a way to compile.**
I recommend getting [Visual Studio Community Edition](https://visualstudio.microsoft.com/) but any IDE supported by CMake should work so use whichever you are comfortable with.

## Step 2: Building the Server from source.

1.Clone the NovusCore repository from Github using git or a program like Github Desktop. (Recommended as it is easier to update in the future)

	*OR* 
	
	Download the NovusCore repository from Github as a zip file and extract it.

2. Open CMake.

	A. The Source code is in the NovusCore folder that you downloaded. (The folder you are looking for is the one that contains the README.md)
  
	B. Where to build the binaries is the folder where the generated project files will be.
  
	C. Press Configure. It will ask you to select a generator. Choose your IDE, "Use default native compiler" and then finish.
	
	D. Press Generate. This will generate the project files.
	
If everything has gone right the Open Project button should be clickable and CMake should look something like this:
![alt text](https://i.imgur.com/7xNTjHe.png "CMake Result")

3. Open the Project. Either by pressing Open Project in CMake or by navigating to the folder and opening the relevant project file (NovusCore.sln if using Visual Studio for other IDEs the file should have the same name but a different file ending).

4. Build the project using your chosen IDE. (In Visual Studio this is done by pressing Build -> Build Solution or pressing F6)

Building the project might take a bit of time depending on your set up but once it is done the output .exe files should be found by the source code inside of the *build* folder.

There you will find 4 exe files.
- *Authserver.exe*
- *Relayserver.exe*
- *Characterserver.exe*
- *Worldserver.exe*
Copy those to an empty folder from which you want to run the server.

## Step 3 Setting up the database.
By now you should have a MySQL server set up.

**NovusCore requires 4 databases**
1. Authserver
(For account data such as username and password)
2. Characterserver
(For Character data such as names, class, level, etc)
3. Worldserver
(Empty right now)
4. DBCData
(Empty right now)

The Authserver, Characterserver and Worldserver currently have template SQL files in [*Resources/Database Files/Templates*](https://github.com/novuscore/NovusCore/tree/master/resources/Database%20Files/Templates) which was included in the downloaded source. To run these files you can use a MySQL client like [HeidiSQL](https://www.heidisql.com/download.php) or [MySQL Workbench](https://www.mysql.com/products/workbench/). 

DBCData is currently unused and you only need to create a new empty database called "DBCData" to satisfy the servers. *Note that this may change in the future*


