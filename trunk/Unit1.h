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
   TEdit *TvEdit;
   TEdit *TrEdit;
   TEdit *TbEdit;
   TEdit *TviEdit;
   TEdit *Tv__IEdit;
   TEdit *TbrEdit;
   TEdit *TbiEdit;
   TCheckBox *Label12;
   TEdit *Tb__REdit;
   TCheckBox *Label13;
   TEdit *Tb__IEdit;
   TCheckBox *Label14;
   TCheckBox *Label15;
   TCheckBox *Label16;
   TCheckBox *Label17;
   TCheckBox *Label18;
   TEdit *TbbEdit;
   TEdit *TvvEdit;
   TEdit *TrrEdit;
   TEdit *TiiEdit;
   TEdit *rTiiEdit;
   TEdit *rTrrEdit;
   TEdit *rTvvEdit;
   TEdit *rTbbEdit;
   TEdit *rTrEdit;
   TEdit *rTv__IEdit;
   TEdit *rTvEdit;
   TEdit *rTb__IEdit;
   TEdit *rTb__REdit;
   TEdit *rTbEdit;
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
   TRadioButton *RadioButton4;
   TRadioButton *RadioButton5;
   TRadioButton *RadioButton6;
   TRadioButton *RadioButton7;
   TRadioButton *RadioButton8;
   TRadioButton *RadioButton9;
   TRadioButton *RadioButton10;
   TRadioButton *RadioButton11;
   TRadioButton *RadioButton12;
   TEdit *TbbConsEdit;
   TEdit *TvvConsEdit;
   TEdit *TrrConsEdit;
   TEdit *TiiConsEdit;
   TLabel *Label21;
   TLabel *Label22;
   TCheckBox *Label11;
   TCheckBox *Label1;
   TRadioButton *RadioButton13;
   void __fastcall FormCreate(TObject *Sender);
   void __fastcall ProcessButtonClick(TObject *Sender);
   void __fastcall NumericOnExit(TObject *Sender);
   void __fastcall ToClipboardButtonClick(TObject *Sender);
   void __fastcall Open1Click(TObject *Sender);
   void __fastcall Save1Click(TObject *Sender);
   void __fastcall Exit1Click(TObject *Sender);
   void __fastcall Saveuntransformedobsfile1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TForm1(TComponent* Owner);
   void __fastcall TForm1::ReadCoefficients(TObject *Sender);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
