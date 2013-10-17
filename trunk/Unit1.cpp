//---------------------------------------------------------------------------
/*
    Copyright 2008, 2009  George A Silvis

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

   /*
   Investigating addition of constant term to Tbb - Tii

   ...
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
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
   : TForm(Owner)
{



}
//---------------------------------------------------------------------------

#define Version  1.91


/* adding a coefficient:
   1. add variable
   2. add screen label, edit box, tab sequence  nb edit box must be <name>"Edit"
   3. add INI read reference
   4. add to the output
   5. add to the documentation
   */

// structure for the gui elements for the coefficients
typedef struct {
   float    *value;
   float    *rsq;
   char     *name;
   char     *coefhint;
   TCheckBox   *coeflab;
   TEdit    *coefedit;
   TEdit    *rsqedit;
} Coef;

// RAM copies of the coefficients
float Tbv, Tbr,   Tbi,   Tvr, Tvi,   Tri
     ,Tb,  Tb__R, Tb__I, Tv,  Tv__I, Tr
     ,Tbb, Tvv,   Trr,   Tii
     ,TbbC,TvvC,  TrrC,  TiiC;
float rTbv, rTbr,   rTbi,   rTvr, rTvi,   rTri
     ,rTb,  rTb__R, rTb__I, rTv,  rTv__I, rTr
     ,rTbb, rTvv,   rTrr,   rTii;

Coef TC[]= {
    {&Tbv,&rTbv,    "Tbv",   "1/slope of (b-v) vs (B-V)"/*,    Label1,  TbvEdit*/   }
   ,{&Tbr,&rTbr,    "Tbr",   "1/slope of (b-r) vs (B-R)"/*,    Label11, TbrEdit*/   }
   ,{&Tbi,&rTbi,    "Tbi",   "1/slope of (b-i) vs (B-I)"/*,    Label12, TbiEdit*/   }
   ,{&Tvr,&rTvr,    "Tvr",   "1/slope of (v-r) vs (V-R)"/*,    Label2,  TvrEdit*/   }
   ,{&Tvi,&rTvi,    "Tvi",   "1/slope of (v-i) vs (V-I)"/*,    Label10, TviEdit*/   }
   ,{&Tri,&rTri,    "Tri",   "1/slope of (r-i) vs (R-I)"/*,    Label3,  TriEdit*/   }
   ,{&Tb, &rTb,     "Tb",    "slope of (B-b) vs (B-V)"/*,      label9,  TbEdit*/    }
   ,{&Tb__R,&rTb__R,"Tb__R", "slope of (B-b) vs (B-R)"/*,      Label13, Tb__REdit*/ }
   ,{&Tb__I,&rTb__I,"Tb__I", "slope of (B-b) vs (B-I)"/*,      Label14, Tb__IEdit*/ }
   ,{&Tv, &rTv,     "Tv",    "slope of (V-v) vs (V-R)"/*,      Label4,  TvEdit*/    }
   ,{&Tv__I,&rTv__I,"Tv__I", "slope of (V-v) vs (V-I)"/*,      Label8,  Tv__IEdit*/ }
   ,{&Tr, &rTr,     "Tr",    "slope of (R-r) vs (R-I)"/*,      Label5,  TrEdit*/    }
   ,{&Tbb,&rTbb,    "Tbb",   "slope of (b) vs (B)"/*,          Label15, TbbEdit*/   }
   ,{&Tvv,&rTvv,    "Tvv",   "slope of (v) vs (V)"/*,          Label16, TvvEdit*/   }
   ,{&Trr,&rTrr,    "Trr",   "slope of (r) vs (R)"/*,          Label18, TrrEdit*/   }
   ,{&Tii,&rTii,    "Tii",   "slope of (i) vs (I)"/*,          Label17, TiiEdit*/   }
   };

#define NumCoef (sizeof(TC)/sizeof(Coef))

void __fastcall TForm1::FormCreate(TObject *Sender)
{
   VersionLabel->Caption= "Version "+ FormatFloat("0.0", Version);
   //nb  The labels are now checkboxes
   // have to assign here because they don't exist until here
   TC[0].coeflab= Label1,   TC[0].coefedit= TbvEdit;  TC[0].rsqedit= rTbvEdit;
   TC[1].coeflab= Label11,  TC[1].coefedit= TbrEdit;  TC[1].rsqedit= rTbrEdit;
   TC[2].coeflab= Label12,  TC[2].coefedit= TbiEdit;  TC[2].rsqedit= rTbiEdit;
   TC[3].coeflab= Label2,   TC[3].coefedit= TvrEdit;  TC[3].rsqedit= rTvrEdit;
   TC[4].coeflab= Label10,  TC[4].coefedit= TviEdit;  TC[4].rsqedit= rTviEdit;
   TC[5].coeflab= Label3,   TC[5].coefedit= TriEdit;  TC[5].rsqedit= rTriEdit;

   TC[6].coeflab= Label9,   TC[6].coefedit= TbEdit;   TC[6].rsqedit= rTbEdit;
   TC[7].coeflab= Label13,  TC[7].coefedit= Tb__REdit;TC[7].rsqedit= rTb__REdit;
   TC[8].coeflab= Label14,  TC[8].coefedit= Tb__IEdit;TC[8].rsqedit= rTb__IEdit;
   TC[9].coeflab= Label4,   TC[9].coefedit= TvEdit;   TC[9].rsqedit= rTvEdit;
   TC[10].coeflab= Label8,  TC[10].coefedit= Tv__IEdit;TC[10].rsqedit= rTv__IEdit;
   TC[11].coeflab= Label5,  TC[11].coefedit= TrEdit;  TC[11].rsqedit= rTrEdit;

   TC[12].coeflab= Label15,  TC[12].coefedit= TbbEdit; TC[12].rsqedit= rTbbEdit;
   TC[13].coeflab= Label16,  TC[13].coefedit= TvvEdit; TC[13].rsqedit= rTvvEdit;
   TC[14].coeflab= Label18,  TC[14].coefedit= TrrEdit; TC[14].rsqedit= rTrrEdit;
   TC[15].coeflab= Label17,  TC[15].coefedit= TiiEdit; TC[15].rsqedit= rTiiEdit;

   ReadCoefficients(Sender);
}

void __fastcall TForm1::ReadCoefficients(TObject *Sender)
{
   // collect INI file entries
   TIniFile *ini;
   ini = new TIniFile(ChangeFileExt( Application->ExeName, ".INI" ) );
   AnsiString as;
   for(int i= 0; i< NumCoef; i++) {
      as.sprintf("%-8s %s", TC[i].name, TC[i].coefhint);
      TC[i].coeflab->Caption= as; TC[i].name;
      TC[i].coeflab->Hint= TC[i].coefhint;
      TC[i].coefedit->Hint= TC[i].coefhint;
      *TC[i].value= ini->ReadFloat("Coefficients", TC[i].name, 0.0);
      TC[i].coefedit->Text= FormatFloat("0.0000", *TC[i].value);
      *TC[i].rsq= ini->ReadFloat("RSquared", TC[i].name, 0.0);
      TC[i].rsqedit->Text= FormatFloat("0.0000", *TC[i].rsq);
   }
   TbbC= ini->ReadFloat("Coefficients", "TbbCons", 0.0);
   TvvC= ini->ReadFloat("Coefficients", "TvvCons", 0.0);
   TrrC= ini->ReadFloat("Coefficients", "TrrCons", 0.0);
   TiiC= ini->ReadFloat("Coefficients", "TiiCons", 0.0);

/*
   Tbv= ini->ReadFloat("Coefficients", "Tbv", 0.0);
   TbvEdit->Text= FormatFloat("0.0000", Tbv);

   Tvr= ini->ReadFloat("Coefficients", "Tvr", 0.0);
   TvrEdit->Text= FormatFloat("0.0000", Tvr);

   Tvi= ini->ReadFloat("Coefficients", "Tvi", 0.0);
   TviEdit->Text= FormatFloat("0.0000", Tvi);

   Tri= ini->ReadFloat("Coefficients", "Tri", 0.0);
   TriEdit->Text= FormatFloat("0.0000", Tri);

   Tv= ini->ReadFloat("Coefficients", "Tv", 0.0);
   TvEdit->Text= FormatFloat("0.0000", Tv);

   Tr= ini->ReadFloat("Coefficients", "Tr", 0.0);
   TrEdit->Text= FormatFloat("0.0000", Tr);

   Tb= ini->ReadFloat("Coefficients", "Tb", 0.0);
   TbEdit->Text= FormatFloat("0.0000", Tb);

   Tv__I= ini->ReadFloat("Coefficients", "Tv__I", 0.0);
   Tv__IEdit->Text= FormatFloat("0.0000", Tv__I);
*/

   OpenDialog1->InitialDir= ini->ReadString("Setup", "Dir", "");

   delete ini;

}



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
      ini = new TIniFile(ChangeFileExt( Application->ExeName, ".INI" ) );
      if('r'== p->Name[1])
         ini->WriteFloat("RSquared", p->Name.SubString(2, p->Name.Pos("Edit")-2), f);
      else
         ini->WriteFloat("Coefficients", p->Name.SubString(1, p->Name.Pos("Edit")-1), f);
      delete ini;
      ReadCoefficients(Sender); // resets ram data
   } catch (EConvertError &error) {
      p->SetFocus();
      ShowMessage("This field must be numeric");
   }
}


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
   AnsiString  record;

   AnsiString  NAME;  // Z BOO
   double      DATE;  // 2454572.64069
   AnsiString  DATEs;
   float       VMAG;  // 9.332
   float       VMAGc; // corrected for zero point
   float       VERR;  // 0.002
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
   float       CMAG;  // 17.591
   AnsiString  KNAME; // 000-BBV-175
   float       KMAG;  // 18.627
   float       AMASS; // 1.2606
   AnsiString  GROUPs; // 0
   AnsiString  CHART; // 080415
   AnsiString  NOTES;

   bool        processed;
   char        filter;// index into filter list
   float       CREFMAG; // reference magnitude of the comparison star
   float       VMAGt; // transformed magnitude
   char        FILTC; // filter combination the star is mixed in with
   AnsiString  StarsUsed;
   AnsiString  ErrorMsg;   // collect comments here to be displayed after the obs is printed
   bool        ensemble;
} StarData;
#define sdiMAX   100
StarData sd[sdiMAX];

// db to hold transform group information
typedef struct GroupData {
   AnsiString Group;
   char       FILTC; // filter combination (an enum)
   float      m[16]; // Star BVRI, bvri   Comp BVRI bvri
} GroupData;
#define gdiMAX 40
GroupData gd[gdiMAX];

void SetStarData(StarData d);
void ProcessStarData(StarData *d, char fc);


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

// indices from sd; which obs is providing data for each filter
char sf[FILT_NUM];


// Filter combinations
enum {
    FILTC_BVIR
   ,FILTC_BV
   ,FILTC_VR
   ,FILTC_VRI
   ,FILTC_BVR
   ,FILTC_RI
   ,FILTC_VI
   ,FILTC_BVI
   ,FILTC_BVIRNewB
   ,FILTC_BVIRNewB__R
   ,FILTC_BVIRNewB_I
   ,FILTC_BVIRNewV
   ,FILTC_BVIRNewV__I
   ,FILTC_BVIRNewR
   ,FILTC_BVRNewB
   ,FILTC_BVINewB
   ,FILTC_VRINew
// add here
   ,FILTC_NUM
};

/* too add a filter combination"
1.  add name to enum of Filter combinations
2.  add mask value combination to enum of mask values
3.  add this mask value to the FILTC_mask[]
4.  add a description of the formula to FILTC_desc
5.  add a case to ProcessStarDat
6.  update help file
*/

enum { // mask values
    FILTC_BVIRx=   (FILT_Bx|FILT_Vx|FILT_Rx|FILT_Ix)
   ,FILTC_BVx=     (FILT_Bx|FILT_Vx)
   ,FILTC_VRx=     (FILT_Vx|FILT_Rx)
   ,FILTC_VRIx=    (FILT_Vx|FILT_Rx|FILT_Ix)
   ,FILTC_BVRx=    (FILT_Bx|FILT_Vx|FILT_Rx)
   ,FILTC_RIx=     (FILT_Rx|FILT_Ix)
   ,FILTC_VIx=     (FILT_Vx|FILT_Ix)
   ,FILTC_BVIx=    (FILT_Bx|FILT_Vx|FILT_Ix)
   ,FILTC_BVIRxNewB   =   (FILT_Bx|FILT_Vx|FILT_Rx|FILT_Ix)
   ,FILTC_BVIRxNewB__R=   (FILT_Bx|FILT_Vx|FILT_Rx|FILT_Ix)
   ,FILTC_BVIRxNewB__I=   (FILT_Bx|FILT_Vx|FILT_Rx|FILT_Ix)
   ,FILTC_BVIRxNewV   =   (FILT_Bx|FILT_Vx|FILT_Rx|FILT_Ix)
   ,FILTC_BVIRxNewV__I=   (FILT_Bx|FILT_Vx|FILT_Rx|FILT_Ix)
   ,FILTC_BVIRxNewR   =   (FILT_Bx|FILT_Vx|FILT_Rx|FILT_Ix)
   ,FILTC_BVRxNewB=    (FILT_Bx|FILT_Vx|FILT_Rx)
   ,FILTC_BVIxNewB=    (FILT_Bx|FILT_Vx|FILT_Ix)
   ,FILTC_VRIxNew=    (FILT_Vx|FILT_Rx|FILT_Ix)
// add here
};



const char* FILTC_names[]= {
    "BVIR", "BV", "VR", "VRI", "BVR", "RI", "VI", "BVI", "BVIR", "BVIR", "BVIR", "BVIR", "BVIR", "BVIR", "BVR", "BVI", "VRI"
};


char FILTC_mask[]= {FILTC_BVIRx, FILTC_BVx, FILTC_VRx, FILTC_VRIx, FILTC_BVRx, FILTC_RIx, FILTC_VIx, FILTC_BVIx
    ,FILTC_BVIRxNewB, FILTC_BVIRxNewB__R, FILTC_BVIRxNewB__I, FILTC_BVIRxNewV, FILTC_BVIRxNewV__I, FILTC_BVIRxNewR
    ,FILTC_BVRxNewB, FILTC_BVIxNewB, FILTC_VRIxNew  };

// global vars for equations
float Bs, bs, Vs, vs, Rs, rs, Is, is,   Bc, bc, Vc, vc, Rc, rc, Ic, ic,  oBs, oVs, oRs, oIs;

#define FILTC_desc_rows 7
AnsiString FILTC_desc[FILTC_NUM][FILTC_desc_rows]= {
    {
     "# BVRI   as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Rs - Is) = (Rc - Ic) + Tri * [(rs - is) - (rc - ic)]"
    ,"# Rs = rs + (Rc - rc) + Tr * [(Rs - Is) - (Rc - Ic)], using the solution for (Rs - Is) in the above line"
    ,"# Is = Rs - (Rs - Is), using Rs from the line above, and (Rs - Is) from the first line"
    ,"# Vs = vs + (Vc - vc) + Tv * Tvr * [(vs - rs) - (vc - rc)]"
    ,"# Bs = Vs + (Bc - Vc) + Tbv * [(bs - vs) - (bc - vc)]"
   }
   ,
   {
     "# BV    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Bs - Vs) = (Bc - Vc) + Tbv * [(bs - vs) - (bc - vc)]"
    ,"# Bs = bs + (Bc - bc) + Tb * [(Bs - Vs) - (Bc - Vc)], using the solution for (Bs - Vs) in the above line"
    ,"# Vs = Bs - (Bs - Vs), using Bs from the line above, and (Bs - Vs) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {
     "# VR    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Vs - Rs) = (Vc - Rc) + Tvr * [(vs - rs) - (vc - rc)]"
    ,"# Vs = vs + (Vc - vc) + Tv * [(Vs - Rs) - (Vc - Rc)], using the solution for (Vs - Rs) in the above line"
    ,"# Rs = Vs - (Vs - Rs), using Vs from the line above, and (Vs - Rs) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {
     "# VRI    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Rs - Is) = (Rc - Ic) + Tri * [(rs - is) - (rc - ic)]"
    ,"# Rs = rs + (Rc - rc) + Tr * [(Rs - Is) - (Rc - Ic)], using the solution for (Rs - Is) in the above line"
    ,"# Is = Rs - (Rs - Is), using Rs from the line above, and (Rs - Is) from the first line"
    ,"# Vs = vs + (Vc - vc) + Tv * Tvr * [(vs - rs) - (vc - rc)]"
    ,NULL
   }
   ,
   {
     "# BVR    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Vs - Rs) = (Vc - Rc) + Tvr * [(vs - rs) - (vc - rc)]"
    ,"# Vs = vs + (Vc - vc) + Tv * [(Vs - Rs) - (Vc - Rc)], using the solution for (Vs - Rs) in the above line"
    ,"# Rs = Vs - (Vs - Rs), using Vs from the line above, and (Vs - Rs) from the first line"
    ,"# Bs = Vs + (Bc - Vc) +  Tbv * [(bs - vs) - (bc - vc)]"
    ,NULL
   }
   ,
   {
     "# RI    as formulated by Bruce L. Gary (GBL) http://reductionism.net.seanic.net/CCD_TE/cte.html"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Rs - Is) = (Rc - Ic) + Tri * [(rs - is) - (rc - ic)]"
    ,"# Rs = rs + (Rc - rc) + Tr * [(Rs - Is) - (Rc - Ic)], using the solution for (Rs - Is) in the above line"
    ,"# Is = Rs - (Rs - Is), using Rs from the line above, and (Rs - Is) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {
     "# VI    modeled on the VR formulation "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Vs - Is) = (Vc - Ic) + Tvi * [(vs - is) - (vc - ic)]"
    ,"# Vs = vs + (Vc - vc) + Tv__I * [(Vs - Is) - (Vc - Ic)], using the solution for (Vs - Is) in the above line"
    ,"# Is = Vs - (Vs - Is), using Vs from the line above, and (Vs - Is) from the first line"
    ,NULL
    ,NULL
   }
   ,
   {
     "# BVI   modeled on the BVR formulation"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"# (Vs - Is) = (Vc - Ic) + Tvi * [(vs - is) - (vc - ic)]"
    ,"# Vs = vs + (Vc - vc) + Tv__I * [(Vs - Is) - (Vc - Ic)], using the solution for (Vs - Is) in the above line"
    ,"# Is = Vs - (Vs - Is), using Vs from the line above, and (Vs - Is) from the first line"
    ,"# Bs = Vs + (Bc - Vc) +  Tbv * [(bs - vs) - (bc - vc)]"
    ,NULL
   }

   ,
   {
     "# BVIR  NewB "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Bs = bs + (Bc-bc) + Tb * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri* ((rs-is)-(rc-ic))"
    ,NULL
   }
   ,
   {
     "# BVIR  NewB__R "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Bs = bs + (Bc-bc) + Tb__R * ((Bs-Rs)-(Bc-Rc))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv   * ((bs-vs)-(bc-vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic))"
    ,NULL
   }
   ,
   {
     "# BVIR  NewB__I"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Bs = bs + (Bc-bc) + Tb__I * ((Bs-Is)-(Bc-Ic))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv   * ((bs-vs)-(bc-vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic))"
    ,NULL
   }
   ,
   {
     "# BVIR  NewV "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Vs = vs + (Vc-vc) + Tv    * ((Vs-Rs)-(Vc-Rc))"
    ,"#  Bs = Vs + (Bc-Vc) + Tbv   * ((bs-vs)-(bc-vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic))"
    ,NULL
   }
   ,
   {
     "# BVIR  NewV__I"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Vs = vs + (Vc-vc) + Tv__I * ((Vs-Is)-(Vc-Ic))"
    ,"#  Bs = Vs + (Bc-Vc) + Tbv   * ((bs-vs)-(bc-vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic))"
    ,NULL
   }
   ,
   {
     "# BVIR  NewR"
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Rs = rs + (Rc-rc) + Tr    * ((Rs-Is)-(Rc-Ic))"
    ,"#  Vs = Rs + (Vc-Rc) + Tvr   * ((vs-rs)-(vc-rc))"
    ,"#  Bs = Vs + (Bc-Vc) + Tbv   * ((bs-vs)-(bc-vc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic))"
    ,NULL
   }
   ,
   {
     "# BVR  NewB "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Bs = bs + (Bc-bc) + Tb * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc))"
    ,NULL
    ,NULL
   }
   ,
   {
     "# BVI  NewB "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Bs = bs + (Bc-bc) + Tb * ((Bs-Vs)-(Bc-Vc))"
    ,"#  Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc))"
    ,"#  Is = Vs - (Vc-Ic) - Tvi* ((vs-is)-(vc-ic))"
    ,NULL
    ,NULL
   }
   ,
   {
     "# VRI  New "
    ,"#  variable notation: filter/star. Star s is the target, c is the comparison. Capital filter is ref, lower case is as observed"
    ,"#  Vs = vs + (Vc-vc) + Tv    * ((Vs-Rs)-(Vc-Rc))"
    ,"#  Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc))"
    ,"#  Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic))"
    ,NULL
    ,NULL
   }
};


bool trans_display;
bool FILTC_display[FILTC_NUM];
AnsiString Formula= "";

//---------------------------------------------------------------------------
void __fastcall TForm1::ProcessButtonClick(TObject *Sender)
{
   char delim= ';';
   char fc, fci; // filter combo and index
   float crefmag= -999, x;
   int i, j, k, m, sdi= 0, gdi= 0;
   AnsiString s, r, sx;

   // init
   // reset flags for displaying information in the output. Only want to display
   // the transforms and the formulas once in the output
   trans_display= false;
   Formula= "";
   for(i= 0; i< FILTC_NUM; i++)
      FILTC_display[i]= false;

   // assessment pass through the input data
   /*
       must have TYPE, TYPE must be EXTENDED
       must have DELIM
       must have OBSCODE
       must have SOFTWARE
       must have DATE
       must have OBSTYPE
       collect CREFMAG
   */
   for(i= 0; i< Memo1->Lines->Count; i++) { // each line in the input text
      s= Memo1->Lines->Strings[i];
      if(s[1]=='#') { // comment line
         //tbd  Could vet for required fields...
         if(s.SubString(2, 5)== "TYPE=") {



         } else
         if(s.SubString(2, 6)== "DELIM=") {
            // simple method. assumes the DELIM= x
            j= 7;
            while(s[++j]) {
               if(s[j]!=' ') {
                  delim= s[j];
                  break;
               }
            }
            // tbd  other cases: named variable "tab" or "comma"
         } else
         if(s.SubString(2, 8)== "CREFMAG=") {
            crefmag= s.SubString(10, 20).ToDouble();
//               ShowMessage(" missed CREFMAG");     EConvertError
         }


      } else { // data line
         if(sdi==sdiMAX) {
            ShowMessage("Too many observations in the file.");
            return;
         }

         // capture the line to sd
         sd[sdi].record= s;
         sd[sdi].processed= false; // clear the flag
         sd[sdi].ErrorMsg= ""; // clear the msg
         sd[sdi].ensemble= false;

         k= 1;
         j= s.Pos(delim);
         sd[sdi].NAME= s.SubString(k, j- 1);
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].DATE= s.SubString(k, j- 1).ToDouble();
         sd[sdi].DATEs= s.SubString(k, j- 1);
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         if(s[k]=='<') {
            sd[sdi].ErrorMsg+= "VMAG is relative.";
            sd[sdi].processed= true; // skip this record
         } else {
            sd[sdi].VMAG= s.SubString(k, j- 1).ToDouble();
         }
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].VERR= s.SubString(k, j- 1).ToDouble();
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].FILT= s.SubString(k, j- 1);
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
            //sd[sdi].ErrorMsg+= " Already transformed.";
            sd[sdi].processed= true;
         }

         j= s.SubString(k, 20).Pos(delim);
         if     (s.SubString(k, j- 1).UpperCase()== "ABS") sd[sdi].MTYPE= 'A';
         else if(s.SubString(k, j- 1).UpperCase()== "DIF") sd[sdi].MTYPE= 'D';
         else if(s.SubString(k, j- 1).UpperCase()== "STD") sd[sdi].MTYPE= 'S';
         else {
            sd[sdi].ErrorMsg+= " MTYPE not properly specified.";
            sd[sdi].processed= true; // skip this record
         }
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].CNAME= s.SubString(k, j- 1);
         k+= j;
         if(sd[sdi].CNAME=="ENSEMBLE")
            sd[sdi].ensemble= true;

         j= s.SubString(k, 20).Pos(delim);
         if(!sd[sdi].ensemble) { // if so, CMAG is na
            sd[sdi].CMAG= s.SubString(k, j- 1).ToDouble();
         }
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].KNAME= s.SubString(k, j- 1);
         if(sd[sdi].KNAME.UpperCase()=="NO") sd[sdi].KNAME= "na"; // AIP uses NO; webobs wants na
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         if(sd[sdi].KNAME!="na")
            sd[sdi].KMAG= s.SubString(k, j- 1).ToDouble();        // will be NO if no check star
         k+= j;

         // if ensemble, the check star is K. Move it to C for processing
         if(sd[sdi].ensemble) {
            if(sd[sdi].KNAME=="na") {
               sd[sdi].ErrorMsg+= " Cannot do ensemble with out K star data.";
               sd[sdi].processed= true; // skip this record
            } else {
               sd[sdi].CNAME = sd[sdi].KNAME;
               sd[sdi].CMAG= sd[sdi].KMAG;
            }
         }

         // is crefmag available? might have seen it a while ago.
         // need to match filter and star name in the table so far.
         if(crefmag == -999) {
            for(int i= 0; i< sdi; i++) {
               if(sd[sdi].CNAME==sd[i].CNAME) { // same star
                  if(sd[sdi].filter==sd[i].filter) { // same filter
                     crefmag= sd[i].CREFMAG;
                     break;
                  }
               }
            }
         }

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].AMASS= s.SubString(k, j- 1).ToDouble();
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         //sd[sdi].GROUP= s.SubString(k, j- 1).ToInt(); might be "NA"
         sd[sdi].GROUPs= s.SubString(k, j- 1);
         if(sd[sdi].GROUPs.UpperCase() =="NA" ) {
            sd[sdi].processed= true; // skip
            sd[sdi].ErrorMsg+= "GROUP is NA.";
         }
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].CHART= s.SubString(k, j- 1);
         k+= j;

         sd[sdi].NOTES= s.SubString(k, 100);

         // correct VMAG from AIPWIN reported value to observed value
         if(sd[sdi].MTYPE == 'A' || sd[sdi].MTYPE == 'S') { // "ABS" or "STD"
            if(crefmag==-999) {
               sd[sdi].ErrorMsg+= " No CREFMAG available.";
               sd[sdi].processed= true;
               break;  // skip record
            }
            sd[sdi].CREFMAG= crefmag;
            sd[sdi].VMAGc= sd[sdi].VMAG + sd[sdi].CMAG - crefmag;
         } else if(sd[sdi].MTYPE == 'D') { // "DIF"
            sd[sdi].VMAGc= sd[sdi].VMAG + sd[sdi].CMAG;
         }

         crefmag= -999; // clear crefmag

         sdi++; // ready for next
      }
   }
   // sdi is the number of obs collected

   // process
   // Go through all the obs to find what combination we have
   do {
      AnsiString cs= NULL;
      for(i= 0; i< sdi; i++) {
         if(sd[i].processed==false) {
            if(cs!=NULL) { // do we have a star name/group?
               if(cs==sd[i].NAME+ sd[i].GROUPs) {
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

      // we are ready to process a star. the RAM variables are all set

      // what is the filter combo for this star (cs)?
      for(j= 0, fc= 0; j< FILT_NUM; j++)
         if(sf[j]!=sdiMAX)
            fc|= FILT_mask[j];

      // process this star/groups obs
      // nb. this is prepared to handle an overloaded group
      for(i= 0; i< sdi; i++) {
         if((sd[i].NAME+ sd[i].GROUPs)==cs && sd[i].processed==false) {
            SetStarData(sd[i]);
            sf[sd[i].filter]= i; // must provide data for its own filter
            ProcessStarData(&sd[i], fc); // sets processed flag
            // capture the group data
            if(gdi==gdiMAX) {
               ShowMessage("Too many groups in the file.");
               return;
            }
            gd[gdi].Group= sd[i].NAME+ sd[i].GROUPs;
            gd[gdi].FILTC= fc;
            for(j= 0; j<16; j++) gd[gdi].m[j]= 0; // clear
            if(fc ^ FILT_Bx)
               gd[gdi].m[0]= Bs, gd[gdi].m[4]= bs, gd[gdi].m[8]= Bc, gd[gdi].m[12]= bc;
            if(fc ^ FILT_Vx)
               gd[gdi].m[1]= Vs, gd[gdi].m[5]= vs, gd[gdi].m[9]= Vc, gd[gdi].m[13]= vc;
            if(fc ^ FILT_Rx)
               gd[gdi].m[2]= Rs, gd[gdi].m[6]= rs, gd[gdi].m[10]= Rc, gd[gdi].m[14]= rc;
            if(fc ^ FILT_Ix)
               gd[gdi].m[3]= Is, gd[gdi].m[7]= is, gd[gdi].m[11]= Ic, gd[gdi].m[15]= ic;
            gdi++;
         }
      }
   } while(42);

//   ShowMessage("Review the convergence of the New formulas (if any)");
   Memo2->Lines->Clear(); // output window
//   Memo4->Lines->Clear(); // report window
   Memo4->Lines->Add(Formula);
   Memo4->Lines->Add("Star                 Date   Filter  Grp    Vmag   TranMag     diff");
   for(i= 0, j= 0; i< Memo1->Lines->Count; i++) {
      s= Memo1->Lines->Strings[i];
      if(s.Length()!=0) // skip if blank
      if(s[1]=='#') { // non-data line
         Memo2->Lines->Add(s); // just copy
      } else { // data line
         if(!sd[j].TRANS) { // not Transformed
            Memo2->Lines->Add(s); // just copy original
            r.sprintf("%s %s %8.3f NA", sd[j].NAME, sd[j].DATEs, sd[j].VMAG);
            //? where was this to go? Not here:   Memo2->Lines->Add(r);
         } else {  // display transformed data    ?? what if it was transformed in the input, thus skipped?
            // Standard info blocks been output once
            if(!trans_display) { // show the transform coefficients
               Memo2->Lines->Add("#   transform coefficients applied by Transformer, version "+ FormatFloat("0.0", Version));
               Memo2->Lines->Add("#             http://www.gasilvis.net/transformer/  ");
               Memo2->Lines->Add("#   transform coefficients:");
               Memo2->Lines->Add("#Tbv= "+ TbvEdit->Text);
               Memo2->Lines->Add("#Tbr= "+ TbrEdit->Text);
               Memo2->Lines->Add("#Tbi= "+ TbiEdit->Text);
               Memo2->Lines->Add("#Tvr= "+ TvrEdit->Text);
               Memo2->Lines->Add("#Tvi= "+ TviEdit->Text);
               Memo2->Lines->Add("#Tri= "+ TriEdit->Text);
               Memo2->Lines->Add("#Tb= " + TbEdit->Text);
               Memo2->Lines->Add("#Tb__R= " + Tb__REdit->Text);
               Memo2->Lines->Add("#Tb__I= " + Tb__IEdit->Text);
               Memo2->Lines->Add("#Tv= " + TvEdit->Text);
               Memo2->Lines->Add("#Tv__I= " + Tv__IEdit->Text);
               Memo2->Lines->Add("#Tr= " + TrEdit->Text);
               trans_display= true;
            }
            for(k= 0; k< FILTC_NUM; k++) {
               if(sd[j].FILTC==FILTC_mask[k]) { // this will end up showing all the BVIR formulas
                   if(!FILTC_display[k]) { // if it has not been displayed
                      for(m= 0; m<FILTC_desc_rows; m++) {
                         if(FILTC_desc[k][m]!= NULL)
                            Memo2->Lines->Add(FILTC_desc[k][m]);
                      }
                      FILTC_display[k]= true;
                   }
                }
             }

            // show un-transformed data as a comment
            Memo2->Lines->Add((IncludeRaw->Checked?"":"#")+ s);

            // now the transformed data
            s= sd[j].NAME+ delim;

            s+= FormatFloat("#.00000", sd[j].DATE)+ delim;

            // all the same...
            if(sd[j].MTYPE == 'A') { // "ABS"
               s+= FormatFloat("0.000", sd[j].VMAGt)+ delim;
            } else { // "DIF" or STD
               s+= FormatFloat("0.000", sd[j].VMAGt)+ delim;
            }

            s+= FormatFloat("0.000", sd[j].VERR)+ delim;    // ???

            s+= sd[j].FILT+ delim;

            s+= sd[j].TRANS? "YES": "NO";
            s+= delim;

            switch(sd[j].MTYPE) {
               case 'A': s+= "ABS"; break;
               case 'D': s+= "DIF"; break;
               case 'S': s+= "STD"; break;
            }
            s+= delim;

            if(sd[j].ensemble) {
               s+= "ENSEMBLE"+ delim; s+= "NA"+ delim;   // cname data
               s+= sd[j].CNAME+ delim; // kname was stored here
               s+= FormatFloat("0.000", sd[j].CMAG)+ delim;
            } else {
               s+= sd[j].CNAME+ delim;
               s+= FormatFloat("0.000", sd[j].CMAG)+ delim;

               s+= sd[j].KNAME+ delim;
               if(sd[j].KNAME=="na")
                  s+="na" , s+= delim;
               else s+= FormatFloat("0.000", sd[j].KMAG)+ delim;
            }
            s+= FormatFloat("0.0000", sd[j].AMASS)+ delim;

            //s+= IntToStr(sd[j].GROUP)+ delim;
            s+= sd[j].GROUPs+ delim;

            s+= sd[j].CHART+ delim;
            s+= sd[j].NOTES;

            Memo2->Lines->Add(s);

            // add a star report line
            r.sprintf("\"%-15s\"   %s %s %3s %8.3f %8.3f  %8.3f", sd[j].NAME, sd[j].DATEs, sd[j].FILT, sd[j].GROUPs, sd[j].VMAG, sd[j].VMAGt, sd[j].VMAGt- sd[j].VMAG);
            Memo4->Lines->Add(r);

            // add info about the analysis
            Memo2->Lines->Add(sd[j].StarsUsed);

         }
         if(sd[j].ErrorMsg != "")
            ShowMessage(sd[j].ErrorMsg);

         j++;
      }
   }
   // group report
   Memo4->Lines->Add("\n name+group      filtC   Bs     Vs     Rs     Is     bs     vs     rs     is     Bc     Vc     Rc     Ic     bc     vc     rc     ic");
   for(i= 0; i<gdi; i++) {
      r.sprintf("\"%-15s\" %3i", gd[i].Group, gd[i].FILTC);
      for(j= 0; j<16; j++) {
         if(gd[i].m[j]==0) s= "      0";
         else              s.sprintf("%7.3f", gd[i].m[j]);
         r= r+ s;
      }
      Memo4->Lines->Add(r);
   }
}
//---------------------------------------------------------------------------

void SetStarData(StarData d)
{
   switch(d.filter) {
      case FILT_Bi: bs= d.VMAGc; bc= d.CMAG; Bc= d.CREFMAG; break;
      case FILT_Vi: vs= d.VMAGc; vc= d.CMAG; Vc= d.CREFMAG; break;
      case FILT_Ri: rs= d.VMAGc; rc= d.CMAG; Rc= d.CREFMAG; break;
      case FILT_Ii: is= d.VMAGc; ic= d.CMAG; Ic= d.CREFMAG; break;
   }
}

void ProcessStarData(StarData *d, char fc)
{
   AnsiString as;
   d->TRANS= true;
   d->FILTC= fc;  // which combination used
   switch(fc) {
      case FILTC_BVIRx:
       if(Form1->RadioButton1->Checked) {  // Sarty
         if(Tr==0 || Tri==0 || Tv==0 || Tvr==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tr, Tri, Tv, Tvr and Tbv.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton1->Caption;
         Rs = rs + (Rc - rc) + Tr * (((Rc - Ic) + Tri * ((rs - is) - (rc - ic))) - (Rc - Ic));
         Is = Rs - ((Rc - Ic) + Tri * ((rs - is) - (rc - ic)));
         Vs = vs + (Vc - vc) + Tv * Tvr * ((vs - rs) - (vc - rc));
         Bs = Vs + (Bc - Vc) + Tbv * ((bs - vs) - (bc - vc));
         d->StarsUsed= "# BVIR using Sarty: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;

       } else if(Form1->RadioButton2->Checked) { // New B  (V)
         if(Tbv==0 || Tvr==0 || Tri==0 || Tb==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr, Tri and Tb.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton2->Caption;
         //Tb = ((Bs-bs)-(Bc-bc)) / ((Bs-Vs)-(Bc-Vc))
         //Tbv= ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvr= ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         //Tri= ((Rs-Is)-(Rc-Ic)) / ((rs-is)-(rc-ic))
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = bs + (Bc-bc) + Tb * ((Bs-Vs)-(Bc-Vc));
            Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc));
            Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc));
            Is = Rs - (Rc-Ic) - Tri* ((rs-is)-(rc-ic));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR using New B: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;

       } else if(Form1->RadioButton3->Checked) { // New B__R
         if(Tbv==0 || Tvr==0 || Tri==0 || Tb__R==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr, Tri and Tb__R.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton3->Caption;
         //Tb__R= ((Bs-bs)-(Bc-bc)) / ((Bs-Rs)-(Bc-Rc))
         //Tbv  = ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvr  = ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         //Tri  = ((Rs-Is)-(Rc-Ic)) / ((rs-is)-(rc-ic))
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = bs + (Bc-bc) + Tb__R * ((Bs-Rs)-(Bc-Rc));
            Vs = Bs - (Bc-Vc) - Tbv   * ((bs-vs)-(bc-vc));
            Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc));
            Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR using New B__R: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;

       } else if(Form1->RadioButton4->Checked) { // New B__I
         Formula= Form1->RadioButton4->Caption;
         if(Tbv==0 || Tvr==0 || Tri==0 || Tb__I==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr, Tri and Tb__I.";
            d->TRANS= false;
            break;
         }
         //Tb__I= ((Bs-bs)-(Bc-bc)) / ((Bs-Is)-(Bc-Ic))
         //Tbv  = ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvr  = ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         //Tri  = ((Rs-Is)-(Rc-Ic)) / ((rs-is)-(rc-ic))
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = bs + (Bc-bc) + Tb__I * ((Bs-Is)-(Bc-Ic));
            Vs = Bs - (Bc-Vc) - Tbv   * ((bs-vs)-(bc-vc));
            Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc));
            Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR using New B__I: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;

       } else if(Form1->RadioButton5->Checked) { // New V (R)
         if(Tbv==0 || Tvr==0 || Tri==0 || Tv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr, Tri and Tv.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton5->Caption;
         //Tv   = ((Vs-vs)-(Vc-vc)) / ((Vs-Rs)-(Vc-Rc))
         //Tbv  = ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvr  = ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         //Tri  = ((Rs-Is)-(Rc-Ic)) / ((rs-is)-(rc-ic))
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Vs = vs + (Vc-vc) + Tv    * ((Vs-Rs)-(Vc-Rc));
            Bs = Vs + (Bc-Vc) + Tbv   * ((bs-vs)-(bc-vc));
            Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc));
            Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR using New V: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;

       } else if(Form1->RadioButton6->Checked) { // New V__I
         if(Tbv==0 || Tvr==0 || Tri==0 || Tv__I==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr, Tri and Tv__I.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton6->Caption;
         //Tv__I= ((Vs-vs)-(Vc-vc)) / ((Vs-Is)-(Vc-Ic))
         //Tbv  = ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvr  = ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         //Tri  = ((Rs-Is)-(Rc-Ic)) / ((rs-is)-(rc-ic))
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Vs = vs + (Vc-vc) + Tv__I * ((Vs-Is)-(Vc-Ic));
            Bs = Vs + (Bc-Vc) + Tbv   * ((bs-vs)-(bc-vc));
            Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc));
            Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR using New V__I: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;

       } else if(Form1->RadioButton7->Checked) { // New R (I)
         if(Tbv==0 || Tvr==0 || Tri==0 || Tr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr, Tri and Tr.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton7->Caption;
         //Tr   = ((Rs-rs)-(Rc-rc)) / ((Rs-Is)-(Rc-Ic))
         //Tbv  = ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvr  = ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         //Tri  = ((Rs-Is)-(Rc-Ic)) / ((rs-is)-(rc-ic))
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Rs = rs + (Rc-rc) + Tr    * ((Rs-Is)-(Rc-Ic));
            Vs = Rs + (Vc-Rc) + Tvr   * ((vs-rs)-(vc-rc));
            Bs = Vs + (Bc-Vc) + Tbv   * ((bs-vs)-(bc-vc));
            Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR using New R: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;


       } else if(Form1->RadioButton8->Checked) { // Newer B
         if(Tbv==0 || Tvr==0 || Tri==0 || Tbb==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr, Tri and Tbb.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton8->Caption;
         //Tbb= (bs-bc)/ (Bs-Bc)
         //Tbv= ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvr= ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         //Tri= ((Rs-Is)-(Rc-Ic)) / ((rs-is)-(rc-ic))
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs= (bs-bc) / Tbb + Bc;
            Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc));
            Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc));
            Is = Rs - (Rc-Ic) - Tri* ((rs-is)-(rc-ic));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR using Newer B: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;

       } else if(Form1->RadioButton9->Checked) { // Newer V
         if(Tbv==0 || Tvr==0 || Tri==0 || Tvv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr, Tri and Tvv.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton9->Caption;
         //Tvv  = (vs-vc)/ (Vs-Vc)
         //Tbv  = ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvr  = ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         //Tri  = ((Rs-Is)-(Rc-Ic)) / ((rs-is)-(rc-ic))
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Vs= (vs-vc)/Tvv + Vc;
            Bs = Vs + (Bc-Vc) + Tbv   * ((bs-vs)-(bc-vc));
            Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc));
            Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR using Newer V : B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;

       } else if(Form1->RadioButton10->Checked) { // Newer R
         if(Tbv==0 || Tvr==0 || Tri==0 || Trr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr, Tri and Trr.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton10->Caption;
         //Trr  = (rs-rc)/ (Rs-Rc)
         //Tbv  = ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvr  = ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         //Tri  = ((Rs-Is)-(Rc-Ic)) / ((rs-is)-(rc-ic))
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Rs = (rs-rc)/Trr + Rc;
            Vs = Rs + (Vc-Rc) + Tvr   * ((vs-rs)-(vc-rc));
            Bs = Vs + (Bc-Vc) + Tbv   * ((bs-vs)-(bc-vc));
            Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR using Newer R : B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;

       } else if(Form1->RadioButton11->Checked) { // Newer I
         if(Tbv==0 || Tvr==0 || Tri==0 || Tii==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr, Tri and Tii.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton11->Caption;
         //Tii  = (is-ic)/ (Is-Ic)
         //Tbv  = ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvr  = ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         //Tri  = ((Rs-Is)-(Rc-Ic)) / ((rs-is)-(rc-ic))
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Is = (is-ic)/Tii + Ic;
            Rs = Is + (Rc-Ic) + Tri   * ((rs-is)-(rc-ic));
            Vs = Rs + (Vc-Rc) + Tvr   * ((vs-rs)-(vc-rc));
            Bs = Vs + (Bc-Vc) + Tbv   * ((bs-vs)-(bc-vc));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR using Newer I: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;

       } else if(Form1->RadioButton12->Checked) { // Newest
         if(Tbb==0 || Tvv==0 || Trr==0 || Tri==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbb, Tvv, Trr and Tii";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton12->Caption;
         //Tbb= (bs-bc)/ (Bs-Bc)
         //Tvv= (vs-vc)/ (Vs-Vc)
         //Trr= (rs-rc)/ (Rs-Rc)
         //Tii= (is-ic)/ (Is-Ic)
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs= (bs-bc)/Tbb + Bc;
            Vs= (vs-vc)/Tvv + Vc;
            Rs= (rs-rc)/Trr + Rc;
            Is= (is-ic)/Tii + Ic;
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR using Newest: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;

       } else if(Form1->RadioButton13->Checked) { // Custom
//         if(Tbb==0 || Tvv==0 || Trr==0 || Tri==0) {
//            d->ErrorMsg+= " Missing a coefficient; need Tbb, Tvv, Trr and Tii";
//            d->TRANS= false;
//            break;
//         }
         Formula= Form1->RadioButton13->Caption;
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Bs = Rs + (Bc-Rc) + Tbr* ((bs-rs)-(bc-rc));
            Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc));
            Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc));
            Is = Rs - (Rc-Ic) - Tri* ((rs-is)-(rc-ic));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVIR using Newest: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;
       }
         break;
      case FILTC_BVx:
         if(Tb==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tb and Tbv.";
            d->TRANS= false;
            break;
         }
         Bs = bs + (Bc - bc) + Tb * (((Bc - Vc) + Tbv * ((bs - vs) - (bc - vc))) - (Bc - Vc));
         Vs = Bs - ((Bc - Vc) + Tbv * ((bs - vs) - (bc - vc)));
         d->StarsUsed= "# BV using: B @ "+ sd[sf[FILT_Bi]].DATEs
                               + ", V @ "+ sd[sf[FILT_Vi]].DATEs;
         break;
      case FILTC_VRx:
         if(Tv==0 || Tvr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv and Tvr.";
            d->TRANS= false;
            break;
         }
         Vs = vs + (Vc - vc) + Tv * (((Vc - Rc) + Tvr * ((vs - rs) - (vc - rc))) - (Vc - Rc));
         Rs = Vs - ((Vc - Rc) + Tvr * ((vs - rs) - (vc - rc)));
         d->StarsUsed= "# VR using: V @ "+ sd[sf[FILT_Vi]].DATEs
                               + ", R @ "+ sd[sf[FILT_Ri]].DATEs;
         break;
      case FILTC_VRIx:
        if(Form1->RadioButton2->Checked) { // New   (V)
         if(Tvr==0 || Tri==0 || Tv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tvr, Tri and Tv.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton5->Caption;
         //Tv   = ((Vs-vs)-(Vc-vc)) / ((Vs-Rs)-(Vc-Rc))
         //Tvr  = ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         //Tri  = ((Rs-Is)-(Rc-Ic)) / ((rs-is)-(rc-ic))
         Bs= bs, Vs= vs, Rs= rs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs, oIs= Is;
            Vs = vs + (Vc-vc) + Tv    * ((Vs-Rs)-(Vc-Rc));
            //Bs = Vs + (Bc-Vc) + Tbv   * ((bs-vs)-(bc-vc));
            Rs = Vs - (Vc-Rc) - Tvr   * ((vs-rs)-(vc-rc));
            Is = Rs - (Rc-Ic) - Tri   * ((rs-is)-(rc-ic));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# VRI using: New    V @ "+ sd[sf[FILT_Vi]].DATEs
                                + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                + ", I @ "+ sd[sf[FILT_Ii]].DATEs;
       } else {
         if(Tr==0 || Tri==0 || Tv==0 || Tvr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tr, Tri, Tv and Tvr.";
            d->TRANS= false;
            break;
         }
         Rs = rs + (Rc - rc) + Tr * (((Rc - Ic) + Tri * ((rs - is) - (rc - ic))) - (Rc - Ic));
         Is = Rs - ((Rc - Ic) + Tri * ((rs - is) - (rc - ic)));
         Vs = vs + (Vc - vc) + Tv * Tvr * ((vs - rs) - (vc - rc));
         d->StarsUsed= "# VRI using: V @ "+ sd[sf[FILT_Vi]].DATEs
                                + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                + ", I @ "+ sd[sf[FILT_Ii]].DATEs;

       }
         break;
      case FILTC_BVRx:
       if(Form1->RadioButton2->Checked) { // New B  (V)
         if(Tbv==0 || Tvr==0 || Tb==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvr and Tb.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton2->Caption;
         //Tb = ((Bs-bs)-(Bc-bc)) / ((Bs-Vs)-(Bc-Vc))
         //Tbv= ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvr= ((Vs-Rs)-(Vc-Rc)) / ((vs-rs)-(vc-rc))
         Bs= bs, Vs= vs, Rs= rs;
         do {
            oBs= Bs, oVs= Vs, oRs= Rs;
            Bs = bs + (Bc-bc) + Tb * ((Bs-Vs)-(Bc-Vc));
            Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc));
            Rs = Vs - (Vc-Rc) - Tvr* ((vs-rs)-(vc-rc));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Rs-oRs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Rs-oRs)>0.0001 );
         d->StarsUsed= "# BVR using New B: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs;
       } else { // Sarty
         if(Tv==0 || Tvr==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv, Tvr and Tbv.";
            d->TRANS= false;
            break;
         }
         Vs = vs + (Vc - vc) + Tv * (((Vc - Rc) + Tvr * ((vs - rs) - (vc - rc))) - (Vc - Rc));
         Rs = Vs - ((Vc - Rc) + Tvr * ((vs - rs) - (vc - rc)));
         Bs = Vs + (Bc - Vc) +  Tbv * ((bs - vs) - (bc - vc));
         d->StarsUsed= "# BVR using: B @ "+ sd[sf[FILT_Bi]].DATEs
                                + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                + ", R @ "+ sd[sf[FILT_Ri]].DATEs;
        }
         break;
      case FILTC_RIx:
         if(Tr==0 || Tri==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tr and Tri.";
            d->TRANS= false;
            break;
         }
         Rs = rs + (Rc - rc) + Tr * (((Rc - Ic) + Tri * ((rs - is) - (rc - ic))) - (Rc - Ic));
         Is = Rs - ((Rc - Ic) + Tri * ((rs - is) - (rc - ic)));
         d->StarsUsed= "# RI using: R @ "+ sd[sf[FILT_Ri]].DATEs
                               + ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         break;
      case FILTC_VIx:
         if(Tv__I==0 || Tvi==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv__I and Tvi.";
            d->TRANS= false;
            break;
         }
         Vs = vs + (Vc - vc) + Tv__I * (((Vc - Ic) + Tvi * ((vs - is) - (vc - ic))) - (Vc - Ic));
         Is = Vs - ((Vc - Ic) + Tvi * ((vs - is) - (vc - ic)));
         d->StarsUsed= "# VI using: V @ "+ sd[sf[FILT_Vi]].DATEs
                               + ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         break;
      case FILTC_BVIx:
       if(Form1->RadioButton2->Checked) { // New B  (V)
         if(Tbv==0 || Tvi==0 || Tb==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tbv, Tvi and Tb.";
            d->TRANS= false;
            break;
         }
         Formula= Form1->RadioButton2->Caption;
         //Tb = ((Bs-bs)-(Bc-bc)) / ((Bs-Vs)-(Bc-Vc))
         //Tbv= ((Bs-Vs)-(Bc-Vc)) / ((bs-vs)-(bc-vc))
         //Tvi= ((Vs-Is)-(Vc-Ic)) / ((vs-is)-(vc-ic))
         Bs= bs, Vs= vs, Is= is;
         do {
            oBs= Bs, oVs= Vs, oIs= Is;
            Bs = bs + (Bc-bc) + Tb * ((Bs-Vs)-(Bc-Vc));
            Vs = Bs - (Bc-Vc) - Tbv* ((bs-vs)-(bc-vc));
            Is = Vs - (Vc-Ic) - Tvi* ((vs-is)-(vc-ic));
            Form1->Memo2->Lines->Add(as.sprintf(" %0.3f, %0.3f, %0.3f", fabs(Bs-oBs), fabs(Vs-oVs), fabs(Is-oIs)));
         } while ( fabs(Bs-oBs)>0.0001 || fabs(Vs-oVs)>0.0001 || fabs(Is-oIs)>0.0001 );
         d->StarsUsed= "# BVI using New B: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;
       } else { // Sarty
         if(Tv__I==0 || Tvi==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tv__I, Tvi and Tbv.";
            d->TRANS= false;
            break;
         }
         Vs = vs + (Vc - vc) + Tv__I * (((Vc - Ic) + Tvi * ((vs - is) - (vc - ic))) - (Vc - Ic));
         Is = Vs - ((Vc - Ic) + Tvi * ((vs - is) - (vc - ic)));
         Bs = Vs + (Bc - Vc) +  Tbv * ((bs - vs) - (bc - vc));
         d->StarsUsed= "# BVI using: B @ "+ sd[sf[FILT_Bi]].DATEs
                                + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                + ", I @ "+ sd[sf[FILT_Ii]].DATEs;
       }
         break;
      default: // unknown combination
         d->ErrorMsg+= " Un-supported filter combination: ";
         for(int j= 0; j< FILT_NUM; j++) {
            if(fc & FILT_mask[j]) {
               d->ErrorMsg+= " ";
               d->ErrorMsg+= FILT_name[j];
            }
         }
         d->TRANS= false;
         break;
   }

   if(d->TRANS) {
      switch(d->filter) {
         case FILT_Bi: d->VMAGt= Bs; break;
         case FILT_Vi: d->VMAGt= Vs; break;
         case FILT_Ri: d->VMAGt= Rs; break;
         case FILT_Ii: d->VMAGt= Is; break;
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
      if ABS then VMAG+= CMAG - CREFMAG
      if DIF then VMAG+= CMAG

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
   if(OpenDialog1->Execute()){
      // put into the Input window
      UnTransformedFile= OpenDialog1->FileName;
      Memo1->Lines->LoadFromFile(UnTransformedFile);
   };
   // Save the directory
   TIniFile *ini;
   ini = new TIniFile(ChangeFileExt( Application->ExeName, ".INI" ) );
   ini->WriteString("Setup", "Dir", OpenDialog1->InitialDir);
   delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Saveuntransformedobsfile1Click(TObject *Sender)
{
   // use same dir as open
   SaveDialog1->InitialDir= OpenDialog1->InitialDir;
   SaveDialog1->FileName= UnTransformedFile;
   if(SaveDialog1->Execute()) {
      Memo1->Lines->SaveToFile(SaveDialog1->FileName);
   }
   OpenDialog1->InitialDir= SaveDialog1->InitialDir;
   // Save the directory
   TIniFile *ini;
   ini = new TIniFile(ChangeFileExt( Application->ExeName, ".INI" ) );
   ini->WriteString("Setup", "Dir", OpenDialog1->InitialDir);
   delete ini;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Save1Click(TObject *Sender)
{
   // use same dir as open
   SaveDialog1->InitialDir= OpenDialog1->InitialDir;
   if(SaveDialog1->Execute()) {
      Memo2->Lines->SaveToFile(SaveDialog1->FileName);
   }
   OpenDialog1->InitialDir= SaveDialog1->InitialDir;
   // Save the directory
   TIniFile *ini;
   ini = new TIniFile(ChangeFileExt( Application->ExeName, ".INI" ) );
   ini->WriteString("Setup", "Dir", OpenDialog1->InitialDir);
   delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Exit1Click(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------


/*
typedef struct {
   TEdit*  SetEdit;
   TEdit*  RefEdit;
   TLabel* PVLabel;
   TLabel* SVLabel;
   float   V[2];
   Track   t_V[2];
   float   temp;
   Track   t_temp;
   float   err[2];
   TLabel* ErrLabel[2];
   float   res[2];
   TLabel* ResLabel[2];
   TButton* SetB;
   TButton* GetB;
} Samp_;
#define NumPts 7
Samp_ Samp[NumPts];

int DateTimestamp;
char  Serial_Number[13];

void __fastcall TCalibrationForm::FormCreate(TObject *Sender)
{
   // fill out array
   Samp[0].SetEdit= Edit1;
   Samp[0].RefEdit= Edit2;
   Samp[0].PVLabel= Label1;
   Samp[0].SVLabel= Label2;
   Samp[0].ErrLabel[0]= Label21;
   Samp[0].ErrLabel[1]= Label22;
   Samp[0].ResLab
   */

