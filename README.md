strecc
======

The strecc barcode purchasing system.

Database
========
The system uses .csv files for all database functionality. The following files are used:
- users.csv //Stores users and user info.
- items.csv //Stores all items and item details.
- administration.csv //Stores the barcodes for the administration functions.
- transactions.csv //Log file for all transactions.
- strecclog.csv //Log file for all other actions such as loggin in/out and administration.

This is done through the home made ccsv library. Work is being done to in the future move to another more full-featured library.

TODO
====
- Implement the missing administration functions such as changing price of an item.
- Add functionality to keep track of item numbers.
- Add calorie and aclohol volume for all items.
- Write a system that can read the transaction logs and compile statistics.
 


Changelog
=========
V1
---------------------------------
- Första versionen.

- Initial release.

---------------------------------
V1.1
---------------------------------
- Varor har nu ett unikt ID för att systemet ska kunna hålla reda på
dem.
- Man kan logga ut genom att lägga kortet mot läsaren när man är
inloggad.
- Man kan lägga till varor.
- Infört administratörer.
- Bara admins kan lägga till nya användare.

- Items now have unique IDs. Will be used to keep track of the items.
- You can log out by pressing your card against the reader when logged in.
- Function for adding items implemented.
- Some users are now administrators. The administrators can add users.

---------------------------------
v1.2
---------------------------------
- Förbättrade transaktionsloggar. Loggarna kommer bli enkla för strecc
att läsa i framtiden för att sammanställa rapporter.
- Changelog på framsidan när man startar strecc.
- Cancel funkar för de flesta funktioner.

- Improved transactions logs.
- Changelog on the front page when starting strecc.
- Cancel function implemented in most functions.

---------------------------------
v1.3
---------------------------------
- Kredit minskat till 50kr.
- Nytt system för användargränssnitt.
- Alla funktioner har anpassats för det nya systemet.
- Mainfilen helt omskriven i städsyfte.
- Förberedelser för att kunna byta csv-bibliotek i framtiden.
- Modularisering för att enklare kunna arbeta flera på programmet.

- Max credit decresed to 50Kr.
- New UI. All funcctions are rewritten to use the new UI.
- Main file rewritten to make it easier to read and work with.
- Preparation for being able to change csv library in the future.
- Cleanup to make it easier for several users to work on the system.

---------------------------------
v1.4
---------------------------------
- Info om en vara visas om den skannas då ingen är inloggad.

- When an item is scanned when not logged in the item details are displayed.
