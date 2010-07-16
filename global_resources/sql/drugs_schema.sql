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
  `DOSAGE` varchar(100)  NOT NULL,
  `DOSAGE_REF` varchar(50)  NOT NULL,
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
