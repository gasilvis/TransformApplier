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



#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <IniFiles.hpp>
#include <stdio.h>

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

#define Version  1.3


/* adding a coefficient:
   1. add variable
   2. add screen label, edit box, tab sequence
   3. add INI read reference
   4. add to the output
   5. add to the documentation
   */
//  idea   { "Tbv", &Tbv, screen label, edit box}
float Tbv, Tvr, Tvi, Tri, Tv, Tr, Tb, Tv__I;



void __fastcall TForm1::ReadCoefficients(TObject *Sender)
{
   // collect INI file entries
   TIniFile *ini;
   ini = new TIniFile(ChangeFileExt( Application->ExeName, ".INI" ) );

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
      ini->WriteFloat("Coefficients", p->Name.SubString(1,  p->Name.Pos("Edit")-1), f);
      delete ini;
      ReadCoefficients(Sender);
   } catch (EConvertError &error) {
      p->SetFocus();
      ShowMessage("This field must be numeric");
   }
}


void __fastcall TForm1::FormCreate(TObject *Sender)
{
   VersionLabel->Caption= "Version "+ FormatFloat("0.0", Version);
   ReadCoefficients(Sender);
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
// add here
   ,FILTC_NUM
};

/* too add a filter combination"
1.  add name to enum of Filter combinations
2.  add mask value combination to enum of mask values
3.  add this mask value to the FILTC_mask[]
4.  add a description of the formula to FILTC_desc
5.  add a case to ProcessStarData
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
// add here
};


/*
const char* FILTC_names= {
    "BVIR"
   ,"BV"
   ,"VR"
};
*/

char FILTC_mask[]= {FILTC_BVIRx, FILTC_BVx, FILTC_VRx, FILTC_VRIx, FILTC_BVRx, FILTC_RIx, FILTC_VIx, FILTC_BVIx};

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
};


bool trans_display;
bool FILTC_display[FILTC_NUM];

//---------------------------------------------------------------------------
void __fastcall TForm1::ProcessButtonClick(TObject *Sender)
{
   char delim= ';';
   char fc, fci; // filter combo and index
   float crefmag= -999, x;
   int i, j, k, m, sdi= 0;
   AnsiString s, sx;

   // init
   // reset flags for displaying information in the output. Only want to display
   // the transforms and the formulas once in the output
   trans_display= false;
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
   for(i= 0; i< Memo1->Lines->Count; i++) {
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

         // capture the line
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
         k+= j;

         j= s.SubString(k, 20).Pos(delim);
         sd[sdi].KMAG= s.SubString(k, j- 1).ToDouble();
         k+= j;

         // if ensemble, the check star is K. Move it to C for processing
         if(sd[sdi].ensemble) {
            sd[sdi].CNAME = sd[sdi].KNAME;
            sd[sdi].CMAG= sd[sdi].KMAG;
         }

         // is crefmag available? might have seen it a while ago.
         // need to match filter and star name in the table so far.
         if(crefmag == -999) {
            for(int i= 0; i< sdi; i++) {
               if(sd[sdi].CNAME==sd[i].CNAME) {
                  if(sd[sdi].filter==sd[i].filter) {
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
         if(sd[sdi].GROUPs=="NA") {
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
            sd[sdi].VMAG+= sd[sdi].CMAG - crefmag;
         } else if(sd[sdi].MTYPE == 'D') { // "DIF"
            sd[sdi].VMAG+= sd[sdi].CMAG;
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
            if(cs!=NULL) { // do we have a star name?
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
      if(cs==NULL)
         break; // we're done

      // what is the filter combo for this star (cs)?
      for(j= 0, fc= 0; j< FILT_NUM; j++)
         if(sf[j]!=sdiMAX)
            fc|= FILT_mask[j];
      // process this stars obs
      for(i= 0; i< sdi; i++) {
         if((sd[i].NAME+ sd[i].GROUPs)==cs && sd[i].processed==false) {
            SetStarData(sd[i]);
            sf[sd[i].filter]= i; // must provide data for its own filter
            ProcessStarData(&sd[i], fc);
         }
      }
   } while(42);

   Memo2->Lines->Clear();
   for(i= 0, j= 0; i< Memo1->Lines->Count; i++) {
      s= Memo1->Lines->Strings[i];
      if(s[1]=='#') { // non-data line
         Memo2->Lines->Add(s); // just copy
      } else { // data line
         if(!sd[j].TRANS) { // not Transformed
            Memo2->Lines->Add(s); // just copy
         } else {  // display transformed data    ?? what if it was transformed in the input, thus skipped?
            // Standard info blocks been output once
            if(!trans_display) { // show the transform coefficients
               Memo2->Lines->Add("#   transform coefficients applied by Transformer, version "+ FormatFloat("0.0", Version));
               Memo2->Lines->Add("#             http://www.gasilvis.net/transformer/  ");
               Memo2->Lines->Add("#   transform coefficients:");
               Memo2->Lines->Add("#Tbv= "+ TbvEdit->Text);
               Memo2->Lines->Add("#Tvr= "+ TvrEdit->Text);
               Memo2->Lines->Add("#Tvi= "+ TviEdit->Text);
               Memo2->Lines->Add("#Tri= "+ TriEdit->Text);
               Memo2->Lines->Add("#Tv= " + TvEdit->Text);
               Memo2->Lines->Add("#Tr= " + TrEdit->Text);
               Memo2->Lines->Add("#Tb= " + TbEdit->Text);
               Memo2->Lines->Add("#Tv__I= " + Tv__IEdit->Text);
               trans_display= true;
            }
            for(k= 0; k< FILTC_NUM; k++) {
               if(sd[j].FILTC==FILTC_mask[k]) {
                   if(!FILTC_display[k]) {
                      for(m= 0; m<FILTC_desc_rows; m++) {
                         if(FILTC_desc[k][m]!= NULL)
                            Memo2->Lines->Add(FILTC_desc[k][m]);
                      }
                      FILTC_display[k]= true;
                   }
                }
             }

            // show un-transformed data
            Memo2->Lines->Add("#"+ s);

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
               s+= FormatFloat("0.000", sd[j].KMAG)+ delim;
            }
            s+= FormatFloat("0.0000", sd[j].AMASS)+ delim;

            //s+= IntToStr(sd[j].GROUP)+ delim;
            s+= sd[j].GROUPs+ delim;

            s+= sd[j].CHART+ delim;
            s+= sd[j].NOTES;

            Memo2->Lines->Add(s);

            // add info about the analysis
            Memo2->Lines->Add(sd[j].StarsUsed);

         }
         if(sd[j].ErrorMsg != "")
            ShowMessage(sd[j].ErrorMsg);

         j++;
      }
   }
}
//---------------------------------------------------------------------------
// global vars for equations
float Bs, bs, Vs, vs, Rs, rs, Is, is, Bc, bc, Vc, vc, Rc, rc, Ic, ic;

void SetStarData(StarData d)
{
   switch(d.filter) {
      case FILT_Bi: bs= d.VMAG; bc= d.CMAG; Bc= d.CREFMAG; break;
      case FILT_Vi: vs= d.VMAG; vc= d.CMAG; Vc= d.CREFMAG; break;
      case FILT_Ri: rs= d.VMAG; rc= d.CMAG; Rc= d.CREFMAG; break;
      case FILT_Ii: is= d.VMAG; ic= d.CMAG; Ic= d.CREFMAG; break;
   }
}

void ProcessStarData(StarData *d, char fc)
{
   d->TRANS= true;
   d->FILTC= fc;  // which combination used
   switch(fc) {
      case FILTC_BVIRx:
         if(Tr==0 || Tri==0 || Tv==0 || Tvr==0 || Tbv==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tr, Tri, Tv, Tvr and Tbv.";
            d->TRANS= false;
            break;
         }
         Rs = rs + (Rc - rc) + Tr * (((Rc - Ic) + Tri * ((rs - is) - (rc - ic))) - (Rc - Ic));
         Is = Rs - ((Rc - Ic) + Tri * ((rs - is) - (rc - ic)));
         Vs = vs + (Vc - vc) + Tv * Tvr * ((vs - rs) - (vc - rc));
         Bs = Vs + (Bc - Vc) + Tbv * ((bs - vs) - (bc - vc));
         d->StarsUsed= "# BVIR using: B @ "+ sd[sf[FILT_Bi]].DATEs
                                 + ", V @ "+ sd[sf[FILT_Vi]].DATEs
                                 + ", R @ "+ sd[sf[FILT_Ri]].DATEs
                                 + ", I @ "+ sd[sf[FILT_Ii]].DATEs;
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
         if(Tr==0 || Tri==0 || Tv==0 || Tvr==0) {
            d->ErrorMsg+= " Missing a coefficient; need Tr, Tri, Tv and Tvr.";
            d->TRANS= false;
            break;
         }
         Rs = rs + (Rc - rc) + Tr * (((Rc - Ic) + Tri * ((rs - is) - (rc - ic))) - (Rc - Ic));
         Is = Rs - ((Rc - Ic) + Tri * ((rs - is) - (rc - ic)));
         Vs = vs + (Vc - vc) + Tv * Tvr * ((vs - rs) - (vc - rc));
         d->StarsUsed= "# VRI using: V @ "+ sd[sf[FILT_Vi]].DATEs
                                + ", R @ "+ sd[sf[FILT_Ri]].DATEs;
                                + ", I @ "+ sd[sf[FILT_Ii]].DATEs;
         break;
      case FILTC_BVRx:
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
