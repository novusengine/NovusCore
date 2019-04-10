# NovusCore Installation (Windows)

#### Note: NovusCore is not ready for production use at the time of writing.

## Part 1: Install prerequisities.

[CMake 3.12 or newer](https://cmake.org/download/)
(Required for building from source)

[OpenSSL 1.0.x](https://www.openssl.org/source/)
(1.1 is not supported)

[MySQL C Connector 6.0 (libmysqlcclient) or newer](https://dev.mysql.com/downloads/connector/c/)

**Additionally to run the server you will need a MySQL Server. Any version compatible with the "MySQl C Connector 6.0" should work.**

(This is where all the data about accounts, characters and the world is stored.)

[MySQL Community Server 8.0.15](https://dev.mysql.com/downloads/mysql/)

**To build the Server from source you will need a way to compile.**
I recommend getting [Visual Studio Community Edition](https://visualstudio.microsoft.com/) but any IDE supported by CMake should work so use whichever you are prefer.

## Part 2: Building the Server from source.

1.Clone the NovusCore repository from Github using git or a program like Github Desktop (Recommended as it is easier to update in the future) **OR** Download the NovusCore repository from Github as a zip file and extract it.

2. Open CMake.

	A. The Source code is in the NovusCore folder that you downloaded. (The folder you are looking for is the one that contains the README.md)
  
	B. Where to build the binaries is the folder where the generated project files will be.
  
	C. Press Configure. It will ask you to select a generator. Choose your IDE, Select x64 as the platform, "Use default native compiler" and then finish.
	
	D. Press Generate. This will generate the project files.
	
If everything has gone right the Open Project button should be clickable and CMake should look something like this:
![alt text](https://i.imgur.com/5akkUtr.png "CMake Result")

3. Open the Project. Either by pressing Open Project in CMake or by navigating to the folder and opening the relevant project file (NovusCore.sln if using Visual Studio for other IDEs the file should have the same name but a different file ending).

4. Build the project using your chosen IDE. (In Visual Studio this is done by pressing Build -> Build Solution or pressing F6)

Building the project might take a bit of time depending on your set up. Once it's done building the output should be found by the source code inside of the *build* folder.

There you will find 4 exe files.
- *Authserver.exe*
- *Relayserver.exe*
- *Characterserver.exe*
- *Worldserver.exe*

![alt text](https://i.imgur.com/wWaK98L.png "Exe files")

These will be used to launch the server.
## Part 3 Setting up the database.
By now you need to have an MySQL server set up.

**NovusCore uses 4 databases**
1. Authserver
(For account data such as username)
2. Characterserver
(For Character data such as names, class, level, etc)
3. Worldserver
(Empty right now)
4. DBCData
(Empty right now)

There are template files for these databases in [*Resources/Database Files/Templates*](https://github.com/novuscore/NovusCore/tree/master/resources/Database%20Files/Templates) which was included in the downloaded source.

![alt text](https://i.imgur.com/dehUFcs.png "Template Files")

To run these files you can use a MySQL client like [HeidiSQL](https://www.heidisql.com/download.php) or [MySQL Workbench](https://www.mysql.com/products/workbench/) **or** run the following command from the commandline:

```mysql -hSERVERADRESS -u USERNAME -pPASSWORD < FILE```

Replacing *SERVERADRESS* with the adress/IP of your MySQL server, *USERNAME/PASSWORD* with your MySQL username/password and *FILE* with one of the template files. (You will need to repeat this for all templates)

For example it should look something like his:

```mysql -h127.0.0.1 -u root -proot < authserver.sql```

That's all you need to do for the database set up.
## Part 4 Map and DBC Data

***TODO As of writing this there is no way to actually set this up by yourself BUT the server also doesn't require either (at the time of writing) so it should be fine anyway. Ignore the step in Part 5 where you are told to copy the map data***
 
## Part 5 Finishing up
Now you should have the executables built, database set up and prerequisites installed. Now you only need to put it all together.

1. Create a new empty folder for the Server files.
2. Copy the 4 exe files that you built in part 2 from the *build folder* (Should be inside of where you put the sourcecode)
3. Copy the Configuration Templates from *Resources/Configuration Templates* to the server folder you created.
4. Edit *database_configuration.json* and put in the IP, Username & password for your MySQL database in each field.
5. Go to where you installed the MySQL connector and into the *lib* folder. From there copy *libmysql.dll* into your server folder.
6. Go to where you installed OpenSSL and copy *libeay32.dll* to your server folder.
7. Copy the map data created in *Part 4* into a new folder inside of your server folder called *maps*

![alt text](https://i.imgur.com/XnwK2tq.png "Final server folder")

Now everything is ready for the last step...

## Part 6 Starting the Server
Some useful information to know before starting the server(s):
The Server is divided into 4 executables.

**The Relay Server** *(Relayserver.exe)*
Clients communicate with this server and it directs the traffic to the right character and world server.

**The Authentication Server** *(Authserver.exe)*
This server handles authentication. The only thing dependant on this server is logging in. Once you are past the log in screen this isn't needed.

**The Character Server** *(Characterserver.exe)*
This server handles characters. Creating new, deleting old and logging into characters goes through this server.

**The World Server** *(Worldserver.exe)*
This server handles the world. Everything from NPCs to walking around happens on this server.

**To actually launch the server just open the exe files, but note:**
1. The Relay server needs the Authentication server to work correctly.
2. All servers except the authentication server rely on the Relay server to work correctly.

**The start order which you need to follow is then:**
1. Auth Server
2. Relay Server
3. Character Server & World Server
*If you don't launch the servers in this order there will be problems.*

Now you should have a working\* NovusCore server running and ready to be played on!

\**Note: By working we are not saying that every feature has been fully or at all implemented.*
