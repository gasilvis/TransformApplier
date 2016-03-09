//---------------------------------------------------------------------------
/*
    Copyright 2008, 2009, 2014  George A Silvis

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <IniFiles.hpp>
#include <stdio.h>
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElastFrm"
#pragma link "AdPort"
#pragma link "AdWnPort"
#pragma link "OoMisc"
#pragma link "HttpProt"
#pragma link "LibXmlComps"
#pragma link "LibXmlParser"
// http://www.destructor.de/xmlparser/doc/xmlscanner.htm
#pragma resource "*.dfm"
TForm1 *Form1;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
   : TForm(Owner)
{

}
//---------------------------------------------------------------------------

#define Version  2.47
// if you change the version, change it too in  TAlog.php
/*
   2.47
   - hints for observation location
   - default AMASS fixed. na case inherited old data
   2.46
   - not finding target in VSX downgraded to warning
   2.45
   - httpget does url encoding (+ sign needs encoding)
   - fail on VSX call if no data available
   - show location when extinction applied
   - string terminate the httpget capture.
   2.44
   - add GROUPs to aggregation critieria
   2.43
   - error in sexigesimal to radians
   - improve xml parsing of VSP data
   - prevent restart of process
   2.42
   - tweak to new api for std_field 
   2.41
   - fix call to new api
   2.40
   - chart data dump avail on button 1
   - use new API for AAVSO photometry
   2.39
   - report Vairmass for AMASS if extinction applied
   - edit of Tb_bv equation description
   - TRANS changed from bool to char
   - TRANS records will aggregate.... not sure its a good idea.
       this allows post transform aggregation. Comment field gets flooded.
   2.38
   - extinction turned back on and uses computed airmasses
   - search backwards for CREFMAG, KREFMAG information
   - capture #BVCOLOR and #VICOLOR information
   - capture colors for later observations
   - B V R and I single filter transforms supported
   2.37
  - exe from GitHub
  - long comments
  - add VX, CX, KX to comments
  - TAlog only once
  - don't compute airmass if obslat/lon not set
  - Test TC output: add VMAGinst
  - if AMASS na, use Vairmass if available
   2.36
  - tidy up httpGet routine
  - add airmass computations using VSX for the star
  - fix bug in output of AMASS= na
   2.35
  - ensemble notes
  - pull comments added until WebObs can handle
   2.34
  - DSLR checkbox. Overrides #OBSTYPE
  - if DSLR, map TG, TB and TR to V, B, R

   2.33
  - version reminder and download
  - delim's clarified
  - #KREFMAG allowed. #CREFMAG can have C or C and K
  - Comp Ref error excluded from final error computation
  - comment field stuffed with support information including TA version
  - Help updated
  -
*/

bool Debug= false; // turn of debug msgs

/* adding a coefficient:
   1. add variable
   2. add screen label, edit box, tab sequence  nb edit box must be <name>"Edit"
   3. add INI read reference
   4. add to the output
   5. add to the documentation
   */

   // filter information
/*  to add:
       add FILT_xxi, FILT_xxx, FILT_mask, FILT_NUM
*/
// filter indices
enum {
    FILT_Bi= 0
   ,FILT_Vi
   ,FILT_Ri
   ,FILT_Ii
   ,FILT_Ui
   ,FILT_NUM
};

char b= 'b', v= 'v', r= 'r', i= 'i', u= 'u';

enum { // mask values
   FILT_Bx= 0x01
  ,FILT_Vx= 0x02
  ,FILT_Rx= 0x04
  ,FILT_Ix= 0x08
  ,FILT_Ux= 0x10
};

// to make mask values avail by index
char  FILT_mask[]= {FILT_Bx, FILT_Vx, FILT_Rx, FILT_Ix, FILT_Ux};
AnsiString FILT_name[]= {"B", "V", "R", "I", "U"};
char FILTname[]= "BVRIU";

// indices from sd; which obs is providing data for each filter
short sf[FILT_NUM];

enum {
    METHOD_Classic= 0x2000
   ,METHOD_AAVSO  = 0x4000
   ,METHOD_Special= 0x8000  // aka alternate
};


// structure for the gui elements for the coefficients
typedef struct {
   float    *value;
   float    *error;
   char     *name;
   char     *coefhint;
   TCheckBox   *coeflab;
   TEdit    *coefedit;
   TEdit    *erroredit;
} Coef;

// RAM copies of the coefficients
float Tub, Tbv, Tbr,   Tbi,   Tvr, Tvi,   Tri
     ,Tu_ub, Tb_bv,  Tb_br, Tb_bi, Tv_vr,  Tv_vi, Tr_ri
     ,Tv_bv, Tr_bv, Ti_bv,  Tr_vr, Tr_vi, Ti_vi   // new
     ,Eb, Ev, Er, Ei, Eu;   // filter extinction coef
// errors for the above
float rTub, rTbv, rTbr,   rTbi,   rTvr, rTvi,   rTri
     ,rTu_ub, rTb_bv,  rTb_br, rTb_bi, rTv_vr,  rTv_vi, rTr_ri
     ,rTv_bv, rTr_bv, rTi_bv, rTr_vr, rTr_vi, rTi_vi
     ,rEb, rEv, rEr, rEi, rEu;

// global vars for equations
float  Us,  us,  Bs,  bs,  Vs,  vs,  Rs,  rs,  Is,  is,   Uc,  uc,  Bc,  bc,  Vc,  vc,  Rc,  rc,  Ic,  ic,  oUs,  oBs,  oVs,  oRs,  oIs;
float rUs, rus, rBs, rbs, rVs, rvs, rRs, rrs, rIs, ris,  rUc, ruc, rBc, rbc, rVc, rvc, rRc, rrc, rIc, ric, roUs, roBs, roVs, roRs, roIs;
AnsiString Uts, Bts, Vts, Rts, Its;  // DATEs of the star providing this data
char Utdesc[200], Btdesc[200], Vtdesc[200], Rtdesc[200], Itdesc[200]; // name and values of the transforms used, ready for NOTES

float* Extinction[]= { &Eb, &Ev, &Er, &Ei, &Eu }; // filter order
float* ExtinctionErr[]= { &rEb, &rEv, &rEr, &rEi, &rEu }; // filter order

Coef TC[]= {
    {&Tbv,&rTbv,    "Tbv",   "1/slope of (b-v) vs (B-V)"}
   ,{&Tbr,&rTbr,    "Tbr",   "1/slope of (b-r) vs (B-R)"}
   ,{&Tbi,&rTbi,    "Tbi",   "1/slope of (b-i) vs (B-I)"}
   ,{&Tvr,&rTvr,    "Tvr",   "1/slope of (v-r) vs (V-R)"}
   ,{&Tvi,&rTvi,    "Tvi",   "1/slope of (v-i) vs (V-I)"}
   ,{&Tri,&rTri,    "Tri",   "1/slope of (r-i) vs (R-I)"}
   ,{&Tb_bv,&rTb_bv,"Tb_bv", "slope of (B-b) vs (B-V)"}
   ,{&Tb_br,&rTb_br,"Tb_br", "slope of (B-b) vs (B-R)"}
   ,{&Tb_bi,&rTb_bi,"Tb_bi", "slope of (B-b) vs (B-I)"}
   ,{&Tv_vr,&rTv_vr,"Tv_vr", "slope of (V-v) vs (V-R)"}
   ,{&Tv_vi,&rTv_vi,"Tv_vi", "slope of (V-v) vs (V-I)"}
   ,{&Tr_ri,&rTr_ri,"Tr_ri", "slope of (R-r) vs (R-I)"}
   ,{&Tv_bv,&rTv_bv,"Tv_bv", "slope of (V-v) vs (B-V)"}
   ,{&Tr_bv,&rTr_bv,"Tr_bv", "slope of (R-r) vs (B-V)"}
   ,{&Ti_bv,&rTi_bv,"Ti_bv", "slope of (I-i) vs (B-V)"}
   ,{&Tr_vr,&rTr_vr,"Tr_vr", "slope of (R-r) vs (V-R)"}
   ,{&Tr_vi,&rTr_vi,"Tr_vi", "slope of (R-r) vs (V-I)"}
   ,{&Ti_vi,&rTi_vi,"Ti_vi", "slope of (I-i) vs (V-I)"}
   ,{&Tub,&rTub  ,  "Tub",   "1/slope of (u-b) vs (U-B)"}
   ,{&Tu_ub,&rTu_ub,"Tu_ub", "slope of (U-u) vs (U-B)"}
   };

#define NumTCoef (sizeof(TC)/sizeof(Coef))

Coef EC[]= { // matches filter enum  eg FILT_Bi
    {&Eb, &rEb,"Eb", "extinction of B filter"}
   ,{&Ev, &rEv,"Ev", "extinction of V filter"}
   ,{&Er, &rEr,"Er", "extinction of R filter"}
   ,{&Ei, &rEi,"Ei", "extinction of I filter"}
   ,{&Eu, &rEu,"Eu", "extinction of U filter"}
};

#define NumECoef (sizeof(EC)/sizeof(Coef))

bool TAlogCalled= false;

void __fastcall TForm1::FormCreate(TObject *Sender)
{
   AnsiString s;
   char cp[1000];  
   float cver;

   Form1->Caption= "TA, the AAVSO Transform Applier application, "+ FormatFloat("version 0.00", Version);

   if(httpGet("http://www.gasilvis.com/TA/TAlog.php", cp, sizeof(cp))) {
      sscanf(cp, "%f", &cver); 
      if(cver > Version+ 0.001) {
         //versionLabel->Tag= 1;
         versionLabel->Font->Color= clBlue;
         versionLabel->Caption= s.sprintf("Click here to download version %.2f", cver);
      } else {
         versionLabel->Caption= s.sprintf("%.2f is the latest version of TA", cver);
      }
   }

   //nb  The labels are now checkboxes
   // have to assign here because they don't exist until here
   TC[0].coeflab= Label1,   TC[0].coefedit= TbvEdit;  TC[0].erroredit= rTbvEdit;
   TC[1].coeflab= Label11,  TC[1].coefedit= TbrEdit;  TC[1].erroredit= rTbrEdit;
   TC[2].coeflab= Label12,  TC[2].coefedit= TbiEdit;  TC[2].erroredit= rTbiEdit;
   TC[3].coeflab= Label2,   TC[3].coefedit= TvrEdit;  TC[3].erroredit= rTvrEdit;
   TC[4].coeflab= Label10,  TC[4].coefedit= TviEdit;  TC[4].erroredit= rTviEdit;
   TC[5].coeflab= Label3,   TC[5].coefedit= TriEdit;  TC[5].erroredit= rTriEdit;

   TC[6].coeflab= Label9,   TC[6].coefedit= Tb_bvEdit;  TC[6].erroredit= rTb_bvEdit;
   TC[7].coeflab= Label13,  TC[7].coefedit= Tb_brEdit;  TC[7].erroredit= rTb_brEdit;
   TC[8].coeflab= Label14,  TC[8].coefedit= Tb_biEdit;  TC[8].erroredit= rTb_biEdit;
   TC[9].coeflab= Label4,   TC[9].coefedit= Tv_vrEdit;  TC[9].erroredit= rTv_vrEdit;
   TC[10].coeflab= Label8,  TC[10].coefedit= Tv_viEdit; TC[10].erroredit= rTv_viEdit;
   TC[11].coeflab= Label5,  TC[11].coefedit= Tr_riEdit; TC[11].erroredit= rTr_riEdit;

   TC[12].coeflab= CheckBox1,  TC[12].coefedit= Tv_bvEdit;  TC[12].erroredit= rTv_bvEdit;
   TC[13].coeflab= CheckBox2,  TC[13].coefedit= Tr_bvEdit;  TC[13].erroredit= rTr_bvEdit;
   TC[14].coeflab= CheckBox3,  TC[14].coefedit= Ti_bvEdit;  TC[14].erroredit= rTi_bvEdit;
   TC[15].coeflab= CheckBox9,  TC[15].coefedit= Tr_vrEdit;  TC[15].erroredit= rTr_vrEdit;
   TC[16].coeflab= CheckBox10, TC[16].coefedit= Tr_viEdit;  TC[16].erroredit= rTr_viEdit;
   TC[17].coeflab= CheckBox11, TC[17].coefedit= Ti_viEdit;  TC[17].erroredit= rTi_viEdit;
   TC[18].coeflab= CheckBox13, TC[18].coefedit= TubEdit;  TC[18].erroredit= rTubEdit;
   TC[19].coeflab= CheckBox12, TC[19].coefedit= Tu_ubEdit;  TC[19].erroredit= rTu_ubEdit;

   EC[0].coeflab= CheckBox4,  EC[0].coefedit= EbEdit;   EC[0].erroredit= rEbEdit;
   EC[1].coeflab= CheckBox5,  EC[1].coefedit= EvEdit;   EC[1].erroredit= rEvEdit;
   EC[2].coeflab= CheckBox6,  EC[2].coefedit= ErEdit;   EC[2].erroredit= rErEdit;
   EC[3].coeflab= CheckBox7,  EC[3].coefedit= EiEdit;   EC[3].erroredit= rEiEdit;
   EC[4].coeflab= CheckBox8,  EC[4].coefedit= EuEdit;   EC[4].erroredit= rEuEdit;
   ReadCoefficients(Sender);
}

AnsiString INIfilename= "";

void __fastcall TForm1::ReadCoefficients(TObject *Sender)
{
   // collect INI file entries
   TIniFile *ini;
   if(0==INIfilename.Length()) { // first time
      INIfilename= ChangeFileExt( Application->ExeName, ".INI");
      ini= new TIniFile(INIfilename);
      INIfilename= ini->ReadString("Setup", "INI", INIfilename);
      delete ini;
   }
   ini = new TIniFile(INIfilename );
   AnsiString as;

   setupEdit->Text= ini->ReadString("Setup", "description", "Describe the setup that has the coefficients below");
   setupEdit->Hint= INIfilename;
   applyExtinction->Checked= ini->ReadBool("Extinction", "apply", false);
   DSLRcb->Checked= ini->ReadBool("Setup", "DSLR", false);
   ObsLonEdit->Text= ini->ReadString("Setup", "ObsLon", "0");  ObsLonEditExit(Sender);
   ObsLatEdit->Text= ini->ReadString("Setup", "ObsLat", "0");  ObsLatEditExit(Sender);


   for(int i= 0; i< NumTCoef; i++) {
      as.sprintf("%-8s %s", TC[i].name, TC[i].coefhint);
      TC[i].coeflab->Caption= as; TC[i].name;
      TC[i].coeflab->Hint= TC[i].coefhint;
      TC[i].coefedit->Hint= TC[i].coefhint;
      *TC[i].value= ini->ReadFloat("Coefficients", TC[i].name, 0.0);
      TC[i].coefedit->Text= FormatFloat("0.0000", *TC[i].value);
      *TC[i].error= ini->ReadFloat("Error", TC[i].name, 0.0);
      TC[i].erroredit->Text= FormatFloat("0.0000", *TC[i].error);
   }

   for(int i= 0; i< FILT_NUM; i++) {
      as.sprintf("%-8s %s", EC[i].name, EC[i].coefhint);
      EC[i].coeflab->Caption= as; EC[i].name;
      EC[i].coeflab->Hint= EC[i].coefhint;
      EC[i].coefedit->Hint= EC[i].coefhint;
      *EC[i].value= ini->ReadFloat("Extinction", EC[i].name, 0.0);
      EC[i].coefedit->Text= FormatFloat("0.0000", *EC[i].value);
      *EC[i].error= ini->ReadFloat("ExtinctionError", EC[i].name, 0.0);
      EC[i].erroredit->Text= FormatFloat("0.0000", *EC[i].error);
   }

   OpenDialog1->InitialDir= ini->ReadString("Setup", "Dir", "");
   delete ini;
}

void __fastcall TForm1::changeINIfileClick(TObject *Sender)
{
   TIniFile *ini;
   OpenDialog1->Title= "select an alternative INI file";
   OpenDialog1->InitialDir= ".";
   OpenDialog1->Filter = "INI files (*.ini)|*.INI";
   if(OpenDialog1->Execute()){
      INIfilename= OpenDialog1->FileName;
      // set name in reference ini
      ini= new TIniFile(ChangeFileExt( Application->ExeName, ".INI"));
      ini->WriteString("Setup", "INI", INIfilename);
      delete ini;
      ReadCoefficients(Sender);
   };
}
//---------------------------------------------------------------------------

void __fastcall TForm1::saveasINIfileClick(TObject *Sender)
{
   TIniFile *ini;
   SaveDialog1->Title= "save as an INI file";
   SaveDialog1->InitialDir= ".";
   SaveDialog1->Filter = "INI files (*.ini)|*.INI";
   if(SaveDialog1->Execute()){
      Memo4->Clear(); // use Memo4 as scratch
      Memo4->Lines->LoadFromFile(INIfilename);
      INIfilename= SaveDialog1->FileName;
      Memo4->Lines->SaveToFile(INIfilename);
      Memo4->Clear();
      // set name in reference ini
      ini= new TIniFile(ChangeFileExt( Application->ExeName, ".INI"));
      ini->WriteString("Setup", "INI", INIfilename);
      delete ini;
      ReadCoefficients(Sender);
   };
}
//---------------------------------------------------------------------------

// handle vetting of coefficient entries
void __fastcall TForm1::NumericOnExit(TObject *Sender)
{
   TEdit *p;
   float f;
   if((p=dynamic_cast<TEdit*>(Sender))==NULL)
       return;
   try {
      p->Text= FloatToStr(f= StrToFloat(p->Text));
      // save the value
      TIniFile *ini= new TIniFile(INIfilename);
      if('r'== p->Name[1])
         if('T'== p->Name[2])
            ini->WriteFloat("Error", p->Name.SubString(2, p->Name.Pos("Edit")-2), f);
         else
            ini->WriteFloat("ExtinctionError", p->Name.SubString(2, p->Name.Pos("Edit")-2), f);
      else
         if('T'== p->Name[1])
            ini->WriteFloat("Coefficients", p->Name.SubString(1, p->Name.Pos("Edit")-1), f);
         else
            ini->WriteFloat("Extinction", p->Name.SubString(1, p->Name.Pos("Edit")-1), f);
      delete ini;
      ReadCoefficients(Sender); // resets ram data
   } catch (EConvertError &error) {
      p->SetFocus();
      ShowMessage("This field must be numeric");
   }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::setupEditExit(TObject *Sender)
{
      TIniFile *ini= new TIniFile(INIfilename);
      ini->WriteString("Setup", "description", setupEdit->Text);
      delete ini;
      ReadCoefficients(Sender); // resets ram data
}
//---------------------------------------------------------------------------
void __fastcall TForm1::applyExtinctionExit(TObject *Sender)
{
      TIniFile *ini= new TIniFile(INIfilename);
      ini->WriteBool("Extinction", "apply", applyExtinction->Checked);
      delete ini;
      ReadCoefficients(Sender); // resets ram data
}
//---------------------------------------------------------------------------

// db to hold transform group information
typedef struct GroupData {
   AnsiString Group;
   char       FILTC; // filter combination (an enum)
   float      m[20]; // Star UBVRI, ubvri   Comp UBVRI ubvri
   AnsiString  ts[5];  // tstamp strings of providers of U B V R I data
} GroupData;
#define gdiMAX 1000
GroupData gd[gdiMAX];



/*  example   of file

#ZEROPOINT= 25.000
#VERR is V(standard deviation) based on user-supplied parameters.
#CREFMAG= 10.006
#NOW=4/26/2008 3:25:53 PM

#NAME;DATE;VMAG;VERR;FILT;TRANS;MTYPE;CNAME;CMAG;KNAME;KMAG;AMASS;GROUP;CHART;NOTES

ParameterKeywords
#TYPE=Extended
#OBSCODE=
#SOFTWARE=
#DELIM=
#DATE=
#OBSTYPE=

#NAME;DATE;VMAG;VERR;FILT;TRANS;MTYPE;CNAME;CMAG;KNAME;KMAG;AMASS;GROUP;CHART;NOTES
Z BOO;2454572.64069;9.332;0.002;V;NO;ABS;000-BBV-178;17.591;000-BBV-175;18.627;1.2606;0;080415;

*/


typedef struct StarData { // eg
   AnsiString  record; // line from the submitted obs file
   AnsiString  narr;  // narrative
   AnsiString  recordT; // line to the output file
   int         gdi;   // index into the GD array //deprecated
   GroupData   gd;
   AnsiString  NAME;  // Z BOO
   double      DATE;  // 2454572.64069
   AnsiString  DATEs;
   float       VMAGraw; // from the file
   float       VMAGinst;  // corrected for zero point  instrumental mag
   float       VMAGex;  // corrected for extinction
   float       VMAG;    // final
   float       VMAGt; // transformed magnitude
   float       VMAGrep;
   float       VERR;    // raw
   float       VERRt;   // transformed
   AnsiString  FILT;  // U: Johnson U,  B: Johnson B  etc
   char        TRANS; // 0= no, 1= already transformed, 2= transformed by TA
   char        MTYPE; // A for ABS, D for DIF, S for STD
   AnsiString  CNAME; // 000-BBV-178
   float       CMAGraw; // from the file
   float       CMAGex;  // corrected for extinction
   float       CMAG;    // final
   float       CERR;
   AnsiString  KNAME; // 000-BBV-175
   float       KMAGraw;  // 18.627
   float       AMASS; // 1.2606
   bool        AMASSna;
   AnsiString  GROUPs; // 0
   AnsiString  CHART; // 080415
   AnsiString  NOTES;
   double      VRA;
   double      VDec;
   float       Vairmass;
   AnsiString  varType;
   AnsiString  specType;

   bool        processed;
   char        filter;// index into filter list
   float       CREFmag; // ref mag of the comparison star
   float       CREFerr;
   double      CRA;
   double      CDec;
   float       Cairmass;
   float       KREFmag;
   float       KREFerr;
   double      KRA;
   double      KDec;
   float       Kairmass;
   unsigned short FILTC; // filter combo the star is mixed with
   AnsiString  StarsUsed;
   AnsiString  ErrorMsg;   // collect comments here to be displayed after the obs is printed
   bool        extinctionApplied;
   bool        ensemble;
   float       BVcolor;
   float       BVcolorErr;
   float       VIcolor;
   float       VIcolorErr;

} StarData;
#define sdiMAX   1000
StarData sd[sdiMAX];
int sdi;  // count of entries in sd.

void SetStarData(StarData *d, int index);
void ProcessStarData(StarData *d, unsigned short fc);


// stuff for fetching std data from the web
typedef struct  {
    float   mag;
    float   err;
} sdatas;

typedef struct  {
   AnsiString  chartid;
   AnsiString  AUID;
   AnsiString  label;
   sdatas      sdata[FILT_NUM];
   double      RA;  // radians
   double      Dec; // radians
} chartrow;
#define chartMAX   1000
chartrow chart[chartMAX];
int chartNext= 0;
int __fastcall checkChart(StarData* sd, bool getc);
int __fastcall getREFMAG(StarData* sd, bool getc);
float __fastcall AirMass(double JD, double RA, double Dec); // Observatory location is global

typedef struct {
   AnsiString   name;
   double       RA;
   double       Dec;
   AnsiString   varType;
   AnsiString   specType;
} targrow;
#define targMAX  100
targrow targs[targMAX];
int targNext= 0;
int __fastcall checkTargs(StarData* sd);
int __fastcall getTargInfo(StarData* sd);


/* too add a filter combination
1.  add name to enum of Filter combinations
2.  add mask value combination to enum of mask values
3.  add this mask value to the FILTC_mask[]
4.  add a description of the formula to FILTC_desc
5.  add a case to ProcessStarData
6.  update help file
*/

enum { // mask values
    FILTC_BVRIc=   (FILT_Bx|FILT_Vx|FILT_Rx|FILT_Ix) | METHOD_Classic
   ,FILTC_BVc=     (FILT_Bx|FILT_Vx)                 | METHOD_Classic
   ,FILTC_VRc=     (FILT_Vx|FILT_Rx)                 | METHOD_Classic
   ,FILTC_VRIc=    (FILT_Vx|FILT_Rx|FILT_Ix)         | METHOD_Classic
   ,FILTC_BVRc=    (FILT_Bx|FILT_Vx|FILT_Rx)         | METHOD_Classic
   ,FILTC_VIc=     (FILT_Vx|FILT_Ix)                 | METHOD_Classic
   ,FILTC_BVIc=    (FILT_Bx|FILT_Vx|FILT_Ix)         | METHOD_Classic

   ,FILTC_BVRIs=   (FILT_Bx|FILT_Vx|FILT_Rx|FILT_Ix) | METHOD_Special
   ,FILTC_BVs=     (FILT_Bx|FILT_Vx)                 | METHOD_Special
   ,FILTC_VRs=     (FILT_Vx|FILT_Rx)                 | METHOD_Special
   ,FILTC_VRIs=    (FILT_Vx|FILT_Rx|FILT_Ix)         | METHOD_Special
   ,FILTC_BVRs=    (FILT_Bx|FILT_Vx|FILT_Rx)         | METHOD_Special
   ,FILTC_VIs=     (FILT_Vx|FILT_Ix)                 | METHOD_Special
   ,FILTC_BVIs=    (FILT_Bx|FILT_Vx|FILT_Ix)         | METHOD_Special

   ,FILTC_BVRIa=   (FILT_Bx|FILT_Vx|FILT_Rx|FILT_Ix) | METHOD_AAVSO
   ,FILTC_BVa=     (FILT_Bx|FILT_Vx)                 | METHOD_AAVSO
   ,FILTC_VRa=     (FILT_Vx|FILT_Rx)                 | METHOD_AAVSO
   ,FILTC_VRIa=    (FILT_Vx|FILT_Rx|FILT_Ix)         | METHOD_AAVSO
   ,FILTC_BVRa=    (FILT_Bx|FILT_Vx|FILT_Rx)         | METHOD_AAVSO
   ,FILTC_VIa=     (FILT_Vx|FILT_Ix)                 | METHOD_AAVSO
   ,FILTC_BVIa=    (FILT_Bx|FILT_Vx|FILT_Ix)         | METHOD_AAVSO

   ,FILTC_UBVRIa=  (FILT_Ux|FILT_Bx|FILT_Vx|FILT_Rx|FILT_Ix) | METHOD_AAVSO
   ,FILTC_UBVIa=   (FILT_Ux|FILT_Bx|FILT_Vx|FILT_Ix)         | METHOD_AAVSO
   ,FILTC_UBVa=    (FILT_Ux|FILT_Bx|FILT_Vx)                 | METHOD_AAVSO
   ,FILTC_UBVRIs=  (FILT_Ux|FILT_Bx|FILT_Vx|FILT_Rx|FILT_Ix) | METHOD_Special
   ,FILTC_UBVIs=   (FILT_Ux|FILT_Bx|FILT_Vx|FILT_Ix)         | METHOD_Special
   ,FILTC_UBVs=    (FILT_Ux|FILT_Bx|FILT_Vx)                 | METHOD_Special
   ,FILTC_Ba=      (FILT_Bx)         | METHOD_AAVSO
   ,FILTC_Va=      (FILT_Vx)         | METHOD_AAVSO
   ,FILTC_Ra=      (FILT_Rx)         | METHOD_AAVSO
   ,FILTC_Ia=      (FILT_Ix)         | METHOD_AAVSO

// add here
};


// this is the list actually supported. it matches masks to the FILTC_desc
unsigned short FILTC_mask[]= {FILTC_BVRIc, FILTC_BVc, FILTC_VRc, FILTC_VRIc, FILTC_BVRc, FILTC_VIc, FILTC_BVIc
                    ,FILTC_BVRIs, FILTC_BVs, FILTC_VRs, FILTC_VRIs, FILTC_BVRs, FILTC_VIs, FILTC_BVIs
                    ,FILTC_BVRIa, FILTC_BVa, FILTC_VRa, FILTC_VRIa, FILTC_BVRa, FILTC_VIa, FILTC_BVIa
                    ,FILTC_UBVRIa ,FILTC_UBVIa ,FILTC_UBVa ,FILTC_UBVRIs ,FILTC_UBVIs ,FILTC_UBVs
                    ,FILTC_Ba, FILTC_Va, FILTC_Ra, FILTC_Ia
       };
#define FILTC_NUM (sizeof(FILTC_mask)/2)

int __fastcall FILTC_mask2index(unsigned short mask) {
            int k;
            for(k= 0; k< FILTC_NUM; k++)
               if(mask==FILTC_mask[k]) break;
            return k;
}


#define FILTC_desc_rows 7
AnsiString FILTC_desc[FILTC_NUM][FILTC_desc_rows]= {
    {   // FILTC_BVRIc
     "# BVRI  classic as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comp. Capital filter is ref/transforme, lower case is obs"
    ,"# (Rs - Is) = (Rc - Ic) + Tri * [(rs - is) - (rc - ic)]"
    ,"# Rs = rs + (Rc - rc) + Tr_ri * [(Rs - Is) - (Rc - Ic)], using the solution for (Rs - Is) in the above line"
    ,"# Is = Rs - (Rs - Is), using Rs from the line above, and (Rs - Is) from the first line"
    ,"# Vs = vs + (Vc - vc) + Tv_vr * Tvr * [(vs - rs) - (vc - rc)]"
    ,"# Bs = Vs + (Bc - Vc) + Tbv * [(bs - vs) - (bc - vc)]"
   }
   ,
   {   // FILTC_BVc
     "# BV    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"# (Bs - Vs) = (Bc - Vc) + Tbv * [(bs - vs) - (bc - vc)]"
    ,"# Bs = bs + (Bc - bc) + Tb_bv * [(Bs - Vs) - (Bc - Vc)], using the solution for (Bs - Vs) in the above line"
    ,"# Vs = Bs - (Bs - Vs), using Bs from the line above, and (Bs - Vs) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_VRc
     "# VR    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"# (Vs - Rs) = (Vc - Rc) + Tvr * [(vs - rs) - (vc - rc)]"
    ,"# Vs = vs + (Vc - vc) + Tv_vr * [(Vs - Rs) - (Vc - Rc)], using the solution for (Vs - Rs) in the above line"
    ,"# Rs = Vs - (Vs - Rs), using Vs from the line above, and (Vs - Rs) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_VRIc
     "# VRI    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"# (Rs - Is) = (Rc - Ic) + Tri * [(rs - is) - (rc - ic)]"
    ,"# Rs = rs + (Rc - rc) + Tr_ri * [(Rs - Is) - (Rc - Ic)], using the solution for (Rs - Is) in the above line"
    ,"# Is = Rs - (Rs - Is), using Rs from the line above, and (Rs - Is) from the first line"
    ,"# Vs = vs + (Vc - vc) + Tv_vr * Tvr * [(vs - rs) - (vc - rc)]"
    ,NULL
   }
   ,
   {   // FILTC_BVRc
     "# BVR    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"# (Vs - Rs) = (Vc - Rc) + Tvr * [(vs - rs) - (vc - rc)]"
    ,"# Vs = vs + (Vc - vc) + Tv_vr * [(Vs - Rs) - (Vc - Rc)], using the solution for (Vs - Rs) in the above line"
    ,"# Rs = Vs - (Vs - Rs), using Vs from the line above, and (Vs - Rs) from the first line"
    ,"# Bs = Vs + (Bc - Vc) +  Tbv * [(bs - vs) - (bc - vc)]"
    ,NULL
   }
   ,
   {   // FILTC_VIc
     "# VI    modeled on the VR formulation "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"# (Vs - Is) = (Vc - Ic) + Tvi * [(vs - is) - (vc - ic)]"
    ,"# Vs = vs + (Vc - vc) + Tv_vi * [(Vs - Is) - (Vc - Ic)], using the solution for (Vs - Is) in the above line"
    ,"# Is = Vs - (Vs - Is), using Vs from the line above, and (Vs - Is) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_BVIc
     "# BVI   modeled on the BVR formulation"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"# (Vs - Is) = (Vc - Ic) + Tvi * [(vs - is) - (vc - ic)]"
    ,"# Vs = vs + (Vc - vc) + Tv_vi * [(Vs - Is) - (Vc - Ic)], using the solution for (Vs - Is) in the above line"
    ,"# Is = Vs - (Vs - Is), using Vs from the line above, and (Vs - Is) from the first line"
    ,"# Bs = Vs + (Bc - Vc) +  Tbv * [(bs - vs) - (bc - vc)]"
    ,NULL
   }

/*
   ,
   {   // FILTC_BVRIs
     "# BVRI alternate "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri* ((rs-is)-(rc-ic))"
    ,NULL
   }
*/
   ,
   {   // FILTC_BVRIs
     "# BVRI alternate "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Bs=  Vs + (Bc-Vc) + Tbv * ((bs-vs)-(bc-vc))"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri* ((rs-is)-(rc-ic))"
    ,NULL
   }
   ,
   {   // FILTC_BVs  same as c
     "# BV    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"# (Bs - Vs) = (Bc - Vc) + Tbv * [(bs - vs) - (bc - vc)]"
    ,"# Bs = bs + (Bc - bc) + Tb_bv * [(Bs - Vs) - (Bc - Vc)], using the solution for (Bs - Vs) in the above line"
    ,"# Vs = Bs - (Bs - Vs), using Bs from the line above, and (Bs - Vs) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_VRs  same as c
     "# VR    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"# (Vs - Rs) = (Vc - Rc) + Tvr * [(vs - rs) - (vc - rc)]"
    ,"# Vs = vs + (Vc - vc) + Tv_vr * [(Vs - Rs) - (Vc - Rc)], using the solution for (Vs - Rs) in the above line"
    ,"# Rs = Vs - (Vs - Rs), using Vs from the line above, and (Vs - Rs) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {  // FILTC_VRIs
     "# VRI alternate "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Vs = vs + (Vc-vc) + Tv_vr * ((Vs-Rs)-(Vc-Rc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic))"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_BVRs
     "# BVR  alternate "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc))"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_VIs   same as classic
     "# VI    modeled on the VR formulation "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"# (Vs - Is) = (Vc - Ic) + Tvi * [(vs - is) - (vc - ic)]"
    ,"# Vs = vs + (Vc - vc) + Tv_vi * [(Vs - Is) - (Vc - Ic)], using the solution for (Vs - Is) in the above line"
    ,"# Is = Vs - (Vs - Is), using Vs from the line above, and (Vs - Is) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_BVIs
     "# BVI alternate "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc))"
    ,"#  Is = Vs - (Vc-Ic) - Tvi* ((vs-is)-(vc-ic))"
    ,NULL
    ,NULL
   }


   ,
   {   // FILTC_BVRIa
     "# BVRI AAVSO recommended "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Rs = rs + (Rc-rc) + Tr_vi * ((Vs-Is)-(Vc-Ic))"
    ,"#  Is = is + (Ic-ic) + Ti_vi * ((Vs-Is)-(Vc-Ic))"
    ,NULL
   }
   ,
   {   // FILTC_BVa
     "# BV, AAVSO recommended"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"# Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"# Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,NULL
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_VRa
     "# VR, AAVSO recommended"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"# Vs = vs + (Vc-vc) + Tv_vr * ((Vs-Rs)-(Vc-Rc))"
    ,"# Rs = rs + (Rc-rc) + Tr_vr * ((Vs-Rs)-(Vc-Rc))"
    ,NULL
    ,NULL
    ,NULL
   }
   ,
   {  // FILTC_VRIa
     "# VRI, AAVSO recommended "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Vs = vs + (Vc-vc) + Tv_vi * ((Vs-Is)-(Vc-Ic))"
    ,"#  Rs = rs + (Rc-rc) + Tr_vi * ((Vs-Is)-(Vc-Ic))"
    ,"#  Is = Vs - (Vc-Ic) - Tvi   * ((vs-is)-(vc-ic))"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_BVRa
     "# BVR,  AAVSO recommended "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc))"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_VIa   same as classic
     "# VI, AAVSO recommended, same as classic "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Vs = vs + (Vc-vc) + Tv_vi * ((Vs-Is)-(Vc-Ic))"
    ,"#  Is = Vs - (Vc-Ic) - Tvi   * ((vs-is)-(vc-ic))"
    ,NULL
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_BVIa
     "# BVI, AAVSO recommended "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Is = is + (Ic-ic) + Ti_vi * ((Vs-Is)-(Vc-Ic))"
    ,NULL
    ,NULL
   }

   ,
   {   // FILTC_UBVRIa
     "# UBVRI, AAVSO recommended "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Us = us + (Uc-uc) + Tu_ub * ((Us-Bs)-(Uc-Bc))"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Rs = rs + (Rc-rc) + Tr_vi * ((Vs-Is)-(Vc-Ic))"
    ,"#  Is = is + (Ic-ic) + Ti_vi * ((Vs-Is)-(Vc-Ic))"
   }
   ,
   {   // FILTC_UBVIa
     "# UBVI, AAVSO recommended "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Us = us + (Uc-uc) + Tu_ub * ((Us-Bs)-(Uc-Bc))"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Is = is + (Ic-ic) + Ti_vi * ((Vs-Is)-(Vc-Ic))"
    ,NULL
   }
   ,
   {   // FILTC_UBVa
     "# UBV, AAVSO recommended "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Us = us + (Uc-uc) + Tu_ub * ((Us-Bs)-(Uc-Bc))"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_UBVRIs
     "# UBVRI, alternate "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Us=  Bs + (Uc-Bc) + Tub * ((us-bs)-(uc-bc))"
    ,"#  Bs=  Vs + (Bc-Vc) + Tbv * ((bs-vs)-(bc-vc))"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri* ((rs-is)-(rc-ic))"
   }
   ,
   {   // FILTC_UBVIs
     "# UBVI, alternate "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Us=  Bs + (Uc-Bc) + Tub * ((us-bs)-(uc-bc))"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc))"
    ,"#  Is = Vs - (Vc-Ic) - Tvi* ((vs-is)-(vc-ic))"
    ,NULL
   }
   ,
   {   // FILTC_UBVs
     "# UBV, alternate "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Us=  Bs + (Uc-Bc) + Tub * ((us-bs)-(uc-bc))"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc))"
    ,NULL
    ,NULL
   }

   ,
   {   // FILTC_Ba
     "# B, AAVSO single filter transform "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((BVcolor)-(Bc-Vc))"
    ,"#  Vs = Bs - BVcolor"
    ,NULL  ,NULL  ,NULL
   }
   ,
   {   // FILTC_Va
     "# V, AAVSO single filter transform "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((BVcolor)-(Bc-Vc))"
    ,"#  Bs = Vs + BVcolor"
    ,NULL  ,NULL  ,NULL
   }
   ,
   {   // FILTC_Ra
     "# R, AAVSO single filter transform "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Rs = rs + (Rc-rc) + Tr_vi * ((VIcolor)-(Vc-Ic))"
    ,NULL  ,NULL  ,NULL,  NULL
   }
   ,
   {   // FILTC_Ia
     "# I, AAVSO single filter transform "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is obs"
    ,"#  Is = is + (Ic-ic) + Ti_vi * ((VIcolor)-(Vc-Ic))"
    ,"#  Vs = Is + VIcolor"
    ,NULL  ,NULL  ,NULL
   }

};


bool trans_display;
bool FILTC_displayed[FILTC_NUM];
AnsiString Formula= "";
short Method_Mask;
short GroupNum;
AnsiString ObsCode, ObsType;
double ObsLongitude, ObsLatitude; // in radians
char delim, ndelim;
//---------------------------------------------------------------------------
void __fastcall TForm1::ProcessButtonClick(TObject *Sender)
{
   unsigned short fc; // filter combo and index
   float crefmag= -999, x, creferr, krefmag= -999, kreferr;
   float bvcolor= -999, bvcolorerr, vicolor= -999, vicolorerr;
   int xi, i, j, k, l, m/*, sdi*/, sdii, n, gdi= 0;
   AnsiString s, r, sx, st;
   StarData sdt;
   ObsCode= "", ObsType= "";
   delim= ';';  ndelim= '|'; // defaults
   Memo6->Lines->Clear();
   ProcessButton->Enabled= false;
   
   // Extinction turned off
//   applyExtinction->Checked= false;

   // init
   // reset flags for displaying information in the output. Only want to display
   // the transforms and the formulas once in the output
   trans_display= false;
   //chartnext= 0; // reset the chart table?
   for(i= 0; i< FILTC_NUM; i++)
      FILTC_displayed[i]= false;

   GroupNum= 100;
   //set method mask
   Method_Mask= 0;
   if     (RadioButton1->Checked) { Method_Mask= METHOD_Classic; Formula= RadioButton1->Caption; }
   else if(RadioButton2->Checked) { Method_Mask= METHOD_AAVSO;   Formula= RadioButton2->Caption; }
   else if(RadioButton3->Checked) { Method_Mask= METHOD_Special; Formula= RadioButton3->Caption; }

   // assessment pass through the input data
   /*
       must have TYPE, TYPE must be EXTENDED
       must have DELIM
       must have OBSCODE
       must have SOFTWARE
       must have DATE
       must have OBSTYPE
       collect CREFmag, CREFerror
   */
   for(i= 0, sdi= -1; i< Memo1->Lines->Count; i++) { // each line in the input text
      s= Memo1->Lines->Strings[i].TrimLeft().TrimRight();
      if(s.Length()==0) continue;
      if(s[1]=='#') { // comment line
         //tbd  Could vet for required fields...
         if(s.SubString(2, 5)== "TYPE=") {
             if(s.SubString(7, 8).UpperCase()!= "EXTENDED") {
                ShowMessage("The file must be of #TYPE=EXTENDED");
                goto processEnd; //return;
             }
         } else
         if(s.SubString(2, 7)== "OBSLON=") {
            ObsLonEdit->Text= s.SubString(9, 15).TrimLeft().TrimRight();
            ObsLonEditExit(Sender);
         } else
         if(s.SubString(2, 7)== "OBSLAT=") {
            ObsLatEdit->Text= s.SubString(9, 15).TrimLeft().TrimRight();
            ObsLatEditExit(Sender);
         } else
         if(s.SubString(2, 8)== "OBSCODE=") {
            ObsCode= s.SubString(10, 10).TrimLeft().TrimRight();
         } else
         if(s.SubString(2, 8)== "OBSTYPE=") {
            ObsType= s.SubString(10, 10).TrimLeft().TrimRight();
            if(ObsType=="DSLR") DSLRcb->Checked= true;
            else if(DSLRcb->Checked)  // fix it
               Memo1->Lines->Strings[i]= "#OBSTYPE=DSLR";
         } else
         if(s.SubString(2, 6)== "DELIM=") {
            if(s.SubString(8, 3).LowerCase()=="tab") delim= '\t';
            else if(s.SubString(8, 5).LowerCase()=="comma") delim= ',';
            else { // simple method. assumes the DELIM= x
               j= 7;
               while(s[++j]) {
                  if(s[j]!=' ') { // first non blank char
                     delim= s[j];
                     break;
                  }
               }
            }
            //todo  restrict delim choices   eg, not ==
            ndelim= (delim=='|')? ';': '|'; // | (use ; if delim is |
         } else
         if(s.SubString(2, 8)== "CREFMAG=") {
            xi= sscanf(s.SubString(10, s.Length()-9).c_str(),"%f %f %f %f",  &crefmag, &creferr, &krefmag, &kreferr);
            if(xi==2) {
               krefmag= -999; // only crefmag defined
            } else if(xi!=4) {
               crefmag= krefmag= -999; // neither defined
               ShowMessage("Bad #CREFMAG comment: must have value AND error term for either C or both C and K .["+s.SubString(10, s.Length()-9)+"]"  );
            } // else   the crefmag/err will be picked up by the next star record
         } else
         if(s.SubString(2, 8)== "KREFMAG=") {
            xi= sscanf(s.SubString(10, s.Length()-9).c_str(),"%f %f",  &krefmag, &kreferr);
            if(xi!=2) {
               krefmag= -999; // only crefmag defined
               ShowMessage("Bad #KREFMAG comment: must have value AND error term for K.["+s.SubString(10, s.Length()-9)+"]"  );
            } // else   the krefmag/err will be picked up by the next star record
         } else
         if(s.SubString(2, 8)== "BVCOLOR=") {
            xi= sscanf(s.SubString(10, s.Length()-9).c_str(),"%f %f",  &bvcolor, &bvcolorerr);
            if(xi!=2) {
               bvcolor= -999;
               ShowMessage("Bad #BVCOLOR comment: must have value AND error term for color.["+s.SubString(10, s.Length()-9)+"]"  );
            }
         } else
         if(s.SubString(2, 8)== "VICOLOR=") {
            xi= sscanf(s.SubString(10, s.Length()-9).c_str(),"%f %f",  &vicolor, &vicolorerr);
            if(xi!=2) {
               vicolor= -999;
               ShowMessage("Bad #VICOLOR comment: must have value AND error term for color.["+s.SubString(10, s.Length()-9)+"]"  );
            }
         }

      } // end comment line
        else { // data line
         if(sdi==sdiMAX) {
            ShowMessage("Too many observations in the file.");
            goto processEnd; //return;
         }
         sdi++;

         // capture the line to sd
         sd[sdi].record= s;
         sd[sdi].narr= s; //sd[sdi].narr+= "\r\n";
         sd[sdi].processed= false; // clear the flag
         sd[sdi].ErrorMsg= ""; // clear the msg
         sd[sdi].ensemble= false;
         sd[sdi].CREFmag= sd[sdi].KREFmag= -999;
         sd[sdi].BVcolor= sd[sdi].VIcolor= -999;

         k= 1;
         j= s.Pos(delim);
         sd[sdi].NAME= s.SubString(k, j- 1).TrimLeft().TrimRight();
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].DATE= s.SubString(k, j- 1).ToDouble();
         sd[sdi].DATEs= s.SubString(k, j- 1);
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         if(s[k]=='<') {
            sd[sdi].ErrorMsg+= " VMAG is relative; can't be '<'.";
            sd[sdi].processed= true; // skip this record
         } else {
            sd[sdi].VMAGraw= s.SubString(k, j- 1).ToDouble();
         }
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].VERR= s.SubString(k, j- 1).ToDouble();
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].FILT= s.SubString(k, j- 1).TrimLeft().TrimRight();
         k+= j;
         // Remap DSLR filters
         if(DSLRcb->Checked && sd[sdi].FILT=="TG") sd[sdi].FILT= "V";
         else if(DSLRcb->Checked && sd[sdi].FILT=="TB") sd[sdi].FILT= "B";
         else if(DSLRcb->Checked && sd[sdi].FILT=="TR") sd[sdi].FILT= "R";
         // do we recognize the filter?
         sd[sdi].filter= -1;
         for(int ii= 0; ii<FILT_NUM; ii++) {
            if(sd[sdi].FILT == FILT_name[ii]) {
               sd[sdi].filter= ii;
               break;
            }
         }
         if(sd[sdi].filter==-1) {
            sd[sdi].ErrorMsg+= " I don't know this filter.";
            sd[sdi].processed= true; // skip this record
         }

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].TRANS= (s.SubString(k, j- 1).UpperCase()== "YES")? 1: 0;
         k+= j;
         // if it has been transformed, don't touch
         if(sd[sdi].TRANS) {
            sd[sdi].ErrorMsg+= " Already transformed.";
            sd[sdi].processed= true;
            //sd[sdi].TRANS= false; // we're not doing it, so let it fall through as presented. .TRANS means by TA
         }

         j= s.SubString(k, 20).Pos(delim);
         if     (s.SubString(k, j- 1).UpperCase().TrimLeft().TrimRight()== "ABS") sd[sdi].MTYPE= 'S'; // ignore ABS. This is STD
         else if(s.SubString(k, j- 1).UpperCase().TrimLeft().TrimRight()== "DIF") sd[sdi].MTYPE= 'D';
         else if(s.SubString(k, j- 1).UpperCase().TrimLeft().TrimRight()== "STD") sd[sdi].MTYPE= 'S';
         else {
            sd[sdi].ErrorMsg+= " MTYPE not properly specified.";
            sd[sdi].processed= true; // skip this record
         }
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].CNAME= s.SubString(k, j- 1).TrimLeft().TrimRight();
         l= sd[sdi].CNAME.Length();
         if(l>11) { // is it label + auid ?
            if(sd[sdi].CNAME[l-3]=='-' && sd[sdi].CNAME[l-7]=='-')
               sd[sdi].CNAME= sd[sdi].CNAME.SubString(l-10, 11); // cut the label
         }
         k+= j;
         if(sd[sdi].CNAME=="ENSEMBLE") {
            sd[sdi].ensemble= true;
            sd[sdi].ErrorMsg+= " Cannot transform ENSEMBLE obs.";
            sd[sdi].processed= true; // skip this record
         }
         j= s.SubString(k, 20).Pos(delim);
         if(!sd[sdi].ensemble) { // if so, CMAG is na
            sd[sdi].CMAGraw= s.SubString(k, j- 1).ToDouble();
         }
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].KNAME= s.SubString(k, j- 1).TrimLeft().TrimRight();
         l= sd[sdi].KNAME.Length();
         if(l>11) { // is it label + auid ?
            if(sd[sdi].KNAME[l-3]=='-' && sd[sdi].KNAME[l-7]=='-')
               sd[sdi].KNAME= sd[sdi].KNAME.SubString(l-10, 11); // cut the label
         }

         if(sd[sdi].KNAME.UpperCase()=="NO") sd[sdi].KNAME= "na"; // AIP uses NO; webobs wants na
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         if(sd[sdi].KNAME!="na")
            sd[sdi].KMAGraw= s.SubString(k, j- 1).ToDouble();        // will be NO if no check star
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         // airmass might be NA
         if(s.SubString(k,2).LowerCase()=="na") {
            sd[sdi].AMASSna= true;
            sd[sdi].AMASS= 0;
         } else {
            sd[sdi].AMASS= s.SubString(k, j- 1).ToDouble();
            sd[sdi].AMASSna= false; // check AMASS for validity?
         }
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         //sd[sdi].GROUP= s.SubString(k, j- 1).ToInt(); might be "NA"
         sd[sdi].GROUPs= s.SubString(k, j- 1).TrimLeft().TrimRight();
         if(sd[sdi].GROUPs.UpperCase() =="NA") sd[sdi].GROUPs= ""; // treat NA as blank
//         if(sd[sdi].GROUPs.UpperCase() =="NA"){ // || sd[sdi].GROUPs=="" ) {
//            sd[sdi].processed= true; // skip
//            sd[sdi].ErrorMsg+= " GROUP is NA or not assigned.";
//         }
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].CHART= s.SubString(k, j- 1). UpperCase();
         k+= j;

         // if they added a comment in the edit window, the first char might be \n
         if(s.SubString(k,1)== '\n') k++; // skip it
         sd[sdi].NOTES= s.SubString(k, 1000);
         // clear if it is na
         if(sd[sdi].NOTES.UpperCase() == "NA") sd[sdi].NOTES= "";

         // get REFMAG info
         if(crefmag != -999) { // found in a prior comment
            sd[sdi].CREFmag= crefmag;
            sd[sdi].CREFerr= creferr;
            crefmag= -999; // don't use again.
         } else {
            //for(int i= 0; i< sdi; i++) { // look in prior star records
            for(int i= sdi- 1; i >= 0; i--) { // look in prior star records
               if(sd[sdi].CNAME==sd[i].CNAME) { // same star
                  if(sd[sdi].filter==sd[i].filter && !sd[i].processed) { // same filter and not rejected
                     sd[sdi].CREFmag= sd[i].CREFmag;
                     sd[sdi].CREFerr= sd[i].CREFerr;
                     sd[sdi].CRA= sd[i].CRA;
                     sd[sdi].CDec= sd[i].CDec;
                     break;
                  }
               }
            }
            if(-999==sd[sdi].CREFmag) { // try internet
               if(!getREFMAG(&sd[sdi], true)) {
                  sd[sdi].ErrorMsg+= " No CREFMAG available. Possibly bad chart reference.";
                  sd[sdi].processed= true; // skip record
               }
            }
         }

         if(krefmag != -999) { // found in a prior comment
            sd[sdi].KREFmag= krefmag;
            sd[sdi].KREFerr= kreferr;
            krefmag= -999; // don't use again.
         } else {
            //for(int i= 0; i< sdi; i++) { // look in prior star records
            for(int i= sdi- 1; i >= 0; i--) { // look in prior star records
               if(sd[sdi].KNAME==sd[i].KNAME) { // same star
                  if(sd[sdi].filter==sd[i].filter && !sd[i].processed) { // same filter and not rejected
                     sd[sdi].KREFmag= sd[i].KREFmag;
                     sd[sdi].KREFerr= sd[i].KREFerr;
                     sd[sdi].KRA= sd[i].KRA;
                     sd[sdi].KDec= sd[i].KDec;
                     break;
                  }
               }
            }
            if(-999==sd[sdi].KREFmag) { // try internet
               if(!getREFMAG(&sd[sdi], false)) {
                  sd[sdi].ErrorMsg+= " No KREFMAG available. Possibly bad chart reference.";
                  sd[sdi].processed= true; // skip record
               }
            }
         }
         // get extra Star info from VSX
         if(1==getTargInfo(&sd[sdi])) {
            // compute airmasses of target, comp and check
            sd[sdi].Vairmass= AirMass(sd[sdi].DATE, sd[sdi].VRA, sd[sdi].VDec);
            sd[sdi].Cairmass= AirMass(sd[sdi].DATE, sd[sdi].CRA, sd[sdi].CDec);
            sd[sdi].Kairmass= AirMass(sd[sdi].DATE, sd[sdi].KRA, sd[sdi].KDec);
            // Use Vairmass if avail and AMASS was na
            if(sd[sdi].AMASSna && sd[sdi].Vairmass!=0) {
               sd[sdi].AMASS= sd[sdi].Vairmass;
               sd[sdi].AMASSna= false;
            }
            st.sprintf("date= %f, AMASS= %.4f", sd[sdi].DATE, sd[sdi].AMASS);
            st+= r.sprintf(", VX= %.4f", sd[sdi].Vairmass);
            st+= r.sprintf(", CX= %.4f", sd[sdi].Cairmass);
            st+= r.sprintf(", KX= %.4f", sd[sdi].Kairmass);
            Memo6->Lines->Add(st);
         } else {
            //ShowMessage("Cannot find target ("+ sd[sdi].NAME + ") in VSX.");
            //goto processEnd; //return;
            sd[sdi].ErrorMsg+= " target not in VSX.";
         }

         // look for color information
         if(bvcolor != -999) { // found in a prior comment
            sd[sdi].BVcolor= bvcolor;
            sd[sdi].BVcolorErr= bvcolorerr;
            bvcolor= -999; // don't use again.
         } // SetStarData will look for this for other obs
         if(vicolor != -999) { // found in a prior comment
            sd[sdi].VIcolor= vicolor;
            sd[sdi].VIcolorErr= vicolorerr;
            vicolor= -999; // don't use again.
         } // SetStarData will look for this for other obs

      } // end data line
   } // end loop through input lines
   sdi++; // sdi is the number of obs collected


   if(AggregateCB->Checked) {
      /*
      scan sd for common NAME, FILTER, CNAME, KNAME, not TRANS, MTYPE,
      if multiple create new sd record with
         combined date, VMAG, VERR, CMAG, KMAG, AIRMASS, blank GROUP, CHART of 0 rec, combined notes
         comment out detailed records with "# aggregated: "
         add new record to sd[sdi++]   carefull.

      */
      sdii= sdi; // because sdi will increase
      // mark TRANS records as not processed so they will aggregate
      for(i= 0; i< sdii; i++) {
         if(sd[i].TRANS) sd[i].processed= false;
      }
      for(i= 0; i< sdii; i++) {
         if(!sd[i].processed) { // && !sd[i].TRANS) {
            sdt= sd[i]; // aggregate record, start with i record
            // to get the SDOM for VERR, we'll use
            // VMAGraw as sum of VMAGraw's and VERR sum of VMAGraw^2 with an offset by sd[i].VMAG
            sdt.VMAGraw= sdt.VERR= 0; // so its starts as 0
            for(j= i+1, n= 1; j<sdii; j++) { // scan the rest for matches
                if(!sd[j].processed
                   && sd[j].TRANS ==  sd[i].TRANS   //!sd[j].TRANS
                   && sd[j].NAME ==   sd[i].NAME
                   && sd[j].FILT ==   sd[i].FILT
                   && sd[j].CNAME ==  sd[i].CNAME
                   && sd[j].KNAME ==  sd[i].KNAME
                   && sd[j].MTYPE ==  sd[i].MTYPE
                   && sd[j].GROUPs ==  sd[i].GROUPs
                   ) {
                   n++;
                   sdt.DATE+= sd[j].DATE;
                   sdt.VMAGraw+= sd[j].VMAGraw- sd[i].VMAGraw;
                   sdt.VERR+= (sd[j].VMAGraw- sd[i].VMAGraw) * (sd[j].VMAGraw- sd[i].VMAGraw);
                   sdt.CMAGraw+= sd[j].CMAGraw;
                   sdt.KMAGraw+= sd[j].KMAGraw;
                   sdt.AMASS+= sd[j].AMASS;
                   sdt.Vairmass+= sd[j].Vairmass;
                   sdt.Cairmass+= sd[j].Cairmass;
                   sdt.Kairmass+= sd[j].Kairmass;
                   if(sd[j].NOTES!="na") sdt.NOTES+= s.sprintf("%c%s", ndelim, sd[j].NOTES); // don't do nanana!
                   // kill j record, falling through as comment
                   sd[j].record= "# aggregated "+ sd[j].record;
                   sd[j].processed= true;
                }
            }
            if(n>1) {
               // kill i record, falling through as comment
               sd[i].record= "# aggregated "+ sd[i].record;
               sd[i].processed= true;
               sdt.DATE/= n;
               // Standard Deviation of the Mean
               x= (sdt.VERR- sdt.VMAGraw*sdt.VMAGraw / n )/ (n-1);
               sdt.VERR= (x>=0)? sqrt(x/n): 0;
               sdt.VMAGraw/= n; sdt.VMAGraw+= sd[i].VMAGraw; // put offset back in
               sdt.CMAGraw/= n;
               sdt.KMAGraw/= n;
               sdt.AMASS/= n;
               sdt.Vairmass/= n;
               sdt.Cairmass/= n;
               sdt.Kairmass/= n;
               sdt.NOTES+= s.sprintf("%c%i records aggregated", ndelim, n);
               // rewrite .record   todo
               s= sdt.NAME+ delim;
               sdt.DATEs= FormatFloat("#.00000", sdt.DATE);
               s+= sdt.DATEs+ delim;
               s+= FormatFloat("0.000", sdt.VMAGraw)+ delim;
               s+= FormatFloat("0.000", sdt.VERR)+ delim;
               s+= sdt.FILT+ delim;
               s+= sdt.TRANS? "YES": "NO"; s+= delim;
               switch(sdt.MTYPE) {
                  case 'A': s+= "ABS"; break;
                  case 'D': s+= "DIF"; break;
                  case 'S': s+= "STD"; break;
               } s+= delim;
               s+= sdt.CNAME+ delim;
               s+= FormatFloat("0.000", sdt.CMAGraw)+ delim;
               s+= sdt.KNAME+ delim;
               if(sdt.KNAME=="na") s+="na" , s+= delim;
               else s+= FormatFloat("0.000", sdt.KMAGraw)+ delim;
               if(sdt.AMASSna) s+= "na"+ delim;
               else   s+= FormatFloat("0.0000", sdt.AMASS)+ delim;
               s+= sdt.GROUPs+ delim; // first record's group
               s+= sdt.CHART+ delim;  // first record's chart
               s+= sdt.NOTES;
               sdt.record= s;
               sdt.narr= s;
               sd[sdi++]= sdt; // add to sd array
            } // else there was only one record; skip it
         }
      } // end aggregate loop
      // make sure all TRANS records are remarked as processed
      for(i= 0; i< sdi; i++) {
         if(sd[i].TRANS) sd[i].processed= true;
      }


   } // if aggregate data


   // Test the TC's by putting the K star into V's place...
   if(TestTCCB->Checked) {
      for(i= 0; i< sdi; i++) {
         if(sd[i].processed==false) {
            sd[i].NAME= sd[i].KNAME;
            sd[i].VMAGraw= sd[i].KMAGraw - sd[i].CMAGraw + sd[i].CREFmag;
            sd[i].Vairmass= sd[i].Kairmass;
            sd[i].BVcolor= sd[i].VIcolor= -999; // fail the TestTC for single filters
         }
      }
   }

   // prep phase
   for(i= 0; i< sdi; i++) {
      if(sd[i].processed==false) {
         // final check
         //if(sd[i].CREFmag == 0) {    shouldn't happen
         //   sd[i].ErrorMsg+= " CREFMAG is 0.";
         //   sd[i].processed= true;
         //} else{
            sd[i].narr+= st.sprintf("\r\nCREFMAG= %cc= %0.3f +/- %0.3f (nb. comp ref error is excluded from final error computation)", toupper(FILTname[sd[i].filter]), sd[i].CREFmag, sd[i].CREFerr);
         //}

         // correct VMAG to observed value  VMAGINS
         if(sd[i].MTYPE == 'S') { // "STD"
            sd[i].VMAGinst= sd[i].VMAGraw + sd[i].CMAGraw - sd[i].CREFmag;
            sd[i].narr+= st.sprintf("\r\nSTD inst mag: Vinst= %0.3f = %0.3f + %0.3f - %0.3f", sd[i].VMAGinst, sd[i].VMAGraw, sd[i].CMAGraw, sd[i].CREFmag);
         } else if(sd[i].MTYPE == 'A') { // "ABS"     deprecated; will have been changed to STD
            sd[i].VMAGinst= sd[i].VMAGraw;
            sd[i].narr+= st.sprintf("\r\nABS inst mag: Vinst= %0.3f = %0.3f ", sd[i].VMAGinst, sd[i].VMAGraw);
         } else { //if(sd[i].MTYPE == 'D') { // "DIF"
            sd[i].VMAGinst= sd[i].VMAGraw + sd[i].CMAGraw;
            sd[i].narr+= st.sprintf("\r\nDIF inst mag: Vinst= %0.3f = %0.3f + %0.3f ", sd[i].VMAGinst, sd[i].VMAGraw, sd[i].CMAGraw);
         }
         sd[i].NOTES+= st.sprintf("%cVMAGINS=%.3f%cVERR=%.3f", ndelim, sd[i].VMAGinst, ndelim, sd[i].VERR);

         // apply Extinction correction
//         if(applyExtinction->Checked && sd[i].AMASSna) {
//            applyExtinction->Checked= false;
//            ShowMessage("Obs found with AMASS na; Extinction feature turned off");
//         }
         if(applyExtinction->Checked && sd[i].Vairmass!=0 && sd[i].Cairmass!=0) {
            sd[i].CMAGex= sd[i].CMAGraw - *Extinction[sd[i].filter] * sd[i].Cairmass; //      Mobs - K * Airmass
            sd[i].narr+= st.sprintf("\r\nCMAG with extinction: %0.3f = %0.3f - %0.3f * %0.4f", sd[i].CMAGex, sd[i].CMAGraw, *Extinction[sd[i].filter], sd[i].Cairmass); //      Mobs - K * Airmass
            sd[i].VMAGex= sd[i].VMAGinst  - *Extinction[sd[i].filter] * sd[i].Vairmass; //      Mobs - K * Airmass
            sd[i].narr+= st.sprintf("\r\nVMAG with extinction: %0.3f = %0.3f - %0.3f * %0.4f", sd[i].VMAGex, sd[i].VMAGinst, *Extinction[sd[i].filter], sd[i].Vairmass); //      Mobs - K * Airmass
            //  add extinction error to sd[i].VERR=  (nb. this is v and c error combined)
            //   extinc err^2 is (K*(Xv-Xc))^2 * ( (XE/(Xv-Xc))^2 + (KE/K)^2),  XE= 0.0002
            #define aK  *Extinction[sd[i].filter]
            #define aKE *ExtinctionErr[sd[i].filter]
            #define aXv sd[i].Vairmass
            #define aXc sd[i].Cairmass
            #define aXE 0.0002
            if(aXv-aXc) // ie difference in airmass exists
               sd[i].VERR= sqrt(pow(sd[i].VERR, 2) + pow(aK*(aXv-aXc),2) * ( pow(aXE/(aXv-aXc),2) + pow(aKE/aK,2) ) );
            sd[i].NOTES+= st.sprintf("%ck1=%.4f%ck1err=%.4f", ndelim, aK, ndelim, aKE);
            sd[i].extinctionApplied= true;
         } else {
            sd[i].CMAGex= sd[i].CMAGraw;
            sd[i].VMAGex= sd[i].VMAGinst;
            sd[i].extinctionApplied= false;
         }
         // corrections done
         sd[i].CMAG= sd[i].CMAGex;
         sd[i].CERR= 0; // same error as vmag, fully correlated;  not really. VERR is supposed to include CERR
         sd[i].narr+= st.sprintf("\r\n%cc= %0.3f +/- %0.3f (c error is included with VERR)", tolower(FILTname[sd[i].filter]), sd[i].CMAG, sd[i].CERR);
         sd[i].VMAG= sd[i].VMAGex;
         sd[i].narr+= st.sprintf("\r\n%cs= %0.3f +/- %0.3f", tolower(FILTname[sd[i].filter]), sd[i].VMAG, sd[i].VERR);
         // update NOTES
         sd[i].NOTES+= st.sprintf("%cCREFMAG=%.3f%cCREFERR=%.3f", ndelim, sd[i].CREFmag, ndelim, sd[i].CREFerr);
         sd[i].NOTES+= st.sprintf("%cKREFMAG=%.3f%cKREFERR=%.3f", ndelim, sd[i].KREFmag, ndelim, sd[i].KREFerr);
         sd[i].NOTES+= st.sprintf("%cVX=%.4f%cCX=%.4f%cKX=%.4f", ndelim, sd[i].Vairmass, ndelim, sd[i].Cairmass, ndelim, sd[i].Kairmass);
      }
   }



   // process
   do {
      AnsiString cs= NULL;
      // Go through all the unprocessed obs to find what combination of
      //   obs we have available for a group
      for(i= 0; i< sdi; i++) {
         if(sd[i].processed==false) { // find first unprocessed star
            if(cs!=NULL) { // do we have a star name/group?
               if(cs==sd[i].NAME+ sd[i].GROUPs && sf[sd[i].filter]== sdiMAX) {
                  // set this star
                  sf[sd[i].filter]= i;
                  SetStarData(&sd[i], i);
               }
            } else { // !cs;  new star
               cs= sd[i].NAME+ sd[i].GROUPs; // defined by name and group
               // clear
               for(j= 0; j< FILT_NUM; j++)
                  sf[j]= sdiMAX;
               // set this star
               sf[sd[i].filter]= i;
               SetStarData(&sd[i], i);
            }
         }
      }
      if(cs==NULL) // all stars have been processed
         break; // we're done


      // we are ready to process a star+group. the RAM variables are all set
      // scanned all unprocessed stars and tagged all with common name+group (cs)
      //     note no filter duplications..

      // what is the filter combo for this star (cs)?
      // also check for blank group designation; assign if necessary
      GroupNum++;
      for(j= 0, fc= 0; j< FILT_NUM; j++) {
         if(sf[j]!=sdiMAX) {
            fc|= FILT_mask[j];
            //if(sd[sf[j]].GROUPs== "") { // assign a groupno,  always
               sd[sf[j]].GROUPs+= GroupNum;
               cs= sd[sf[j]].NAME+ sd[sf[j]].GROUPs;
            //}
         }
      }
      fc|= Method_Mask;

      // process this star/groups obs
      // nb. this is prepared to handle an overloaded group (not any more, because group nums are assigned)
      for(i= 0; i< sdi; i++) {
//   AnsiString as;
         int k, m;
         if((sd[i].NAME+ sd[i].GROUPs)==cs && sd[i].processed==false) {
            SetStarData(&sd[i], i);
            sf[sd[i].filter]= i; // must provide data for its own filter
// Form1->Memo2->Lines->Add(as.sprintf("I= %i, fc= %i", i, fc));

            ProcessStarData(&sd[i], fc); // sets processed flag

            if(sd[i].TRANS == 2) { // transform successful. capture data used
               // and to NOTES
               sd[i].NOTES+= st.sprintf("%cTAver=%.2f", ndelim, Version);
               sd[i].FILTC= fc;
               k= FILTC_mask2index(sd[i].FILTC);
               for(m= 0; m<FILTC_desc_rows; m++) {
                  if(FILTC_desc[k][m]!= NULL)
                     sd[i].narr+= "\r\n"+ FILTC_desc[k][m];// + "\r\n";
               }
               if(fc & FILT_Ux)
                  sd[i].narr+= st.sprintf("\r\nStar: %s  Us= %6.3f, us= %6.3f, Uc= %6.3f, uc= %6.3f", Uts, Us, us, Uc, uc);
               if(fc & FILT_Bx)
                  sd[i].narr+= st.sprintf("\r\nStar: %s  Bs= %6.3f, bs= %6.3f, Bc= %6.3f, bc= %6.3f", Bts, Bs, bs, Bc, bc);
               if(fc & FILT_Vx)
                  sd[i].narr+= st.sprintf("\r\nStar: %s  Vs= %6.3f, vs= %6.3f, Vc= %6.3f, vc= %6.3f", Vts, Vs, vs, Vc, vc);
               if(fc & FILT_Rx)
                  sd[i].narr+= st.sprintf("\r\nStar: %s  Rs= %6.3f, rs= %6.3f, Rc= %6.3f, rc= %6.3f", Rts, Rs, rs, Rc, rc);
               if(fc & FILT_Ix)
                  sd[i].narr+= st.sprintf("\r\nStar: %s  Is= %6.3f, is= %6.3f, Ic= %6.3f, ic= %6.3f", Its, Is, is, Ic, ic);
               // show color information
               if(fc == FILTC_Ba || fc == FILTC_Va)
                  sd[i].narr+= st.sprintf("\r\nBVcolor= %6.3f, error= %6.3f", sd[i].BVcolor, sd[i].BVcolorErr);
               if(fc == FILTC_Ra || fc == FILTC_Ia)
                  sd[i].narr+= st.sprintf("\r\nVIcolor= %6.3f, error= %6.3f", sd[i].VIcolor, sd[i].VIcolorErr);
               // capture color information for later use
               if(fc & FILT_Bx  &&  fc & FILT_Vx) {
                  sd[i].BVcolor= Bs - Vs;
                  sd[i].BVcolorErr= sqrt( pow(rBs, 2) + pow(rVs, 2));
                  sd[i].narr+= st.sprintf("\r\nBVcolor= %6.3f +/- %6.3f", sd[i].BVcolor, sd[i].BVcolorErr);
               }
               if(fc & FILT_Vx  &&  fc & FILT_Ix) {
                  sd[i].VIcolor= Vs - Is;
                  sd[i].VIcolorErr= sqrt( pow(rVs, 2) + pow(rIs, 2));
                  sd[i].narr+= st.sprintf("\r\nVIcolor= %6.3f +/- %6.3f", sd[i].VIcolor, sd[i].VIcolorErr);
               }

               sd[i].VMAGrep= sd[i].VMAGt;
               sd[i].narr+= st.sprintf("\r\nVMAGtrans= %0.3f", sd[i].VMAGt);
               /*   don't re standardize
               if(sd[i].MTYPE == 'S') { // "STD"
                  sd[i].VMAGrep= ( sd[i].VMAGt - sd[i].CMAGraw ) + sd[i].CREFmag;
                  sd[i].narr+= st.sprintf("\r\nVMAGreport= %0.3f= (%0.3f - %0.3f) + %0.3f  ", sd[i].VMAGrep, sd[i].VMAGt, sd[i].CMAGraw, sd[i].CREFmag);
               } else if(sd[i].MTYPE == 'A') { // "ABS"
                  sd[i].VMAGrep= sd[i].VMAGt;
                  sd[i].narr+= st.sprintf("\r\nVMAGreport= %0.3f= %0.3f", sd[i].VMAGrep, sd[i].VMAGt);
               } else { // "DIF"
                  sd[i].VMAGrep= sd[i].VMAGt - sd[i].CMAGraw;
                  sd[i].narr+= st.sprintf("\r\nVMAGreport= %0.3f= %0.3f - %0.3f", sd[i].VMAGrep, sd[i].VMAGt, sd[i].CMAGraw);
               }
               */

           } else { // failed transform. clean up for TestTC report
              sd[i].VMAGrep= sd[i].VMAGraw;
              sd[i].VERRt= sd[i].VERR;
           }



            // capture the group data
            //  into sd     not really necessary; narrative got the data
            sd[i].gd.Group= sd[i].NAME+ sd[i].GROUPs;
            sd[i].gd.FILTC= fc;
            for(j= 0; j<20; j++)
               sd[i].gd.m[j]= 0; // clear
            if(fc & FILT_Ux)
               sd[i].gd.m[0]= Us, sd[i].gd.m[5]= us, sd[i].gd.m[10]= Uc, sd[i].gd.m[15]= uc;
            if(fc & FILT_Bx)
               sd[i].gd.m[1]= Bs, sd[i].gd.m[6]= bs, sd[i].gd.m[11]= Bc, sd[i].gd.m[16]= bc;
            if(fc & FILT_Vx)
               sd[i].gd.m[2]= Vs, sd[i].gd.m[7]= vs, sd[i].gd.m[12]= Vc, sd[i].gd.m[17]= vc;
            if(fc & FILT_Rx)
               sd[i].gd.m[3]= Rs, sd[i].gd.m[8]= rs, sd[i].gd.m[13]= Rc, sd[i].gd.m[18]= rc;
            if(fc & FILT_Ix)
               sd[i].gd.m[4]= Is, sd[i].gd.m[9]= is, sd[i].gd.m[14]= Ic, sd[i].gd.m[19]= ic;



         } //
      } // for
         //  don't really need this
            // and gd
            if(gdi==gdiMAX) {
               ShowMessage("Too many groups in the file.");
               goto processEnd; //return;
            }
            gd[gdi].Group= cs; //sd[i].NAME+ sd[i].GROUPs;
            gd[gdi].FILTC= fc;
            for(j= 0; j<20; j++)
               gd[gdi].m[j]= 0; // clear
            if(fc & FILT_Ux)
               gd[gdi].m[0]= Us, gd[gdi].m[5]= us, gd[gdi].m[10]= Uc, gd[gdi].m[15]= uc;
            if(fc & FILT_Bx)
               gd[gdi].m[1]= Bs, gd[gdi].m[6]= bs, gd[gdi].m[11]= Bc, gd[gdi].m[16]= bc;
            if(fc & FILT_Vx)
               gd[gdi].m[2]= Vs, gd[gdi].m[7]= vs, gd[gdi].m[12]= Vc, gd[gdi].m[17]= vc;
            if(fc & FILT_Rx)
               gd[gdi].m[3]= Rs, gd[gdi].m[8]= rs, gd[gdi].m[13]= Rc, gd[gdi].m[18]= rc;
            if(fc & FILT_Ix)
               gd[gdi].m[4]= Is, gd[gdi].m[9]= is, gd[gdi].m[14]= Ic, gd[gdi].m[19]= ic;
            gdi++;
           // */
   } while(42);  // no cs will break us out


   // display the data
   //ShowMessage("Review the convergence of the New formulas (if any)");
   Memo2->Lines->Clear(); // output window
//   Memo4->Lines->Clear(); // report window

   Memo4->Lines->Add(" "); // blank line
   //Memo4->Lines->Add(Formula);
   Memo4->Lines->Add("Star                 Date    Filter Grp    Vraw      Vinst    Vex    TranMag      diff       VERR    VERRt");
   // build
   for(i= 0, j= 0; i< Memo1->Lines->Count || j< sdi; i++) { // i counts lines, j stars
      if(i< Memo1->Lines->Count) s= Memo1->Lines->Strings[i].TrimLeft().TrimRight();
      else s= "  "; // to fall through to data line
      if(s.Length()!=0) // skip if blank
      if(s[1]=='#') { // non-data line
         Memo2->Lines->Add(s); // just copy
      } else { // data line
         if(sd[j].record[1]=='#') { // record aggregated
            Memo2->Lines->Add(sd[j].record); // just copy
         } else if(sd[j].TRANS == 0) { // not Transformed
            Memo2->Lines->Add(sd[j].record); //     Add(s); // just copy original
            //r.sprintf("%s %s %8.3f NA", sd[j].NAME, sd[j].DATEs, sd[j].VMAG);
            //? where was this to go? Not here:   Memo2->Lines->Add(r);
            r.sprintf("\"%-15s\"   %s %s %3s %8.3f %8.3f %8.3f    not transformed", sd[j].NAME, sd[j].DATEs, sd[j].FILT, sd[j].GROUPs
                                  , sd[j].VMAGraw, sd[j].VMAGinst, sd[j].VMAG );// ,    sd[j].VMAGt, sd[j].VMAGt- sd[j].VMAG, sd[j].VMAGrep, sd[j].VERR, sd[j].VERRt);
            Memo4->Lines->Add(r);
            sd[j].recordT= ""; // no transformed record
         } else if(sd[j].TRANS == 1) { // not Transformed by TA
            Memo2->Lines->Add(sd[j].record); //     Add(s); // just copy original
//"S HYA          "   2455280.61343 I 3103    5.262    7.071    7.071     5.239   -0.02345     0.354    0.481
//"S HYA          "   2455280.60816 B 1101    9.314    0.000    0.000    not transformed
            r.sprintf("\"%-15s\"   %s %s %3s    already transformed      %8.3f                      %8.3f", sd[j].NAME, sd[j].DATEs, sd[j].FILT, sd[j].GROUPs
                                  , sd[j].VMAGraw, sd[j].VERR);
            Memo4->Lines->Add(r);
            sd[j].recordT= ""; // no transformed record
        } else {  // display transformed data
            // Standard info blocks will be output once
            if(!trans_display) { // show the transform coefficients just once
               //Memo2->Lines->Add("#SOFTWARE=TransformApplier, V"+ FormatFloat("0.00", Version));
               Memo2->Lines->Add("#  TransformApplier, V"+ FormatFloat("0.00", Version));
               Memo2->Lines->Add("#   transform coefficients from "+ INIfilename + "   "+ DateToStr(Date() )  );
               Memo2->Lines->Add("#      desc: "+ setupEdit->Text);
               for(int i= 0; i<NumTCoef; i++)
                  if(*TC[i].value!=0) // if non-zero
                     Memo2->Lines->Add(st.sprintf("#%s= %0.4f, +/- %0.4f", TC[i].name, *TC[i].value, *TC[i].error));
               if(applyExtinction->Checked) {
                  Memo2->Lines->Add("#   extinction coefficients:");
                  for(int i= 0; i<NumECoef; i++)
                     if(*EC[i].value!=0) // if non-zero
                        Memo2->Lines->Add(st.sprintf("#%s= %0.4f, +/- %0.4f", EC[i].name, *EC[i].value, *EC[i].error));
                  // show location
                  Memo2->Lines->Add(st.sprintf("#location: long= %s and lat= %s", ObsLonEdit->Text, ObsLatEdit->Text));
               }
               trans_display= true;
            }

            // show un-transformed data as a comment
            Memo2->Lines->Add((IncludeRaw->Checked?"":"#")+ sd[j].record);

            // now the transformed data: fmt record from sd
            s= sd[j].NAME+ delim;

            s+= FormatFloat("#.00000", sd[j].DATE)+ delim;

            // all the same...
            s+= FormatFloat("0.000", sd[j].VMAGrep)+ delim;
            s+= FormatFloat("0.000", sd[j].VERRt)+ delim;    // ???
            s+= sd[j].FILT+ delim;
            s+= sd[j].TRANS? "YES": "NO";
            s+= delim;

            switch(sd[j].MTYPE) {
               case 'A': s+= "ABS"; break;
               case 'D': s+= "DIF"; break;
               case 'S': s+= "STD"; break;
            }
            s+= delim;

            s+= sd[j].CNAME+ delim;
            s+= FormatFloat("0.000", sd[j].CMAGraw)+ delim;

            s+= sd[j].KNAME+ delim;
            if(sd[j].KNAME=="na") s+="na" , s+= delim;
            else s+= FormatFloat("0.000", sd[j].KMAGraw)+ delim;

            if(sd[j].AMASSna) s+= "na", s+= delim;
            else  {
              if(sd[j].extinctionApplied) // use Vairmass if extinction was applied
                 s+= FormatFloat("0.0000", sd[j].Vairmass)+ delim;
              else
                 s+= FormatFloat("0.0000", sd[j].AMASS)+ delim;
            }
            // use Vairmass?  if extinction applied or requested


            //s+= IntToStr(sd[j].GROUP)+ delim;
            s+= sd[j].GROUPs+ delim;

            s+= sd[j].CHART+ delim;
            s+= sd[j].NOTES;

            sd[j].recordT= s; // capture the final record and publish
            Memo2->Lines->Add(s);


            // add a star report line
            r.sprintf("\"%-15s\"   %s %s %3s %8.3f %8.3f %8.3f  %8.3f %10.5f  %8.3f %8.3f", sd[j].NAME, sd[j].DATEs, sd[j].FILT, sd[j].GROUPs
                                  , sd[j].VMAGraw, sd[j].VMAGinst, sd[j].VMAG, sd[j].VMAGt, sd[j].VMAGt- sd[j].VMAGraw, sd[j].VERR, sd[j].VERRt);
            Memo4->Lines->Add(r);

            // add info about the analysis
            Memo2->Lines->Add(sd[j].StarsUsed);

         }
         if(sd[j].ErrorMsg != "")
            Memo2->Lines->Add("#"+ sd[j].ErrorMsg);

         j++;
      } // end of data line
   } // end loop of input lines

   //todo display records if j short of sdi


   // do a detail, narrative report

   Memo4->Lines->Add("\r\n    Detail narrative for each observation  \r\n");
   for(i= 0; i<sdi; i++) {
      if(sd[i].record[1 ]!='#') { // not aggregated
         sd[i].narr+= "\r\n"+ sd[i].recordT;
         Memo4->Lines->Add(sd[i].narr);
         Memo4->Lines->Add(sd[i].ErrorMsg + "\r\n");
      }
   }

   // extinction report


   // group report
   Memo4->Lines->Add("\r\n          Summary of group data");
   Memo4->Lines->Add("\n name+group      filtC   Us     Bs     Vs     Rs     Is     us     bs     vs     rs     is");
   for(i= 0; i<gdi; i++) {
      r.sprintf("\"%-15s\" %3i", gd[i].Group, gd[i].FILTC);
      for(j= 0; j<10; j++) {
         if(gd[i].m[j]==0) s= "    na ";
         else              s.sprintf("%7.3f", gd[i].m[j]);
         r= r+ s;
      }
      Memo4->Lines->Add(r);
   }
   Memo4->Lines->Add("\n name+group      filtC   Uc     Bc     Vc     Rc     Ic     uc     bc     vc     rc     ic");
   for(i= 0; i<gdi; i++) {
      r.sprintf("\"%-15s\" %3i", gd[i].Group, gd[i].FILTC);
      for(j= 10; j<20; j++) {
         if(gd[i].m[j]==0) s= "    na ";
         else              s.sprintf("%7.3f", gd[i].m[j]);
         r= r+ s;
      }
      Memo4->Lines->Add(r);
   }

   // show coeffcients
               Memo4->Lines->Add("\r\n   transform coefficients applied by Transformer Applier, version "+ FormatFloat("0.00", Version));
               Memo4->Lines->Add("   transform coefficients from "+ INIfilename+ "   "+ DateToStr(Date()));
               Memo4->Lines->Add("      desc: "+ setupEdit->Text);
               for(int i= 0; i<NumTCoef; i++)
                  if(*TC[i].value!=0) // if non-zero
                     Memo4->Lines->Add(st.sprintf("%s= %0.4f, +/- %0.4f", TC[i].name, *TC[i].value, *TC[i].error));
               if(applyExtinction->Checked) {
                  Memo4->Lines->Add("   extinction coefficients:");
                  for(int i= 0; i<NumECoef; i++)
                     if(*EC[i].value!=0) // if non-zero
                        Memo4->Lines->Add(st.sprintf("%s= %0.4f, +/- %0.4f", EC[i].name, *EC[i].value, *EC[i].error));
               }

    // TC Test report
    if(TestTCCB->Checked) {
       Memo4->Lines->Add("\r\n    Transform Coefficient Test Report  \r\n");
       Memo4->Lines->Add("name      filter  Refmag  VMAGinst  VMAGrep   diff     error  transformed");
       for(i= 0; i<sdi; i++) {
          if(sd[i].record[1]!='#') { // not aggregated
             Memo4->Lines->Add(st.sprintf("%11s %3s %8.3f %8.3f %8.3f %8.3f %8.3f    %3s", sd[i].NAME, sd[i].FILT, sd[i].KREFmag, sd[i].VMAGinst, sd[i].VMAGrep, sd[i].VMAGrep - sd[i].KREFmag, sd[i].VERRt, sd[i].TRANS?"yes":"no"));
          }
       }
       // try to derive coefficients?
       //  need to extinct since they are from all over....

    }

    // log the process
    if(!TAlogCalled) {
       s= "http://www.gasilvis.com/TA/TAlog.php?logentry=";
       s+= ObsCode;
       s+= ","+ FormatFloat("0.00", Version);
       s+= ","+ Formula; // methodology
       s+= AggregateCB->Checked? ",true": ",false"; // aggregation?
       s+= TestTCCB->Checked? ",true": ",false";    // TestTC?
       httpGet(s, NULL, 0);
       TAlogCalled= true;
    }

processEnd:
   ProcessButton->Enabled= true;
}
//---------------------------------------------------------------------------

void SetStarData(StarData* d, int index)
{
   switch(d->filter) {
      case FILT_Ui: us= d->VMAG; uc= d->CMAG; Uc= d->CREFmag;    Uts= d->DATEs;
                    rus= d->VERR; ruc= d->CERR; rUc= d->CREFerr; break;
      case FILT_Bi: bs= d->VMAG; bc= d->CMAG; Bc= d->CREFmag;    Bts= d->DATEs;
                    rbs= d->VERR; rbc= d->CERR; rBc= d->CREFerr; break;
      case FILT_Vi: vs= d->VMAG; vc= d->CMAG; Vc= d->CREFmag;    Vts= d->DATEs;
                    rvs= d->VERR; rvc= d->CERR; rVc= d->CREFerr; break;
      case FILT_Ri: rs= d->VMAG; rc= d->CMAG; Rc= d->CREFmag;    Rts= d->DATEs;
                    rrs= d->VERR; rrc= d->CERR; rRc= d->CREFerr; break;
      case FILT_Ii: is= d->VMAG; ic= d->CMAG; Ic= d->CREFmag;    Its= d->DATEs;
                    ris= d->VERR; ric= d->CERR; rIc= d->CREFerr; break;
   }
   // capture latest color information if needed and available
   if(d->BVcolor== -999) { // doesn't have color info
      for(int i= index- 1; i >= 0; i--) { // look in prior star records
         if(d->NAME==sd[i].NAME && sd[i].processed) { // same star and already processed
            if(d->filter==sd[i].filter && sd[i].BVcolor!= -999) { // same filter and valid data
               d->BVcolor= sd[i].BVcolor;
               d->BVcolorErr= sd[i].BVcolorErr;
               break;
            }
         }
      }
   }
   if(d->VIcolor== -999) { // doesn't have color info
      for(int i= index- 1; i >= 0; i--) { // look in prior star records
         if(d->NAME==sd[i].NAME && sd[i].processed) { // same star and already processed
            if(d->filter==sd[i].filter && sd[i].VIcolor!= -999) { // same filter and valid data
               d->VIcolor= sd[i].VIcolor;
               d->VIcolorErr= sd[i].VIcolorErr;
               break;
            }
         }
      }
   }


}

float fTxy   (char x, char y,         float Txy,   float rTxy,   int mode, char* tdesc);
float fTx_yz (char x, char y, char z, float Tx_yz, float rTx_yz, int mode, char* tdesc);

#define MAXITER 15
void ProcessStarData(StarData *d, unsigned short fc)
{
   AnsiString as;
   float x;    int loop;
   d->TRANS= 2;
   d->FILTC= fc;  // which combination used
   rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; // default error is the obs error
   Utdesc[0]= Btdesc[0]= Vtdesc[0]= Rtdesc[0]= Itdesc[0]= 0;
   switch(fc) {
      case (FILTC_BVRIc):
         if(Tr_ri==0 || Tri==0 || Tv_vr==0 || Tvr==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tr_ri, Tri, Tv_vr, Tvr and Tbv.";
            d->TRANS= 0;
            break;
         }
         Rs = rs + (Rc - rc) + Tr_ri * (((Rc - Ic) + Tri * ((rs - is) - (rc - ic))) - (Rc - Ic));
          sprintf(Rtdesc, "%cTr_ri=%.4f%cTr_riErr=%.4f%cTri=%.4f%cTriErr=%.4f", ndelim, Tr_ri, ndelim, rTr_ri, ndelim, Tri, ndelim, rTri);
         Is = Rs - ((Rc - Ic) + Tri * ((rs - is) - (rc - ic)));
          sprintf(Itdesc, "%cTri=%.4f%cTriErr=%.4f", ndelim, Tri, ndelim, rTri);
         Vs = vs + (Vc - vc) + Tv_vr * Tvr * ((vs - rs) - (vc - rc));
          sprintf(Vtdesc, "%cTv_vr=%.4f%cTv_vrErr=%.4f%cTvr=%.4f%cTvrErr=%.4f", ndelim, Tv_vr, ndelim, rTv_vr, ndelim, Tvr, ndelim, rTvr);
         Bs = Vs + (Bc - Vc) + Tbv * ((bs - vs) - (bc - vc));
          sprintf(Btdesc, "%cTbv=%.4f%cTbvErr=%.4f", ndelim, Tbv, ndelim, rTbv);
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs
            + ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         // no error computation
         break;

      case (FILTC_BVRIa):
         if(Tb_bv==0 || Tv_bv==0 || Tr_vi==0 || Ti_vi==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv, Tv_bv, Tr_vi and Ti_vi.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 1, Btdesc); //bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            Vs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 1, Vtdesc); //vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc));
            Rs = fTx_yz( r, v, i, Tr_vi, rTr_vi, 1, Rtdesc); //rs + (Rc-rc) + Tr_vi * ((Vs-Is)-(Vc-Ic));
            Is = fTx_yz( i, v, i, Ti_vi, rTi_vi, 1, Itdesc); //is + (Ic-ic) + Ti_vi * ((Vs-Is)-(Vc-Ic));
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs
           + ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            rVs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 11, Vtdesc);
            rRs = fTx_yz( r, v, i, Tr_vi, rTr_vi, 11, Rtdesc);
            rIs = fTx_yz( i, v, i, Ti_vi, rTi_vi, 11, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

             /*
      case (FILTC_BVRIs): // mine
         if(Tbv==0 || Tvr==0 || Tri==0 || Tb_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr, Tri and Tb_bv.";
            d->TRANS= 0;
            break;
         }
         //Tb_bv = ((Bs-bs)-(Bc-bc)) / ((Bs-Vs)-(Bc-Vc))
         //Tbv= ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvr= ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         //Tri= ((Rs-Is)-(Rc-Ic)) / ((rs-is)-(rc-ic))
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc));
            Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc));
            Is = Rs - (Rc-Ic) - Tri* ((rs-is)-(rc-ic));
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs
           + ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
       error comp
         break;
     */

      case (FILTC_BVRIs):  // Arne's alternate as used with the BSM
         if(Tbv==0 || Tvr==0 || Tri==0 || Tv_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tv_bv, Tvr, and Tri.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTxy( b, v, Tbv, rTbv, 1, Btdesc);
            Vs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 1, Vtdesc);
            Rs = fTxy( v, r, Tvr, rTvr, 2, Rtdesc);
            Is = fTxy( r, i, Tri, rTri, 2, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVRI Arne alternate : B @ "+ sd[sf[FILT_Bi]].DATEs
           + ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTxy( b, v, Tbv, rTbv, 11, Btdesc);
            rVs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 11, Vtdesc);
            rRs = fTxy( v, r, Tvr, rTvr, 12, Rtdesc);
            rIs = fTxy( r, i, Tri, rTri, 12, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case FILTC_BVa:
         if(Tb_bv==0 || Tv_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv and Tv_bv.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1, Btdesc);
            Vs = fTx_yz(v, b, v, Tv_bv, rTv_bv, 1, Vtdesc);
            //Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            //Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc));
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            rVs = fTx_yz(v, b, v, Tv_bv, rTv_bv, 11, Vtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_BVs:
      case FILTC_BVc:
         if(Tb_bv==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv and Tbv.";
            d->TRANS= 0;
            break;
         }
         //Bs = bs + (Bc - bc) + Tb_bv * (((Bc - Vc) + Tbv * ((bs - vs) - (bc - vc))) - (Bc - Vc));
         //Vs = Bs - ((Bc - Vc) + Tbv * ((bs - vs) - (bc - vc)));
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1, Btdesc);
            Vs = fTxy(b, v, Tbv, rTbv, 2, Vtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            rVs = fTxy(b, v, Tbv, rTbv, 12, Vtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;



      case FILTC_VRa:
         if(Tv_vr==0 || Tr_vr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv_vr and Tr_vr.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            //Vs = vs + (Vc-vc) + Tv_vr * ((Vs-Rs)-(Vc-Rc));
            //Rs = rs + (Rc-rc) + Tr_vr * ((Vs-Rs)-(Vc-Rc));
            Vs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 1, Vtdesc);
            Rs = fTx_yz(r, v, r, Tr_vr, rTr_vr, 1, Rtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 11, Vtdesc);
            rRs = fTx_yz(r, v, r, Tr_vr, rTr_vr, 11, Rtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_VRs:
      case FILTC_VRc:
         if(Tv_vr==0 || Tvr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv_vr and Tvr.";
            d->TRANS= 0;
            break;
         }
         //Vs = vs + (Vc - vc) + Tv_vr * (((Vc - Rc) + Tvr * ((vs - rs) - (vc - rc))) - (Vc - Rc));
         //Rs = Vs - ((Vc - Rc) + Tvr * ((vs - rs) - (vc - rc)));
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Vs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 1, Vtdesc);
            Rs = fTxy(v, r, Tvr, rTvr, 2, Rtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 11, Vtdesc);
            rRs = fTxy(v, r, Tvr, rTvr, 12, Rtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case FILTC_VRIa:
         if(Tv_vi==0 || Tr_vi==0 || Tvi==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv_vi, Tr_vi and Tvi.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            //Vs = vs + (Vc-vc) + Tv_vi * ((Vs-Is)-(Vc-Ic));
            //Rs = rs + (Rc-rc) + Tr_vi * ((Vs-Is)-(Vc-Ic));
            //Is = Vs - (Vc-Ic) - Tvi   * ((vs-is)-(vc-ic));
            Vs = fTx_yz(v, v, i, Tv_vi, rTv_vi, 1, Vtdesc);
            Rs = fTx_yz(r, v, i, Tr_vi, rTr_vi, 1, Rtdesc);
            Is = fTxy(v, i, Tvi, rTvi, 2, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+ " using:  V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs = fTx_yz(v, v, i, Tv_vi, rTv_vi, 11, Vtdesc);
            rRs = fTx_yz(r, v, i, Tr_vi, rTr_vi, 11, Rtdesc);
            rIs = fTxy(v, i, Tvi, rTvi, 12, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_VRIs:
         if(Tvr==0 || Tri==0 || Tv_vr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv_vr, Tvr, and Tri.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            //Vs = vs + (Vc-vc) + Tv_vr    * ((Vs-Rs)-(Vc-Rc));
            //Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc));
            //Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic));
            Vs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 1, Vtdesc);
            Rs = fTxy(v, r, Tvr, rTvr, 2, Rtdesc);
            Is = fTxy(r, i, Tri, rTri, 2, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using:  V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 11, Vtdesc);
            rRs = fTxy(v, r, Tvr, rTvr, 12, Rtdesc);
            rIs = fTxy(r, i, Tri, rTri, 12, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_VRIc:
         if(Tr_ri==0 || Tri==0 || Tv_vr==0 || Tvr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tr_ri, Tri, Tv_vr and Tvr.";
            d->TRANS= 0;
            break;
         }
         Rs = rs + (Rc - rc) + Tr_ri * (((Rc - Ic) + Tri * ((rs - is) - (rc - ic))) - (Rc - Ic));
          sprintf(Rtdesc, "%cTr_ri=%.4f%cTr_riErr=%.4f%cTri=%.4f%cTriErr=%.4f", ndelim, Tr_ri, ndelim, rTr_ri, ndelim, Tri, ndelim, rTri);
         Is = Rs - ((Rc - Ic) + Tri * ((rs - is) - (rc - ic)));
          sprintf(Itdesc, "%cTri=%.4f%cTriErr=%.4f", ndelim, Tri, ndelim, rTri);
         Vs = vs + (Vc - vc) + Tv_vr * Tvr * ((vs - rs) - (vc - rc));
          sprintf(Vtdesc, "%cTv_vr=%.4f%cTv_vrErr=%.4f%cTvr=%.4f%cTvrErr=%.4f", ndelim, Tv_vr, ndelim, rTv_vr, ndelim, Tvr, ndelim, rTvr);
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         break;

      case FILTC_BVRa:
         if(Tb_bv==0 || Tv_bv==0 || Tvr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv, Tv_bv and Tvr.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            //Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            //Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc));
            //Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc));
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1, Btdesc);
            Vs = fTx_yz(v, b, v, Tv_bv, rTv_bv, 1, Vtdesc);
            Rs = fTxy(v, r, Tvr, rTvr, 2, Rtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            rVs = fTx_yz(v, b, v, Tv_bv, rTv_bv, 11, Vtdesc);
            rRs = fTxy(v, r, Tvr, rTvr, 12, Rtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_BVRs:
         if(Tbv==0 || Tvr==0 || Tb_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv, Tbv and Tvr.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            //Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            //Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc));
            //Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc));
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1, Btdesc);
            Vs = fTxy(b, v, Tbv, rTbv, 2, Vtdesc);
            Rs = fTxy(v, r, Tvr, rTvr, 2, Rtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            rVs = fTxy(b, v, Tbv, rTbv, 12, Vtdesc);
            rRs = fTxy(v, r, Tvr, rTvr, 12, Rtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_BVRc:
         if(Tv_vr==0 || Tvr==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tv_vr and Tvr.";
            d->TRANS= 0;
            break;
         }
         //Vs = vs + (Vc - vc) + Tv_vr * (((Vc - Rc) + Tvr * ((vs - rs) - (vc - rc))) - (Vc - Rc));
         //Rs = Vs - ((Vc - Rc) + Tvr * ((vs - rs) - (vc - rc)));
         //Bs = Vs + (Bc - Vc) +  Tbv * ((bs - vs) - (bc - vc));
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTxy(b, v, Tbv, rTbv, 1, Btdesc);
            Vs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 1, Vtdesc);
            Rs = fTxy(v, r, Tvr, rTvr, 2, Rtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTxy(b, v, Tbv, rTbv, 11, Btdesc);
            rVs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 11, Vtdesc);
            rRs = fTxy(v, r, Tvr, rTvr, 12, Rtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case FILTC_VIs:
      case FILTC_VIa:
      case FILTC_VIc:
         if(Tv_vi==0 || Tvi==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv_vi and Tvi.";
            d->TRANS= 0;
            break;
         }
         //Vs = vs + (Vc - vc) + Tv_vi * (((Vc - Ic) + Tvi * ((vs - is) - (vc - ic))) - (Vc - Ic));
         //Is = Vs - ((Vc - Ic) + Tvi * ((vs - is) - (vc - ic)));
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Vs = fTx_yz(v, v, i, Tv_vi, rTv_vi, 1, Vtdesc);
            Is = fTxy(v, i, Tvi, rTvi, 2, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: V @ "+ sd[sf[FILT_Vi]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs = fTx_yz(v, v, i, Tv_vi, rTv_vi, 11, Vtdesc);
            rIs = fTxy(v, i, Tvi, rTvi, 12, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case FILTC_BVIs:
         if(Tbv==0 || Tvi==0 || Tb_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv, Tbv and Tvi.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1, Btdesc); //Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            Vs = fTxy(b, v, Tbv, rTbv, 2, Vtdesc); //Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc));
            Is = fTxy(v, i, Tvi, rTvi, 2, Itdesc); //Is = Vs - (Vc-Ic) - Tvi* ((vs-is)-(vc-ic));
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            rVs = fTxy(b, v, Tbv, rTbv, 12, Vtdesc);
            rIs = fTxy(v, i, Tvi, rTvi, 12, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_BVIc:
         if(Tv_vi==0 || Tvi==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tv_vi and Tvi.";
            d->TRANS= 0;
            break;
         }
         //Vs = vs + (Vc - vc) + Tv_vi * (((Vc - Ic) + Tvi * ((vs - is) - (vc - ic))) - (Vc - Ic));
         //Is = Vs - ((Vc - Ic) + Tvi * ((vs - is) - (vc - ic)));
         //Bs = Vs + (Bc - Vc) +  Tbv * ((bs - vs) - (bc - vc));
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTxy(b, v, Tbv, rTbv, 1, Btdesc);
            Vs = fTx_yz(v, v, i, Tv_vi, rTv_vi, 1, Vtdesc);
            Is = fTxy(v, i, Tvi, rTvi, 2, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTxy(b, v, Tbv, rTbv, 11, Btdesc);
            rVs = fTx_yz(v, v, i, Tv_vi, rTv_vi, 11, Vtdesc);
            rIs = fTxy(v, i, Tvi, rTvi, 12, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_BVIa:
         if(Tb_bv==0 || Tv_bv==0 || Ti_vi==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv, Tv_bv and Ti_vi.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 1, Btdesc); //bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            Vs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 1, Vtdesc); //vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc));
            Is = fTx_yz( i, v, i, Ti_vi, rTi_vi, 1, Itdesc); //is + (Ic-ic) + Ti_vi * ((Vs-Is)-(Vc-Ic));
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            rVs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 11, Vtdesc);
            rIs = fTx_yz( i, v, i, Ti_vi, rTi_vi, 11, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case (FILTC_UBVRIa):
         if(Tu_ub==0 || Tb_bv==0 || Tv_bv==0 || Tr_vi==0 || Ti_vi==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tu_ub, Tb_bv, Tv_bv, Tr_vi and Ti_vi.";
            d->TRANS= 0;
            break;
         }
         Us= us, Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oUs= Us, oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Us = fTx_yz( u, u, b, Tu_ub, rTu_ub, 1, Utdesc); //us + (Uc-uc) + Tu_ub * ((Us-Bs)-(Uc-Bc));
            Bs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 1, Btdesc); //bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            Vs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 1, Vtdesc); //vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc));
            Rs = fTx_yz( r, v, i, Tr_vi, rTr_vi, 1, Rtdesc); //rs + (Rc-rc) + Tr_vi * ((Vs-Is)-(Vc-Ic));
            Is = fTx_yz( i, v, i, Ti_vi, rTi_vi, 1, Itdesc); //is + (Ic-ic) + Ti_vi * ((Vs-Is)-(Vc-Ic));
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f, %0.4f", fabs(Us-oUs), fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Us-oUs)>0.0001 || fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: U @ "+ sd[sf[FILT_Ui]].DATEs + ", B @ "+ sd[sf[FILT_Bi]].DATEs
           + ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rUs= rus, rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roUs= rUs, roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rUs = fTx_yz( u, u, b, Tu_ub, rTu_ub, 11, Utdesc);
            rBs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            rVs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 11, Vtdesc);
            rRs = fTx_yz( r, v, i, Tr_vi, rTr_vi, 11, Rtdesc);
            rIs = fTx_yz( i, v, i, Ti_vi, rTi_vi, 11, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f, %0.4f", fabs(rUs-roUs), fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rUs-roUs)>0.0001 || fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case (FILTC_UBVIa):
         if(Tu_ub==0 || Tb_bv==0 || Tv_bv==0 || Ti_vi==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tu_ub, Tb_bv, Tv_bv and Ti_vi.";
            d->TRANS= 0;
            break;
         }
         Us= us, Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oUs= Us, oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Us = fTx_yz( u, u, b, Tu_ub, rTu_ub, 1, Utdesc); //us + (Uc-uc) + Tu_ub * ((Us-Bs)-(Uc-Bc));
            Bs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 1, Btdesc); //bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            Vs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 1, Vtdesc); //vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc));
            Is = fTx_yz( i, v, i, Ti_vi, rTi_vi, 1, Itdesc); //is + (Ic-ic) + Ti_vi * ((Vs-Is)-(Vc-Ic));
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Us-oUs), fabs(Bs-oBs), fabs(Vs-oVs), fabs(Is-oIs)));
         } while ( fabs(Us-oUs)>0.0001 || fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: U @ "+ sd[sf[FILT_Ui]].DATEs + ", B @ "+ sd[sf[FILT_Bi]].DATEs
           + ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rUs= rus, rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roUs= rUs, roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rUs = fTx_yz( u, u, b, Tu_ub, rTu_ub, 11, Utdesc);
            rBs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            rVs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 11, Vtdesc);
            rIs = fTx_yz( i, v, i, Ti_vi, rTi_vi, 11, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rUs-roUs), fabs(rBs-roBs), fabs(rVs-roVs), fabs(rIs-roIs)));
         } while ( fabs(rUs-roUs)>0.0001 || fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case (FILTC_UBVa):
         if(Tu_ub==0 || Tb_bv==0 || Tv_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tu_ub, Tb_bv, and Tv_bv.";
            d->TRANS= 0;
            break;
         }
         Us= us, Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oUs= Us, oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Us = fTx_yz( u, u, b, Tu_ub, rTu_ub, 1, Utdesc); //us + (Uc-uc) + Tu_ub * ((Us-Bs)-(Uc-Bc));
            Bs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 1, Btdesc); //bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            Vs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 1, Vtdesc); //vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc));
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f", fabs(Us-oUs), fabs(Bs-oBs), fabs(Vs-oVs)));
         } while ( fabs(Us-oUs)>0.0001 || fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001);
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: U @ "+ sd[sf[FILT_Ui]].DATEs + ", B @ "+ sd[sf[FILT_Bi]].DATEs
           + ", V @ "+ sd[sf[FILT_Vi]].DATEs;
         rUs= rus, rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roUs= rUs, roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rUs = fTx_yz( u, u, b, Tu_ub, rTu_ub, 11, Utdesc);
            rBs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            rVs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 11, Vtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f", fabs(rUs-roUs), fabs(rBs-roBs), fabs(rVs-roVs)));
         } while ( fabs(rUs-roUs)>0.0001 || fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001);
         break;

      case (FILTC_UBVRIs):
         if(Tub==0 || Tbv==0 || Tvr==0 || Tri==0 || Tv_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tub, Tbv, Tv_bv, Tvr, and Tri.";
            d->TRANS= 0;
            break;
         }
         Us= us, Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oUs= Us, oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Us = fTxy( u, b, Tub, rTub, 1, Utdesc);
            Bs = fTxy( b, v, Tbv, rTbv, 1, Btdesc);
            Vs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 1, Vtdesc);
            Rs = fTxy( v, r, Tvr, rTvr, 2, Rtdesc);
            Is = fTxy( r, i, Tri, rTri, 2, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f, %0.4f", fabs(Us-oUs), fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Us-oUs)>0.0001 || fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: U @ "+ sd[sf[FILT_Ui]].DATEs + ", B @ "+ sd[sf[FILT_Bi]].DATEs
           + ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rUs= rus, rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roUs= rUs, roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rUs = fTxy( u, b, Tub, rTub, 11, Utdesc);
            rBs = fTxy( b, v, Tbv, rTbv, 11, Btdesc);
            rVs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 11, Vtdesc);
            rRs = fTxy( v, r, Tvr, rTvr, 12, Rtdesc);
            rIs = fTxy( r, i, Tri, rTri, 12, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f, %0.4f", fabs(rUs-roUs), fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rUs-roUs)>0.0001 || fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case (FILTC_UBVIs):
         if(Tub==0 || Tbv==0 || Tvi==0 || Tb_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tub, Tb_bv, Tbv and Tvi.";
            d->TRANS= 0;
            break;
         }
         Us= us, Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oUs= Us, oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Us = fTxy(u, b, Tub, rTub, 1, Utdesc);
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1, Btdesc); //Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            Vs = fTxy(b, v, Tbv, rTbv, 2, Vtdesc); //Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc));
            Is = fTxy(v, i, Tvi, rTvi, 2, Itdesc); //Is = Vs - (Vc-Ic) - Tvi* ((vs-is)-(vc-ic));
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Us-oUs), fabs(Bs-oBs), fabs(Vs-oVs), fabs(Is-oIs)));
         } while ( fabs(Us-oUs)>0.0001 || fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: U @ "+ sd[sf[FILT_Ui]].DATEs + ", B @ "+ sd[sf[FILT_Bi]].DATEs
           + ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rUs= rus, rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roUs= rUs, roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rUs = fTxy(u, b, Tub, rTub, 11, Utdesc);
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            rVs = fTxy(b, v, Tbv, rTbv, 12, Vtdesc);
            rIs = fTxy(v, i, Tvi, rTvi, 12, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rUs-roUs), fabs(rBs-roBs), fabs(rVs-roVs), fabs(rIs-roIs)));
         } while ( fabs(rUs-roUs)>0.0001 || fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case (FILTC_UBVs):
         if(Tub==0 || Tb_bv==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tub, Tb_bv and Tbv.";
            d->TRANS= 0;
            break;
         }
         Us= us, Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oUs= Us, oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Us = fTxy(u, b, Tub, rTub, 1, Utdesc);
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1, Btdesc);
            Vs = fTxy(b, v, Tbv, rTbv, 2, Vtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f", fabs(Us-oUs), fabs(Bs-oBs), fabs(Vs-oVs)));
         } while ( fabs(Us-oUs)>0.0001 || fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001);
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: U @ "+ sd[sf[FILT_Ui]].DATEs + ", B @ "+ sd[sf[FILT_Bi]].DATEs
           + ", V @ "+ sd[sf[FILT_Vi]].DATEs;
         rUs= rus, rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roUs= rUs, roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rUs = fTxy(u, b, Tub, rTub, 11, Utdesc);
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            rVs = fTxy(b, v, Tbv, rTbv, 12, Vtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f", fabs(rUs-roUs), fabs(rBs-roBs), fabs(rVs-roVs)));
         } while ( fabs(rUs-roUs)>0.0001 || fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001);
         break;

      // single filter cases
      case FILTC_Ba:
         if(Tb_bv==0 || d->BVcolor==-999) {
            d->ErrorMsg+= " Missing information; need Tb_bv and BVcolor.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1, Btdesc);
            Vs = Bs - d->BVcolor;
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: estimated BVcolor ";
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs= (rBs > d->BVcolorErr)? 0: sqrt( pow(d->BVcolorErr, 2) - pow(rBs, 2) );
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11, Btdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         sprintf(Vtdesc, "%c%c%ccolor=%.4f%c%c%ccolorErr=%.4f", ndelim, toupper(b), toupper(v), d->BVcolor, ndelim, toupper(b), toupper(v), d->BVcolorErr);
         strcat(Btdesc, Vtdesc);
         break;

      case FILTC_Va:
         if(Tv_bv==0 || d->BVcolor==-999) {
            d->ErrorMsg+= " Missing information; need Tv_bv and BVcolor.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Vs = fTx_yz(v, b, v, Tv_bv, rTv_bv, 1, Vtdesc);
            Bs = Vs + d->BVcolor;
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: estimated BVcolor ";
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs= (rVs > d->BVcolorErr)? 0: sqrt( pow(d->BVcolorErr, 2) - pow(rVs, 2) );
            rVs = fTx_yz(v, b, v, Tv_bv, rTv_bv, 11, Vtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         sprintf(Btdesc, "%c%c%ccolor=%.4f%c%c%ccolorErr=%.4f", ndelim, toupper(b), toupper(v), d->BVcolor, ndelim, toupper(b), toupper(v), d->BVcolorErr);
         strcat(Vtdesc, Btdesc);
         break;

      case FILTC_Ra:
         if(Tr_vi==0 || d->VIcolor==-999) {
            d->ErrorMsg+= " Missing information; need Tr_vi and VIcolor.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Vs = 10; // arbitrary; doesn't matter
            Is = Vs - d->VIcolor;
            Rs = fTx_yz(r, v, i, Tr_vi, rTr_vi, 1, Rtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: estimated VIcolor ";
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs= 0;
            rIs= d->VIcolorErr;
            rRs = fTx_yz(r, v, i, Tr_vi, rTr_vi, 11, Rtdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         sprintf(Vtdesc, "%c%c%ccolor=%.4f%c%c%ccolorErr=%.4f", ndelim, toupper(v), toupper(i), d->VIcolor, ndelim, toupper(v), toupper(i), d->VIcolorErr);
         strcat(Rtdesc, Vtdesc);
         break;



      case FILTC_Ia:
         if(Ti_vi==0 || d->VIcolor==-999) {
            d->ErrorMsg+= " Missing information; need Ti_vi and VIcolor.";
            d->TRANS= 0;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= 0; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Is = fTx_yz(i, v, i, Ti_vi, rTi_vi, 1, Itdesc);
            Vs = Is + d->VIcolor;
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("V %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: estimated VIcolor ";
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= 0; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs= (rIs > d->VIcolorErr)? 0: sqrt( pow(d->VIcolorErr, 2) - pow(rIs, 2) );
            rIs = fTx_yz(i, v, i, Ti_vi, rTi_vi, 11, Itdesc);
            if(Debug) Form1->Memo4->Lines->Add(as.sprintf("E %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         sprintf(Vtdesc, "%c%c%ccolor=%.4f%c%c%ccolorErr=%.4f", ndelim, toupper(v), toupper(i), d->VIcolor, ndelim, toupper(v), toupper(i), d->VIcolorErr);
         strcat(Itdesc, Vtdesc);
         break;








      default: // unknown combination
         d->ErrorMsg+= " Un-supported filter combination: ";
         for(int j= 0; j< FILT_NUM; j++) {
            if(fc & FILT_mask[j]) {
               d->ErrorMsg+= " ";
               d->ErrorMsg+= FILT_name[j];
            }
         }
         d->ErrorMsg+= "; not transformed.";
         d->TRANS= 0;
         break;
   }

   if(d->TRANS == 2) {
      switch(d->filter) {
         case FILT_Ui: d->VMAGt= Us; d->VERRt= rUs; d->NOTES+= Utdesc; break;
         case FILT_Bi: d->VMAGt= Bs; d->VERRt= rBs; d->NOTES+= Btdesc; break;
         case FILT_Vi: d->VMAGt= Vs; d->VERRt= rVs; d->NOTES+= Vtdesc; break;
         case FILT_Ri: d->VMAGt= Rs; d->VERRt= rRs; d->NOTES+= Rtdesc; break;
         case FILT_Ii: d->VMAGt= Is; d->VERRt= rIs; d->NOTES+= Itdesc; break;
      }
   }
   d->processed= true;
}



    /*
structures

Star
   name


Process:
- work form AIPWINV2 output
       #SOFTWARE=AIP4Win v. 2.2.0
       #TOOL=Multi-Image Photometry Tool

- first pass through file:
   - get DELIMT
   - pick up CREFMAG if available
   - collect obs data so that VMAG is observerd
      if ABS or STD then VMAG+= CMAG - CREFMAG
      if DIF        then VMAG+= CMAG

      */

void __fastcall TForm1::ToClipboardButtonClick(TObject *Sender)
{
      Memo2->SelectAll();
      Memo2->CopyToClipboard();
}
//---------------------------------------------------------------------------



AnsiString UnTransformedFile= "";

void __fastcall TForm1::Open1Click(TObject *Sender)
{
  FILE *stream;
  char Line[512];
  AnsiString s;

   OpenDialog1->Title= "Add a un-transformed observation file(s)";
   //OpenDialog1->Filter = "any txt file (*.*)|*.*";
   OpenDialog1->Options << ofAllowMultiSelect << ofFileMustExist;
   OpenDialog1->Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*";
   OpenDialog1->FilterIndex = 0; // start the dialog showing all files
   if(OpenDialog1->Execute()){
      // put into the Input window
      UnTransformedFile= OpenDialog1->FileName;
      //Memo1->Lines->LoadFromFile(UnTransformedFile);
      for (int I = 0; I < OpenDialog1->Files->Count; I ++) {
         // Memo1->Lines->LoadFromFile(OpenDialog1->Files->Strings[I]); clears
         stream = fopen(OpenDialog1->Files->Strings[I].c_str(), "r");
         if(stream) {
           while(fgets(Line, sizeof(Line), stream))
              Memo1->Lines->Append(Line);
           fclose(stream);
         }
      }
   };
   // Save the directory
   TIniFile *ini= new TIniFile(INIfilename);
   ini->WriteString("Setup", "Dir", OpenDialog1->InitialDir);
   delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Saveuntransformedobsfile1Click(TObject *Sender)
{
   // use same dir as open
   SaveDialog1->InitialDir= OpenDialog1->InitialDir;
   SaveDialog1->FileName= UnTransformedFile;
   SaveDialog1->Title= "Save the un-transformed observation file";
   SaveDialog1->Filter = "TXT files (*.txt)|*.TXT";
   if(SaveDialog1->Execute()) {
//      // clean up empty lines
//      for(int i= 0; i< Memo1->Lines->Count; i++)
//         if(Memo1->Lines->Strings[i].Length() < 2)
//           //Memo4->Lines->Add(Memo1->Lines->Strings[i].Length());
//            Memo1->Lines->Delete(i),  i--;
//  see http://delphi.about.com/od/objectpascalide/a/remove-empty-line-tstringlist-savetofile.htm
      Memo1->Lines->SaveToFile(SaveDialog1->FileName);
      UnTransformedFile= SaveDialog1->FileName;
   }
   OpenDialog1->InitialDir= SaveDialog1->InitialDir;
   // Save the directory
   TIniFile *ini= new TIniFile(INIfilename);
   ini->WriteString("Setup", "Dir", OpenDialog1->InitialDir); // nb same as SaveDialog1
   delete ini;
}
//---------------------------------------------------------------------------

// save transformed data
void __fastcall TForm1::Save1Click(TObject *Sender)
{
   // use same dir as open
   SaveDialog1->InitialDir= OpenDialog1->InitialDir;
   SaveDialog1->Title= "Save the transformed observation file";
   SaveDialog1->Filter = "TXT files (*.txt)|*.TXT";
   AnsiString f= UnTransformedFile;
   if(f.Length()> 5 && '.'== f[f.Length()- 3]) f= f.SubString(1, f.Length()-4)+ "_TA" + f.SubString(f.Length()-3, 4);
   SaveDialog1->FileName= f;
   if(SaveDialog1->Execute()) {
      Memo2->Lines->SaveToFile(SaveDialog1->FileName);
   }
   OpenDialog1->InitialDir= SaveDialog1->InitialDir;
   // Save the directory
   TIniFile *ini= new TIniFile(INIfilename);
   ini->WriteString("Setup", "Dir", OpenDialog1->InitialDir);
   delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SaveReporttofile1Click(TObject *Sender)
{
   // use same dir as open
   SaveDialog1->InitialDir= OpenDialog1->InitialDir;
   SaveDialog1->Title= "Save the transformation report to file";
   SaveDialog1->Filter = "TXT files (*.txt)|*.TXT";
   AnsiString f= UnTransformedFile;
   if(f.Length()> 5 && '.'== f[f.Length()- 3]) f= f.SubString(1, f.Length()-4)+ "_TA Report" + f.SubString(f.Length()-3, 4);
   SaveDialog1->FileName= f;
   if(SaveDialog1->Execute()) {
      Memo4->Lines->SaveToFile(SaveDialog1->FileName);
   }
   OpenDialog1->InitialDir= SaveDialog1->InitialDir;
   /*
   // Save the directory
   TIniFile *ini= new TIniFile(INIfilename);
   ini->WriteString("Setup", "Dir", OpenDialog1->InitialDir);
   delete ini;
   */
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Exit1Click(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------


// test code for getREFMAG
void __fastcall TForm1::Button2Click(TObject *Sender)
{
   AnsiString s;
   StarData sd;
   sd.CNAME= "000-BCN-754";
   sd.CHART= "13300NXP"; //"13311ABQ"; //"13300NXP";
   sd.filter= 2;
   getREFMAG(&sd, true);

   s.printf("%f %f", sd.CREFmag, sd.CREFerr);
   Memo4->Lines->Add(s );
   Memo4->Lines->Add(sd.ErrorMsg);

   sd.filter= 3;
   getREFMAG(&sd, true);

   s.printf("%f %f", sd.CREFmag, sd.CREFerr);
   Memo4->Lines->Add(s );
   Memo4->Lines->Add(sd.ErrorMsg);


}


//int __fastcall TForm1::checkChart(TObject *Sender, StarData* sd) {
int __fastcall checkChart(StarData* sd, bool getc) {
    bool   auid= false, chartExists= false;
    int  ret= 1, i, labelCnt= 0, labelIndex= 0;
    AnsiString NAME= getc? sd->CNAME: sd->KNAME;
    // CNAME an AUID?
    if(NAME.Length()==11 && NAME[4]=='-' && NAME[8]=='-')
       auid= true;
    // look for it in the current chart data
    for(i= 0; i< min(chartNext, chartMAX); i++) {
       if(chart[i].chartid == sd->CHART) { // same chart
          chartExists= true;
          if(auid) {
             if(chart[i].AUID == NAME) break;
          } else { // maybe its a label
             if(chart[i].label== NAME) labelCnt++, labelIndex= i;
             // don't break here; looking for duplicate labels
          }
       }
    }
    // did we find it?
    if(i != min(chartNext, chartMAX)) { // yes
       if(getc) {
          sd->CREFmag= chart[i].sdata[sd->filter].mag;
          sd->CREFerr= chart[i].sdata[sd->filter].err;
          sd->CRA= chart[i].RA;
          sd->CDec= chart[i].Dec;
       } else {
          sd->KREFmag= chart[i].sdata[sd->filter].mag;
          sd->KREFerr= chart[i].sdata[sd->filter].err;
          sd->KRA= chart[i].RA;
          sd->KDec= chart[i].Dec;
       }
    } else if(labelCnt == 1) { // found unique label
       if(getc) {
          sd->CREFmag= chart[labelIndex].sdata[sd->filter].mag;
          sd->CREFerr= chart[labelIndex].sdata[sd->filter].err;
          sd->CNAME= chart[labelIndex].AUID; // replace label with AUID
          sd->CRA= chart[labelIndex].RA;
          sd->CDec= chart[labelIndex].Dec;
       } else {
          sd->KREFmag= chart[labelIndex].sdata[sd->filter].mag;
          sd->KREFerr= chart[labelIndex].sdata[sd->filter].err;
          sd->KNAME= chart[labelIndex].AUID; // replace label with AUID
          sd->KRA= chart[labelIndex].RA;
          sd->KDec= chart[labelIndex].Dec;
       }
    } else if(labelCnt > 1) {
       sd->ErrorMsg+= " Duplicate label in chart.";
       ret= -1;
    } else if(chartExists) {
       ret= -1;
    } else  ret= 0;
    return ret;
}

double __fastcall hhmmss2radians(char *z) {
   double h, m, s, r;
   if(3==sscanf(z, "%lf:%lf:%lf", &h, &m, &s))
      r= 15.0 * ((s/60.0+ m)/60.0 + h); // deg
   else // it was in decimal degrees
      r= h;
   return r * 0.017453293; // pi/180
}

double __fastcall ddmmss2radians(char* z) {
   double d, m, s, r;
   bool neg;
   neg= z[0]=='-';
   if(neg) z[0]= ' ';
   if(3==sscanf(z, "%lf:%lf:%lf", &d, &m, &s))
      r= ((s/60.0+ m)/60.0 + d);
   else // it was in decimal degrees
      r= d;
   if(neg) r*= -1.0;
   return r * 0.017453293; // pi/180
}

//---------------------------------------------------------------------------
//int __fastcall TForm1::getCREFMAG(TObject *Sender, StarData* sd)
int __fastcall getREFMAG(StarData* sd, bool getc)
{   // fill in sd C and K info from chart[]
   int     I, ret, j, i;
   char cp[100000];
   AnsiString as;

   ret= checkChart(sd, getc);
   if(ret==1 || ret==-1) { // found or not in chart
      return (ret==1)? 1: 0;
   } else { // no. let's get more chart data
      if(sd->CHART.Length()) { // assuming they gave us a chartid
/*
          // old VSP method
          char* pch;
          char  pchart[45][20];
          int pi[]= {7, 10, 13, 16, 4};  // [FILT_NUM];
          AnsiString u= "http://www.aavso.org/cgi-bin/vsp.pl?chartid="+ sd->CHART+ "&delimited=yes&ccdtable=on";
//          AnsiString u= "http://www.aavso.org/cgi-bin/vsp_test.pl?chartid="+ sd->CHART+ "&delimited=yes&ccdtable=on";
          if(Form1->UseStdField->Checked) u+= "&std_field=on";
          // eg
          if(!Form1->httpGet(u, cp, sizeof(cp))) {
             sd->ErrorMsg+= " failed chart request.";
             return 0;
          }
          if(strstr(cp, "Sorry, we cannot")) {
             sd->ErrorMsg+= " Bad chart reference.";
             return 0;
          }
          pch= strtok(cp, "|\n\r"); j= 0;
          while(pch != NULL) {
             strcpy(pchart[j % 45 ], pch);
             pch= strtok(NULL, "|\n\r");
             j++;
             if(0== j % 45) {
                // doc at http://www.aavso.org/get-api-access-variable-star-database
                chart[chartNext % chartMAX].chartid= AnsiString(pchart[44]).TrimLeft().TrimRight();
                chart[chartNext % chartMAX].AUID= AnsiString(pchart[0]).TrimLeft().TrimRight();
                chart[chartNext % chartMAX].label= AnsiString(pchart[3]).TrimLeft().TrimRight();
                for(i= 0; i< FILT_NUM; i++) {
                   if(strcmp(" ", pchart[pi[i]]))
                      sscanf(pchart[pi[i]], " %f", &chart[chartNext % chartMAX].sdata[i].mag);
                   if(strcmp(" ", pchart[pi[i]+1]))
                      sscanf(pchart[pi[i]+1], " %f", &chart[chartNext % chartMAX].sdata[i].err);
                }
                chart[chartNext % chartMAX].RA=  hhmmss2radians(pchart[1]);
                chart[chartNext % chartMAX].Dec= ddmmss2radians(pchart[2]);
                chartNext++;
             }
          }

*/

         // new API call
         AnsiString u= "http://www.aavso.org/apps/vsp/api/chart/"+ sd->CHART+ "/?format=xml";
         if(Form1->UseStdField->Checked) u+= "&special=std_field";
         // eg   http://www.aavso.org/apps/vsp/api/chart/2164EAF/?format=xml
         if(!Form1->httpGet(u, cp, sizeof(cp))) {
            sd->ErrorMsg+= " failed VSP API chart request.";
            return 0;
         }
          // cp not big enough?
//          FILE * tf= fopen("testing.txt", "wt");
//          fwrite(cp, 1, strlen(cp), tf);
//          fclose(tf);
         Form1->EasyXmlScanner1->LoadFromBuffer(cp);
         Form1->EasyXmlScanner1->XmlParser->Normalize= true;
         Form1->EasyXmlScanner1->XmlParser->StartScan();
         int fref, listLevel= 0;
         chartrow cr; sdatas sds;
         AnsiString CurName, CurContent, ChartID= sd->CHART;
         while (Form1->EasyXmlScanner1->XmlParser->Scan()) {

            CurName= Form1->EasyXmlScanner1->XmlParser->CurName;
            CurContent= Form1->EasyXmlScanner1->XmlParser->CurContent;
            switch(Form1->EasyXmlScanner1->XmlParser->CurPartType) {
               case ptStartTag:
                  if(CurName == "list-item") {
                     listLevel+= 1;
                     // prep for collecting data from the list item
                     switch(listLevel) {
                        case 1: // photometry
                           cr.chartid= ChartID;
                           cr.AUID= "";
                           cr.label= "";
                           cr.RA= cr.Dec= 0.0;
                           for(int i= 0; i< FILT_NUM; i++) cr.sdata[i].mag= cr.sdata[i].err= 0.0;
                           break;
                        case 2: // band
                           fref= 99;
                           sds.mag= sds.err= 0.0;
                           break;
                     }
                  }
                  break;

               case ptContent:
                  switch(listLevel) {
                     case 0: // root
                        //<comment/>
                        //<star>RR And</star>
                        //<maglimit>16.0</maglimit>
                        //<special/>
                        //<auid>000-BBC-124</auid>
                        //<fov>30.0</fov>
                        //<dec>24:59:55.9</dec> nb not RR And
                        //<title/>
                        //<dss>False</dss>
                        if(CurName == "chartid") { //<chartid>X15267EM</chartid>
                           if(CurContent!=sd->CHART) {
                              sd->ErrorMsg+= " chart mismatch: fetched= "+ CurContent+ " and requested="+ ChartID;
                              return 0;
                           }   
                        }
                        //<image_uri>https://www.aavso.org/apps/vsp/chart/X15267EM.png</image_uri>
                        //<ra>21:44:39.68</ra> nb not RR And
                        //<resolution>100</resolution>
                        //<photometry> list
                        break; // root
                     case 1: // photometry
                        if(CurName == "auid")  //<auid>000-BBC-116</auid>
                           cr.AUID= CurContent;
                        //<bands> list
                        //<comments/>
                        else if(CurName == "label")  //<label>94</label>
                           cr.label= CurContent;
                        else if(CurName == "ra")  //<ra>00:51:10.69</ra>
                           cr.RA= hhmmss2radians(CurContent.c_str());
                        else if(CurName == "dec")  //<dec>34:16:27.3</dec>
                           cr.Dec= ddmmss2radians(CurContent.c_str());
                        break;
                     case 2: // bands
                        if(CurName == "band") //<band>V</band>
                           for(fref= 0; fref< FILT_NUM; fref++) { // which filter? set fref
                              if(CurContent.SubString(1, FILT_name[fref].Length()) == FILT_name[fref])
                                 break; // nb. API returns "B", "V", "Rc", "Ic"
                           }
                        else if(CurName == "mag") sds.mag= CurContent.ToDouble(); //<mag>9.384</mag>
                        else if(CurName == "error") sds.err= CurContent.ToDouble(); //<error>0.082</error>
                        break;
                  }
                  break;

               case ptEndTag:
                  if(CurName == "list-item") {
                     // use the data collected the list item
                     switch(listLevel) {
                        case 2: // band
                           if(fref< FILT_NUM)
                              cr.sdata[fref]= sds;
                           break;
                        case 1: // photometry
                           chart[chartNext % chartMAX]= cr;
                           chartNext++;
                           break;
                     }
                     listLevel-= 1;
                  }
                  break;
            } //switch CurPartType
         } //while
          // so, let's try again
          return (checkChart(sd, getc)== 1)? 1: 0;
      } else {
          sd->ErrorMsg+= " No chartid.";
          return 0;
      }
   }
}




// get target data from VSX
int __fastcall checkTargs(StarData* sd)
{
    int  ret= 1, i;
    // look for it in the current target data
    for(i= 0; i< min(targNext, targMAX); i++) {
       if(targs[i].name == sd->NAME)
          break;
    }
    // did we find it?
    if(i != min(targNext, targMAX)) { // yes
       sd->VRA= targs[i].RA;
       sd->VDec= targs[i].Dec;
       sd->varType= targs[i].varType;
       sd->specType= targs[i].specType;
    } else
       ret= 0;
    return ret;
}

int __fastcall getTargInfo(StarData* sd)
{
    int  ret;
    char cp[60000];
    AnsiString s, u;
    double RA, Dec;

    ret= checkTargs(sd);
    if(ret==0) { // found?
       if(sd->NAME.Length()) { // assuming they gave us a NAME
          u= "http://www.aavso.org/vsx/index.php?view=query.votable&ident="+ sd->NAME;
          // eg   http://www.aavso.org/vsx/index.php?view=query.votable&ident=SX+Uma
          if(!Form1->httpGet(u, cp, sizeof(cp))) {
             sd->ErrorMsg+= " failed VSX request.";
          } else {
             Form1->EasyXmlScanner1->LoadFromBuffer(cp);
             Form1->EasyXmlScanner1->XmlParser->Normalize= true;
             Form1->EasyXmlScanner1->XmlParser->StartScan();
             //-- Loop thru the file for each item. Just look for TD items, assuming
             //   that they will not changed order (ie, not keying off the field table
             int cnt= 0;
             // not a very safe parsing. Assumes too much
             while (Form1->EasyXmlScanner1->XmlParser->Scan()) {
                if(Form1->EasyXmlScanner1->XmlParser->CurPartType == ptStartTag) {
                   if(Form1->EasyXmlScanner1->XmlParser->CurName == "TD") {
                      Form1->EasyXmlScanner1->XmlParser->Scan(); // pop out content
                      switch(++cnt) {
                         case  2: s= Form1->EasyXmlScanner1->XmlParser->CurContent;
                                  if(sd->NAME.UpperCase() != s.UpperCase()){
                                     sd->ErrorMsg+= " VSX did not find star: "+ sd->NAME+ "  "+ s;
                                     return 0;
                                  }
                                  break;
                         case  4: s= Form1->EasyXmlScanner1->XmlParser->CurContent;
                                  sscanf(s.c_str(), "%lf,%lf", &RA, &Dec);
                                  sd->VRA= RA/ 57.29577951; // to radians
                                  sd->VDec= Dec/ 57.29577951;
                                  break;
                         case  5: sd->varType= Form1->EasyXmlScanner1->XmlParser->CurContent;
                                  break;
                         case 14: sd->specType= Form1->EasyXmlScanner1->XmlParser->CurContent;
                                  // add to targs
                                  targs[targNext % targMAX].name= sd->NAME;
                                  targs[targNext % targMAX].RA= sd->VRA;
                                  targs[targNext % targMAX].Dec= sd->VDec;
                                  targs[targNext % targMAX].varType= sd->varType;
                                  targs[targNext % targMAX].specType= sd->specType;
                                  targNext++;
                                  return 1;
                                  //break;
                      }
                   }
                }
             } // end while
          }
       }
    }
    return ret;
}

/*   http://www.aavso.org/vsx/index.php?view=query.votable&ident=SX+Uma
<?xml version="1.0" encoding="UTF-8"?>
<VOTABLE version="1.0">
    <RESOURCE>
        <DESCRIPTION>International Variable Star Index (VSX) Query Results</DESCRIPTION>
        <TABLE>
            <FIELD id="auid" name="AUID"/>
            <FIELD id="name" name="Name"/>
            <FIELD id="const" name="Const"/>
            <FIELD id="radec2000" name="Coords(J2000)"/>
            <FIELD id="varType" name="VarType"/>
            <FIELD id="maxMag" name="MaxMag"/>
            <FIELD id="maxPass" name="MaxMagPassband"/>
            <FIELD id="minMag" name="MinMag"/>
            <FIELD id="minPass" name="MinMagPassband"/>
            <FIELD id="epoch" name="Epoch"/>
            <FIELD id="novaYr" name="NovaYear"/>
            <FIELD id="period" name="Period"/>
            <FIELD id="riseDur" name="RiseDuration"/>
            <FIELD id="specType" name="SpecType"/>
            <FIELD id="disc" name="Discoverer"/>
            <DATA>
                <TABLEDATA>
                    <TR>
                        <TD>000-BDB-211</TD>
                        <TD>SX UMa</TD>
                        <TD>UMa</TD>
                        <TD>201.55608000,56.25697000</TD>
                        <TD>RRC</TD>
                        <TD>10.580</TD>
                        <TD>V</TD>
                        <TD>11.210</TD>
                        <TD>V</TD>
                        <TD>45109.33300</TD>
                        <TD/>
                        <TD>0</TD>
                        <TD>38</TD>
                        <TD>A4-F5</TD>
                        <TD/>
                    </TR>
                </TABLEDATA>
            </DATA>
        </TABLE>
    </RESOURCE>
</VOTABLE>
*/




void __fastcall TForm1::Button1Click(TObject *Sender)
{

// dump chart data
   AnsiString s, a;
   Memo5->Clear();
   for(int i= 0; i< chartNext; i++) {
      s.sprintf("%s, %s, %s", chart[i].chartid, chart[i].AUID, chart[i].label);
      for(int j= 0; j< FILT_NUM; j++) {
         s+= a.sprintf( ", %5.3f, %5.3f", chart[i].sdata[j].mag, chart[i].sdata[j].err);
      }
      s+= a.sprintf(", %10.6f, %10.6f", chart[i].RA, chart[i].Dec);
      Memo5->Lines->Add(s);
   }

/*
// xml xperiments http://www.aavso.org/vsx/index.php?view=query.votable&ident=S+Hya in file S_Hya.xml

//   http://www.aavso.org/apps/vsp/api/chart/2164EAF/?format=xml  is test.xml
//   http://www.destructor.de/xmlparser/doc.htm
  EasyXmlScanner1->LoadFromFile("test.xml");
//  XmlScanner1->Execute();

  // This is a really ugly way to extract data from the VOTable xml. sigh.
  //-- Init the scanning.
  EasyXmlScanner1->XmlParser->Normalize= true;
  EasyXmlScanner1->XmlParser->StartScan();

  //-- Loop thru the file for each item.
  AnsiString s, a;
  Memo5->Clear();
  while (EasyXmlScanner1->XmlParser->Scan()) {
     if(EasyXmlScanner1->XmlParser->CurPartType == ptContent) {
        s.sprintf("%s : %s ", EasyXmlScanner1->XmlParser->CurName, EasyXmlScanner1->XmlParser->CurContent);
        Memo5->Lines->Add(s);
     }
  }
*/
}
//---------------------------------------------------------------------------




void __fastcall TForm1::Button3Click(TObject *Sender)
{
    Memo4->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
   Memo1->Clear();
}
//---------------------------------------------------------------------------




float fTxy (char x, char y, float Txy, float rTxy, int mode, char* tdesc) {
     // Secondary or Color form:  Txy
     //  1/slope of (x-y) vs (X-Y) :
     //    Txy= ((Xs-Ys)-(Xc-Yc)) / ((xs-ys)-(xc-yc))
    float Xs, xs, Xc, xc, Ys, ys, Yc, yc, r, s, rXs, rxs, rXc, rxc, rYs, rys, rYc, ryc, a, ra2, b, rb2;
    sprintf(tdesc, "%cT%c%c=%.4f%cT%c%cErr=%.4f", ndelim, x, y, Txy, ndelim, x, y, rTxy);
    switch(x) {
       case 'u': Xs= Us; xs= us; Xc= Uc; xc= uc;  rXs= rUs; rxs= rus; rXc= rUc; rxc= ruc; break;
       case 'b': Xs= Bs; xs= bs; Xc= Bc; xc= bc;  rXs= rBs; rxs= rbs; rXc= rBc; rxc= rbc; break;
       case 'v': Xs= Vs; xs= vs; Xc= Vc; xc= vc;  rXs= rVs; rxs= rvs; rXc= rVc; rxc= rvc; break;
       case 'r': Xs= Rs; xs= rs; Xc= Rc; xc= rc;  rXs= rRs; rxs= rrs; rXc= rRc; rxc= rrc; break;
       case 'i': Xs= Is; xs= is; Xc= Ic; xc= ic;  rXs= rIs; rxs= ris; rXc= rIc; rxc= ric; break;
    }
    switch(y) {
       case 'u': Ys= Us; ys= us; Yc= Uc; yc= uc;  rYs= rUs; rys= rus; rYc= rUc; ryc= ruc; break;
       case 'b': Ys= Bs; ys= bs; Yc= Bc; yc= bc;  rYs= rBs; rys= rbs; rYc= rBc; ryc= rbc; break;
       case 'v': Ys= Vs; ys= vs; Yc= Vc; yc= vc;  rYs= rVs; rys= rvs; rYc= rVc; ryc= rvc; break;
       case 'r': Ys= Rs; ys= rs; Yc= Rc; yc= rc;  rYs= rRs; rys= rrs; rYc= rRc; ryc= rrc; break;
       case 'i': Ys= Is; ys= is; Yc= Ic; yc= ic;  rYs= rIs; rys= ris; rYc= rIc; ryc= ric; break;
    }
    // zero the comp ref error; comp reference errors excluded from final error
    rXc= rYc= 0;
    switch(mode) {
       case 1: r= Xs=  Ys + (Xc-Yc) +  Txy * ((xs-ys)-(xc-yc));   break;
       case 2: r= Ys=  Xs - (Xc-Yc) -  Txy * ((xs-ys)-(xc-yc));   break;

       case 11: // Xs= a + Txy * b
	        /*a= Ys + (Xc-Yc);*/ ra2= pow(rYs,2)+pow(rXc,2)+pow(rYc,2);
			b= ((xs-ys)-(xc-yc)); rb2= pow(rxs,2)+pow(rys,2)+pow(rxc,2)+pow(ryc,2);
			r= rXs= sqrt( ra2 + pow(Txy*b,2)*( pow(rTxy/Txy,2) + ((b==0)?0:rb2/pow(b,2)) ));
            //s= b; r= rXs=  sqrt( (pow(rYs,2)+pow(rXc,2)+pow(rYc,2)) +  pow(Txy,2)* (pow(rTxy/Txy,2) + (pow(rxs,2)+pow(rys,2)+pow(rxc,2)+pow(ryc,2))/pow(s,2))  );
            break;
       case 12: // Ys= a - Txy * b
	        /*a= Xs - (Xc-Yc);*/ ra2= pow(rXs,2)+pow(rXc,2)+pow(rYc,2);
			b= ((xs-ys)-(xc-yc)); rb2= pow(rxs,2)+pow(rys,2)+pow(rxc,2)+pow(ryc,2);
			r= rYs= sqrt( ra2 + pow(Txy*b,2)*( pow(rTxy/Txy,2) + ((b==0)?0:rb2/pow(b,2)) ));
            // orig s= b; r= rYs=  sqrt( (pow(rXs,2)+pow(rXc,2)+pow(rYc,2)) +  pow(Txy  ,2)* (pow(rTxy/Txy,2) + (pow(rxs,2)+pow(rys,2)+pow(rxc,2)+pow(ryc,2))/pow(s,2))  );
            //      s= b; r= rYs=  sqrt( (pow(rXs,2)+pow(rXc,2)+pow(rYc,2)) +  pow(Txy*s,2)* (pow(rTxy/Txy,2) + (pow(rxs,2)+pow(rys,2)+pow(rxc,2)+pow(ryc,2))/pow(s,2))  );
            break;
    }
    return r;
}

// not used yet
char * fTx_yzFmt(char x, char y, char z, int mode, char* s) {
    char X= toupper(x);
    char Y= toupper(y);
    char Z= toupper(z);
    switch(mode) {
      case 1: sprintf(s, "%cs= %cs + (%cc-%cc) + T%c_%c%c * ((%cs-%cs)-(%cc-%cc))", X, x, X, x, x, y, z, Y, Z, Y, Z);  break;
      case 2: sprintf(s, "%cs= %cs - (%cc-%cc) + ((%cs-%cs)-(%cc-%cc)) / T%c_%c%c", Y, Z, Y, Z, X, x, X, x, x, y, z);  break;
      //case 3: r= Zs= Ys + (Yc-Zc) + ((Xs-xs)-(Xc-xc)) / Tx_yz;  break;
    }
    return s;

}
float fTx_yz (char x, char y, char z, float Tx_yz, float rTx_yz, int mode, char *tdesc) {
    //Primary or star form:  Tx_yz
    //  slope of (X-x) vs (Y-Z) :
    //  Tx_yz= ((Xs-xs)-(Xc-xc)) / ((Ys-Zs)-(Yc-Zc))
    float Xs, xs, Xc, xc, Ys, ys, Yc, yc, Zs, zs, Zc, zc, r, s;
    float rXs, rxs, rXc, rxc, rYs, rys, rYc, ryc, rZs, rzs, rZc, rzc;
    sprintf(tdesc, "%cT%c_%c%c=%.4f%cT%c_%c%cErr=%.4f", ndelim, x, y, z, Tx_yz, ndelim, x, y, z, rTx_yz);

    switch(x) {
       case 'u': Xs= Us; xs= us; Xc= Uc; xc= uc;  rXs= rUs; rxs= rus; rXc= rUc; rxc= ruc; break;
       case 'b': Xs= Bs; xs= bs; Xc= Bc; xc= bc;  rXs= rBs; rxs= rbs; rXc= rBc; rxc= rbc; break;
       case 'v': Xs= Vs; xs= vs; Xc= Vc; xc= vc;  rXs= rVs; rxs= rvs; rXc= rVc; rxc= rvc; break;
       case 'r': Xs= Rs; xs= rs; Xc= Rc; xc= rc;  rXs= rRs; rxs= rrs; rXc= rRc; rxc= rrc; break;
       case 'i': Xs= Is; xs= is; Xc= Ic; xc= ic;  rXs= rIs; rxs= ris; rXc= rIc; rxc= ric; break;
    }
    switch(y) {
       case 'u': Ys= Us;  Yc= Uc;  rYs= rUs;  rYc= rUc; break;
       case 'b': Ys= Bs;  Yc= Bc;  rYs= rBs;  rYc= rBc; break;
       case 'v': Ys= Vs;  Yc= Vc;  rYs= rVs;  rYc= rVc; break;
       case 'r': Ys= Rs;  Yc= Rc;  rYs= rRs;  rYc= rRc; break;
       case 'i': Ys= Is;  Yc= Ic;  rYs= rIs;  rYc= rIc; break;
    }
    switch(z) {
       case 'u': Zs= Us;  Zc= Uc;  rZs= rUs;  rZc= rUc; break;
       case 'b': Zs= Bs;  Zc= Bc;  rZs= rBs;  rZc= rBc; break;
       case 'v': Zs= Vs;  Zc= Vc;  rZs= rVs;  rZc= rVc; break;
       case 'r': Zs= Rs;  Zc= Rc;  rZs= rRs;  rZc= rRc; break;
       case 'i': Zs= Is;  Zc= Ic;  rZs= rIs;  rZc= rIc; break;
    }

    // zero the comp ref error; comp reference errors excluded from final error
    rXc= rYc= rZc= 0;
    switch(mode) {
      case 1: r= Xs= xs + (Xc-xc) + Tx_yz * ((Ys-Zs)-(Yc-Zc));  break;
      case 2: r= Ys= Zs - (Yc-Zc) + ((Xs-xs)-(Xc-xc)) / Tx_yz;  break;
      case 3: r= Zs= Ys + (Yc-Zc) + ((Xs-xs)-(Xc-xc)) / Tx_yz;  break;

      case 11:
         s= ((Ys-Zs)-(Yc-Zc));
         if(s==0) r= rXs= 0;
         else r= rXs= sqrt((pow(rxs,2)+pow(rXc,2)+pow(rxc,2)) + pow(Tx_yz*s,2)*(pow(rTx_yz/Tx_yz, 2) + (pow(rYs,2)+pow(rZs,2)+pow(rYc,2)+pow(rZc,2))/pow(s,2)));
         break;
      case 12:
         s= ((Xs-xs)-(Xc-xc));
         if(s==0) r= rYs= 0;
         else r= rYs= sqrt((pow(rZs,2)+pow(rYc,2)+pow(rZc,2)) + pow(s/Tx_yz,2)*(pow(rTx_yz/Tx_yz, 2) + (pow(rXs,2)+pow(rxs,2)+pow(rXc,2)+pow(rxc,2))/pow(s,2)));
         break;
      case 13:
         s= ((Xs-xs)-(Xc-xc));
         if(s==0) r= rYs= 0;
         else r= rYs= sqrt((pow(rYs,2)+pow(rYc,2)+pow(rZc,2)) + pow(s/Tx_yz,2)*(pow(rTx_yz/Tx_yz, 2) + (pow(rXs,2)+pow(rxs,2)+pow(rXc,2)+pow(rxc,2))/pow(s,2)));
         break;
    }
    return r;
}









void __fastcall TForm1::UseStdFieldClick(TObject *Sender)
{
    // flush gathered chart data
    chartNext= 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox4Click(TObject *Sender)
{
    // suppress clicks on check boxes on the coefficients tab
    TCheckBox *p;
    if((p=dynamic_cast<TCheckBox*>(Sender))==NULL)
       return;
    p->Checked= false;
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button5Click(TObject *Sender)
{
   // test airmass
   AnsiString s;
   float X;

   //X= AirMass(



}
//---------------------------------------------------------------------------

void __fastcall TForm1::CopyReportButtonClick(TObject *Sender)
{
      Memo4->SelectAll();
      Memo4->CopyToClipboard();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::AnalysisTabSheetEnter(TObject *Sender)
{
   // prep menu

}
//---------------------------------------------------------------------------

void __fastcall TForm1::versionLabelClick(TObject *Sender)
{
    if(versionLabel->Tag) {
       ShellExecute(Handle,"open", "https://github.com/gasilvis/TransformApplier/raw/TA/TransformApplier.exe",0,0,SW_SHOW);
//       ShellExecute(Handle,"open", "https://drive.google.com/file/d/0B8twDwcGOO6cSS00V0Z3WFR6dGs/view?usp=sharing",0,0,SW_SHOW);
//       ShellExecute(Handle,"open", "http://www.gasilvis.com/SID/SidDataGrabber.exe",0,0,SW_SHOW);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DSLRcbClick(TObject *Sender)
{
      TIniFile *ini= new TIniFile(INIfilename);
      ini->WriteBool("Setup", "DSLR", DSLRcb->Checked);
      delete ini;
}
//---------------------------------------------------------------------------



bool __fastcall TForm1::IsSafeChar(int ch)
{
   bool Result;
   if     (ch >= 48 && ch <= 57) Result= true;    // 0-9
   else if(ch >= 61 && ch <= 90) Result= true;  // =?>@A-Z
   else if(ch >= 97 && ch <= 122) Result= true;  // a-z
   else if(ch == 33) Result= true; // !
   else if(ch >= 38 && ch <= 42) Result= true; // &'()*
   else if(ch >= 44 && ch <= 46) Result= true; // ,-.
   else if(ch == 95) Result= true; // _
   else if(ch == 126) Result= true; // ~
   else if(ch == 58) Result= true; // :
   else if(ch == 47) Result= true; // /
   else Result= false;
   return Result;
}

AnsiString __fastcall TForm1::EncodeURIComponent(AnsiString ASrc)
{
   AnsiString UTF8String, HexMap= "0123456789ABCDEF", Result= "", ASrcUTF8;
   int I= 1, J= 1;

   ASrcUTF8= ASrc; //ASrcUTF8 := UTF8Encode(ASrc);
   // UTF8Encode call not strictly necessary but
   // prevents implicit conversion warning

   Result.SetLength(ASrcUTF8.Length() * 3); // space to %xx encode every byte
   while(I <= ASrcUTF8.Length()) {
      if(IsSafeChar(ASrcUTF8[I])) {
         Result[J]= ASrcUTF8[I];
         J++;
      }
      else if(ASrcUTF8[I] == ' ') {
         Result[J]= '+';
         J++;
      }
      else {
         Result[J]= '%';
         Result[J+1]= HexMap[(ASrcUTF8[I] >> 4) + 1];
         Result[J+2]= HexMap[(ASrcUTF8[I] & 0x0F) + 1];
         J+= 3;
      }
      I++;
   }
   Result.SetLength(J-1);
   return Result;
}



// http Get stuff
bool __fastcall TForm1::httpGet(AnsiString URL, char* buffer, int bufsize)
{
   TStream *DataIn;
   HttpCli1->URL        = EncodeURIComponent(URL);
   HttpCli1->RcvdStream = NULL;
   char altbuffer[100];
   char* buf;
   if(buffer==NULL) { // return not expected
      buf= altbuffer;
      bufsize= sizeof(buffer);
   } else { buf= buffer; }
   try {
      HttpCli1->Get();
      DataIn = new TFileStream(Form1->HttpCli1->DocName, fmOpenRead);
      DataIn->ReadBuffer(buf, min(bufsize, DataIn->Size));
      buf[min(bufsize, DataIn->Size)]= 0; // terminate string
      delete DataIn;
      remove(HttpCli1->DocName.c_str());
      return true;
   } __except (TRUE) {
      if(Form1->HttpCli1->StatusCode==404) {
         return false; // let caller cope with "not found"
      } else {
         Form1->Memo4->Lines->Add("GET Failed !");
         Form1->Memo4->Lines->Add("StatusCode   = " + IntToStr(Form1->HttpCli1->StatusCode));
         Form1->Memo4->Lines->Add("ReasonPhrase = " + Form1->HttpCli1->ReasonPhrase);
         return false;
      }
   }
}

void __fastcall TForm1::HttpCli1DocBegin(TObject *Sender)
{
    //Memo4->Lines->Add(HttpCli1->ContentType + " => " + HttpCli1->DocName);
    //Memo4->Lines->Add("Document = " + HttpCli1->DocName);
    HttpCli1->RcvdStream = new TFileStream(HttpCli1->DocName, fmCreate);
}

void __fastcall TForm1::HttpCli1DocEnd(TObject *Sender)
{
    if(HttpCli1->RcvdStream) {
        delete HttpCli1->RcvdStream;
        HttpCli1->RcvdStream = NULL;
    }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ObsLonEditExit(TObject *Sender)
{
   sscanf(ObsLonEdit->Text.c_str(), "%lf", &ObsLongitude);
   ObsLongitude/= 57.29577951;
   TIniFile *ini= new TIniFile(INIfilename);
   ini->WriteString("Setup", "ObsLon", ObsLonEdit->Text);
   delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ObsLatEditExit(TObject *Sender)
{
   sscanf(ObsLatEdit->Text.c_str(), "%lf", &ObsLatitude);
   ObsLatitude/= 57.29577951;
   TIniFile *ini= new TIniFile(INIfilename);
   ini->WriteString("Setup", "ObsLat", ObsLatEdit->Text);
   delete ini;
}
//---------------------------------------------------------------------------

float __fastcall AirMass(double JD, double RA, double Dec) // Observatory location is global
{
   // Air Mass	Constants from Citizen Sky spreadsheet
   double a=	1.002432000;
   double b=	0.148386000;
   double c=	0.009646700;
   double d=	0.149864000;
   double e=	0.010296300;
   double f=	0.000303978;
   if(ObsLatitude==0 || ObsLongitude==0) return 0;
   // from https://answers.yahoo.com/question/index?qid=20070830185150AAoNT4i
   double D = JD - 2451545.0; // reference to 1/1/2000 12:00
   double GMST= 18.697374558 + 24.06570982441908* D; // hrs
   GMST*= 0.261799388 ; // hrs to radians
   double LMST= GMST+ ObsLongitude;
   double G11= Dec;
   double G12= LMST- RA;// fmod( LMST- RA, 6.283185307); // HA  not necessary to normalize
   double G13= sin(ObsLatitude)*sin(G11)+ cos(ObsLatitude)*cos(G11)*cos(G12);
   return ((a*G13+ b)*G13+ c)/((((G13+ d)*G13)+ e)*G13+ f);
}




