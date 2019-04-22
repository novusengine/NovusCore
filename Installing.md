# NovusCore Installation (Windows)

#### Note: NovusCore is not ready for production use at the time of writing.

## Part 1: Install prerequisities.

[CMake 3.12+](https://cmake.org/download/)
(Required for building from source)

[OpenSSL 1.0.x](https://www.openssl.org/source/)
(1.1 is not supported)

[MySQL C Connector 6.0 (libmysqlcclient) or newer](https://dev.mysql.com/downloads/connector/c/)

**Additionally to run the server you will need a MySQL Server. Any version compatible with the "MySQl C Connector 6.0" should work.**

(This is where all the data about accounts, characters and the world is stored.)

[MySQL Community Server](https://dev.mysql.com/downloads/mysql/)

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
- *Realmserver.exe*
- *Worldnode.exe*
- *Dataextractor.exe*

![alt text](https://i.imgur.com/izxIiWS.png "Exe files")

The first three will be used to launch the server whilst *Dataextractor.exe* will be used to extract the map & DBC data from the client.
## Part 3 Setting up the database.
By now you need to have an MySQL server set up.

**NovusCore uses 4 databases**
1. Auth
(For account data such as username)
2. Characters
(For Character data such as names, class, level, etc)
3. World
(Empty right now)
4. DBC
(Empty right now)

There are template files for these databases in [*Resources/Database Files/Templates*](https://github.com/novuscore/NovusCore/tree/master/resources/Database%20Files/Templates) which was included in the downloaded source.

![alt text](https://i.imgur.com/tzRyjhf.png "Template Files")

To run these files you can use a MySQL client like [HeidiSQL](https://www.heidisql.com/download.php) or [MySQL Workbench](https://www.mysql.com/products/workbench/) **or** run the following command from the commandline:

```mysql -hSERVERADRESS -u USERNAME -pPASSWORD < FILE```

Replacing *SERVERADRESS* with the adress/IP of your MySQL server, *USERNAME/PASSWORD* with your MySQL username/password and *FILE* with one of the template files. (You will need to repeat this for all templates)

For example it should look something like his:

```mysql -h127.0.0.1 -u root -proot < auth.sql```

That's all you need to do for the database set up.
## Part 4 Map and DBC Data
Now we will extract the Map and DBC data from the client. You will need the WotLK client downloaded for this step.

1. Copy the *Dataextrator.exe* file from *Part 2* to where the *Data* directoy is (It is in the same directory as the *WoW.exe* file)
2. Run *Dataextractor.exe* and wait for it to finish. There should now be a folder called "NovusExtractor" containing *DBCImportData.sql* and a folder called "maps".
3. Execute *DBCImportData.sql*. You will need to manually select the database you want to import the data to, this will change the command you execute if you are using the commandline to the following:

```mysql -hSERVERADRESS -u USERNAME -pPASSWORD DATABASENAME < DBCImportData.sql```

Or for example:

```mysql -h127.0.0.1 -u root -proot dbc < DBCImportData.sql```


Now all we need to do is put all the parts together.
## Part 5 Finishing up
Now you should have the executables built, database set up and prerequisites installed. Now you only need to put it all together.

1. Create a new folder for the Server files.
2. Copy the *Authserver, Realmserver & Worldserver* exe files that you built in *Part 2* into the Server folder you created.
3. Copy the Configuration Templates from *Resources/Configuration Templates* to the server folder.
4. Copy the map folder which you created in *Part 4* to the server folder.
5. Edit *database.json* and put in the IP, Username & password for your MySQL database in each field.
6. Go to where you installed the MySQL connector and into the *lib* folder. From there copy *libmysql.dll* into your server folder.
7. Go to where you installed OpenSSL and copy *libeay32.dll* to your server folder.

![alt text](https://i.imgur.com/kdhbR5Y.png "Final server folder")

Now everything is ready for the last step...

## Part 6 Starting the Server
The Server is divided into 3 executables.

**The Realm Server** *(Relayserver.exe)*
This server handles everything that happens on the character select screen. Creating, Deleting, Customizing characters and logging into them.

**The Authentication Server** *(Authserver.exe)*
This server handles authentication. The only thing dependant on this server is logging in. Once you are past the log in screen this isn't needed.

**The World Server** *(Worldserver.exe)*
This server handles the world. Everything in the world from NPCs to combat is handled by this server.

All you need to do now to start up the server is execute the exe files. The order does not matter.
