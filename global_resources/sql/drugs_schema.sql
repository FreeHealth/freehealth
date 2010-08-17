CREATE TABLE IF NOT EXISTS `DRUGS` (
  `UID` int(11) NOT NULL,
  `NAME` varchar(1000) NOT NULL,
  `FORM` varchar(500),
  `ROUTE` varchar(100),
  `ATC` varchar(7),
  `GLOBAL_STRENGTH` varchar(40),
  `TYPE_MP` varchar(1),
  `AUTHORIZATION` varchar(1),
  `MARKETED` bool NOT NULL DEFAULT 1,
  `LINK_SPC` varchar(250)
);

CREATE TABLE IF NOT EXISTS `COMPOSITION` (
  `UID` int(10) NOT NULL,
  `MOLECULE_FORM` varchar(100),
  `MOLECULE_CODE` int(11) NOT NULL,
  `MOLECULE_NAME` varchar(200) NOT NULL,
  `MOLECULE_ATC` varchar(7) NOT NULL DEFAULT "",
  `DOSAGE` varchar(100)  NOT NULL DEFAULT "",
  `DOSAGE_REF` varchar(50)  NOT NULL DEFAULT "",
  `NATURE` varchar(2) NOT NULL DEFAULT "SA",
  `LK_NATURE` int(11) NOT NULL DEFAULT 1
);

CREATE TABLE IF NOT EXISTS `PACKAGING` (
  `UID` int(11) NOT NULL,
  `PACKAGE_UID` int(20) NOT NULL,
  `LABEL` varchar(500) NOT NULL,
  `STATUS` varchar(1),
  `MARKETING` int(1) NOT NULL DEFAULT 1,
  `DATE` varchar(25),
  `OPTIONAL_CODE` int(20)
);

-- Link table : molecule code  1<->N  ATC id
CREATE TABLE IF NOT EXISTS `LK_MOL_ATC`  (
  `MOLECULE_CODE`  integer NOT NULL,
  `ATC_ID`         integer NOT NULL
);

CREATE TABLE IF NOT EXISTS `INFORMATIONS` (
  `VERSION` varchar(10),
  `NAME` varchar(2000),
  `IDENTIFIANT` varchar(50),
  `COMPAT_VERSION` varchar(10),
  `PROVIDER` varchar(200),
  `WEBLINK` varchar(500),
  `COMPLEMENTARY_WEBSITE` varchar(200),
  `AUTHOR` varchar(200),
  `LICENSE` varchar(1000),
  `LICENSE_TERMS` varchar(10000),
  `DATE` varchar(20),
  `DRUG_UID_NAME` varchar(50),
  `PACK_MAIN_CODE_NAME` varchar(50),
  `ATC` bool NOT NULL,
  `INTERACTIONS`  bool NOT NULL DEFAULT FALSE,
  `AUTHOR_COMMENTS` varchar(10000),
  `LANGUAGE_COUNTRY` varchar(5),
  `DRUGS_NAME_CONSTRUCTOR` varchar(200)
);

CREATE TABLE IF NOT EXISTS `SEARCH_ENGINES` (
  `ID` integer primary key,
  `LABEL` varchar(25),
  `URL` varchar(1000)
);

INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "WHO ATC description", "http://www.whocc.no/atc_ddd_index/?&code=[[ONE_ATC_CODE]]&showdescription=yes");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Search drug name @ NIH", "http://vsearch.nlm.nih.gov/vivisimo/cgi-bin/query-meta?v%3Aproject=medlineplus&query=[[DRUG_NAME]]&x=0&y=0");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Search INN name @ NIH", "http://vsearch.nlm.nih.gov/vivisimo/cgi-bin/query-meta?v%3Aproject=medlineplus&query=[[ONE_ATC_CODE]]&x=0&y=0");

CREATE TABLE IF NOT EXISTS `DB_SCHEMA_VERSION` (
  `VERSION` varchar(10),
  `DATE`  date,
  `COMMENT` varchar(500)
);

INSERT INTO `DB_SCHEMA_VERSION` VALUES ("0.3.0","2010-03-01","Translated all fields");
INSERT INTO `DB_SCHEMA_VERSION` VALUES ("0.4.0","2010-03-15","DRUGS table : Adding GLOBAL_STRENGTH");
INSERT INTO `DB_SCHEMA_VERSION` VALUES ("0.4.0","2010-06-01","Adding the information table");
INSERT INTO `DB_SCHEMA_VERSION` VALUES ("0.4.4","2010-07-16","Adding LK_MOL_ATC table");
INSERT INTO `DB_SCHEMA_VERSION` VALUES ("0.4.4","2010-07-18","INFORMATIONS table : Adding the COMPLEMENTARY_WEBSITE");
INSERT INTO `DB_SCHEMA_VERSION` VALUES ("0.4.4","2010-07-22","COMPOSITION table : Adding ATC Molecule code");
INSERT INTO `DB_SCHEMA_VERSION` VALUES ("0.4.4","2010-07-22","Adding DB_SCHEMA_VERSION table");
