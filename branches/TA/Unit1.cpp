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
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
   : TForm(Owner)
{

}
//---------------------------------------------------------------------------

#define Version  2.20
bool DEBUG= false;

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
   ,METHOD_Special= 0x8000
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
float Tbv, Tbr,   Tbi,   Tvr, Tvi,   Tri
     ,Tb_bv,  Tb_br, Tb_bi, Tv_vr,  Tv_vi, Tr_ri
     ,Tv_bv, Tr_bv, Ti_bv,  Tr_vr, Tr_vi, Ti_vi   // new
     ,Eb, Ev, Er, Ei, Eu;   // filter extinction coef
// errors for the above
float rTbv, rTbr,   rTbi,   rTvr, rTvi,   rTri
     ,rTb_bv,  rTb_br, rTb_bi, rTv_vr,  rTv_vi, rTr_ri
     ,rTv_bv, rTr_bv, rTi_bv, rTr_vr, rTr_vi, rTi_vi
     ,rEb, rEv, rEr, rEi, rEu;

// global vars for equations
float Bs, bs, Vs, vs, Rs, rs, Is, is,    Bc, bc, Vc, vc, Rc, rc, Ic, ic,    oBs, oVs, oRs, oIs;
float rBs, rbs, rVs, rvs, rRs, rrs, rIs, ris,    rBc, rbc, rVc, rvc, rRc, rrc, rIc, ric,    roBs, roVs, roRs, roIs;
AnsiString Bts, Vts, Rts, Its, Uts;  // DATEs of the star providing this data

float* Extinction[]= { &Eb, &Ev, &Er, &Ei, &Eu }; // filter order

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

void __fastcall TForm1::FormCreate(TObject *Sender)
{
   VersionLabel->Caption= "Version "+ FormatFloat("0.00", Version);
   Form1->Caption= "TA, the AAVSO Transform Applier application, "+ FormatFloat("0.00", Version);
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
      TIniFile *ini;
      ini = new TIniFile(INIfilename);
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
      TIniFile *ini;
      ini = new TIniFile(INIfilename);
      ini->WriteString("Setup", "description", setupEdit->Text);
      delete ini;
      ReadCoefficients(Sender); // resets ram data
}
//---------------------------------------------------------------------------
void __fastcall TForm1::applyExtinctionExit(TObject *Sender)
{
      TIniFile *ini;
      ini = new TIniFile(INIfilename);
      ini->WriteBool("Extinction", "apply", applyExtinction->Checked);
      delete ini;
      ReadCoefficients(Sender); // resets ram data
}
//---------------------------------------------------------------------------

// db to hold transform group information
typedef struct GroupData {
   AnsiString Group;
   char       FILTC; // filter combination (an enum)
   float      m[16]; // Star BVRI, bvri   Comp BVRI bvri
   AnsiString  ts[4];  // tstamp strings of providers of B V R I data
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
   AnsiString  record; // line from the submitted observation file
   AnsiString  narr;  // narrative
   AnsiString  recordT; // line to the output file
   int         gdi;   // index into the GD array   //deprecated
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
   AnsiString  FILT;  /*
    * U: Johnson U
    * B: Johnson B
    * V: Johnson V
    * R: Cousins R
    * I: Cousins I
    * J: NIR 1.2 micron
    * H: NIR 1.6 micron
    * K: NIR 2.2 micron
    * TG: Green Filter
    * Z: Sloan Z
    * CV: Clear (unfiltered), V-band comp star magnitudes (this is more common than CR)
    * CR: Clear (unfiltered), R-band comp star magnitudes
                      */
   bool        TRANS; // YES or NO
   char        MTYPE; // A for ABS, D for DIF, S for STD
   AnsiString  CNAME; // 000-BBV-178
   float       CMAGraw; // from the file
   float       CMAGex;  // corrected for extinction
   float       CMAG;    // final
   float       CERR;
   AnsiString  KNAME; // 000-BBV-175
   float       KMAG;  // 18.627
   float       AMASS; // 1.2606
   bool        AMASSna;
   AnsiString  GROUPs; // 0
   AnsiString  CHART; // 080415
   AnsiString  NOTES;

   bool        processed;
   char        filter;// index into filter list
   float       CREFmag; // reference magnitude of the comparison star
   float       CREFerr;
   unsigned short FILTC; // filter combination the star is mixed in with
   AnsiString  StarsUsed;
   AnsiString  ErrorMsg;   // collect comments here to be displayed after the obs is printed
   bool        ensemble;
} StarData;
#define sdiMAX   1000
StarData sd[sdiMAX];

void SetStarData(StarData d);
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
} chartrow;
#define chartMAX   1000
chartrow chart[chartMAX];
int chartnext= 0;
int __fastcall checkChart(StarData* sd);
int __fastcall getCREFMAG(StarData* sd);



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

// add here
};


// this is the list actually supported. it matches masks to the FILTC_desc
unsigned short FILTC_mask[]= {FILTC_BVRIc, FILTC_BVc, FILTC_VRc, FILTC_VRIc, FILTC_BVRc, FILTC_VIc, FILTC_BVIc
                    ,FILTC_BVRIs, FILTC_BVs, FILTC_VRs, FILTC_VRIs, FILTC_BVRs, FILTC_VIs, FILTC_BVIs
                    ,FILTC_BVRIa, FILTC_BVa, FILTC_VRa, FILTC_VRIa, FILTC_BVRa, FILTC_VIa, FILTC_BVIa
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
    ,"#  variable notation: filter/star. Star s is the target, c is the comp. Capital filter is ref/transforme, lower case is as observed"
    ,"# (Rs - Is) = (Rc - Ic) + Tri * [(rs - is) - (rc - ic)]"
    ,"# Rs = rs + (Rc - rc) + Tr_ri * [(Rs - Is) - (Rc - Ic)], using the solution for (Rs - Is) in the above line"
    ,"# Is = Rs - (Rs - Is), using Rs from the line above, and (Rs - Is) from the first line"
    ,"# Vs = vs + (Vc - vc) + Tv_vr * Tvr * [(vs - rs) - (vc - rc)]"
    ,"# Bs = Vs + (Bc - Vc) + Tbv * [(bs - vs) - (bc - vc)]"
   }
   ,
   {   // FILTC_BVc
     "# BV    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Bs - Vs) = (Bc - Vc) + Tbv * [(bs - vs) - (bc - vc)]"
    ,"# Bs = bs + (Bc - bc) + Tb_bv * [(Bs - Vs) - (Bc - Vc)], using the solution for (Bs - Vs) in the above line"
    ,"# Vs = Bs - (Bs - Vs), using Bs from the line above, and (Bs - Vs) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_VRc
     "# VR    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Vs - Rs) = (Vc - Rc) + Tvr * [(vs - rs) - (vc - rc)]"
    ,"# Vs = vs + (Vc - vc) + Tv_vr * [(Vs - Rs) - (Vc - Rc)], using the solution for (Vs - Rs) in the above line"
    ,"# Rs = Vs - (Vs - Rs), using Vs from the line above, and (Vs - Rs) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_VRIc
     "# VRI    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Rs - Is) = (Rc - Ic) + Tri * [(rs - is) - (rc - ic)]"
    ,"# Rs = rs + (Rc - rc) + Tr_ri * [(Rs - Is) - (Rc - Ic)], using the solution for (Rs - Is) in the above line"
    ,"# Is = Rs - (Rs - Is), using Rs from the line above, and (Rs - Is) from the first line"
    ,"# Vs = vs + (Vc - vc) + Tv_vr * Tvr * [(vs - rs) - (vc - rc)]"
    ,NULL
   }
   ,
   {   // FILTC_BVRc
     "# BVR    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Vs - Rs) = (Vc - Rc) + Tvr * [(vs - rs) - (vc - rc)]"
    ,"# Vs = vs + (Vc - vc) + Tv_vr * [(Vs - Rs) - (Vc - Rc)], using the solution for (Vs - Rs) in the above line"
    ,"# Rs = Vs - (Vs - Rs), using Vs from the line above, and (Vs - Rs) from the first line"
    ,"# Bs = Vs + (Bc - Vc) +  Tbv * [(bs - vs) - (bc - vc)]"
    ,NULL
   }
   ,
   {   // FILTC_VIc
     "# VI    modeled on the VR formulation "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Vs - Is) = (Vc - Ic) + Tvi * [(vs - is) - (vc - ic)]"
    ,"# Vs = vs + (Vc - vc) + Tv_vi * [(Vs - Is) - (Vc - Ic)], using the solution for (Vs - Is) in the above line"
    ,"# Is = Vs - (Vs - Is), using Vs from the line above, and (Vs - Is) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_BVIc
     "# BVI   modeled on the BVR formulation"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Vs - Is) = (Vc - Ic) + Tvi * [(vs - is) - (vc - ic)]"
    ,"# Vs = vs + (Vc - vc) + Tv_vi * [(Vs - Is) - (Vc - Ic)], using the solution for (Vs - Is) in the above line"
    ,"# Is = Vs - (Vs - Is), using Vs from the line above, and (Vs - Is) from the first line"
    ,"# Bs = Vs + (Bc - Vc) +  Tbv * [(bs - vs) - (bc - vc)]"
    ,NULL
   }

/*
   ,
   {   // FILTC_BVRIs
     "# BVRI special "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri* ((rs-is)-(rc-ic))"
    ,NULL
   }
*/
   ,
   {   // FILTC_BVRIs
     "# BVRI special "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Bs=  Vs + (Bc-Vc) + Tbv * ((bs-vs)-(bc-vc))"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Vc-Vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri* ((rs-is)-(rc-ic))"
    ,NULL
   }
   ,
   {   // FILTC_BVs  same as c
     "# BV    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Bs - Vs) = (Bc - Vc) + Tbv * [(bs - vs) - (bc - vc)]"
    ,"# Bs = bs + (Bc - bc) + Tb_bv * [(Bs - Vs) - (Bc - Vc)], using the solution for (Bs - Vs) in the above line"
    ,"# Vs = Bs - (Bs - Vs), using Bs from the line above, and (Bs - Vs) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_VRs  same as c
     "# VR    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Vs - Rs) = (Vc - Rc) + Tvr * [(vs - rs) - (vc - rc)]"
    ,"# Vs = vs + (Vc - vc) + Tv_vr * [(Vs - Rs) - (Vc - Rc)], using the solution for (Vs - Rs) in the above line"
    ,"# Rs = Vs - (Vs - Rs), using Vs from the line above, and (Vs - Rs) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {  // FILTC_VRIs
     "# VRI special "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Vs = vs + (Vc-vc) + Tv_vr * ((Vs-Rs)-(Vc-Rc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic))"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_BVRs
     "# BVR  special "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc))"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_VIs   same as classic
     "# VI    modeled on the VR formulation "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Vs - Is) = (Vc - Ic) + Tvi * [(vs - is) - (vc - ic)]"
    ,"# Vs = vs + (Vc - vc) + Tv_vi * [(Vs - Is) - (Vc - Ic)], using the solution for (Vs - Is) in the above line"
    ,"# Is = Vs - (Vs - Is), using Vs from the line above, and (Vs - Is) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_BVIs
     "# BVI special "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc))"
    ,"#  Is = Vs - (Vc-Ic) - Tvi* ((vs-is)-(vc-ic))"
    ,NULL
    ,NULL
   }


   ,
   {   // FILTC_BVRIa
     "# BVRI AAVSO recommended "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Rs = rs + (Rc-rc) + Tr_vi * ((Vs-Is)-(Vc-Ic))"
    ,"#  Is = is + (Ic-ic) + Ti_vi * ((Vs-Is)-(Vc-Ic))"
    ,NULL
   }
   ,
   {   // FILTC_BVa
     "# BV, AAVSO recommended"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"# Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,NULL
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_VRa  
     "# VR, AAVSO recommended"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# Vs = vs + (Vc-vc) + Tv_vr * ((Vs-Rs)-(Vc-Rc))"
    ,"# Rs = rs + (Rc-rc) + Tr_vr * ((Vs-Rs)-(Vc-Rc))"
    ,NULL
    ,NULL
    ,NULL
   }
   ,
   {  // FILTC_VRIa
     "# VRI, AAVSO recommended "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Vs = vs + (Vc-vc) + Tv_vi * ((Vs-Is)-(Vc-Ic))"
    ,"#  Rs = rs + (Rc-rc) + Tr_vi * ((Vs-Is)-(Vc-Ic))"
    ,"#  Is = Vs - (Vc-Ic) - Tvi   * ((vs-is)-(vc-ic))"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_BVRa
     "# BVR,  AAVSO recommended "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc))"
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_VIa   same as classic
     "# VI, AAVSO recommended, same as classic "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Vs = vs + (Vc-vc) + Tv_vi * ((Vs-Is)-(Vc-Ic))"
    ,"#  Is = Vs - (Vc-Ic) - Tvi   * ((vs-is)-(vc-ic))"
    ,NULL
    ,NULL
    ,NULL
   }
   ,
   {   // FILTC_BVIa
     "# BVI, AAVSO recommended "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Is = is + (Ic-ic) + Ti_vi * ((Vs-Is)-(Vc-Ic))"
    ,NULL
    ,NULL
   }
};


bool trans_display;
bool FILTC_displayed[FILTC_NUM];
AnsiString Formula= "";
short Method_Mask;
short GroupNum;

//---------------------------------------------------------------------------
void __fastcall TForm1::ProcessButtonClick(TObject *Sender)
{
   char delim= ';';
   unsigned short fc; // filter combo and index
   float crefmag= -999, x, creferr;
   int i, j, k, l, m, sdi, gdi= 0;
   AnsiString s, r, sx, st;

   // init
   // reset flags for displaying information in the output. Only want to display
   // the transforms and the formulas once in the output
   trans_display= false;
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
      s= Memo1->Lines->Strings[i].TrimLeft();
      if(s.Length()==0) continue;
      if(s[1]=='#') { // comment line
         //tbd  Could vet for required fields...
         if(s.SubString(2, 5)== "TYPE=") {
             if(s.SubString(7, 8).UpperCase()!= "EXTENDED") {
                ShowMessage("The file must be of #TYPE=EXTENDED");
                return;
             }
         } else
         if(s.SubString(2, 6)== "DELIM=") {
            if(s.SubString(8, 3).LowerCase()=="tab") delim= '\t';
            else if(s.SubString(8, 5).LowerCase()=="comma") delim= ',';
            else { // simple method. assumes the DELIM= x
               j= 7;
               while(s[++j]) {
                  if(s[j]!=' ') {
                     delim= s[j];
                     break;
                  }
            }
         }
            // tbd  other cases: named variable "tab" or "comma"
         } else
         if(s.SubString(2, 8)== "CREFMAG=") {
            if(2 != sscanf(s.SubString(10, s.Length()-9).c_str(),"%f %f",  &crefmag, &creferr)) {
                crefmag=  -999;
                ShowMessage("Bad #CREFMAG comment; must have value AND error term.["+s.SubString(10, s.Length()-9)+"]"  );
            //crefmag= s.SubString(10, 20).ToDouble();
//               ShowMessage(" missed CREFMAG");     EConvertError
            } // else   the crefmag/err will be picked up by the next star record
         }

      } else { // data line
         if(sdi==sdiMAX) {
            ShowMessage("Too many observations in the file.");
            return;
         }
         sdi++;

         // capture the line to sd
         sd[sdi].record= s;
         sd[sdi].narr= s; //sd[sdi].narr+= "\r\n";
         sd[sdi].processed= false; // clear the flag
         sd[sdi].ErrorMsg= ""; // clear the msg
         sd[sdi].ensemble= false;
         sd[sdi].CREFmag= -999;

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
         sd[sdi].TRANS= (s.SubString(k, j- 1).UpperCase()== "YES")? true: false;
         k+= j;
         // if it has been transformed, don't touch
         if(sd[sdi].TRANS) {
            sd[sdi].ErrorMsg+= " Already transformed.";
            sd[sdi].processed= true;
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
            sd[sdi].KMAG= s.SubString(k, j- 1).ToDouble();        // will be NO if no check star
         k+= j;
         /*    not a valid way to do transforms with ensemble data
         // if ensemble, the check star is K. Move it to C for processing
         if(sd[sdi].ensemble) {
            if(sd[sdi].KNAME=="na") {
               sd[sdi].ErrorMsg+= " Cannot do ensemble without K star data.";
               sd[sdi].processed= true; // skip this record
            } else {
               sd[sdi].CNAME = sd[sdi].KNAME;
               sd[sdi].CMAGraw  = sd[sdi].KMAG;
            }
         }
         */
         j= s.SubString(k, 20).Pos(delim);
         // airmass might be NA
         if(s.SubString(k,2).LowerCase()=="na") sd[sdi].AMASSna= true;
         else {
            sd[sdi].AMASS= s.SubString(k, j- 1).ToDouble();
            sd[sdi].AMASSna= false;
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
         sd[sdi].CHART= s.SubString(k, j- 1);
         k+= j;

         sd[sdi].NOTES= s.SubString(k, 100);


         // get CREFMAG info
         if(crefmag != -999) { // found in a prior comment
            sd[sdi].CREFmag= crefmag;
            sd[sdi].CREFerr= creferr;
            crefmag= -999; // don't use again.
         } else {
            for(int i= 0; i< sdi; i++) { // look in prior star records
               if(sd[sdi].CNAME==sd[i].CNAME) { // same star
                  if(sd[sdi].filter==sd[i].filter && !sd[i].processed) { // same filter and not rejected
                     sd[sdi].CREFmag= sd[i].CREFmag;
                     sd[sdi].CREFerr= sd[i].CREFerr;
                     break;
                  }
               }
            }
            if(-999==sd[sdi].CREFmag) { // try internet
               if(!getCREFMAG(&sd[sdi])) {
                  sd[sdi].ErrorMsg+= " No CREFMAG available. Possibly bad chart reference";
                  sd[sdi].processed= true; // skip record
                  //break;  // full break here
               }
            }
         }
         // final check
         if(sd[sdi].CREFmag == 0) {
            sd[sdi].ErrorMsg+= " CREFMAG is 0.";
            sd[sdi].processed= true;
         } else {
            sd[sdi].narr+= st.sprintf("\r\nCREFMAG= %cc= %0.3f +/- %0.3f", toupper(FILTname[sd[sdi].filter]), sd[sdi].CREFmag, sd[sdi].CREFerr);
         }

         // correct VMAG from AIPWIN reported value to observed value
         if(sd[sdi].MTYPE == 'S') { // "STD"
            sd[sdi].VMAGinst= sd[sdi].VMAGraw + sd[sdi].CMAGraw - sd[sdi].CREFmag;
            sd[sdi].narr+= st.sprintf("\r\nSTD inst mag: Vinst= %0.3f = %0.3f + %0.3f - %0.3f", sd[sdi].VMAGinst, sd[sdi].VMAGraw, sd[sdi].CMAGraw, sd[sdi].CREFmag);
         } else if(sd[sdi].MTYPE == 'A') { // "ABS"     deprecated; will have been changed to STD
            sd[sdi].VMAGinst= sd[sdi].VMAGraw;
            sd[sdi].narr+= st.sprintf("\r\nABS inst mag: Vinst= %0.3f = %0.3f ", sd[sdi].VMAGinst, sd[sdi].VMAGraw);
         } else { //if(sd[sdi].MTYPE == 'D') { // "DIF"
            sd[sdi].VMAGinst= sd[sdi].VMAGraw + sd[sdi].CMAGraw;
            sd[sdi].narr+= st.sprintf("\r\nDIF inst mag: Vinst= %0.3f = %0.3f + %0.3f ", sd[sdi].VMAGinst, sd[sdi].VMAGraw, sd[sdi].CMAGraw);
         }

         // apply Extinction correction
         if(applyExtinction->Checked && sd[sdi].AMASSna) {
            applyExtinction->Checked= false;
            ShowMessage("Obs found witn AMASS na; Extinction feature turned off");
         }
         if(applyExtinction->Checked) {
            sd[sdi].CMAGex= sd[sdi].CMAGraw - *Extinction[sd[sdi].filter] * sd[sdi].AMASS; //      Mobs - K * Airmass
            sd[sdi].narr+= st.sprintf("\r\nCMAG with extinction: %0.3f = %0.3f - %0.3f * %0.4f", sd[sdi].CMAGex, sd[sdi].CMAGraw, *Extinction[sd[sdi].filter], sd[sdi].AMASS); //      Mobs - K * Airmass
            sd[sdi].VMAGex= sd[sdi].VMAGinst  - *Extinction[sd[sdi].filter] * sd[sdi].AMASS; //      Mobs - K * Airmass
            sd[sdi].narr+= st.sprintf("\r\nVMAG with extinction: %0.3f = %0.3f - %0.3f * %0.4f", sd[sdi].VMAGex, sd[sdi].VMAGinst, *Extinction[sd[sdi].filter], sd[sdi].AMASS); //      Mobs - K * Airmass
         } else {
            sd[sdi].CMAGex= sd[sdi].CMAGraw;
            sd[sdi].VMAGex= sd[sdi].VMAGinst;
         }
         // corrections done
         sd[sdi].CMAG= sd[sdi].CMAGex;
         sd[sdi].CERR= 0; // same error as vmag, fully correlated
         sd[sdi].narr+= st.sprintf("\r\n%cc= %0.3f +/- %0.3f (assume correlated with v error)", tolower(FILTname[sd[sdi].filter]), sd[sdi].CMAG, sd[sdi].CERR);
         sd[sdi].VMAG= sd[sdi].VMAGex;
         sd[sdi].narr+= st.sprintf("\r\n%cs= %0.3f +/- %0.3f", tolower(FILTname[sd[sdi].filter]), sd[sdi].VMAG, sd[sdi].VERR);

      }
   }
   sdi++; // sdi is the number of obs collected

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
                  SetStarData(sd[i]);
               }
            } else { // !cs;  new star
               cs= sd[i].NAME+ sd[i].GROUPs; // defined by name and group
               // clear
               for(j= 0; j< FILT_NUM; j++)
                  sf[j]= sdiMAX;
               // set this star
               sf[sd[i].filter]= i;
               SetStarData(sd[i]);
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
            SetStarData(sd[i]);
            sf[sd[i].filter]= i; // must provide data for its own filter
// Form1->Memo2->Lines->Add(as.sprintf("I= %i, fc= %i", i, fc));

            ProcessStarData(&sd[i], fc); // sets processed flag

            if(sd[i].TRANS) { // transform successful. capture data used
               sd[i].FILTC= fc;
               k= FILTC_mask2index(sd[i].FILTC);
               for(m= 0; m<FILTC_desc_rows; m++) {
                  if(FILTC_desc[k][m]!= NULL)
                     sd[i].narr+= "\r\n"+ FILTC_desc[k][m];// + "\r\n";
               }
               if(fc & FILT_Bx)
                  sd[i].narr+= st.sprintf("\r\nStar: %s  Bs= %6.3f, bs= %6.3f, Bc= %6.3f, bc= %6.3f", Bts, Bs, bs, Bc, bc);
               if(fc & FILT_Vx)
                  sd[i].narr+= st.sprintf("\r\nStar: %s  Vs= %6.3f, vs= %6.3f, Vc= %6.3f, vc= %6.3f", Vts, Vs, vs, Vc, vc);
               if(fc & FILT_Rx)
                  sd[i].narr+= st.sprintf("\r\nStar: %s  Rs= %6.3f, rs= %6.3f, Rc= %6.3f, rc= %6.3f", Rts, Rs, rs, Rc, rc);
               if(fc & FILT_Ix)
                  sd[i].narr+= st.sprintf("\r\nStar: %s  Is= %6.3f, is= %6.3f, Ic= %6.3f, ic= %6.3f", Its, Is, is, Ic, ic);

               sd[i].VMAGrep= sd[i].VMAGt;
               sd[i].narr+= st.sprintf("\r\nVMAGtrans= %0.3f", sd[i].VMAGt);
               /*
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
           }



            // capture the group data
            //  into sd     not really necessary; narrative got the data
            sd[i].gd.Group= sd[i].NAME+ sd[i].GROUPs;
            sd[i].gd.FILTC= fc;
            for(j= 0; j<16; j++)
               sd[i].gd.m[j]= 0; // clear
            if(fc & FILT_Bx)
               sd[i].gd.m[0]= Bs, sd[i].gd.m[4]= bs, sd[i].gd.m[8]= Bc, sd[i].gd.m[12]= bc;
            if(fc & FILT_Vx)
               sd[i].gd.m[1]= Vs, sd[i].gd.m[5]= vs, sd[i].gd.m[9]= Vc, sd[i].gd.m[13]= vc;
            if(fc & FILT_Rx)
               sd[i].gd.m[2]= Rs, sd[i].gd.m[6]= rs, sd[i].gd.m[10]= Rc, sd[i].gd.m[14]= rc;
            if(fc & FILT_Ix)
               sd[i].gd.m[3]= Is, sd[i].gd.m[7]= is, sd[i].gd.m[11]= Ic, sd[i].gd.m[15]= ic;



         } //
      } // for
         //  don't really need this
            // and gd
            if(gdi==gdiMAX) {
               ShowMessage("Too many groups in the file.");
               return;
            }
            gd[gdi].Group= cs; //sd[i].NAME+ sd[i].GROUPs;
            gd[gdi].FILTC= fc;
            for(j= 0; j<16; j++)
               gd[gdi].m[j]= 0; // clear
            if(fc & FILT_Bx)
               gd[gdi].m[0]= Bs, gd[gdi].m[4]= bs, gd[gdi].m[8]= Bc, gd[gdi].m[12]= bc;
            if(fc & FILT_Vx)
               gd[gdi].m[1]= Vs, gd[gdi].m[5]= vs, gd[gdi].m[9]= Vc, gd[gdi].m[13]= vc;
            if(fc & FILT_Rx)
               gd[gdi].m[2]= Rs, gd[gdi].m[6]= rs, gd[gdi].m[10]= Rc, gd[gdi].m[14]= rc;
            if(fc & FILT_Ix)
               gd[gdi].m[3]= Is, gd[gdi].m[7]= is, gd[gdi].m[11]= Ic, gd[gdi].m[15]= ic;
            gdi++;
           // */
   } while(42);  // no cs will break us out



   //ShowMessage("Review the convergence of the New formulas (if any)");
   Memo2->Lines->Clear(); // output window
//   Memo4->Lines->Clear(); // report window

   Memo4->Lines->Add(" "); // blank line
   //Memo4->Lines->Add(Formula);
   Memo4->Lines->Add("Star                 Date        Filter  Grp    Vraw      Vinst    Vex    TranMag      diff       VERR    VERRt");
   // build
   for(i= 0, j= 0; i< Memo1->Lines->Count; i++) {
      s= Memo1->Lines->Strings[i].TrimLeft();
      if(s.Length()!=0) // skip if blank
      if(s[1]=='#') { // non-data line
         Memo2->Lines->Add(s); // just copy
      } else { // data line
         if(!sd[j].TRANS) { // not Transformed
            Memo2->Lines->Add(s); // just copy original
            //r.sprintf("%s %s %8.3f NA", sd[j].NAME, sd[j].DATEs, sd[j].VMAG);
            //? where was this to go? Not here:   Memo2->Lines->Add(r);
            r.sprintf("\"%-15s\"   %s %s %3s %8.3f %8.3f %8.3f    not transformed", sd[j].NAME, sd[j].DATEs, sd[j].FILT, sd[j].GROUPs
                                  , sd[j].VMAGraw, sd[j].VMAGinst, sd[j].VMAG );// ,    sd[j].VMAGt, sd[j].VMAGt- sd[j].VMAG, sd[j].VMAGrep, sd[j].VERR, sd[j].VERRt);
            Memo4->Lines->Add(r);
        } else {  // display transformed data    ?? what if it was transformed in the input, thus skipped?
            // Standard info blocks will be output once
            if(!trans_display) { // show the transform coefficients
               Memo2->Lines->Add("#   transform coefficients applied by Transformer Applier, version "+ FormatFloat("0.00", Version));
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
               }
               trans_display= true;
            }
            //for(k= 0; k< FILTC_NUM; k++) {
            //   if(sd[j].FILTC==FILTC_mask[k]) {
            /*   appears in narrative
                   k= FILTC_mask2index(sd[j].FILTC);
                   if(!FILTC_displayed[k]) { // if it has not been displayed
                      for(m= 0; m<FILTC_desc_rows; m++) {
                         if(FILTC_desc[k][m]!= NULL) {
                            Memo2->Lines->Add(FILTC_desc[k][m]);
                            Memo4->Lines->Add(FILTC_desc[k][m]);
                         }
                      }
                      FILTC_displayed[k]= true; // so we don't display again
                   }
             */
            //   }
            // }

            // show un-transformed data as a comment
            Memo2->Lines->Add((IncludeRaw->Checked?"":"#")+ s);

            // now the transformed data
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

            //if(sd[j].ensemble) {
            //   s+= "ENSEMBLE"+ delim; s+= "NA"+ delim;   // cname data
            //   s+= sd[j].CNAME+ delim; // kname was stored here
            //   s+= FormatFloat("0.000", sd[j].CMAGraw)+ delim;
            //} else {
               s+= sd[j].CNAME+ delim;
               s+= FormatFloat("0.000", sd[j].CMAGraw)+ delim;

               s+= sd[j].KNAME+ delim;
               if(sd[j].KNAME=="na")
                  s+="na" , s+= delim;
               else s+= FormatFloat("0.000", sd[j].KMAG)+ delim;
            //}
            if(sd[j].AMASSna) s+= "na"+ delim;
            else   s+= FormatFloat("0.0000", sd[j].AMASS)+ delim;

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
      }
   }

   // do a detail, narrative report

   Memo4->Lines->Add("\r\n    Detail narrative for each observation  \r\n");
   for(i= 0; i<sdi; i++) {
      sd[i].narr+= "\r\n"+ sd[i].recordT;
      Memo4->Lines->Add(sd[i].narr);
      Memo4->Lines->Add(sd[i].ErrorMsg + "\r\n");
   }

   // extinction report

   // group report
   Memo4->Lines->Add("\r\n          Summary of group data");
   Memo4->Lines->Add("\n name+group      filtC   Bs     Vs     Rs     Is     bs     vs     rs     is     Bc     Vc     Rc     Ic     bc     vc     rc     ic");
   for(i= 0; i<gdi; i++) {
      r.sprintf("\"%-15s\" %3i", gd[i].Group, gd[i].FILTC);
      for(j= 0; j<16; j++) {
         if(gd[i].m[j]==0) s= "   na  ";
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


}
//---------------------------------------------------------------------------

void SetStarData(StarData d)
{
   switch(d.filter) {
      case FILT_Bi: bs= d.VMAG; bc= d.CMAG; Bc= d.CREFmag;    Bts= d.DATEs;
                    rbs= d.VERR; rbc= d.CERR; rBc= d.CREFerr; break;
      case FILT_Vi: vs= d.VMAG; vc= d.CMAG; Vc= d.CREFmag;    Vts= d.DATEs;
                    rvs= d.VERR; rvc= d.CERR; rVc= d.CREFerr; break;
      case FILT_Ri: rs= d.VMAG; rc= d.CMAG; Rc= d.CREFmag;    Rts= d.DATEs;
                    rrs= d.VERR; rrc= d.CERR; rRc= d.CREFerr; break;
      case FILT_Ii: is= d.VMAG; ic= d.CMAG; Ic= d.CREFmag;    Its= d.DATEs;
                    ris= d.VERR; ric= d.CERR; rIc= d.CREFerr; break;
   }
}

float fTxy   (char x, char y,         float Txy,   float rTxy,   int mode);
float fTx_yz (char x, char y, char z, float Tx_yz, float rTx_yz, int mode);

#define MAXITER 15
void ProcessStarData(StarData *d, unsigned short fc)
{
   AnsiString as;
   float x;    int loop;
   d->TRANS= true;
   d->FILTC= fc;  // which combination used
   rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; // default error is the obs error
   switch(fc) {
      case (FILTC_BVRIc):
         if(Tr_ri==0 || Tri==0 || Tv_vr==0 || Tvr==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tr_ri, Tri, Tv_vr, Tvr and Tbv.";
            d->TRANS= false;
            break;
         }
         Rs = rs + (Rc - rc) + Tr_ri * (((Rc - Ic) + Tri * ((rs - is) - (rc - ic))) - (Rc - Ic));
         Is = Rs - ((Rc - Ic) + Tri * ((rs - is) - (rc - ic)));
         Vs = vs + (Vc - vc) + Tv_vr * Tvr * ((vs - rs) - (vc - rc));
         Bs = Vs + (Bc - Vc) + Tbv * ((bs - vs) - (bc - vc));
         d->StarsUsed= "# BVIR Classic using: B @ "+ sd[sf[FILT_Bi]].DATEs
            + ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         break;

      case (FILTC_BVRIa):
         if(Tb_bv==0 || Tv_bv==0 || Tr_vi==0 || Ti_vi==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv, Tv_bv, Tr_vi and Ti_vi.";
            d->TRANS= false;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 1); //bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            Vs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 1); //vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc));
            Rs = fTx_yz( r, v, i, Tr_vi, rTr_vi, 1); //rs + (Rc-rc) + Tr_vi * ((Vs-Is)-(Vc-Ic));
            Is = fTx_yz( i, v, i, Ti_vi, rTi_vi, 1); //is + (Ic-ic) + Ti_vi * ((Vs-Is)-(Vc-Ic));
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR AAVSO using: B @ "+ sd[sf[FILT_Bi]].DATEs
           + ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 11);
            rVs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 11);
            rRs = fTx_yz( r, v, i, Tr_vi, rTr_vi, 11);
            rIs = fTx_yz( i, v, i, Ti_vi, rTi_vi, 11);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

             /*
      case (FILTC_BVRIs): // mine
         if(Tbv==0 || Tvr==0 || Tri==0 || Tb_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr, Tri and Tb_bv.";
            d->TRANS= false;
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
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR special using: B @ "+ sd[sf[FILT_Bi]].DATEs
           + ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
       error comp
         break;
     */

      case (FILTC_BVRIs):  // Arne's alternate as used with the BSM
         if(Tbv==0 || Tvr==0 || Tri==0 || Tv_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tv_bv, Tvr, and Tri.";
            d->TRANS= false;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTxy( b, v, Tbv, rTbv, 1);
            Vs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 1);
            Rs = fTxy( v, r, Tvr, rTvr, 2);
            Is = fTxy( r, i, Tri, rTri, 2);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR Arne special using: B @ "+ sd[sf[FILT_Bi]].DATEs
           + ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTxy( b, v, Tbv, rTbv, 11);
            rVs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 11);
            rRs = fTxy( v, r, Tvr, rTvr, 12);
            rIs = fTxy( r, i, Tri, rTri, 12);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case FILTC_BVa:
         if(Tb_bv==0 || Tv_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv and Tv_bv.";
            d->TRANS= false;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1);
            Vs = fTx_yz(v, b, v, Tv_bv, rTv_bv, 1);
            //Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            //Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc));
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11);
            rVs = fTx_yz(v, b, v, Tv_bv, rTv_bv, 11);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_BVs:
      case FILTC_BVc:
         if(Tb_bv==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv and Tbv.";
            d->TRANS= false;
            break;
         }
         //Bs = bs + (Bc - bc) + Tb_bv * (((Bc - Vc) + Tbv * ((bs - vs) - (bc - vc))) - (Bc - Vc));
         //Vs = Bs - ((Bc - Vc) + Tbv * ((bs - vs) - (bc - vc)));
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1);
            vs = fTxy(b, v, Tbv, rTbv, 2);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BV classic and alternative using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11);
            rvs = fTxy(b, v, Tbv, rTbv, 12);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;



      case FILTC_VRa:
         if(Tv_vr==0 || Tr_vr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv_vr and Tr_vr.";
            d->TRANS= false;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            //Vs = vs + (Vc-vc) + Tv_vr * ((Vs-Rs)-(Vc-Rc));
            //Rs = rs + (Rc-rc) + Tr_vr * ((Vs-Rs)-(Vc-Rc));
            Vs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 1);
            Rs = fTx_yz(r, v, r, Tr_vr, rTr_vr, 1);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 11);
            rRs = fTx_yz(r, v, r, Tr_vr, rTr_vr, 11);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_VRs:
      case FILTC_VRc:
         if(Tv_vr==0 || Tvr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv_vr and Tvr.";
            d->TRANS= false;
            break;
         }
         //Vs = vs + (Vc - vc) + Tv_vr * (((Vc - Rc) + Tvr * ((vs - rs) - (vc - rc))) - (Vc - Rc));
         //Rs = Vs - ((Vc - Rc) + Tvr * ((vs - rs) - (vc - rc)));
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Vs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 1);
            Rs = fTxy(v, r, Tvr, rTvr, 2);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 11);
            rRs = fTxy(v, r, Tvr, rTvr, 12);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case FILTC_VRIa:
         if(Tv_vi==0 || Tr_vi==0 || Tvi==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv_vi, Tr_vi and Tvi.";
            d->TRANS= false;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            //Vs = vs + (Vc-vc) + Tv_vi * ((Vs-Is)-(Vc-Ic));
            //Rs = rs + (Rc-rc) + Tr_vi * ((Vs-Is)-(Vc-Ic));
            //Is = Vs - (Vc-Ic) - Tvi   * ((vs-is)-(vc-ic));
            Vs = fTx_yz(v, v, i, Tv_vi, rTv_vi, 1);
            Rs = fTx_yz(r, v, i, Tr_vi, rTr_vi, 1);
            Is = fTxy(v, i, Tvi, rTvi, 2);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+ " using:  V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs = fTx_yz(v, v, i, Tv_vi, rTv_vi, 11);
            rRs = fTx_yz(r, v, i, Tr_vi, rTr_vi, 11);
            rIs = fTxy(v, i, Tvi, rTvi, 12);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_VRIs:
         if(Tvr==0 || Tri==0 || Tv_vr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv_vr, Tvr, and Tri.";
            d->TRANS= false;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            //Vs = vs + (Vc-vc) + Tv_vr    * ((Vs-Rs)-(Vc-Rc));
            //Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc));
            //Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic));
            Vs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 1);
            Rs = fTxy(v, r, Tvr, rTvr, 2);
            Is = fTxy(r, i, Tri, rTri, 2);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using:  V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 11);
            rRs = fTxy(v, r, Tvr, rTvr, 12);
            rIs = fTxy(r, i, Tri, rTri, 12);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_VRIc:
         if(Tr_ri==0 || Tri==0 || Tv_vr==0 || Tvr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tr_ri, Tri, Tv_vr and Tvr.";
            d->TRANS= false;
            break;
         }
         Rs = rs + (Rc - rc) + Tr_ri * (((Rc - Ic) + Tri * ((rs - is) - (rc - ic))) - (Rc - Ic));
         Is = Rs - ((Rc - Ic) + Tri * ((rs - is) - (rc - ic)));
         Vs = vs + (Vc - vc) + Tv_vr * Tvr * ((vs - rs) - (vc - rc));
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         break;

      case FILTC_BVRa:
         if(Tb_bv==0 || Tv_bv==0 || Tvr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv, Tv_bv and Tvr.";
            d->TRANS= false;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            //Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            //Vs = vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc));
            //Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc));
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1);
            Vs = fTx_yz(v, b, v, Tv_bv, rTv_bv, 1);
            Rs = fTxy(v, r, Tvr, rTvr, 2);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11);
            rVs = fTx_yz(v, b, v, Tv_bv, rTv_bv, 11);
            rRs = fTxy(v, r, Tvr, rTvr, 12);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_BVRs:
         if(Tbv==0 || Tvr==0 || Tb_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv, Tbv and Tvr.";
            d->TRANS= false;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            //Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            //Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc));
            //Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc));
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1);
            Vs = fTxy(b, v, Tbv, rTbv, 2);
            Rs = fTxy(v, r, Tvr, rTvr, 2);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11);
            rVs = fTxy(b, v, Tbv, rTbv, 12);
            rRs = fTxy(v, r, Tvr, rTvr, 12);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_BVRc:
         if(Tv_vr==0 || Tvr==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tv_vr and Tvr.";
            d->TRANS= false;
            break;
         }
         //Vs = vs + (Vc - vc) + Tv_vr * (((Vc - Rc) + Tvr * ((vs - rs) - (vc - rc))) - (Vc - Rc));
         //Rs = Vs - ((Vc - Rc) + Tvr * ((vs - rs) - (vc - rc)));
         //Bs = Vs + (Bc - Vc) +  Tbv * ((bs - vs) - (bc - vc));
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTxy(b, v, Tbv, rTbv, 1);
            Vs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 1);
            Rs = fTxy(v, r, Tvr, rTvr, 2);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", R @ "+ sd[sf[FILT_Ri]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs = fTx_yz(v, v, r, Tv_vr, rTv_vr, 11);
            rRs = fTxy(v, r, Tvr, rTvr, 12);
            rBs = fTxy(b, v, Tbv, rTbv, 11);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case FILTC_VIs:
      case FILTC_VIa:
      case FILTC_VIc:
         if(Tv_vi==0 || Tvi==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv_vi and Tvi.";
            d->TRANS= false;
            break;
         }
         //Vs = vs + (Vc - vc) + Tv_vi * (((Vc - Ic) + Tvi * ((vs - is) - (vc - ic))) - (Vc - Ic));
         //Is = Vs - ((Vc - Ic) + Tvi * ((vs - is) - (vc - ic)));
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Vs = fTx_yz(v, v, i, Tv_vi, rTv_vi, 1);
            Is = fTxy(v, i, Tvi, rTvi, 2);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: V @ "+ sd[sf[FILT_Vi]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs = fTx_yz(v, v, i, Tv_vi, rTv_vi, 11);
            rIs = fTxy(v, i, Tvi, rTvi, 12);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;

      case FILTC_BVIs:
         if(Tbv==0 || Tvi==0 || Tb_bv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv, Tbv and Tvi.";
            d->TRANS= false;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            //Bs = bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            //Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc));
            //Is = Vs - (Vc-Ic) - Tvi* ((vs-is)-(vc-ic));
            Bs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 1);
            Vs = fTxy(b, v, Tbv, rTbv, 2);
            Is = fTxy(v, i, Tvi, rTvi, 2);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz(b, b, v, Tb_bv, rTb_bv, 11);
            rVs = fTxy(b, v, Tbv, rTbv, 12);
            rIs = fTxy(v, i, Tvi, rTvi, 12);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_BVIc:
         if(Tv_vi==0 || Tvi==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tv_vi and Tvi.";
            d->TRANS= false;
            break;
         }
         //Vs = vs + (Vc - vc) + Tv_vi * (((Vc - Ic) + Tvi * ((vs - is) - (vc - ic))) - (Vc - Ic));
         //Is = Vs - ((Vc - Ic) + Tvi * ((vs - is) - (vc - ic)));
         //Bs = Vs + (Bc - Vc) +  Tbv * ((bs - vs) - (bc - vc));
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTxy(b, v, Tbv, rTbv, 1);
            Vs = fTx_yz(v, v, i, Tv_vi, rTv_vi, 1);
            Is = fTxy(v, i, Tvi, rTvi, 2);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rVs = fTx_yz(v, v, i, Tv_vi, rTv_vi, 11);
            rIs = fTxy(v, i, Tvi, rTvi, 12);
            rBs = fTxy(b, v, Tbv, rTbv, 11);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
         break;
      case FILTC_BVIa:
         if(Tb_bv==0 || Tv_bv==0 || Ti_vi==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb_bv, Tv_bv and Ti_vi.";
            d->TRANS= false;
            break;
         }
         Bs= bs, Vs= vs, Rs= rs, Is= is; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula error! Check your coefficients!"; d->TRANS= false; break; }
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 1); //bs + (Bc-bc) + Tb_bv * ((Bs-Vs)-(Bc-Vc));
            Vs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 1); //vs + (Vc-vc) + Tv_bv * ((Bs-Vs)-(Bc-Vc));
            Is = fTx_yz( i, v, i, Ti_vi, rTi_vi, 1); //is + (Ic-ic) + Ti_vi * ((Vs-Is)-(Vc-Ic));
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= FILTC_desc[FILTC_mask2index(fc)][0]+" using: B @ "+ sd[sf[FILT_Bi]].DATEs+ ", V @ "+ sd[sf[FILT_Vi]].DATEs+ ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         rBs= rbs, rVs= rvs, rRs= rrs, rIs= ris; loop= 0;
         do { if(++loop > MAXITER) { d->ErrorMsg+= " Formula e error! Check your coefficients!"; d->TRANS= false; break; }
            roBs= rBs, roVs= rVs, roRs= rRs, roIs= rIs;
            rBs = fTx_yz( b, b, v, Tb_bv, rTb_bv, 11);
            rVs = fTx_yz( v, b, v, Tv_bv, rTv_bv, 11);
            rIs = fTx_yz( i, v, i, Ti_vi, rTi_vi, 11);
            if(DEBUG) Form1->Memo4->Lines->Add(as.sprintf(" %0.4f, %0.4f, %0.4f, %0.4f", fabs(rBs-roBs), fabs(rVs-roVs), fabs(rRs-roRs), fabs(rIs-roIs)));
         } while ( fabs(rBs-roBs)>0.0001 || fabs(rVs-roVs)>0.0001 || fabs(rRs-roRs)>0.0001 || fabs(rIs-roIs)>0.0001 );
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
         d->TRANS= false;
         break;
   }

   if(d->TRANS) {
      switch(d->filter) {
         case FILT_Bi: d->VMAGt= Bs; d->VERRt= rBs; break;
         case FILT_Vi: d->VMAGt= Vs; d->VERRt= rVs; break;
         case FILT_Ri: d->VMAGt= Rs; d->VERRt= rRs; break;
         case FILT_Ii: d->VMAGt= Is; d->VERRt= rIs; break;
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
   OpenDialog1->Title= "Open a un-transformed observation file";
   OpenDialog1->Filter = "any txt file (*.*)|*.*";
   if(OpenDialog1->Execute()){
      // put into the Input window
      UnTransformedFile= OpenDialog1->FileName;
      Memo1->Lines->LoadFromFile(UnTransformedFile);
   };
   // Save the directory
   TIniFile *ini;
   ini = new TIniFile(INIfilename);
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
   OpenDialog1->Filter = "TXT files (*.txt)|*.TXT";
   if(SaveDialog1->Execute()) {
      Memo1->Lines->SaveToFile(SaveDialog1->FileName);
   }
   OpenDialog1->InitialDir= SaveDialog1->InitialDir;
   // Save the directory
   TIniFile *ini;
   ini = new TIniFile(INIfilename);
   ini->WriteString("Setup", "Dir", OpenDialog1->InitialDir);
   delete ini;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Save1Click(TObject *Sender)
{
   // use same dir as open
   SaveDialog1->InitialDir= OpenDialog1->InitialDir;
   SaveDialog1->Title= "Save the transformed observation file";
   AnsiString f= UnTransformedFile;
   if(f.Length()> 5 && '.'== f[f.Length()- 3]) f= f.SubString(1, f.Length()-4)+ "_TA" + f.SubString(f.Length()-3, 4);
   SaveDialog1->FileName= f;
   if(SaveDialog1->Execute()) {
      Memo2->Lines->SaveToFile(SaveDialog1->FileName);
   }
   OpenDialog1->InitialDir= SaveDialog1->InitialDir;
   // Save the directory
   TIniFile *ini;
   ini = new TIniFile(INIfilename);
   ini->WriteString("Setup", "Dir", OpenDialog1->InitialDir);
   delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Exit1Click(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------


//int __fastcall TForm1::checkChart(TObject *Sender, StarData* sd) {
int __fastcall checkChart(StarData* sd) {
    bool   auid= false, chartExists= false;
    int  ret= 1, i, labelCnt= 0, labelIndex= 0;
    // CNAME an AUID?
    if(sd->CNAME.Length()==11 && sd->CNAME[4]=='-' && sd->CNAME[8]=='-')
       auid= true;
    // look for it in the current chart data
    for(i= 0; i< min(chartnext, chartMAX); i++) {
       if(chart[i].chartid == sd->CHART) { // same chart
          chartExists= true;
          if(auid) {
             if(chart[i].AUID == sd->CNAME) break;
          } else { // maybe its a label
             if(chart[i].label== sd->CNAME) labelCnt++, labelIndex= i;
          }
       }
    }
    // did we find it?
    if(i != min(chartnext, chartMAX)) { // yes
       sd->CREFmag= chart[i].sdata[sd->filter].mag;
       sd->CREFerr= chart[i].sdata[sd->filter].err;
    } else if(labelCnt == 1) { // found unique label
       sd->CREFmag= chart[labelIndex].sdata[sd->filter].mag;
       sd->CREFerr= chart[labelIndex].sdata[sd->filter].err;
    } else if(labelCnt > 1) {
       sd->ErrorMsg+= " Duplicate label in chart.";
       ret= -1;
    } else if(chartExists) {
       ret= -1;
    } else  ret= 0;
    return ret;
}

void __fastcall TForm1::Button2Click(TObject *Sender)
{
   AnsiString s;
   StarData sd;
   sd.CNAME= "000-BCN-754";
   sd.CHART= "13300NXP"; //"13311ABQ"; //"13300NXP";
   sd.filter= 2;
   getCREFMAG(&sd);

   s.printf("%f %f", sd.CREFmag, sd.CREFerr);
   Memo4->Lines->Add(s );
   Memo4->Lines->Add(sd.ErrorMsg);

   sd.filter= 3;
   getCREFMAG(&sd);

   s.printf("%f %f", sd.CREFmag, sd.CREFerr);
   Memo4->Lines->Add(s );
   Memo4->Lines->Add(sd.ErrorMsg);


}
//---------------------------------------------------------------------------
#define cpBufSize 10000
//int __fastcall TForm1::getCREFMAG(TObject *Sender, StarData* sd)
int __fastcall getCREFMAG(StarData* sd)
{
    int     I, ret;
    TStream *DataIn;
    char* pch; int j, i;
    char  pchart[45][20];
    int pi[]= {7, 10, 13, 16, 4};  // [FILT_NUM];
    char cp[cpBufSize];
    AnsiString as;

    ret= checkChart(sd);
    if(ret==1 || ret==-1) { // found or not in chart
       return (ret==1)? 1: 0;
    } else { // no. let's get more chart data
       if(sd->CHART.Length()) { // assuming they gave us a chartid
          AnsiString u= "http://www.aavso.org/cgi-bin/vsp.pl?chartid=";
          u+= sd->CHART; u+= "&delimited=yes";
          if(Form1->UseStdField->Checked) u+= "&std_field=on";
          //HttpCli1->URL        = "http://www.aavso.org/cgi-bin/vsp.pl?chartid=13221ASP&delimited=yes";
          Form1->HttpCli1->URL        = u;
          Form1->HttpCli1->RcvdStream = NULL;
          try {
             Form1->HttpCli1->Get();
          } __except (TRUE) {
             Form1->Memo4->Lines->Add("GET Failed !");
             Form1->Memo4->Lines->Add("StatusCode   = " + IntToStr(Form1->HttpCli1->StatusCode));
             Form1->Memo4->Lines->Add("ReasonPhrase = " + Form1->HttpCli1->ReasonPhrase);
             return 0;
          }
          //Form1->Memo4->Lines->Add("StatusCode = " + IntToStr(Form1->HttpCli1->StatusCode));
          //for (I = 0; I < Form1->HttpCli1->RcvdHeader->Count; I++)
          //   Form1->Memo4->Lines->Add("hdr>" + Form1->HttpCli1->RcvdHeader->Strings[I]);
          DataIn = new TFileStream(Form1->HttpCli1->DocName, fmOpenRead);
          //Memo4->Lines->LoadFromStream(DataIn);
          DataIn->ReadBuffer(cp, min(cpBufSize, DataIn->Size));
          delete DataIn;
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
                chart[chartnext % chartMAX].chartid= AnsiString(pchart[44]).TrimLeft().TrimRight();
                chart[chartnext % chartMAX].AUID= AnsiString(pchart[0]).TrimLeft().TrimRight();
                chart[chartnext % chartMAX].label= AnsiString(pchart[3]).TrimLeft().TrimRight();
                for(i= 0; i< FILT_NUM; i++) {
                   if(strcmp(" ", pchart[pi[i]]))
                      sscanf(pchart[pi[i]], " %f", &chart[chartnext % chartMAX].sdata[i].mag);
                   if(strcmp(" ", pchart[pi[i]+1]))
                      sscanf(pchart[pi[i]+1], " %f", &chart[chartnext % chartMAX].sdata[i].err);
                }
                chartnext++;
             }
          }
          // so, let's tryagain
          return (checkChart(sd)== 1)? 1: 0;
       } else {
          sd->ErrorMsg+= " No chartid.";
          return 0;
       }
    }
}






void __fastcall TForm1::Button1Click(TObject *Sender)
{
    int     I;
    TStream *DataIn;

    HttpCli1->URL        = "http://www.aavso.org/cgi-bin/vsp.pl?chartid=11331ABQ&delimited=yes";
    HttpCli1->Proxy      = "";//ProxyHostEdit->Text;
    HttpCli1->ProxyPort  = "";//ProxyPortEdit->Text;
    HttpCli1->RcvdStream = NULL;
   try {
        HttpCli1->Get();
    } __except (TRUE) {
        Memo4->Lines->Add("GET Failed !");
        Memo4->Lines->Add("StatusCode   = " + IntToStr(HttpCli1->StatusCode));
        Memo4->Lines->Add("ReasonPhrase = " + HttpCli1->ReasonPhrase);
        return;
    }

    Memo4->Lines->Add("StatusCode = " + IntToStr(HttpCli1->StatusCode));

    for (I = 0; I < HttpCli1->RcvdHeader->Count; I++)
        Memo4->Lines->Add("hdr>" + HttpCli1->RcvdHeader->Strings[I]);

    DataIn = new TFileStream(HttpCli1->DocName, fmOpenRead);
    Memo4->Lines->LoadFromStream(DataIn);
    delete DataIn;

}
//---------------------------------------------------------------------------


void __fastcall TForm1::HttpCli1DocBegin(TObject *Sender)
{
    //Memo4->Lines->Add(HttpCli1->ContentType + " => " + HttpCli1->DocName);
    //Memo4->Lines->Add("Document = " + HttpCli1->DocName);
    HttpCli1->RcvdStream = new TFileStream(HttpCli1->DocName, fmCreate);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::HttpCli1DocEnd(TObject *Sender)
{
    if (HttpCli1->RcvdStream) {
        delete HttpCli1->RcvdStream;
        HttpCli1->RcvdStream = NULL;
    }
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




float fTxy (char x, char y, float Txy, float rTxy, int mode) {
     // Secondary or Color form:  Txy
     //  1/slope of (x-y) vs (X-Y) :
     //    Txy= ((Xs-Ys)-(Xc-Yc)) / ((xs-ys)-(xc-yc))
     float Xs, xs, Xc, xc, Ys, ys, Yc, yc, r, s, rXs, rxs, rXc, rxc, rYs, rys, rYc, ryc;

    switch(x) {
       case 'b': Xs= Bs; xs= bs; Xc= Bc; xc= bc;  rXs= rBs; rxs= rbs; rXc= rBc; rxc= rbc; break;
       case 'v': Xs= Vs; xs= vs; Xc= Vc; xc= vc;  rXs= rVs; rxs= rvs; rXc= rVc; rxc= rvc; break;
       case 'r': Xs= Rs; xs= rs; Xc= Rc; xc= rc;  rXs= rRs; rxs= rrs; rXc= rRc; rxc= rrc; break;
       case 'i': Xs= Is; xs= is; Xc= Ic; xc= ic;  rXs= rIs; rxs= ris; rXc= rIc; rxc= ric; break;
    }
    switch(y) {
       case 'b': Ys= Bs; ys= bs; Yc= Bc; yc= bc;  rYs= rBs; rys= rbs; rYc= rBc; ryc= rbc; break;
       case 'v': Ys= Vs; ys= vs; Yc= Vc; yc= vc;  rYs= rVs; rys= rvs; rYc= rVc; ryc= rvc; break;
       case 'r': Ys= Rs; ys= rs; Yc= Rc; yc= rc;  rYs= rRs; rys= rrs; rYc= rRc; ryc= rrc; break;
       case 'i': Ys= Is; ys= is; Yc= Ic; yc= ic;  rYs= rIs; rys= ris; rYc= rIc; ryc= ric; break;
    }

    switch(mode) {
       case 1: r= Xs=  Ys + (Xc-Yc) +  Txy * ((xs-ys)-(xc-yc));   break;
       case 2: r= Ys=  Xs - (Xc-Yc) -  Txy * ((xs-ys)-(xc-yc));   break;

       case 11: s= ((xs-ys)-(xc-yc));
            r= rXs=  sqrt( (pow(rYs,2)+pow(rXc,2)+pow(rYc,2)) +  pow(Txy,2)* (pow(rTxy/Txy,2) + (pow(rxs,2)+pow(rys,2)+pow(rxc,2)+pow(ryc,2))/pow(s,2))  );
            break;
       case 12: s= ((xs-ys)-(xc-yc));
            r= rYs=  sqrt( (pow(rXs,2)+pow(rXc,2)+pow(rYc,2)) +  pow(Txy,2)* (pow(rTxy/Txy,2) + (pow(rxs,2)+pow(rys,2)+pow(rxc,2)+pow(ryc,2))/pow(s,2))  );
            break;
    }
    return r;
}

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
float fTx_yz (char x, char y, char z, float Tx_yz, float rTx_yz, int mode) {
    //Primary or star form:  Tx_yz
    //  slope of (X-x) vs (Y-Z) :
    //  Tx_yz= ((Xs-xs)-(Xc-xc)) / ((Ys-Zs)-(Yc-Zc))
    float Xs, xs, Xc, xc, Ys, ys, Yc, yc, Zs, zs, Zc, zc, r, s;
    float rXs, rxs, rXc, rxc, rYs, rys, rYc, ryc, rZs, rzs, rZc, rzc;

    switch(x) {
       case 'b': Xs= Bs; xs= bs; Xc= Bc; xc= bc;  rXs= rBs; rxs= rbs; rXc= rBc; rxc= rbc; break;
       case 'v': Xs= Vs; xs= vs; Xc= Vc; xc= vc;  rXs= rVs; rxs= rvs; rXc= rVc; rxc= rvc; break;
       case 'r': Xs= Rs; xs= rs; Xc= Rc; xc= rc;  rXs= rRs; rxs= rrs; rXc= rRc; rxc= rrc; break;
       case 'i': Xs= Is; xs= is; Xc= Ic; xc= ic;  rXs= rIs; rxs= ris; rXc= rIc; rxc= ric; break;
    }
    switch(y) {
       case 'b': Ys= Bs;  Yc= Bc;  rYs= rBs;  rYc= rBc; break;
       case 'v': Ys= Vs;  Yc= Vc;  rYs= rVs;  rYc= rVc; break;
       case 'r': Ys= Rs;  Yc= Rc;  rYs= rRs;  rYc= rRc; break;
       case 'i': Ys= Is;  Yc= Ic;  rYs= rIs;  rYc= rIc; break;
    }
    switch(z) {
       case 'b': Zs= Bs;  Zc= Bc;  rZs= rBs;  rZc= rBc; break;
       case 'v': Zs= Vs;  Zc= Vc;  rZs= rVs;  rZc= rVc; break;
       case 'r': Zs= Rs;  Zc= Rc;  rZs= rRs;  rZc= rRc; break;
       case 'i': Zs= Is;  Zc= Ic;  rZs= rIs;  rZc= rIc; break;
    }

    switch(mode) {
      case 1: r= Xs= xs + (Xc-xc) + Tx_yz * ((Ys-Zs)-(Yc-Zc));  break;
      case 2: r= Ys= Zs - (Yc-Zc) + ((Xs-xs)-(Xc-xc)) / Tx_yz;  break;
      case 3: r= Zs= Ys + (Yc-Zc) + ((Xs-xs)-(Xc-xc)) / Tx_yz;  break;

      case 11: s= ((Ys-Zs)-(Yc-Zc));
         r= rXs= sqrt((pow(rxs,2)+pow(rXc,2)+pow(rxc,2)) + pow(Tx_yz*s,2)*(pow(rTx_yz/Tx_yz, 2) + (pow(rYs,2)+pow(rZs,2)+pow(rYc,2)+pow(rZc,2))/pow(s,2)));
         break;
      case 12: s= ((Xs-xs)-(Xc-xc));
         r= rYs= sqrt((pow(rZs,2)+pow(rYc,2)+pow(rZc,2)) + pow(Tx_yz*s,2)*(pow(rTx_yz/Tx_yz, 2) + (pow(rXs,2)+pow(rxs,2)+pow(rXc,2)+pow(rxc,2))/pow(s,2)));
         break;
      case 13: s= ((Xs-xs)-(Xc-xc));
         r= rYs= sqrt((pow(rYs,2)+pow(rYc,2)+pow(rZc,2)) + pow(Tx_yz*s,2)*(pow(rTx_yz/Tx_yz, 2) + (pow(rXs,2)+pow(rxs,2)+pow(rXc,2)+pow(rxc,2))/pow(s,2)));
         break;
    }
    return r;
}









void __fastcall TForm1::UseStdFieldClick(TObject *Sender)
{
    // flush gathered chart data
    chartnext= 0;
}
//---------------------------------------------------------------------------

