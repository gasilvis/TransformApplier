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
   TGroupBox *GroupBox1;
   TLabel *Label1;
   TLabel *Label2;
   TLabel *Label3;
   TLabel *Label4;
   TLabel *Label5;
   TLabel *label9;
   TLabel *Label10;
   TEdit *TbvEdit;
   TEdit *TvrEdit;
   TEdit *TriEdit;
   TEdit *TvEdit;
   TEdit *TrEdit;
   TEdit *TbEdit;
   TEdit *TviEdit;
   TEdit *Tv__IEdit;
   TLabel *Label8;
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
