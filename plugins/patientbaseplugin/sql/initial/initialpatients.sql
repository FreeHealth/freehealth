--
-- Database: `fmf_patients`
-- Initial state
-- Version 0.1
-- See https://freehealth.io/en/code/database
--

-- --------------------------------------------------------

--
-- Table structure for table `PATIENT_IDENTITY`
--

CREATE TABLE `PATIENT_IDENTITY` (
  `IDENT_ID` int(10) UNSIGNED NOT NULL,
  `IDENT_UID` varchar(32) DEFAULT NULL,
  `TO_PRACT_LKID` int(11) DEFAULT NULL,
  `IDENT_FAMILY_UID` varchar(32) DEFAULT NULL,
  `IDENT_ISACTIVE` int(1) DEFAULT '1',
  `IDENT_ISVIRTUAL` int(1) DEFAULT '0',
  `NAME` varchar(200) DEFAULT NULL,
  `SURNAME` varchar(200) DEFAULT NULL,
  `SECONDNAME` varchar(200) DEFAULT NULL,
  `GENDER` varchar(1) DEFAULT NULL,
  `TITLE` int(11) DEFAULT NULL,
  `DOB` date DEFAULT NULL,
  `MARITAL_STATUS` varchar(1) DEFAULT NULL,
  `DATEOFDEATH` date DEFAULT NULL,
  `PROFESSION` varchar(200) DEFAULT NULL,
  `STREET` varchar(200) DEFAULT NULL,
  `STREET_NUMBER` varchar(200) DEFAULT NULL,
  `ZIP` varchar(200) DEFAULT NULL,
  `CITY` varchar(200) DEFAULT NULL,
  `PROV` varchar(200) DEFAULT NULL,
  `COUNTRY` varchar(2) DEFAULT NULL,
  `NOTE` varchar(2000) DEFAULT NULL,
  `MAILS` varchar(2000) DEFAULT NULL,
  `TELS` varchar(2000) DEFAULT NULL,
  `FAXES` varchar(2000) DEFAULT NULL
);

-- --------------------------------------------------------

--
-- Table structure for table `PATIENT_PHOTO`
--

CREATE TABLE `PATIENT_PHOTO` (
  `PHOTO_ID` int(10) UNSIGNED NOT NULL,
  `PHOTO_UID` varchar(32) DEFAULT NULL,
  `PATIENT_UID` varchar(32) DEFAULT NULL,
  `PHOTO` longblob
);

-- --------------------------------------------------------

--
-- Table structure for table `VERSION`
--

CREATE TABLE `VERSION` (
  `VERSION` varchar(200) DEFAULT NULL
);

--
-- Indexes for dumped tables
--

--
-- Indexes for table `PATIENT_IDENTITY`
--
ALTER TABLE `PATIENT_IDENTITY`
  ADD PRIMARY KEY (`IDENT_ID`),
  ADD KEY `PATIENT_IDENTITY__IDENT_UID` (`IDENT_UID`),
  ADD KEY `PATIENT_IDENTITY__TO_PRACT_LKID` (`TO_PRACT_LKID`),
  ADD KEY `PATIENT_IDENTITY__IDENT_FAMILY_UID` (`IDENT_FAMILY_UID`),
  ADD KEY `PATIENT_IDENTITY__NAME` (`NAME`),
  ADD KEY `PATIENT_IDENTITY__SURNAME` (`SURNAME`),
  ADD KEY `PATIENT_IDENTITY__SECONDNAME` (`SECONDNAME`),
  ADD KEY `PATIENT_IDENTITY__DOB` (`DOB`);

--
-- Indexes for table `PATIENT_PHOTO`
--
ALTER TABLE `PATIENT_PHOTO`
  ADD PRIMARY KEY (`PHOTO_ID`),
  ADD KEY `PATIENT_PHOTO__PHOTO_UID` (`PHOTO_UID`),
  ADD KEY `PATIENT_PHOTO__PATIENT_UID` (`PATIENT_UID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `PATIENT_IDENTITY`
--
ALTER TABLE `PATIENT_IDENTITY`
  MODIFY `IDENT_ID` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `PATIENT_PHOTO`
--
ALTER TABLE `PATIENT_PHOTO`
  MODIFY `PHOTO_ID` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- Dumping data for table `VERSION`
--

INSERT INTO `VERSION` (`VERSION`) VALUES
('0.1');
