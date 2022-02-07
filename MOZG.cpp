//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "Unit1.h"
//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>
USEFORM("Unit1.cpp", Form1);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR arg, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		TStyleManager::TrySetStyle("Windows10");
		Application->CreateForm(__classid(TForm1), &Form1);

		AnsiString T = arg;
		if (!T.IsEmpty())
		{
			// ��������� ���������� ������
			// 1-� :  ���-�� ���� ��� ������� (1-�� �����)
			// 2-� :  ������� ��� ������� �����/��������
			// 3-� :  ������������ ������� � ���� � ����� ������ ���-�� ������
			// 4-� :  �������� ��� ��� ������� (����. 1-����� ���������� ���, 10-���������� �� 10 ���� �� �������)
			// 5-� :  ������������ costs
			char *separator = " ";
			UnicodeString str_count_day =  strtok(T.c_str(),separator);
			UnicodeString str_type_export =  strtok(NULL,separator);
			UnicodeString str_no_guest =  strtok(NULL,separator);
			UnicodeString str_crop_day =  strtok(NULL,separator);
			UnicodeString str_no_cost =  strtok(NULL,separator);



			int count_day = 0;
			TryStrToInt(str_count_day.Trim(),count_day);

			int crop_day = 0;
			TryStrToInt(str_crop_day.Trim(),crop_day);

			TDateTime dtStart = Now() - count_day;
			TDateTime dtEnd = Now() - crop_day;
			Form1->DateTimePicker1->DateTime = dtStart;
			Form1->DateTimePicker2->DateTime = dtEnd;
			Form1->type_export = str_type_export;
			Form1->str_no_guest = str_no_guest;
			Form1->str_no_cost = str_no_cost;
			Form1->Button4Click(NULL);

		}

		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
