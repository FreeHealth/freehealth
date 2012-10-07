-- TYPE=MyISAM COMMENT='Table description and capacity'
CREATE TABLE `chapter` (
  `chap` integer primary key,
  `SID` int(11) NOT NULL default '0',
  `rom` varchar(5) NOT NULL default ''
);
CREATE INDEX ChapterSIDIndex ON chapter (SID);

-- TYPE=MyISAM COMMENT='Common attributes of certain codes' AUTO_INCREMENT=12312
CREATE TABLE `common` (
  `SID` integer primary key,
  `male` tinyint(4) NOT NULL default '0',
  `female` tinyint(4) NOT NULL default '0',
  `sequella` tinyint(4) NOT NULL default '0',
  `post` tinyint(4) NOT NULL default '0',
  `second` tinyint(4) NOT NULL default '0',
  `nocode` tinyint(4) NOT NULL default '0'
 );
 CREATE INDEX CommonSIDIndex ON common (SID);

-- TYPE=MyISAM COMMENT='Dags and stars structure and links to libelles'
CREATE TABLE `dagstar` (
  `SID` bigint(20) NOT NULL default '0',
  `LID` bigint(20) NOT NULL default '0',
  `assoc` bigint(20) NOT NULL default '0',
  `daget` char(1) NOT NULL default '',
  `plus` tinyint(4) NOT NULL default '0'
  );
CREATE INDEX DagStarSIDIndex ON dagstar (SID);
CREATE INDEX DagStarLIDIndex ON dagstar (LID);

-- TYPE=MyISAM COMMENT='Links to descriptor libelles'
CREATE TABLE `descr` (
  `SID` bigint(20) NOT NULL default '0',
  `LID` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`SID`,`LID`)
);
CREATE INDEX DescrSIDIndex ON descr (SID);
CREATE INDEX DescrLIDIndex ON descr (LID);

-- TYPE=MyISAM COMMENT=''
CREATE TABLE `descrlib` (
  `source` varchar(1) NOT NULL,
  `descr_FR` varchar(50),
  `descr_EN` varchar(50),
  `descr_GE` varchar(50),
  `sortSID` integer
);


--  TYPE=MyISAM COMMENT='Exclusion structure and links to exclusion libelles'
CREATE TABLE `exclude` (
  `SID` int(11) NOT NULL default '0',
  `excl` int(11) NOT NULL default '0',
  `plus` tinyint(4) NOT NULL default '0',
  `LID` bigint(20) NOT NULL default '0',
  `daget` char(1) default ''
  );
CREATE INDEX ExcludeSIDIndex ON exclude (SID);
CREATE INDEX ExcludeExclIndex ON exclude (excl);
CREATE INDEX ExcludeLIDIndex ON exclude (LID);

--  TYPE=MyISAM COMMENT='Links to glossary memos'
CREATE TABLE `glossaire` (
  `SID` bigint(20) NOT NULL default '0',
  `MID` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`SID`,`MID`)
);
CREATE INDEX GlossaireSIDIndex ON glossaire (SID);
CREATE INDEX GlossaireMIDIndex ON glossaire (MID);

--  TYPE=MyISAM COMMENT='HTML table pages in different languages'
CREATE TABLE `html` (
  `ref` varchar(20) NOT NULL default '',
  `FR` text NOT NULL,
  `EN` text NOT NULL,
  `GE` text NOT NULL,
  PRIMARY KEY  (`ref`)
);
CREATE INDEX HtmlRefIndex ON html (ref);

--  TYPE=MyISAM COMMENT='Links to inclusion libelles'
CREATE TABLE `include` (
  `SID` int(11) NOT NULL default '0',
  `LID` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`SID`,`LID`)
);
CREATE INDEX IncludeSIDIndex ON include (SID);
CREATE INDEX IncludeLIDIndex ON include (LID);

--  TYPE=MyISAM COMMENT='Links to indirect exclusion libelles'
CREATE TABLE `indir` (
  `SID` int(11) NOT NULL default '0',
  `LID` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`SID`,`LID`)
);
CREATE INDEX InDirSIDIndex ON indir (SID);
CREATE INDEX InDirLIDIndex ON indir (LID);

--  TYPE=MyISAM COMMENT='Table of libelles' AUTO_INCREMENT=70601
CREATE TABLE `libelle` (
  `LID` integer primary key,
  `SID` bigint(20) NOT NULL default '0',
  `source` char(1) NOT NULL default '',
  `valid` tinyint(4) NOT NULL default '0',
  `libelle` varchar(255) default '',
  `FR_OMS` varchar(255) default '',
  `EN_OMS` varchar(255) default '',
  `GE_DIMDI` varchar(255) default '',
  `GE_AUTO` varchar(255) default '',
  `FR_CHRONOS` varchar(255) default '',
  `date` datetime NOT NULL default '0000-00-00 00:00:00',
  `author` varchar(10) default '',
  `comment` varchar(255) default ''
);
CREATE INDEX LibelleLIDIndex ON libelle (LID);
CREATE INDEX LibelleSIDIndex ON libelle (SID);

-- TYPE=MyISAM COMMENT='Hierarchical structure of ICD10 and code declaration' AUTO_INCREMENT=19551
CREATE TABLE `master` (
  `SID` integer prinmary key,
  `code` varchar(10) NOT NULL default '',
  `sort` varchar(10) NOT NULL default '',
  `abbrev` varchar(10) NOT NULL default '',
  `level` char(1) NOT NULL default '',
  `type` char(1) NOT NULL default '',
  `id1` bigint(20) NOT NULL default '0',
  `id2` bigint(20) NOT NULL default '0',
  `id3` bigint(20) NOT NULL default '0',
  `id4` bigint(20) NOT NULL default '0',
  `id5` bigint(20) NOT NULL default '0',
  `id6` bigint(20) NOT NULL default '0',
  `id7` bigint(20) NOT NULL default '0',
  `valid` tinyint(4) NOT NULL default '0',
  `date` datetime NOT NULL default '0000-00-00 00:00:00',
  `author` varchar(10) NOT NULL default '',
  `comment` varchar(100) NOT NULL default ''
);
CREATE INDEX MasterSIDIndex ON master (SID);
CREATE INDEX MasterCodeIndex ON master (code);

--  TYPE=MyISAM COMMENT='Table of memo texts' AUTO_INCREMENT=619
CREATE TABLE `memo` (
  `MID` integer primary key,
  `SID` bigint(20) NOT NULL default '0',
  `source` char(1) NOT NULL default '',
  `valid` varchar(50) NOT NULL default '',
  `memo` text default '',
  `FR_OMS` text default '',
  `EN_OMS` text default '',
  `GE_DIMDI` text default '',
  `date` datetime NOT NULL default '0000-00-00 00:00:00',
  `author` varchar(10) NOT NULL default '',
  `comment` varchar(255) default ''
);
CREATE INDEX MemoSIDIndex ON memo (SID);
CREATE INDEX MemoMIDIndex ON memo (MID);

--  TYPE=MyISAM COMMENT='Links to note memos'
CREATE TABLE `note` (
  `SID` bigint(20) NOT NULL default '0',
  `MID` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`SID`,`MID`)
);
CREATE INDEX NoteSIDIndex ON note (SID);
CREATE INDEX NoteMIDIndex ON note (MID);

--  TYPE=MyISAM COMMENT='Links to references for specific tables and texts'
CREATE TABLE `refer` (
  `SID` bigint(20) NOT NULL default '0',
  `LID` bigint(20) NOT NULL default '0',
  `ref` varchar(20) NOT NULL default '',
  PRIMARY KEY  (`SID`,`LID`)
);
CREATE INDEX ReferSIDIndex ON refer (SID);
CREATE INDEX ReferLIDIndex ON refer (LID);

--  TYPE=MyISAM COMMENT='Links to systematic libelles'
CREATE TABLE `system` (
  `SID` bigint(20) NOT NULL default '0',
  `LID` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`SID`,`LID`)
);
CREATE INDEX SystemSIDIndex ON system (SID);
CREATE INDEX SystemLIDIndex ON system (LID);

-- TYPE=MyISAM COMMENT='Database version and comment'
CREATE TABLE `version`(
  `name`  varchar(50) NOT NULL,
  `version` varchar(3) NOT NULL,
  `build` integer,
  `valid` bool,
  `date` varchar(18),
  `expl`varchar(500)
);

