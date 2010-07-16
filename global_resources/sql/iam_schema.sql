CREATE TABLE IF NOT EXISTS `IAM_DENOMINATION` (
`ID_DENOMINATION` int(10) NOT NULL ,
`DENOMINATION` varchar( 200 ) NOT NULL
);

-- Create the interactions table
CREATE TABLE IF NOT EXISTS `IAM_IMPORT` (
`IAM_ID` INTEGER PRIMARY KEY,
`ID1` int(10) NOT NULL ,
`ID2` int(10) NOT NULL ,
`TYPE` INT UNSIGNED NOT NULL ,
`TEXT_IAM` VARCHAR( 2000 ) NULL ,
`TEXT_CAT` VARCHAR( 2000 ) NULL
 );

-- Generic ATC table (more than 5000 ATC codes are known)
-- Codes ID > 100 000 are interacting molecule names without ATC
-- Codes ID > 200 000 are interactions classes denomination
CREATE TABLE IF NOT EXISTS `ATC` (
`ID` INTEGER PRIMARY KEY,
`CODE` VARCHAR(7) NULL ,
`ENGLISH` VARCHAR(127) NULL ,
`FRENCH` VARCHAR(127) NULL
 );

-- Add IAM classes tree (one class can contains multiple INN)
CREATE TABLE IF NOT EXISTS `IAM_TREE` (
`ID_CLASS` INTEGER NOT NULL,
`ID_ATC`   INTEGER NOT NULL
);
