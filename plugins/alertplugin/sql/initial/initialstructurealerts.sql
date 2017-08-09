--
-- Database: `fmf_alerts`
--

-- --------------------------------------------------------

--
-- Table structure for table `ALR`
--

CREATE TABLE `ALR` (
  `A_ID` int(10) UNSIGNED NOT NULL,
  `A_UID` varchar(32) DEFAULT NULL,
  `A_PUID` varchar(2000) DEFAULT NULL,
  `C_UID` varchar(32) DEFAULT NULL,
  `R_ID` int(11) DEFAULT NULL,
  `SCR_ID` int(11) DEFAULT NULL,
  `VAL_ID` int(11) DEFAULT NULL,
  `TIM_ID` int(11) DEFAULT NULL,
  `ISV` int(1) DEFAULT NULL,
  `REMIND` int(1) DEFAULT NULL,
  `VIEW_ID` int(11) DEFAULT NULL,
  `CONTENT_ID` int(11) DEFAULT NULL,
  `COND_ID` int(11) DEFAULT NULL,
  `PRIOR` int(11) DEFAULT NULL,
  `VRUC` int(1) DEFAULT NULL,
  `MBR` int(1) DEFAULT NULL,
  `LBL_LID` int(11) DEFAULT NULL,
  `CAT_LID` int(11) DEFAULT NULL,
  `DES_LID` int(11) DEFAULT NULL,
  `COM_LID` int(11) DEFAULT NULL,
  `C_DATE` date DEFAULT NULL,
  `U_DATE` date DEFAULT NULL,
  `THM_ICON` varchar(200) DEFAULT NULL,
  `CSS` varchar(2000) DEFAULT NULL,
  `PASS` varchar(200) DEFAULT NULL,
  `EXTRA` longblob
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `LBL`
--

CREATE TABLE `LBL` (
  `ID` int(10) UNSIGNED NOT NULL,
  `LID` int(11) DEFAULT NULL,
  `LNG` varchar(2) DEFAULT NULL,
  `VAL` varchar(200) DEFAULT NULL,
  `ISV` int(1) DEFAULT '1'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `PACKS`
--

CREATE TABLE `PACKS` (
  `ID` int(10) UNSIGNED NOT NULL,
  `UID` varchar(2000) DEFAULT NULL,
  `ISV` int(1) DEFAULT NULL,
  `INU` int(1) DEFAULT NULL,
  `LBL_LID` int(11) DEFAULT NULL,
  `CAT_LID` int(11) DEFAULT NULL,
  `DESC_LID` int(11) DEFAULT NULL,
  `ATH` varchar(200) DEFAULT NULL,
  `VEN` varchar(200) DEFAULT NULL,
  `URL` varchar(200) DEFAULT NULL,
  `ICO` varchar(200) DEFAULT NULL,
  `VER` varchar(200) DEFAULT NULL,
  `FMFV` varchar(200) DEFAULT NULL,
  `CDT` datetime DEFAULT NULL,
  `UDT` datetime DEFAULT NULL,
  `XTR` longblob
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `REL`
--

CREATE TABLE `REL` (
  `ID` int(10) UNSIGNED NOT NULL,
  `RID` int(11) DEFAULT NULL,
  `TO` int(11) DEFAULT NULL,
  `UID` varchar(32) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `SCR`
--

CREATE TABLE `SCR` (
  `ID` int(10) UNSIGNED NOT NULL,
  `SID` int(11) DEFAULT NULL,
  `S_UID` varchar(32) DEFAULT NULL,
  `ISV` int(1) DEFAULT '1',
  `S_TP` varchar(200) DEFAULT NULL,
  `S_CT` varchar(2000) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `TIM`
--

CREATE TABLE `TIM` (
  `ID` int(10) UNSIGNED NOT NULL,
  `TIM_ID` int(11) DEFAULT NULL,
  `ISV` int(1) DEFAULT '1',
  `STR` datetime DEFAULT NULL,
  `END` datetime DEFAULT NULL,
  `CYC` int(11) DEFAULT NULL,
  `CDY` bigint(20) UNSIGNED DEFAULT NULL,
  `NCY` datetime DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `VAL`
--

CREATE TABLE `VAL` (
  `ID` int(10) UNSIGNED NOT NULL,
  `VID` int(11) DEFAULT NULL,
  `DT` datetime DEFAULT NULL,
  `U_U` varchar(32) DEFAULT NULL,
  `OVR` int(1) DEFAULT NULL,
  `U_C` varchar(2000) DEFAULT NULL,
  `U_P` varchar(32) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `VER`
--

CREATE TABLE `VER` (
  `TXT` varchar(200) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `ALR`
--
ALTER TABLE `ALR`
  ADD PRIMARY KEY (`A_ID`),
  ADD KEY `ALR__A_ID` (`A_ID`),
  ADD KEY `ALR__A_UID` (`A_UID`),
  ADD KEY `ALR__A_PUID` (`A_PUID`(767)),
  ADD KEY `ALR__R_ID` (`R_ID`),
  ADD KEY `ALR__C_UID` (`C_UID`);

--
-- Indexes for table `LBL`
--
ALTER TABLE `LBL`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `LBL__ID` (`ID`),
  ADD KEY `LBL__LID` (`LID`),
  ADD KEY `LBL__LNG` (`LNG`);

--
-- Indexes for table `PACKS`
--
ALTER TABLE `PACKS`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `PACKS__UID` (`UID`(767));

--
-- Indexes for table `REL`
--
ALTER TABLE `REL`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `REL__ID` (`ID`),
  ADD KEY `REL__RID` (`RID`);

--
-- Indexes for table `SCR`
--
ALTER TABLE `SCR`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `SCR__ID` (`ID`),
  ADD KEY `SCR__SID` (`SID`),
  ADD KEY `SCR__S_UID` (`S_UID`);

--
-- Indexes for table `TIM`
--
ALTER TABLE `TIM`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `TIM__ID` (`ID`),
  ADD KEY `TIM__TIM_ID` (`TIM_ID`),
  ADD KEY `TIM__STR` (`STR`),
  ADD KEY `TIM__END` (`END`);

--
-- Indexes for table `VAL`
--
ALTER TABLE `VAL`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `VAL__ID` (`ID`),
  ADD KEY `VAL__VID` (`VID`),
  ADD KEY `VAL__U_U` (`U_U`),
  ADD KEY `VAL__U_P` (`U_P`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `ALR`
--
ALTER TABLE `ALR`
  MODIFY `A_ID` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `LBL`
--
ALTER TABLE `LBL`
  MODIFY `ID` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `PACKS`
--
ALTER TABLE `PACKS`
  MODIFY `ID` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `REL`
--
ALTER TABLE `REL`
  MODIFY `ID` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `SCR`
--
ALTER TABLE `SCR`
  MODIFY `ID` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `TIM`
--
ALTER TABLE `TIM`
  MODIFY `ID` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `VAL`
--
ALTER TABLE `VAL`
  MODIFY `ID` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
