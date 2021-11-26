//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include "Unit2.h"
#include "SQLServerUniProvider.hpp"
#include "Uni.hpp"
#include "UniProvider.hpp"
#include <Data.DB.hpp>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TEdit *Edit1;
	TButton *Button4;
	TMemo *Memo1;
	TDateTimePicker *DateTimePicker1;
	TDateTimePicker *DateTimePicker2;
	TCheckBox *CheckBox1;
	TRadioButton *RadioButton1;
	TRadioButton *RadioButton2;
	TSQLServerUniProvider *SQLServerUniProvider1;
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall RadioButton1Click(TObject *Sender);
	void __fastcall RadioButton2Click(TObject *Sender);
private:	// User declarations



public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
	UnicodeString ShieldingMes(UnicodeString mes);
	UnicodeString type_export;
	UnicodeString str_no_guest;


};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
