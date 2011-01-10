CREATE TABLE IF NOT EXISTS `MASTER` (
  `DID`      INTEGER PRIMARY KEY,
  `UID1`     varchar(50) not null,
  `UID2`     varchar(50),
  `UID3`     varchar(50),
  `OLD_UID`  varchar(50),
  `SID`      integer,
  UNIQUE(UID1, UID2, UID3, OLD_UID)
);

CREATE TABLE IF NOT EXISTS `SOURCES` (
  SID             INTEGER PRIMARY KEY,
  DATABASE_UID    varchar(20) not null,
  LCID            integer not null,
  WEB             varchar(250),
  COPYRIGHT       varchar(500),
  DATE            date,
  DRUGS_VERSION   varchar(10),
  AUTHORS         varchar(200),
  VERSION         varchar(10),
  PROVIDER        varchar(200),
  WEBLINK         varchar(500),
  DRUG_UID_NAME   varchar(50),
  ATC             bool not null,
  INTERACTIONS    bool not null default true,
  COMPLEMENTARY_WEBSITE  varchar(200),
  PACK_MAIN_CODE_NAME    varchar(50),
  MOL_LINK_COMPLETION    integer default 0,
  AUTHOR_COMMENTS        varchar(10000),
  DRUGS_NAME_CONSTRUCTOR varchar(200),
  FREEMEDFORMS_COMPTA_VERSION varchar(10),
  WEBPORTAL_COMPTA_VERSION    varchar(10)
);

-- No PK till this table is a linkage table 1 -> N
CREATE TABLE IF NOT EXISTS `SOURCES_LABELS` (
  SID    integer,
  LID    integer,
  UNIQUE(SID, LID)
);

CREATE TABLE IF NOT EXISTS `LABELS` (
  LID   INTEGER PRIMARY KEY,
  LCID  integer,
  LABEL varchar(250)
);


CREATE TABLE IF NOT EXISTS `LANGUAGES` (
  LCID  INTEGER PRIMARY KEY,
  ISO   varchar(5)
);

CREATE TABLE IF NOT EXISTS `DRUGS` (
  ID        INTEGER PRIMARY KEY,
  DID       integer,
  SID       integer,
  NAME      varchar(200),
  ATC_ID    integer,
  STRENGTH  varchar(40),
  FORMS_ID  integer,
  ROUTES_ID integer,
  AID       integer,
  VALID     boolean,
  MARKETED  boolean,
  LINK_SPC  varchar(500),
  EXTRA_XML varchar(5000)
);

-- No PK till this table is a linkage table 1 -> N
CREATE TABLE IF NOT EXISTS `AUTHORIZATIONS` (
  AID   integer,
  LID   integer,
  UNIQUE(AID,LID)
);

CREATE TABLE IF NOT EXISTS `COMPOSITION` (
  DID           integer,
  MID           integer,
  STRENGTH      real,
  STRENGHT_NID  integer,
  DOSE_REF      real,
  DOSE_REF_NID  integer
);

CREATE TABLE IF NOT EXISTS `MOLS` (
  MID    INTEGER PRIMARY KEY,
  NAME   varchar(150),
  ATC_ID integer,
  WWW    varchar(200)
)

CREATE TABLE IF NOT EXISTS `UNITS` (
  NID     INTEGER PRIMARY KEY,
  VALUE   varchar(20)
);

CREATE TABLE IF NOT EXISTS `LK_MOL_ATC`  (
  `MOLECULE_CODE`  integer NOT NULL,
  `ATC_ID`         integer NOT NULL
);

-- TP IMPROVE
CREATE TABLE IF NOT EXISTS `PACKAGING` (
  `DID` integer not null,
  `PACKAGE_UID` int(20) NOT NULL,
  `LABEL` varchar(500) NOT NULL,
  `STATUS` varchar(1),
  `MARKETING` int(1) NOT NULL DEFAULT 1,
  `DATE` varchar(25),
  `OPTIONAL_CODE` int(20)
);

-- No PK till this table is a linkage table 1 -> N
CREATE TABLE IF NOT EXISTS `DRUG_ROUTES`  (
  DID  integer NOT NULL,
  RID  integer NOT NULL,
  UNIQUE(DID,RID)
);

-- No PK till this table is a linkage table 1 -> N
CREATE TABLE IF NOT EXISTS `ROUTES` (
  RID integer,
  LID integer
  UNIQUE(RID_LID)
);


CREATE TABLE IF NOT EXISTS `SEARCH_ENGINES` (
  `ID` integer primary key,
  `LABEL` varchar(25),
  `URL` varchar(1000)
);

DELETE FROM `SEARCH_ENGINES`;
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
INSERT INTO `DB_SCHEMA_VERSION` VALUES ("0.5.0","2010-09-22","Replacing UID from int to varchar");
INSERT INTO `DB_SCHEMA_VERSION` VALUES ("0.5.0","2010-09-23","Adding molecule links completion percent to INFORMATION table");
INSERT INTO `DB_SCHEMA_VERSION` VALUES ("0.5.2","2010-09-23","Adding routes tables and link table");
INSERT INTO `DB_SCHEMA_VERSION` VALUES ("0.5.5","2011-01-10","Totally redefined schema");
