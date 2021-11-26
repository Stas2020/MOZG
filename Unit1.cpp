//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
    type_export = "city";
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
UnicodeString TForm1::ShieldingMes(UnicodeString mes)
{
	System::DynamicArray<int> position;
	int idx = 0;

	wchar_t CH = 0x0027;
	for (int i = 0; i < mes.Length(); i++)
	{
		if (mes[i+1] == CH)
		{
		   int len = idx+1;
		   position.set_length(len);
		   position[idx] = i+1;
           idx++;
		}
	}

	for (int i = 0; i < position.Length; i++)
	{
	  mes.Insert("'",position[i] + i);
	}
	return mes;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button4Click(TObject *Sender)
{
	ThreadQuery *tq = new ThreadQuery(true);
	tq->FreeOnTerminate = true;

	tq->StartDT = DateTimePicker1->Date;
	tq->EndDT = DateTimePicker2->Date;
	tq->Close = CheckBox1->Checked;
	tq->type_export = type_export;

	bool no_guest = false;
	TryStrToBool(str_no_guest,no_guest);
	tq->no_guest = no_guest;

	tq->Start();

	if (type_export == "aeroport")
	{
		RadioButton2->Checked = true;
	}
	else
	{
		RadioButton1->Checked = true;
    }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormCreate(TObject *Sender)
{
   DateTimePicker1->Date = Now();
   DateTimePicker2->Date = Now();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RadioButton1Click(TObject *Sender)
{
	TRadioButton *RadioButton = (TRadioButton *)Sender;
	if (RadioButton->Checked)
	{
	   type_export = "city";
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RadioButton2Click(TObject *Sender)
{
	TRadioButton *RadioButton = (TRadioButton *)Sender;
	if (RadioButton->Checked)
	{
	   type_export = "aeroport";
	}
}
//---------------------------------------------------------------------------

