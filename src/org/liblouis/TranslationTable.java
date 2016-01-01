package org.liblouis;

public enum TranslationTable {
  AFR_ZA_G1 ("Afrikaans"),
  BG ("Bulgarian"),
  BH ("Bihari"),
  BO ("Tibetan"),
  BOXES ("(box drawing characters)"),
  CA_G1 ("Catalan"),
  CHR_US_G1 ("Cherokee"),
  CKB_G1 ("Sorani (Kurdish)"),
  CS_G1 ("Czech"),
  CY_CY_G2 ("Welsh, Grade 2"),
  DA_DK_G26 ("Danish, Grade 2 (six dots)"),
  DA_DK_G28 ("Danish, Grade 2 (eight dots)"),
  DA_LT ("Danish, LogText (ISO-8859-1)"),
  DE_CHESS ("Chess, German"),
  DE_CH_G1 ("Swiss German, Grade 1"),
  DE_CH_G2 ("Swiss German, Grade 2"),
  DE_DE_COMP8 ("German (eight-dot computer braille)"),
  DE_DE_G1 ("German, Grade 1"),
  DE_DE_G2 ("German, Grade 2"),
  DRA ("Dravidian"),
  EN_ca ("English, Canada"),
  EN_CHESS ("Chess, English"),
  EN_GB_COMP8 ("English, UK (eight-dot computer braille)"),
  EN_gb_G2 ("English, UK Grade 2"),
  EN_IN_G1 ("English, India"),
  EN_UEB_G1 ("English, Unified, Grade 1"),
  EN_UEB_G2 ("English, Unified, Grade 2"),
  EN_US_COMP6 ("English, US (six-dot computer braille)"),
  EN_US_COMP8 ("English, US (eight-dot computer braille)"),
  EN_US_COMPBRL (""),
  EN_US_G1 ("English, US, Grade 1"),
  EN_US_G2 ("English, US, Grade 2"),
  EN_US_INTERLINE ("English, US, Grade 1"),
  EN_US_MATHTEXT ("English, US, Grade 1"),
  EO_G1 ("Esperanto"),
  EO_G1_X_SYSTEM ("Esperanto (X System)"),
  ES_G1 ("Spanish"),
  ET ("Estonian"),
  ETHIO_G1 ("Ethiopic, Grade 1"),
  FI1 ("Finnish (eight dots, similar to six dots)"),
  FI2 ("Finnish (eight dots, similar to six dots, enhanced)"),
  FI_FI_8DOT ("Finnish (eight dots)"),
  FI_FI ("Finnish (eight dots)"),
  FR_2007 ("French, Unified (2007)"),
  FR_BFU_G2 ("French, Unified, Grade 2"),
  fR_cA_G2 ("French, Canada, Grade 2"),
  fR_fR_G2 ("French, France, Grade 2"),
  GA_G2 ("Irish, Grade 2"),
  GD ("Gaelic"),
  GON ("Gondi"),
  GR_BB ("(Septuagint and Greek New Testament)"),
  HAW_US_G1 ("Hawaiian"),
  HE ("Hebrew"),
  HR ("Croatian"),
  HU_HU_COMP8 ("Hungarian (eight-dot computer braille)"),
  HU_HU_G1 ("Hungarian, Grade 1"),
  HY ("Armenian"),
  IS ("Icelandic"),
  IU_CA_G1 ("Inuktitut"),
  KO_2006_G1 ("Korean, Grade 1 (2006)"),
  KO_2006_G2 ("Korean, Grade 2 (2006)"),
  KO_G1 ("Korean, Grade 1"),
  KO_G2 ("Korean, Grade 2"),
  KOK ("Konkani"),
  KRU ("Kurukh"),
  LT ("Lituanian"),
  MAO_NZ_G1 ("Maori"),
  MARBURG ("Mathematics, Marburg"),
  MARBURG_EDIT ("Mathematics, Marburg (for post-translation editing)"),
  MT ("Maltese"),
  MUN ("Munda"),
  MWR ("Marwari"),
  NE ("Nepali"),
  NEMETH ("Mathematics, Nemeth"),
  NEMETH_EDIT ("Mathematics, Nemeth (for post-translation editing)"),
  NL_be_G1 ("Dutch, Belgium"),
  NL_nl_G1 ("Dutch, Netherlands"),
  NO_NO ("Norwegian (based on the Offentlig Utvalg for Blindeskrift [Public Commission for Braille] translation table for CP1252)"),
  NO_NO_G1 ("Norwegian, Level 1"),
  NO_NO_G2 ("Norwegian, Level 2"),
  NO_NO_G3 ("Norwegian, Level 3"),
  NO_NO_GENERIC ("Norwegian (includes some unofficial character representations to accommodate multilingual usage)"),
  PI ("Pali"),
  PL_PL_COMP8 ("Polish (eight-dot computer braille)"),
  PT_PT_COMP8 ("Portuguese (eight-dot computer braille)"),
  PT_PT_G2 ("Portuguese, Grade 2"),
  RO ("Romanian"),
  RU_COMPBRL (""),
  RU ("Russian"),
  RU_LITBRL ("Russian, Literary"),
  SE_SE ("Swedish (1996)"),
  SL_SI_COMP8 ("Slovenian (eight-dot computer braille)"),
  SOT_ZA_G1 ("Sotho"),
  SPACES ("(space characters)"),
  SR_G1 ("Serbian"),
  SV_1989 ("Swedish (1989)"),
  SV_1996 ("Swedish (1996)"),
  TA ("Tamil"),
  TA_TA_G1 ("Tamil, Grade 1"),
  TR ("Turkish"),
  TSN_ZA_G1 ("Tswana"),
  uebc_G2 ("English, Unified, Grade 2"),
  UKMATHS ("Mathematics, UK"),
  UKMATHS_EDIT ("Mathematics, UK (for post-translation editing)"),
  VI ("Vietnamese"),
  VI_G1 ("Vietnamese, Grade 1"),
  WISKUNDE ("Mathematics, Flemish (Woluwe)"),
  ZH_HK ("Chinese, Hong Kong, Cantonese"),
  ZH_TW ("Chinese, Taiwan"),
  ;

  public final static String SUBDIRECTORY = "tables";
  public final static String EXTENSION = ".ctb";

  private final String tableDescription;

  private String fileName = null;

  public final String getDescription () {
    return tableDescription;
  }

  public final String getFileName () {
    synchronized (this) {
      if (fileName == null) {
        fileName = name().replace('_', '-').toLowerCase() + EXTENSION;
      }
    }

    return fileName;
  }

  TranslationTable (String description) {
    tableDescription = description;
  }
}
