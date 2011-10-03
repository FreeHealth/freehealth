CREATE TABLE `ZIPS` (
  `ID` integer primary key,
  `ZIP`     varchar(16),
  `CITY`    varchar(255),
  `EXTRACODE` varchar(255),
  `COUNTRY` varchar(2) NOT NULL
 );
 CREATE INDEX ZipZipIndex ON ZIPS (ZIP);
 CREATE INDEX ZipCityIndex ON ZIPS (CITY);
 CREATE INDEX ZipCountryIndex ON ZIPS (COUNTRY);
