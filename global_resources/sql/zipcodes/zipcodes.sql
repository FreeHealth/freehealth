-- Zipcodes table including city/country (missing county/province, community);
CREATE TABLE `ZIPS` (
  `ID`      integer primary key,
  `ZIP`     varchar(16),
  `CITY`    varchar(255),
  `EXTRACODE` varchar(255),
  `COUNTRY` varchar(2) NOT NULL
 );
 CREATE INDEX ZipZipIndex ON ZIPS (ZIP);
 CREATE INDEX ZipCityIndex ON ZIPS (CITY);
 CREATE INDEX ZipCountryIndex ON ZIPS (COUNTRY);

-- Store the version of the database;
CREATE TABLE `VERSION` (
  `CURRENT` varchar(16),
  `DATE` date
 );

-- Import table;
-- country code      : iso country code, 2 characters;
-- postal code       : varchar(20);
-- place name        : varchar(180);
-- admin name1       : 1. order subdivision (state) varchar(100);
-- admin code1       : 1. order subdivision (state) varchar(20);
-- admin name2       : 2. order subdivision (county/province) varchar(100);
-- admin code2       : 2. order subdivision (county/province) varchar(20);
-- admin name3       : 3. order subdivision (community) varchar(100);
-- admin code3       : 3. order subdivision (community) varchar(20);
-- latitude          : estimated latitude (wgs84);
-- longitude         : estimated longitude (wgs84);
-- accuracy          : accuracy of lat/lng from 1=estimated to 6=centroid;
CREATE TABLE `IMPORT` (
  `COUNTRY`     varchar(2) NOT NULL,
  `ZIP`         varchar(20),
  `CITY`        varchar(180),
  `ADMIN_NAME1` varchar(100),
  `ADMIN_CODE1` varchar(20),
  `ADMIN_NAME2` varchar(100),
  `ADMIN_CODE2` varchar(20),
  `ADMIN_NAME3` varchar(100),
  `ADMIN_CODE3` varchar(20),
  `LATITUDE`    varchar(20),
  `LONGITUDE`   varchar(20),
  `ACCURARY`    integer
);
