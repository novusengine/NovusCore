#!/bin/bash

# Change these to correspond with your database set up
DBADRESS=127.0.0.1
DBUSERNAME=root
DBPASSWORD=root

# Loops through and runs all SQL scripts.
for FILE in authserver.sql charserver.sql worldserver.sql dbcdata.sql
do
	mysql -h $DBADRESS -u $DBUSERNAME -p$DBPASSWORD < $FILE
done