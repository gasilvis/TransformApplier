//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include "ElastFrm.hpp"
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include "AdPort.hpp"
#include "AdWnPort.hpp"
#include "OoMisc.hpp"
#include <ScktComp.hpp>
#include <NMHttp.hpp>
#include <Psock.hpp>
#include <HttpProt.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
   TPageControl *CoefPage;
   TTabSheet *TabSheet1;
   TMainMenu *MainMenu1;
   TMenuItem *File1;
   TElasticForm *ElasticForm1;
   TLabel *Label6;
   TMemo *Memo2;
   TMemo *Memo1;
   TButton *ProcessButton;
   TButton *ToClipboardButton;
   TLabel *Label7;
   TLabel *VersionLabel;
   TMenuItem *Open1;
   TMenuItem *Save1;
   TMenuItem *Exit1;
   TOpenDialog *OpenDialog1;
   TSaveDialog *SaveDialog1;
   TTabSheet *TabSheet2;
   TMemo *Memo3;
   TMenuItem *Saveuntransformedobsfile1;
   TTabSheet *Coefficients;
   TGroupBox *GroupBox1;
   TCheckBox *Label2;
   TCheckBox *Label3;
   TCheckBox *Label4;
   TCheckBox *Label5;
   TCheckBox *Label9;
   TCheckBox *Label10;
   TCheckBox *Label8;
   TEdit *TbvEdit;
   TEdit *TvrEdit;
   TEdit *TriEdit;
   TEdit *Tv_vrEdit;
   TEdit *Tr_riEdit;
   TEdit *Tb_bvEdit;
   TEdit *TviEdit;
   TEdit *Tv_viEdit;
   TEdit *TbrEdit;
   TEdit *TbiEdit;
   TCheckBox *Label12;
   TEdit *Tb_brEdit;
   TCheckBox *Label13;
   TEdit *Tb_biEdit;
   TCheckBox *Label14;
   TEdit *rTr_riEdit;
   TEdit *rTv_viEdit;
   TEdit *rTv_vrEdit;
   TEdit *rTb_biEdit;
   TEdit *rTb_brEdit;
   TEdit *rTb_bvEdit;
   TEdit *rTriEdit;
   TEdit *rTviEdit;
   TEdit *rTvrEdit;
   TEdit *rTbiEdit;
   TEdit *rTbrEdit;
   TEdit *rTbvEdit;
   TLabel *Label19;
   TLabel *Label20;
   TTabSheet *Report;
   TMemo *Memo4;
   TRadioGroup *RadioGroup1;
   TRadioButton *RadioButton1;
   TRadioButton *RadioButton2;
   TRadioButton *RadioButton3;
   TCheckBox *Label11;
   TCheckBox *Label1;
   TCheckBox *IncludeRaw;
   TCheckBox *CheckBox1;
   TCheckBox *CheckBox2;
   TCheckBox *CheckBox3;
   TEdit *Tv_bvEdit;
   TEdit *rTv_bvEdit;
   TEdit *rTr_bvEdit;
   TEdit *rTi_bvEdit;
   TEdit *Ti_bvEdit;
   TEdit *Tr_bvEdit;
   TButton *Button1;
   THttpCli *HttpCli1;
   TButton *Button2;
   TButton *Button3;
   TButton *Button4;
   TCheckBox *CheckBox9;
   TCheckBox *CheckBox10;
   TCheckBox *CheckBox11;
   TEdit *Tr_vrEdit;
   TEdit *rTr_vrEdit;
   TEdit *rTr_viEdit;
   TEdit *rTi_viEdit;
   TEdit *Ti_viEdit;
   TEdit *Tr_viEdit;
   TEdit *setupEdit;
   TLabel *Label17;
   TButton *changeINIfile;
   TButton *saveasINIfile;
   TButton *saveButton;
   TCheckBox *UseStdField;
   TGroupBox *GroupBox2;
   TEdit *rEuEdit;
   TEdit *rEiEdit;
   TEdit *rErEdit;
   TEdit *rEvEdit;
   TEdit *rEbEdit;
   TLabel *Label23;
   TEdit *EuEdit;
   TEdit *EiEdit;
   TEdit *ErEdit;
   TEdit *EvEdit;
   TEdit *EbEdit;
   TLabel *Label15;
   TCheckBox *applyExtinction;
   TCheckBox *CheckBox4;
   TCheckBox *CheckBox5;
   TCheckBox *CheckBox6;
   TCheckBox *CheckBox7;
   TCheckBox *CheckBox8;
   TLabel *Label24;
   TLabel *Label25;
   TLabel *Label26;
   TLabel *Label27;
   TLabel *Label28;
   TMenuItem *SaveReporttofile1;
   TCheckBox *AggregateCB;
   void __fastcall FormCreate(TObject *Sender);
   void __fastcall ProcessButtonClick(TObject *Sender);
   void __fastcall NumericOnExit(TObject *Sender);
   void __fastcall ToClipboardButtonClick(TObject *Sender);
   void __fastcall Open1Click(TObject *Sender);
   void __fastcall Save1Click(TObject *Sender);
   void __fastcall Exit1Click(TObject *Sender);
   void __fastcall Saveuntransformedobsfile1Click(TObject *Sender);
   void __fastcall Button1Click(TObject *Sender);
   void __fastcall HttpCli1DocBegin(TObject *Sender);
   void __fastcall HttpCli1DocEnd(TObject *Sender);
   void __fastcall Button2Click(TObject *Sender);
   void __fastcall Button3Click(TObject *Sender);
   void __fastcall Button4Click(TObject *Sender);
   void __fastcall setupEditExit(TObject *Sender);
   void __fastcall applyExtinctionExit(TObject *Sender);
   void __fastcall changeINIfileClick(TObject *Sender);
   void __fastcall saveasINIfileClick(TObject *Sender);
   void __fastcall UseStdFieldClick(TObject *Sender);
   void __fastcall CheckBox4Click(TObject *Sender);
   void __fastcall SaveReporttofile1Click(TObject *Sender);

private:	// User declarations
public:		// User declarations
   __fastcall TForm1(TComponent* Owner);
   void __fastcall TForm1::ReadCoefficients(TObject *Sender);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
