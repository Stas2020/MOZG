//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop
#include "Unit2.h"
#pragma package(smart_init)
#pragma link "Uni"


//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall ThreadQuery::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall ThreadQuery::ThreadQuery(bool CreateSuspended)
	: TThread(CreateSuspended)
{
	list_order = new TListOrder();
	ListPartionRatio = new TStringList();

	List_pbnd = new TList();
	List_pmnt = new TList();

	list_group_menu = new TStringList();

}
//---------------------------------------------------------------------------
UnicodeString ThreadQuery::GenerateUID(void)
{
  UnicodeString Resilt__ ;
  TGUID aGUID;
  CreateGUID(aGUID);

  Resilt__ = System::Sysutils::GUIDToString(aGUID);
  Resilt__ = Resilt__.SubString(2,Resilt__.Length() - 2);

  return Resilt__;

}
//---------------------------------------------------------------------------
void __fastcall ThreadQuery::Execute()
{

	try
	{
		message = "connect to SQL ";
		Synchronize(&UpdateCaption2);

/**/

		if (ConnectSQL1() == false)
		{
			return;
		}



		if (ConnectSQL2() == false)
		{
			return;
		}


		if (type_export == "aeroport")
		{
			rest_aeroport();// Рестораны
			writeoff_reason2(); //справочник причин списания 1
			writeoff();		// Списания
			cot_aeroport(); // Типы заказа
			hall();         // Залы
			order();  		// Заказы
			pbnd();  		// Проводки
			pmnt(); 		// Платежи
			rcpt();      	// Чеки
			cost();         //Себестоимость
			group(); 		// Группы меню
			item(); 		// Позиции меню
			empl(); 		// Сотрудники
			table_aeroport();// Столы
			item_size();	// Приказы (типы скидок)
			curr();  		// Методы оплат
		}
		else
		{
			rest();         // Рестораны
			writeoff_reason2(); //справочник причин списания 1
			writeoff();		// Списания
			GetAllGuestFromDb();
			cot();          // Типы заказа
			hall();         // Залы
			order();  		// Заказы
			pbnd();  		// Проводки
			pmnt(); 		// Платежи
			rcpt();      	// Чеки
			cost();         //Себестоимость
			group(); 		// Группы меню
			item(); 		// Позиции меню
			empl(); 		// Сотрудники
			table(); 		// Столы
			item_size();	// Приказы (типы скидок)
			curr();  		// Методы оплат
		}


		if (!Close)
		{
		   Synchronize(&exit);
		}


	}
	catch (Exception &exception)
	{
		message = exception.Message;
		Synchronize(&UpdateCaption2);
	}

}
//---------------------------------------------------------------------------
void __fastcall ThreadQuery::exit() //exit
{
  Form1->Close();
}
//---------------------------------------------------------------------------
bool ThreadQuery::DateTimeInRange_(TDateTime DT, TDateTime Start, TDateTime End)
{
	//return DateTimeInRange(DT,Start,End);

	bool result = false;
	if (DateTimeInRange(DT,Start,End))
	{
		int count_minute = MinutesBetween(DT,End);

		if(count_minute <= 15)
		{
			result = false;
		}
		else
		{
		   result = true;
		}

	}
	return result;
}
//---------------------------------------------------------------------------
TDateTime ThreadQuery::ConvertToDateTime(TDateTime DT, UnicodeString Time)
{
	return StrToDateTime(DT.FormatString("dd.MM.YY")+ " " + Time);
}
//---------------------------------------------------------------------------
TDateTime ThreadQuery::AddTime(TDateTime DT,UnicodeString Time, int Interval)
{
	TDateTime  DTtime = ConvertToDateTime(DT, Time);
	DTtime=IncMinute(DTtime,-1 * Interval);

	return DTtime;
}
//---------------------------------------------------------------------------
int ThreadQuery::GetIntervalDateTime(TDateTime Start, TDateTime End)
{
	return MinutesBetween(Start,End);
}
//---------------------------------------------------------------------------
void ThreadQuery::AddCheckToOrderList(TCheck *check)
{
	//message = "In check, num_check: "+IntToStr(check->NumCheck) ;
	//Synchronize(&UpdateCaption2);

	bool flag = true;
	for (TListOrder::reverse_iterator it_order = list_order->rbegin(); it_order != list_order->rend(); ++it_order)  //переюор с конца
	{
		if (check->NumTable == (*it_order)->NumTable && check->NumShop == (*it_order)->NumShop && DateTimeInRange_(check->Start,	(*it_order)->Start, (*it_order)->End) && check->BusinessDate == (*it_order)->BusinessDate)
		{
			if(check->End > (*it_order)->End)
			{
			   (*it_order)->End = check->End;
			}

			(*it_order)->SumPaymentOrder += check->SumPayment;
			(*it_order)->NumLastCheck = check->NumCheck;

			(*it_order)->list_check->push_back(check);

			//message = "Add check to order, num_order: "+IntToStr(it_order->NumFirstCheck)+" , num_check: "+IntToStr(check->NumCheck) ;
			//Synchronize(&UpdateCaption2);

			flag = false;
			break;

		}

		if (check->NumTable == (*it_order)->NumTable && check->NumShop == (*it_order)->NumShop && check->Start > (*it_order)->End)
		{
			break;
		}

	}


	if (list_order->size() == 0 || flag)
	{
		TOrder *order = new TOrder();
		order->NumTable = check->NumTable;
		order->NumFirstCheck = check->NumCheck;
		order->NumShop = check->NumShop;
		order->CountGuest = check->CountGuest;
		order->idWaiter = check->idWaiter;
		order->Start = check->Start;
		order->End = check->End;
		order->BusinessDate = check->BusinessDate;
		order->SumPaymentOrder = check->SumPayment;
		order->NumLastCheck = check->NumCheck;
		order->TypeOrder = check->TypeOrder;

		order->list_check = new TListCheck();
		order->list_check->push_back(check);

		//message = "Create order, num_order: "+IntToStr(check->NumCheck) ;
		//Synchronize(&UpdateCaption2);
		//message = "Add check to order, num_check: "+IntToStr(check->NumCheck) ;
		//Synchronize(&UpdateCaption2);

		list_order->push_back(order);
	}
}
//---------------------------------------------------------------------------
void ThreadQuery::AddCheckToOrderListAeroport(TCheck *check)
{
	//message = "In check, num_check: "+IntToStr(check->NumCheck) ;
	//Synchronize(&UpdateCaption2);



	TOrder *order = new TOrder();
	order->NumTable = check->NumTable;
	order->NumOrder =  check->cas_n + check->NumCheck;
	order->NumFirstCheck = check->NumCheck;
	order->NumShop = check->NumShop;
	order->CountGuest = check->CountGuest;
	order->idWaiter = check->idWaiter;
	order->Start = check->Start;
	order->End = check->End;
	order->BusinessDate = check->BusinessDate;
	order->SumPaymentOrder = check->SumPayment;
	order->NumLastCheck = check->NumCheck;
	order->TypeOrder = check->TypeOrder;

	order->list_check = new TListCheck();
	order->list_check->push_back(check);

	//message = "Create order, num_order: "+IntToStr(check->NumCheck) ;
	//Synchronize(&UpdateCaption2);
	//message = "Add check to order, num_check: "+IntToStr(check->NumCheck) ;
	//Synchronize(&UpdateCaption2);

	list_order->push_back(order);

}
//---------------------------------------------------------------------------
void ThreadQuery::AddCheckToOrderListTabId(TCheck *check)
{
	//message = "In check, num_check: "+IntToStr(check->NumCheck) ;
	//Synchronize(&UpdateCaption2);

	bool flag = true;
	for (TListOrder::reverse_iterator it_order = list_order->rbegin(); it_order != list_order->rend(); ++it_order)  //переюор с конца
	{
		if (check->NumTable == (*it_order)->NumTable && check->NumShop == (*it_order)->NumShop && check->TableId == (*it_order)->TableId && check->BusinessDate == (*it_order)->BusinessDate)
		{
			if(check->End > (*it_order)->End)
			{
			   (*it_order)->End = check->End;
			}

			(*it_order)->SumPaymentOrder += check->SumPayment;
			(*it_order)->NumLastCheck = check->NumCheck;

			(*it_order)->list_check->push_back(check);

			//message = "Add check to order, num_order: "+IntToStr(it_order->NumFirstCheck)+" , num_check: "+IntToStr(check->NumCheck) ;
			//Synchronize(&UpdateCaption2);

			flag = false;
			break;

		}

		if (check->NumTable == (*it_order)->NumTable && check->NumShop == (*it_order)->NumShop && check->Start > (*it_order)->End)
		{
			break;
		}

	}


	if (list_order->size() == 0 || flag)
	{
		TOrder *order = new TOrder();
		order->NumTable = check->NumTable;
		order->NumFirstCheck = check->NumCheck;
		order->NumShop = check->NumShop;
		order->CountGuest = check->CountGuest;
		order->idWaiter = check->idWaiter;
		order->Start = check->Start;
		order->End = check->End;
		order->BusinessDate = check->BusinessDate;
		order->SumPaymentOrder = check->SumPayment;
		order->NumLastCheck = check->NumCheck;
		order->TypeOrder = check->TypeOrder;
		order->TableId = check->TableId;

		order->list_check = new TListCheck();
		order->list_check->push_back(check);

		//message = "Create order, num_order: "+IntToStr(check->NumCheck) ;
		//Synchronize(&UpdateCaption2);
		//message = "Add check to order, num_check: "+IntToStr(check->NumCheck) ;
		//Synchronize(&UpdateCaption2);

		list_order->push_back(order);
	}
}
//---------------------------------------------------------------------------
void ThreadQuery::SetTypeOrder(TCheck *check)
{
	if ((check->NumTable >= 231 && check->NumTable <= 240) || (check->NumTable >= 900 && check->NumTable <= 909))
	{
	   check->TypeOrder = 1;  ////Яндекс еда
	   return;
	}
	if (check->NumTable >= 169 && check->NumTable <= 173)
	{
	   check->TypeOrder = 1;  ////Яндекс еда
	   return;
	}
	if (check->NumTable >= 200 && check->NumTable <= 207)
	{
	   check->TypeOrder = 2;  ////Delivery club
	   return;
	}
	if (check->NumTable >= 208 && check->NumTable <= 209)
	{
	   check->TypeOrder = 2;  ////Delivery club самовывоз
	   return;
	}
	if (check->NumTable >= 245 && check->NumTable <= 245)
	{
	   check->TypeOrder = 3;  ////Поднеси
	   return;
	}

	if (check->id_discount == 2)
	{
	   check->TypeOrder = 5; //Персонал
	   return;
	}

    //-----------NEW---------------------------------
	if (check->NumTable >= 177 && check->NumTable <= 179)
	{
	   check->TypeOrder = 7;  ////Домиленд
	   return;
	}
	if (check->NumTable >= 174 && check->NumTable <= 176)
	{
	   check->TypeOrder = 8;  ////Myply
	   return;
	}
	if (check->NumTable >= 163 && check->NumTable <= 168)
	{
	   check->TypeOrder = 9;  ////Яндекс-еда самовынос
	   return;
	}
	if (check->NumTable >= 180 && check->NumTable <= 184)
	{
	   check->TypeOrder = 10;  ////Доставка с сайта  Пешком
	   return;
	}
	if ((check->NumTable >= 185 && check->NumTable <= 195)
	 || (check->NumTable >= 920 && check->NumTable <= 929)
	 || (check->NumTable >= 951 && check->NumTable <= 960))
	{
	   check->TypeOrder = 11;  ////Доставка с сайта  Автомобиль
	   return;
	}
	if ((check->NumTable >= 196 && check->NumTable <= 199)
	  ||(check->NumTable >= 930 && check->NumTable <= 934)
	  ||(check->NumTable >= 935 && check->NumTable <= 940))
	{
	   check->TypeOrder = 12;  ////Доставка с сайта  Самовынос
	   return;
	}
	if (check->NumTable >= 221 && check->NumTable <= 224)
	{
	   check->TypeOrder = 13;  ////Звонок гостя, Доставка пешком
	   return;
	}
	if (check->NumTable >= 215 && check->NumTable <= 220)
	{
	   check->TypeOrder = 14;  ////Звонок гостя, Доставка авто
	   return;
	}
	if (check->NumTable >= 146 && check->NumTable <= 150)
	{
	   check->TypeOrder = 15;  ////Звонок гостя, Самовынос
	   return;
	}
	if (check->NumTable >= 210 && check->NumTable <= 213)
	{
	   check->TypeOrder = 16;  //WA, Доставка пешком
	   return;
	}
	if (check->NumTable >= 225 && check->NumTable <= 230)
	{
	   check->TypeOrder = 17;  //WA, Доставка авто
	   return;
	}
	if (check->NumTable >= 151 && check->NumTable <= 155)
	{
	   check->TypeOrder = 18;  //WA, Самовынос
	   return;
	}
	if (check->NumTable >= 156 && check->NumTable <= 160)
	{
	   check->TypeOrder = 19;  //Сам пришел, сам унес
	   return;
	}
	if (check->NumTable >= 214 && check->NumTable <= 214)
	{
	   check->TypeOrder = 19;  //Сам пришел, сам унес
	   return;
	}
	if (check->NumTable >= 241 && check->NumTable <= 244)
	{
	   check->TypeOrder = 19;  //Сам пришел, сам унес
	   return;
	}
	if (check->NumTable >= 251 && check->NumTable <= 254)
	{
	   check->TypeOrder = 20;  //Кофе с собой
	   return;
	}
	if (check->NumTable >= 246 && check->NumTable <= 248)
	{
	   check->TypeOrder = 21;  ////старый тип ASAP Новый Кофе по пути
	   return;
	}

	//if (check->NumTable >= 920 && check->NumTable <= 929)
	//{
	//   check->TypeOrder = 23;  //Интернет магазин (Филиас-онлайн)
	//   return;
	//}

	if (check->NumTable >= 941 && check->NumTable <= 944)   // Added 08.02
	{
	   check->TypeOrder = 25;  ////мир сити
	   return;
	}

	if (check->NumTable >= 945 && check->NumTable <= 950)   // Added 08.02
	{
	   check->TypeOrder = 27;  ////Бронибой
	   return;
	}

	//Только для Весны
	if (check->NumShop == 270 && (check->NumTable >= 72 && check->NumTable <= 78))
	{
		check->TypeOrder = 24;  //Интернет магазин (Филиас-онлайн)
		return;
	}

	check->TypeOrder = 6;  //Остальные продажи
}
//---------------------------------------------------------------------------
void ThreadQuery::cot()
{
	message = "_____________________Start cot (Типы заказа)";
	Synchronize(&UpdateCaption2);

	list_typeorder = new TListTypeOrder();
	TTypeOrder *typeorder;


	typeorder = new TTypeOrder();
	typeorder->id = 1;
	typeorder->Name = "Яндекс еда";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 2;
	typeorder->Name = "Delivery club";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 3;
	typeorder->Name = "Поднеси";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 5;
	typeorder->Name = "Персонал";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 6;
	typeorder->Name = "Остальные продажи";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 7;
	typeorder->Name = "Домиленд";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 8;
	typeorder->Name = "Myply";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 9;
	typeorder->Name = "Яндекс-еда самовынос";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 10;
	typeorder->Name = "Доставка с сайта  Пешком";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 11;
	typeorder->Name = "Доставка с сайта  Автомобиль";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 12;
	typeorder->Name = "Доставка с сайта  Самовынос";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 13;
	typeorder->Name = "Звонок гостя, Доставка пешком";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 14;
	typeorder->Name = "Звонок гостя, Доставка авто";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 15;
	typeorder->Name = "Звонок гостя, Самовынос";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 16;
	typeorder->Name = "WA, Доставка пешком";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 17;
	typeorder->Name = "WA, Доставка авто";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 18;
	typeorder->Name = "WA, Самовынос";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 19;
	typeorder->Name = "Сам пришел, сам унес";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 20;
	typeorder->Name = "Кофе с собой";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 21;
	typeorder->Name = "Кофе по пути";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 22;
	typeorder->Name = "Веранда";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 23;
	typeorder->Name = "Интернет магазин (Филиас-онлайн)";
	list_typeorder->push_back(typeorder);

 	typeorder = new TTypeOrder();
	typeorder->id = 24;
	typeorder->Name = "Бар";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 25;
	typeorder->Name = "мир сити";
	list_typeorder->push_back(typeorder);


	typeorder = new TTypeOrder();
	typeorder->id = 26;
	typeorder->Name = "Без Тарелок";
	list_typeorder->push_back(typeorder);


	typeorder = new TTypeOrder();
	typeorder->id = 26;
	typeorder->Name = "Без Тарелок";
	list_typeorder->push_back(typeorder);

	// Added 08.02
	typeorder = new TTypeOrder();
	typeorder->id = 27;
	typeorder->Name = "Бронибой";
	list_typeorder->push_back(typeorder);
	// End of Added 08.02


	message = "write file cot";
	Synchronize(&UpdateCaption2);

	for (TListTypeOrder::iterator it_typeorder = list_typeorder->begin(); it_typeorder != list_typeorder->end(); ++it_typeorder)
	{
		UnicodeString msg = "1;"+   						   					//id базы данных
							IntToStr(it_typeorder.operator *()->id)+";"+     	//id типа заказа
							it_typeorder.operator *()->Name+";"+     	 		//название типа заказа
							"3";   												//статус
		WriteToFile("cot.csv",msg);
	}

	message = "End cot (Типы заказа)";
	Synchronize(&UpdateCaption2);

}
//---------------------------------------------------------------------------
void ThreadQuery::cot_aeroport()
{
	message = "_____________________Start cot (Типы заказа)";
	Synchronize(&UpdateCaption2);

	list_typeorder = new TListTypeOrder();
	TTypeOrder *typeorder;


	typeorder = new TTypeOrder();
	typeorder->id = 5;
	typeorder->Name = "Персонал";
	list_typeorder->push_back(typeorder);

	typeorder = new TTypeOrder();
	typeorder->id = 6;
	typeorder->Name = "Остальные продажи";
	list_typeorder->push_back(typeorder);


	message = "write file cot";
	Synchronize(&UpdateCaption2);

	for (TListTypeOrder::iterator it_typeorder = list_typeorder->begin(); it_typeorder != list_typeorder->end(); ++it_typeorder)
	{
		UnicodeString msg = "1;"+   						   					//id базы данных
							IntToStr(it_typeorder.operator *()->id)+";"+     	//id типа заказа
							it_typeorder.operator *()->Name+";"+     	 		//название типа заказа
							"3";   												//статус
		WriteToFile("cot.csv",msg);
	}

	message = "End cot (Типы заказа)";
	Synchronize(&UpdateCaption2);

}
//---------------------------------------------------------------------------
void ThreadQuery::rcpt()
{
	message = "_____________________Start rcpt (Чеки)";
	Synchronize(&UpdateCaption2);

	UniQuery1->SQL->Clear();
	UniQuery1->SQL->Add("SELECT cas_n, dat_chek, cod_shop, stol, spoolname, timer, [Action] AS action, sum_b, chek_sn FROM chekA WHERE (" + GetListShop() + ") AND business_date >= convert(datetime,'"+StartDT.FormatString("DD/MM/YYYY")+"',104) AND business_date <= convert(datetime,'"+EndDT.FormatString("DD/MM/YYYY")+"',104)");
	UniQuery1->Execute();

	CountRecord = UniQuery1->RecordCount;
	message = "Count: " + IntToStr(CountRecord);
	Synchronize(&UpdateCaption2);
	double summ = 0;

	if (!UniQuery1->IsEmpty())
	{

		message = "write file";
		Synchronize(&UpdateCaption2);

		UnicodeString msg;
		while (!UniQuery1->Eof)
		{
			CountRecord--;
			Synchronize(&UpdateCaptionDelay);
			msg = "";
			msg += "1;"; 																						//id базы данных
			msg +=UniQuery1->FieldByName("cas_n")->AsString+UniQuery1->FieldByName("chek_sn")->AsString+";";   	//id визита
			msg +=ConvertShopNum(UniQuery1->FieldByName("cod_shop")->AsString)+";"; 						   	//id ресторана
			msg +=UniQuery1->FieldByName("chek_sn")->AsString+";";   											//id чека внутри визита
			msg +="0;";   																						//id заказа
			msg +=AddDateTime(UniQuery1->FieldByName("dat_chek")->AsString, UniQuery1->FieldByName("timer")->AsString)+";";   //Дата время закрытия чека
			msg +=UniQuery1->FieldByName("chek_sn")->AsString+";";   											//номер чека
			msg +="0;";   													   									//чек удален
			msg +=UniQuery1->FieldByName("sum_b")->AsString+";";      											//сумма оплачено
			msg +=UniQuery1->FieldByName("sum_b")->AsString+";";   												//сумма по меню
			msg += "";                                                  										//наценка за обслуживание


			summ += UniQuery1->FieldByName("sum_b")->AsFloat;

			WriteToFileDelayed("rcpt.csv",msg);
			UniQuery1->Next();
		}

		Synchronize(&UpdateCaption);

		message = "End write file";
		Synchronize(&UpdateCaption2);

	}

	WriteToFileDelayed_WriteBufferLeftOvers();

	message = "Summ : " + FormatFloat("0.##",summ) ;
	Synchronize(&UpdateCaption2);

	message = "End rcpt (Чеки)";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
void ThreadQuery::GetAllGuestFromDb()
{

	message = "_____________________Get count all guest";
	Synchronize(&UpdateCaption2);

	if (no_guest)
	{
		message = "Количество гостей отключено";
		Synchronize(&UpdateCaption2);
		return;
	}


	// Максимальное количество дней для запроса, чтобы кубы не зависли
	// Статистика зависаний
	// с 26.11.2021 - значение = 3 дня   Зависло раз:
	int MaxDayCount = 3;     // из-за получения значений "не целым куском" происх некорр обраб в дальнейшем в итераторе

	TDateTime StartDTFrag = IncDay(StartDT, 0);
	TDateTime EndDTFrag = IncDay(StartDTFrag, MaxDayCount);
		if(EndDTFrag >= EndDT)
			EndDTFrag = IncDay(EndDT, 0);

	bool done = false;

	ListDateTime = new TListDateTime();  // перемещено снизу

	while(!done)
	{

	// debug
	message = "CUBE request executing. Dates:   "+StartDTFrag.FormatString("DD/MM/YYYY")+" - "+EndDTFrag.FormatString("DD/MM/YYYY");
	Synchronize(&UpdateCaption2NoLog);
	// debug


	UniQuery2->SQL->Clear();
	UniQuery2->SQL->Add("SELECT Guests, TableId, Depnum, BusinessDate FROM [Diogen].[dbo].[GuestCount]");
	UniQuery2->SQL->Add("WHERE ("+GetListShop("Depnum")+") AND BusinessDate >= convert(datetime,'"+StartDTFrag.FormatString("DD/MM/YYYY")+"',104) AND BusinessDate <= convert(datetime,'"+EndDTFrag.FormatString("DD/MM/YYYY")+"',104)");
	UniQuery2->SQL->Add("ORDER BY BusinessDate, Depnum"); //Сортировка не нужна ???
	UniQuery2->Execute();

	CountRecord = UniQuery2->RecordCount;
	message = "Count record: " + IntToStr(CountRecord);
	Synchronize(&UpdateCaption2NoLog);

	// ListDateTime = new TListDateTime(); Перемещено вверх


	if (true) {

	}

	int CountResult = 0;
	while (!UniQuery2->Eof)
	{
		int CountGuests = abs(UniQuery2->FieldByName("Guests")->AsInteger);
		int TableId = UniQuery2->FieldByName("TableId")->AsInteger;
		int Depnum = UniQuery2->FieldByName("Depnum")->AsInteger;
		TDateTime BusinessDate = UniQuery2->FieldByName("BusinessDate")->AsDateTime;

		if (CountGuests > 20 || CountGuests == 0 || CountGuests < 0)
		{
            CountGuests = 1;
		}



		TListDateTime::iterator it =  ListDateTime->find(BusinessDate);

		if (it == ListDateTime->end())
		{
		   TListGuestOfTebleId *ListGuestOfTebleId  = new TListGuestOfTebleId();
		   ListGuestOfTebleId->insert(std::pair <int,int>(TableId,CountGuests));

		   if (CountGuests > 1) {
			  CountResult++;
		   }

		   TListDepNum *ListDepNum = new TListDepNum();
		   ListDepNum->insert(std::pair <int,TListGuestOfTebleId*>(Depnum,ListGuestOfTebleId));

		   ListDateTime->insert(std::pair <TDateTime,TListDepNum*>(BusinessDate,ListDepNum));
		}
		else
		{
		   TListDepNum *ListDepNum =  ListDateTime->operator [](BusinessDate);

		   TListDepNum::iterator itdn =  ListDepNum->find(Depnum);
		   if (itdn == ListDepNum->end())
		   {
				TListGuestOfTebleId *ListGuestOfTebleId  = new TListGuestOfTebleId();
				ListGuestOfTebleId->insert(std::pair <int,int>(TableId,CountGuests));
				if (CountGuests > 1) {
					CountResult++;
				}
				ListDepNum->insert(std::pair <int,TListGuestOfTebleId*>(Depnum,ListGuestOfTebleId));

		   }
		   else
		   {
			   TListGuestOfTebleId *ListGuestOfTebleId =  ListDepNum->operator [](Depnum);

			   TListGuestOfTebleId::iterator itti =  ListGuestOfTebleId->find(TableId);

				if (itti == ListGuestOfTebleId->end())
				{
					ListGuestOfTebleId->insert(std::pair <int,int>(TableId,CountGuests));
					if (CountGuests > 1) {
						CountResult++;
					}
				}
		   }
		}


		CountRecord--;
		Synchronize(&UpdateCaptionDelay);

		UniQuery2->Next();
	}

	Synchronize(&UpdateCaption);

	message = "CountResult > 2:    " + IntToStr(CountResult);
	Synchronize(&UpdateCaption2NoLog);


	if(EndDTFrag >= EndDT)
	{
		done = true;
	}
	else
	{
		StartDTFrag = IncDay(EndDTFrag, 1);
		EndDTFrag = IncDay(StartDTFrag, MaxDayCount);
		if(EndDTFrag >= EndDT)
			EndDTFrag = IncDay(EndDT, 0);
	}



	} // End of DATETIME fragment cycle

	message = "_____________________END count all guest";
	Synchronize(&UpdateCaption2);

}
//---------------------------------------------------------------------------
int ThreadQuery::GetCountGuest(TDateTime BusinessDate, int DepNum, int TableId)
{
	int CountGuest = 1;
	TListDateTime::iterator it =  ListDateTime->find(BusinessDate);

	if (it != ListDateTime->end())
	{
		TListDepNum *ListDepNum =  ListDateTime->operator [](BusinessDate);

		TListDepNum::iterator itdn =  ListDepNum->find(DepNum);
		if (itdn != ListDepNum->end())
		{
			TListGuestOfTebleId *ListGuestOfTebleId =  ListDepNum->operator [](DepNum);

			TListGuestOfTebleId::iterator itti =  ListGuestOfTebleId->find(TableId);
			if (itti != ListGuestOfTebleId->end())
			{
				CountGuest = itti->second;

			}
		}
	}

	return  CountGuest;

}
//---------------------------------------------------------------------------
int ThreadQuery::GetCountGuestAeroport(TDateTime BusinessDate, int DepNum, int TableId)
{
	int CountGuest = 1;
	return  CountGuest;

}
//---------------------------------------------------------------------------
UnicodeString ThreadQuery::ConvertShopNum(int value)
{
	if (value == 191)
	{
		return "190";
	}
	if (value == 121)
	{
		return "111";
	}
	if (value == 123)
	{
		return "122";
	}
	if (value == 236)
	{
		return "235";
	}
	if (value == 196)
	{
		return "195";
	}
	if (value == 239)
	{
		return "237";
	}
	if (value == 114)
	{
		return "111";
	}
	if (value == 124)
	{
		return "104";
	}
	return IntToStr(value);
}
//---------------------------------------------------------------------------
UnicodeString ThreadQuery::ConvertShopNum(UnicodeString value)
{
	if (value == "191")
	{
		return "190";
	}
	if (value == "121")
	{
		return "111";
	}
	if (value == "123")
	{
		return "122";
	}
	if (value == "236")
	{
		return "235";
	}
	if (value == "114")
	{
		return "111";
	}
	if (value == "124")
	{
		return "104";
	}
	return value;

}
//---------------------------------------------------------------------------
void ThreadQuery::order()
{
	message = "_____________________Start query order (Заказы)";
	Synchronize(&UpdateCaption2);


	int MaxMonthCount = 4; // Если загрузка более чем за меся, фрагментировать

	TDateTime StartDTFrag = IncDay(StartDT, 0);
	TDateTime EndDTFrag = IncMonth(StartDTFrag, MaxMonthCount);
		if(EndDTFrag >= EndDT)
			EndDTFrag = IncDay(EndDT, 0);

	bool done = false;



	//int CountRecordLast = 0;

	while(!done)
	{
		// debug
	message = "SQL request executing. Dates:   "+StartDTFrag.FormatString("DD/MM/YYYY")+" - "+EndDTFrag.FormatString("DD/MM/YYYY");
	Synchronize(&UpdateCaption2);
	// debug


	UniQuery1->SQL->Clear();
	UniQuery1->SQL->Add("SELECT saleman, cod_shop, stol, spoolname, timer, [Action] AS action, business_date, sum_b, chek_sn, dat_chek, dk_dcod, cas_n, comment");
	UniQuery1->SQL->Add("FROM chekA");
	UniQuery1->SQL->Add("WHERE sum_b <> 0 AND ( "+GetListShop()+") AND business_date >= convert(datetime,'"+StartDTFrag.FormatString("DD/MM/YYYY")+"',104) AND business_date <= convert(datetime,'"+EndDTFrag.FormatString("DD/MM/YYYY")+"',104)");
	UniQuery1->SQL->Add("ORDER BY dat_chek, timer");
	UniQuery1->Execute();

	CountRecord = UniQuery1->RecordCount;
	message = "Count checks in DB: " + IntToStr(CountRecord);
	Synchronize(&UpdateCaption2);

	int CountGuestResult = 0;

	//CountRecordLast = list_order->size();


	if (!UniQuery1->IsEmpty())
	{
		message = "prepare data";
		Synchronize(&UpdateCaption2);

		while (!UniQuery1->Eof)
		{
			CountRecord--;
			Synchronize(&UpdateCaptionDelay);

			TCheck *check = new TCheck();

			if (type_export == "aeroport")
			{
				check->NumTable = 1;
			}
			else
			{
				check->NumTable = UniQuery1->FieldByName("stol")->AsInteger;
			}

			check->NumShop = UniQuery1->FieldByName("cod_shop")->AsInteger;
			check->idWaiter = UniQuery1->FieldByName("saleman")->AsInteger;
			check->Start = AddTime(UniQuery1->FieldByName("dat_chek")->AsDateTime,UniQuery1->FieldByName("timer")->AsString, UniQuery1->FieldByName("action")->AsInteger);
			check->End = ConvertToDateTime(UniQuery1->FieldByName("dat_chek")->AsDateTime,UniQuery1->FieldByName("timer")->AsString);
			check->NumCheck = UniQuery1->FieldByName("chek_sn")->AsWideString;
			check->BusinessDate = UniQuery1->FieldByName("business_date")->AsDateTime;
			check->SumPayment = UniQuery1->FieldByName("sum_b")->AsFloat;
			check->id_discount = UniQuery1->FieldByName("dk_dcod")->AsInteger;
			check->cas_n = UniQuery1->FieldByName("cas_n")->AsWideString;

			UnicodeString TableId = UniQuery1->FieldByName("comment")->AsWideString;

			if (TableId != "")
			{
				int table_int = 0;
				TryStrToInt(TableId,table_int);
				check->TableId = table_int;
			}
			else
			{
				TableId = 0;
			}


			if (type_export == "aeroport")
			{
				check->CountGuest = GetCountGuestAeroport(check->BusinessDate, check->NumShop, check->TableId);
			}
			else
			{
				check->CountGuest = GetCountGuest(check->BusinessDate, check->NumShop, check->TableId);
			}

			if (check->CountGuest > 1)
			{
				CountGuestResult++;
				//message = "CountGuest: " + IntToStr(check->CountGuest) + " NumShop: " + IntToStr(check->NumShop) + " TableId: " + IntToStr(check->TableId);
				//Synchronize(&UpdateCaption2);
			}

			SetTypeOrder(check);

			if (type_export == "aeroport")
			{
				AddCheckToOrderListAeroport(check);
			}
			else
			{
				//AddCheckToOrderList(check);   Не нужно чтобы чеки обединялись в один заказ
				AddCheckToOrderListAeroport(check);
			}

			//AddCheckToOrderListTabId(check);

			UniQuery1->Next();
		}

		Synchronize(&UpdateCaption);

		message = "End prepare data";
		Synchronize(&UpdateCaption2);

		CountRecord = list_order->size();
		message = "Count order: " + IntToStr(CountRecord);
		Synchronize(&UpdateCaption2);


		message = "CountGuest > 1:  " + IntToStr(CountGuestResult);
		Synchronize(&UpdateCaption2);


	if(EndDTFrag >= EndDT)
	{
		done = true;
	}
	else
	{
		StartDTFrag = IncDay(EndDTFrag, 1);
		EndDTFrag = IncMonth(StartDTFrag, MaxMonthCount);
		if(EndDTFrag >= EndDT)
			EndDTFrag = IncDay(EndDT, 0);
	}



	} // End of DATETIME fragment cycle



	}


			UnicodeString msg = "";
		for (TListOrder::iterator it_order = list_order->begin(); it_order != list_order->end(); ++it_order)
		{
			CountRecord--;

			Synchronize(&UpdateCaptionDelay);

			msg = "1;";   																//1 id базы данных
			msg += (*it_order)->NumOrder+";";   							            //2 id визита
			msg += ConvertShopNum((*it_order)->NumShop)+";";   							//3 id ресторана
			msg += "0;";   																//4 id заказа внутри визита
			msg += "0;";   																//5 id неизменяемого типа заказа
			msg += IntToStr((*it_order)->TypeOrder)+";";   								//6 id типа заказа
			msg += (*it_order)->BusinessDate.FormatString("YYYY-MM-DD")+";";				//7 дата смены
			msg += (*it_order)->Start.FormatString("HH:MM:SS")+";";   						//8 время открытия заказа time
			msg += (*it_order)->End.FormatString("YYYY-MM-DD")+";";   						//9 дата закрытия заказа  date
			msg += (*it_order)->End.FormatString("HH:MM:SS")+";";   						//10 время закрытия заказа time
			msg += IntToStr((*it_order)->NumTable)+";";   									//11 id стола
			msg += IntToStr((*it_order)->NumTable)+";";   																//12 номер стола
			msg += ConvertShopNum((*it_order)->NumShop)+";";   															//13 id ресторана зала
			msg += IntToStr(GetIdHall((*it_order)->NumTable, StrToInt(ConvertShopNum((*it_order)->NumShop))))+";";  	//14 id зала
			msg += IntToStr((*it_order)->CountGuest)+";";   															//15 количество гостей
			msg += IntToStr((*it_order)->idWaiter)+";";   												//16 id сотрудника главного официанта
			msg += IntToStr((*it_order)->idWaiter)+";";   												//17 id сотрудника кассира
			msg += IntToStr((*it_order)->idWaiter)+";";   												//18 id сотрудника официанта
			msg += IntToStr(GetIntervalDateTime((*it_order)->Start,(*it_order)->End))+";";  			//19 длительность заказа
			msg += ReplaceComma(FloatToStr((*it_order)->SumPaymentOrder))+";";   						//20 оплаченная сумма заказа
			msg += (*it_order)->NumLastCheck+";";      													//21 id последнего чека
			msg += ";";                                //id основного визита
			msg += ";";                                //дата и время пречека
			msg += " ";                                //id адреса доставки
			WriteToFileDelayed("order.csv",msg);
		}
		Synchronize(&UpdateCaption);
		WriteToFileDelayed_WriteBufferLeftOvers();

		message = "End order (Заказы)";
		Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
TList *ThreadQuery::GetPayments(UnicodeString check_sn, UnicodeString cas_n)
{
	UniQuery1->SQL->Clear();
	UniQuery1->SQL->Add("SELECT cod_cash, amount FROM chekC WHERE chek_sn = " + check_sn + " AND cas_n = " + cas_n);
	UniQuery1->Execute();

	TList *list_pay = new TList();

	double Quant = 0;
	while (!UniQuery1->Eof)
	{
		TPayment *payment = new TPayment();
		payment->id = UniQuery1->FieldByName("cod_cash")->AsInteger;
		payment->amount = UniQuery1->FieldByName("amount")->AsFloat;
		Quant += payment->amount;
		list_pay->Add(payment);

		UniQuery1->Next();
	}

	for (int i = 0; i < list_pay->Count; i++)
	{
	   TPayment *p = (TPayment*)list_pay->Items[i];
	   p->part = p->amount / Quant;
	}

	return list_pay;
}
//---------------------------------------------------------------------------
int ThreadQuery::GetPrePayments(int payment_id)
{
	int prepayment = 0; //предоплата
	if (payment_id == 30)
	{
		prepayment = 1;
	}

	return  0;
}
//---------------------------------------------------------------------------
void ThreadQuery::AddPartionRatio(UnicodeString Barcode)
{
	ListPartionRatio->Add(Barcode);
}
//---------------------------------------------------------------------------
bool ThreadQuery::CheckPartionRatio(UnicodeString Barcode)
{
	if (ListPartionRatio->IndexOf(Barcode) == -1)
	{
		return false;
	}

    return true;
}
//---------------------------------------------------------------------------
void ThreadQuery::pbnd()
{
	message = "_____________________Start pbnd (Проводки)";
	Synchronize(&UpdateCaption2);

	double Summ = 0;
	double pbnd_Summ = 0;
	double sum_plast = 0;
	double sum_cash = 0;
	double sum_30 = 0;
	double sum_card_present = 0;

	CountRecord = 0;
	for (TListOrder::iterator it_order = list_order->begin(); it_order != list_order->end(); ++it_order)
	{
		CountRecord++;
		Synchronize(&UpdateCaptionDelay);

		TListCheck *list_check = (*it_order)->list_check;
		int id_pr = 0;

		Summ += (*it_order)->SumPaymentOrder;
		double itm_Summ = 0;

		for (TListCheck::iterator it_check = list_check->begin(); it_check != list_check->end(); ++it_check)
		{
			UnicodeString check_sn = it_check.operator *()->NumCheck;
			UnicodeString cas_n = it_check.operator *()->cas_n;

			TList *list_payment = GetPayments(check_sn, cas_n);

			UniQuery1->SQL->Clear();
			UniQuery1->SQL->Add("SELECT quantity, bar_cod, cur_price, netto FROM chekB WHERE chek_sn = " + check_sn + " AND cas_n = " + cas_n);
			UniQuery1->Execute();

			if (UniQuery1->IsEmpty())
			{
				continue;
			}

			while (!UniQuery1->Eof)
			{
				float quantityDB = UniQuery1->FieldByName("quantity")->AsFloat;  //количество
				UnicodeString BarCode = UniQuery1->FieldByName("bar_cod")->AsString;
				float netto = UniQuery1->FieldByName("netto")->AsFloat; // итого оплачено за количество quantityDB
				float cur_price = UniQuery1->FieldByName("cur_price")->AsFloat; // цена за еденицу



				UnicodeString LocalCode = BarCode;
				UniQuery3->SQL->Clear();
				UniQuery3->SQL->Add("SELECT cod_good FROM barcod WHERE bar_cod  = '" + BarCode + "'");
				UniQuery3->Execute();
				if (!UniQuery3->IsEmpty())
				{
					LocalCode = UniQuery3->FieldByName("cod_good")->AsWideString;
				}

				float massa = 1;
				UniQuery3->SQL->Clear();
				UniQuery3->SQL->Add("SELECT volume FROM katalog WHERE cod_good = " + LocalCode + "AND volume > 0 AND wflag = 1;");
				UniQuery3->Execute();

				if (!UniQuery3->IsEmpty())// товар весовой
				{
					massa = UniQuery3->FieldByName("volume")->AsFloat; //количество в одной порции

					quantityDB = quantityDB / massa;
					cur_price = cur_price * massa;

					//message = "BarCode: " + BarCode + " quantityDB: " + FloatToStr(quantityDB) + " cur_price: " + FloatToStr(cur_price);
					//Synchronize(&UpdateCaption2);

					//AddPartionRatio(BarCode);
				}


				for (int i = 0; i < list_payment->Count; i++)
				{
					TPayment *payment = (TPayment *)list_payment->Items[i];

					//double Quantity = abs(payment->part * quantityDB);
					double Quantity = payment->part * quantityDB;


					UnicodeString  msg;
					msg += "1;";   																				  	//id базы данных
					msg +=	(*it_order)->NumOrder+";";   																//id визита
					msg +=	ConvertShopNum((*it_order)->NumShop)+";";   														//id ресторана
					msg +=	IntToStr(id_pr)+";";   																	//id проводки внутри визита
					msg +=	"0;";   						    													//id заказа
					msg +=	BarCode+";";   										                                    //id позиции меню
					msg +=	IntToStr(payment->id)+";";																//id метода оплаты
					msg +=	ReplaceComma(FormatFloat("0.###",Quantity * payment->part * cur_price))+";";   			//цена в меню
					msg +=	ReplaceComma(FormatFloat("0.###",payment->part * netto))+";";   						//цена оплачено
					msg +=	ReplaceComma(FormatFloat("0.###",Quantity))+";";	                                    //количество
					msg +=	IntToStr(GetPrePayments(payment->id))+";";   											//предоплата
					msg +=	"0;";  																				    //сумма наценки
					msg +=	"0;";   																				//название наценки
					msg +=	"0;";   																				//id блюда, с которым продан модификатор
					msg +=	"0;";   																				//id проводки, с которой продан модификатор
					msg +=	ReplaceComma(FormatFloat("0.###",payment->part * netto))+";";   						//цена оплачено без модификаторов
					msg +=	";";   									                                                //id приказа
					msg +=	"0;";   																				//id места приготовления
					msg +=	IntToStr(it_check.operator *()->id_discount);   									    //id скидки

					WriteToFileDelayed("pbnd.csv",msg);
					id_pr++;

					itm_Summ += payment->part * netto ;
					pbnd_Summ += payment->part * netto;

					AddData_pbnd((*it_order)->NumShop, (*it_order)->BusinessDate, payment->id, payment->part * netto);

					switch (payment->id)
					{
							case 8 : sum_plast += payment->part * netto ;
									 break;
							case 13 : sum_cash += payment->part * netto ;
									 break;
							case 30 : sum_30 += payment->part * netto ;
									 break;
							case 25 : sum_card_present += payment->part * netto ;
									 break;
					}


				}


				UniQuery1->Next();
			}

		}

			message = "SumPaymentOrder: " + FloatToStr((*it_order)->SumPaymentOrder);
			message+= " Summ itm:" + FloatToStr(itm_Summ);
			//message+= " Count item: " + IntToStr(id_pr);
			message+= " NumCheck: " + (*it_order)->NumFirstCheck;
			//Synchronize(&UpdateCaption2);

	}
	WriteToFileDelayed_WriteBufferLeftOvers();
	Synchronize(&UpdateCaption);


	message = "Count: " + IntToStr(CountRecord);
	Synchronize(&UpdateCaption2);

	message = "Summ: " + FormatFloat("0.##",Summ);
	Synchronize(&UpdateCaption2);

	message = "pbnd_Summ: " + FormatFloat("0.##",pbnd_Summ);
	Synchronize(&UpdateCaption2);

	message = "Summ plast: " + FormatFloat("0.##",sum_plast) + " Summ cash: " + FormatFloat("0.##",sum_cash) + " Summ 30: " + FormatFloat("0.##",sum_30)+ " Summ Card Present: " + FormatFloat("0.##",sum_card_present);
	Synchronize(&UpdateCaption2);





	message = "End pbnd";
	Synchronize(&UpdateCaption2);

	ShowData_pbnd();
}
//---------------------------------------------------------------------------
void ThreadQuery::AddData_pbnd(int NumShop,TDateTime b_date, int payment_id, double payment)
{
	bool check_pay = false;
	for (int i = 0; i < List_pbnd->Count; i++)
	{
	   TShopPayment *shop_pay =(TShopPayment*)List_pbnd->operator [](i);
	   if (shop_pay->NumShop == NumShop && shop_pay->BusinessDate == b_date)
	   {
			switch (payment_id)
			{
					case 8 : shop_pay->sum_plast += payment ;
							 break;
					case 13 : shop_pay->sum_cash += payment ;
							 break;
					case 30 : shop_pay->sum_30 += payment ;
							 break;
					case 25 : shop_pay->sum_card_present += payment ;
							 break;
			}

			check_pay = true;
			break;
	   }
	}


	if (check_pay == false)
	{
		TShopPayment *shop_pay = new TShopPayment();
		shop_pay->NumShop = NumShop;
		shop_pay->BusinessDate = b_date;
		shop_pay->sum_plast = 0;
		shop_pay->sum_cash = 0;
		shop_pay->sum_30 = 0;
		shop_pay->sum_card_present = 0;

		switch (payment_id)
		{
				case 8 : shop_pay->sum_plast = payment ;
						 break;
				case 13 : shop_pay->sum_cash = payment ;
						 break;
				case 30 : shop_pay->sum_30 = payment ;
						 break;
				case 25 : shop_pay->sum_card_present += payment ;
						 break;
		}

		List_pbnd->Add(shop_pay);
	}

}
//---------------------------------------------------------------------------
void ThreadQuery::AddData_pmnt(int NumShop,TDateTime b_date, int payment_id, double payment)
{
 	bool check_pay = false;
	for (int i = 0; i < List_pmnt->Count; i++)
	{
	   TShopPayment *shop_pay =(TShopPayment*)List_pmnt->operator [](i);
	   if (shop_pay->NumShop == NumShop && shop_pay->BusinessDate == b_date)
	   {
			switch (payment_id)
			{
					case 8 : shop_pay->sum_plast += payment ;
							 break;
					case 13 : shop_pay->sum_cash += payment ;
							 break;
					case 30 : shop_pay->sum_30 += payment ;
							 break;
					case 25 : shop_pay->sum_card_present += payment ;
							 break;
			}

			check_pay = true;
			break;
	   }
	}


	if (check_pay == false)
	{
		TShopPayment *shop_pay = new TShopPayment();
		shop_pay->NumShop = NumShop;
		shop_pay->BusinessDate = b_date;
		shop_pay->sum_plast = 0;
		shop_pay->sum_cash = 0;
		shop_pay->sum_30 = 0;
        shop_pay->sum_card_present = 0;

		switch (payment_id)
		{
				case 8 : shop_pay->sum_plast = payment ;
						 break;
				case 13 : shop_pay->sum_cash = payment ;
						 break;
				case 30 : shop_pay->sum_30 = payment ;
						 break;
				case 25 : shop_pay->sum_card_present += payment ;
						 break;
		}

		List_pmnt->Add(shop_pay);
	}

}
//---------------------------------------------------------------------------
void ThreadQuery::ShowData_pbnd()
{
	double summm = 0;
	for (int i = 0; i < List_pbnd->Count; i++)
	{
	   TShopPayment *shop_pay =(TShopPayment*)List_pbnd->operator [](i);
	   message = "Shop:" + IntToStr(shop_pay->NumShop) + " " + shop_pay->BusinessDate.FormatString("dd-mm-yy")+"     Итого: " + FormatFloat("0.##",shop_pay->sum_plast+shop_pay->sum_cash+shop_pay->sum_30+shop_pay->sum_card_present) + " " + "     Summ plast: " + FormatFloat("0.##",shop_pay->sum_plast) + "     Summ cash: " + FormatFloat("0.##",shop_pay->sum_cash) + "     Summ 30: " + FormatFloat("0.##",shop_pay->sum_30)+ "     Summ Card Present: " + FormatFloat("0.##",shop_pay->sum_card_present);
	   Synchronize(&UpdateCaption2NoLog);

	   summm +=  shop_pay->sum_plast+shop_pay->sum_cash+shop_pay->sum_30+shop_pay->sum_card_present;
	}

	message = "-------------------------------------";
	Synchronize(&UpdateCaption2);

	message = "Итого: " + FormatFloat("0.##",summm);
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
void ThreadQuery::ShowData_pmnt()
{
    double summm = 0;
	for (int i = 0; i < List_pmnt->Count; i++)
	{
	   TShopPayment *shop_pay =(TShopPayment*)List_pmnt->operator [](i);
	   message = "Shop:" + IntToStr(shop_pay->NumShop) + " " + shop_pay->BusinessDate.FormatString("dd-mm-yy")+"    Итого: " + FormatFloat("0.##",shop_pay->sum_plast+shop_pay->sum_cash+shop_pay->sum_30+shop_pay->sum_card_present) + " " + "    Summ plast: " + FormatFloat("0.##",shop_pay->sum_plast) + "    Summ cash: " + FormatFloat("0.##",shop_pay->sum_cash) + "    Summ 30: " + FormatFloat("0.##",shop_pay->sum_30)+ "    Summ Card Present: " + FormatFloat("0.##",shop_pay->sum_card_present);
	   Synchronize(&UpdateCaption2NoLog);

       summm +=  shop_pay->sum_plast+shop_pay->sum_cash+shop_pay->sum_30+shop_pay->sum_card_present;
	}

	message = "-------------------------------------";
	Synchronize(&UpdateCaption2);

	message = "Итого: " + FormatFloat("0.####",summm);
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
bool ThreadQuery::ConnectSQL1()
{

	CoInitialize(NULL);
	UniConnection1 = new TUniConnection(NULL);


	UniConnection1->ProviderName = "SQL Server";
	UniConnection1->LoginPrompt = false;
	UniConnection1->Port = 1433;
	UniConnection1->Username = "sa";
	UniConnection1->Password = "sa";
	UniConnection1->Database = "OpenEDGE";
	UniConnection1->Server = "localhost";


/*
    	UniConnection1->ProviderName = "ODBC";
    	UniConnection1->LoginPrompt = false;
    	UniConnection1->Username = "sysprogress";
    	UniConnection1->Password = "progress";
    	UniConnection1->Database = "ges3";
		UniConnection1->Server = "localhost";
*/

	try
	{
		UniConnection1->Connect();

		UniQuery1 = new TUniQuery(NULL);
		UniQuery1->Connection = UniConnection1;

		UniQuery3 = new TUniQuery(NULL);
		UniQuery3->Connection = UniConnection1;

		UniQuery4 = new TUniQuery(NULL);
		UniQuery4->Connection = UniConnection1;

		return true;
	}
	catch (Exception &exception)
	{
		message = exception.Message;
		Synchronize(&UpdateCaption2);
		return false;
	}
}
//---------------------------------------------------------------------------
bool ThreadQuery::ConnectSQL2()
{

	CoInitialize(NULL);
	UniConnection2 = new TUniConnection(NULL);
    UniConnection2->LoginPrompt = false;
	UniConnection2->ProviderName = "SQL Server";
	UniConnection2->Port = 1433;
	UniConnection2->Username = "DiogenMaster";
	UniConnection2->Password = "12345678";
	UniConnection2->Database = "Diogen";
	UniConnection2->Server = "s2010";


	try
	{
		UniConnection2->Connect();
		UniQuery2 = new TUniQuery(NULL);
		UniQuery2->Connection = UniConnection2;

		return true;
	}
	catch (Exception &exception)
	{
		message = exception.Message;
		Synchronize(&UpdateCaption2);
		return false;
	}

}
//---------------------------------------------------------------------------
void ThreadQuery::item_size() //Приказы (типы скидок)
{
	message = "_____________________Start item_size ";
	Synchronize(&UpdateCaption2);


	UniQuery2->SQL->Clear();
	UniQuery2->SQL->Add("SELECT Id, AlohaCompId, Name FROM [Diogen].[dbo].[AlohaMenuComps]");
	UniQuery2->Execute();

	UnicodeString msg;
	while (!UniQuery2->Eof)
	{
		msg = "";
		msg += "1;"+ 															//id базы данных
		msg += UniQuery2->FieldByName("AlohaCompId")->AsString+";";   	//id приказа
		msg += UniQuery2->FieldByName("Name")->AsString+";";   			//название
		msg += UniQuery2->FieldByName("Id")->AsString+";";   			//код
		msg += "3;";   															//статус
		msg += "0";   															//id группы размеров позиций меню

		WriteToFile("item_size.csv",msg.Trim());
		UniQuery2->Next();
	}


	message = "End item_size";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------

void ThreadQuery::curr()
{
	message = "_____________________Start curr (Методы оплат) ";
	Synchronize(&UpdateCaption2);

	UnicodeString filter_typepay[] = {13,8,25,30,21};
	int count_f = 5;
	UnicodeString filter;

	for (int i = 0; i < count_f; i++)
	{
		if (i != count_f-1)
		{
			filter += "p_type =" + filter_typepay[i] + " OR ";
		}
		else
		{
			filter += "p_type =" + filter_typepay[i];
		}
	}

	UniQuery1->SQL->Clear();
	UniQuery1->SQL->Add("SELECT p_type, payment FROM payment WHERE ("+filter+")");
	UniQuery1->Execute();

	message = "write file";
	Synchronize(&UpdateCaption2);
	UnicodeString msg;
	while (!UniQuery1->Eof)
	{
		msg = "";
		msg += "1;"; 												//id базы данных
		msg += UniQuery1->FieldByName("p_type")->AsString+";";   	//id метода оплат
		msg += UniQuery1->FieldByName("payment")->AsString+";";   	//название метода оплат
		msg += "3;";  												//статус

		WriteToFile("curr.csv",msg.Trim());
		UniQuery1->Next();
	}


	UniQuery2->SQL->Clear();
	UniQuery2->SQL->Add("SELECT AlohaCompId, Name FROM [Diogen].[dbo].[AlohaMenuComps] WHERE AlohaCompId >= 11 AND AlohaCompId <= 30");

	UniQuery2->Execute();

	if (!UniQuery2->IsEmpty())
	{

		while (!UniQuery2->Eof)
		{
			UnicodeString msg;
			msg += "1;"; 															//id базы данных
			msg += "333" + UniQuery2->FieldByName("AlohaCompId")->AsString+";";   	//id метода оплат
			msg += UniQuery2->FieldByName("Name")->AsString+";";   					//название метода оплат
			msg += "3";  														    //статус

			WriteToFile("curr.csv",msg.Trim());
			UniQuery2->Next();

		}
	}

	msg = "";
	msg += "1;";
	msg += "22234;";
	msg += "Вывод товаров из меню;";
	msg += "3";
	WriteToFile("curr.csv",msg.Trim());

	msg = "";
	msg += "1;";
	msg += "22233;";
	msg += "Списание отходов;";
	msg += "3";
	WriteToFile("curr.csv",msg.Trim());

	msg = "";
	msg += "1;";
	msg += "222353;";
	msg += "Разработка новых блюд;";
	msg += "3";
	WriteToFile("curr.csv",msg.Trim());

	msg = "";
	msg += "1;";
	msg += "22237;";
	msg += "Списание на технол.нужды;";
	msg += "3";
	WriteToFile("curr.csv",msg.Trim());

 	msg = "";
	msg += "1;";
	msg += "4441;";
	msg += "Списание бонуса;";
	msg += "3";
	WriteToFile("curr.csv",msg.Trim());

	msg = "";
	msg += "1;";
	msg += "4442;";
	msg += "Бракераж;";
	msg += "3";
	WriteToFile("curr.csv",msg.Trim());

	msg = "";
	msg += "1;";
	msg += "4443;";
	msg += "Списание по сроку годности;";
	msg += "3";
	WriteToFile("curr.csv",msg.Trim());

	msg = "";
	msg += "1;";
	msg += "4444;";
	msg += "Списание на проработку;";
	msg += "3";
	WriteToFile("curr.csv",msg.Trim());

 	msg = "";
	msg += "1;";
	msg += "4445 ;";
	msg += "Списание некачественн. товаров;";
	msg += "3";
	WriteToFile("curr.csv",msg.Trim());

	message = "End curr";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
void ThreadQuery::item()
{
	message = "_____________________Start item (Позиции меню)";
	Synchronize(&UpdateCaption2);


	UniQuery2->SQL->Clear();
	UniQuery2->SQL->Add("SELECT BarCode, Name, CategoryId, Weight FROM [Diogen].[dbo].[AlohaMenuItemsAll]");

	UniQuery2->Execute();
	CountRecord = UniQuery2->RecordCount;

	if (!UniQuery2->IsEmpty())
	{
		message = "write file";
		Synchronize(&UpdateCaption2);
		UnicodeString msg;

		while (!UniQuery2->Eof)
		{

			if (list_group_menu->IndexOf(UniQuery2->FieldByName("CategoryId")->AsString) == -1)
			{
				message = UniQuery2->FieldByName("BarCode")->AsString + " no category: " +UniQuery2->FieldByName("CategoryId")->AsString;
				Synchronize(&UpdateCaption2);
			}

			int portion_ratio = UniQuery2->FieldByName("Weight")->AsInteger;

			CountRecord--;
			Synchronize(&UpdateCaption);
			msg = "";
			msg += "1;";													//id базы данных
			msg += UniQuery2->FieldByName("BarCode")->AsString+";";   		//id позиции меню
			msg += UniQuery2->FieldByName("CategoryId")->AsString+";";   	//id группы меню
			msg += UniQuery2->FieldByName("Name")->AsString+";";   			//название позиции меню
			msg += "3;";   													//статус
			msg += "0;";   													//id принтера
			msg += "0;";   													//id категории меню
			msg += "0;";                                                    //id категории мозга new!!!!!!!!!!
			msg += "0;";   													//id группы мею для мозга
			msg += "0;";   													//id дополнительной группировки
			msg += IntToStr(portion_ratio)+";";   							//порционный коэфицент
			msg += "0;";   													//Не используется
			msg += "0;"; 													//Не используется
			msg += ";";   													//id ресторана
			msg += "DISH;";   												//Тип позиции
			msg += "0";   													//Не используется

			WriteToFile("item.csv",msg.Trim());
			UniQuery2->Next();
		}



		message = "end write file";
		Synchronize(&UpdateCaption2);

	}
	message = "End item (Позиции меню)";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
void ThreadQuery::group()
{
	message = "_____________________Start item (Группы меню)";
	Synchronize(&UpdateCaption2);

    list_group_menu->Clear();
	UniQuery2->SQL->Clear();
	UniQuery2->SQL->Add("SELECT Cat, Name FROM [Diogen].[dbo].[AlohaMenuCatAll]");

	UniQuery2->Execute();
	CountRecord = UniQuery2->RecordCount;


	if (!UniQuery2->IsEmpty())
	{
		message = "write file";
		Synchronize(&UpdateCaption2);

		UnicodeString msg;


		while (!UniQuery2->Eof)
		{
			CountRecord--;
			Synchronize(&UpdateCaption);
			msg = "";
			msg += "1;"; 																									//id базы данных
			msg += UniQuery2->FieldByName("Cat")->AsString+";";   															//id группы меню
			msg += StringReplace(UniQuery2->FieldByName("Name")->AsWideString,"&"," ",TReplaceFlags()<<rfReplaceAll)+";"; 	//название группы меню
			msg += "3;";   																									//статус
			msg += "0;";   																									//id родительской группы
			msg += ""; 	    																								//id ресторана

			list_group_menu->Add(UniQuery2->FieldByName("Cat")->AsString);

			WriteToFile("group.csv",msg.Trim());
			UniQuery2->Next();
		}


		message = "end write file";
		Synchronize(&UpdateCaption2);
	}

	message = "End item (Группы меню)";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
UnicodeString ThreadQuery::ReplaceComma(UnicodeString value)
{
   return StringReplace(value,",",".",TReplaceFlags()<<rfReplaceAll);
}
//---------------------------------------------------------------------------
void ThreadQuery::rest()
{
	message = "_____________________Start rest (Рестораны)";
	Synchronize(&UpdateCaption2);

	TShop *shop;
	list_shop = new TListShop();




	shop = new TShop();
	shop->NumShop = 205;
	shop->NameShop = "Аврора";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 371;
	shop->NameShop = "Новая площадь";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 390;
	shop->NameShop = "Белая площадь";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 177;
	shop->NameShop = "Неглинная";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 255;
	shop->NameShop = "Белые сады";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 180;
	shop->NameShop = "Осенняя";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 104;
	shop->NameShop = "Кофейня на Никитской";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 111;
	shop->NameShop = "Четыре Ветра";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 130;
	shop->NameShop = "Кофейня на Кудринской";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 190;
	shop->NameShop = "Депо";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 260;
	shop->NameShop = "Якиманка";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 264;
	shop->NameShop = "Мэрия";
	list_shop->push_back(shop);


	shop = new TShop();
	shop->NumShop = 270;
	shop->NameShop = "Весна";
	list_shop->push_back(shop);


	shop = new TShop();
	shop->NumShop = 235;
	shop->NameShop = "Павлово";
	list_shop->push_back(shop);


	shop = new TShop();
	shop->NumShop = 280;
	shop->NameShop = "Отель";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 285;
	shop->NameShop = "Усачевский";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 290;
	shop->NameShop = "Кофейня в ТРЦ Хорошо";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 295;
	shop->NameShop = "КОМСОМОЛЬСКИЙ";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 310;
	shop->NameShop = "Ресторан Внуково Аутлет";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 311;
	shop->NameShop = "Новая Рига Аутлет";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 320;
	shop->NameShop = "Вьетнамка";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 350;
	shop->NameShop = "Метрополис";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 370;
	shop->NameShop = "Кутузовский";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 375;
	shop->NameShop = "Тверская";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 380;
	shop->NameShop = "ГУМ";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 395;
	shop->NameShop = "Покровка";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 122;
	shop->NameShop = "Wall Street";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 237;
	shop->NameShop = "Горки-2";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 195;
	shop->NameShop = "Вишневый Сад";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 321;
	shop->NameShop = "Украинский бульвар";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 197;
	shop->NameShop = "Черемушкинский рынок";
	list_shop->push_back(shop);

 	shop = new TShop();
	shop->NumShop = 194;
	shop->NameShop = "Даниловский рынок";
	list_shop->push_back(shop);

 	shop = new TShop();
	shop->NumShop = 198;
	shop->NameShop = "Аркус";
	list_shop->push_back(shop);

 	shop = new TShop();
	shop->NumShop = 193;
	shop->NameShop = "Ленинский рынок";
	list_shop->push_back(shop);


	UnicodeString msg;
	for (TListShop::iterator it_shop = list_shop->begin(); it_shop != list_shop->end(); ++it_shop)
	{
		msg = "";
		msg += "1;"+ 											//id базы данных
		msg += IntToStr(it_shop.operator *()->NumShop)+";"+   	//id ресторана
		msg += it_shop.operator *()->NameShop+";"+   			//название ресторана
		msg += "3;";   											//статус
		msg += IntToStr(it_shop.operator *()->NumShop)+";"; 	//id ресторана для объединения
		msg += "0;"; 											//Центральный офис
		msg += ""; 												//Комментарий

		WriteToFile("rest.csv",msg.Trim());
	}

	message = "End rest (Рестораны)";
	Synchronize(&UpdateCaption2);

}
//---------------------------------------------------------------------------
void ThreadQuery::rest_aeroport()
{
	message = "_____________________Start rest (Рестораны)";
	Synchronize(&UpdateCaption2);

	TShop *shop;
	list_shop = new TListShop();

	shop = new TShop();
	shop->NumShop = 212;
	shop->NameShop = "Шереметьево-1";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 213;
	shop->NameShop = "Шереметьево-2";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 216;
	shop->NameShop = "Кофейня Шереметьево Е";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 217;
	shop->NameShop = "Шереметьево D";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 222;
	shop->NameShop = "Узбечка Шереметьево";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 224;
	shop->NameShop = "Шереметьево В";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 226;
	shop->NameShop = "Узбечка ШВ";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 300;
	shop->NameShop = "Домодедово-2";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 231;
	shop->NameShop = "Домодедово 4";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 227;
	shop->NameShop = "Шереметьево С";
	list_shop->push_back(shop);

	shop = new TShop();
	shop->NumShop = 228;
	shop->NameShop = "Узбечка ШС";
	list_shop->push_back(shop);

	UnicodeString msg;
	for (TListShop::iterator it_shop = list_shop->begin(); it_shop != list_shop->end(); ++it_shop)
	{
		msg = "";
		msg += "1;"+ 											//id базы данных
		msg += IntToStr(it_shop.operator *()->NumShop)+";"+   	//id ресторана
		msg += it_shop.operator *()->NameShop+";"+   			//название ресторана
		msg += "3;";   											//статус
		msg += IntToStr(it_shop.operator *()->NumShop)+";"; 	//id ресторана для объединения
		msg += "0;"; 											//Центральный офис
		msg += ""; 												//Комментарий

		WriteToFile("rest.csv",msg.Trim());
	}

	message = "End rest (Рестораны)";
	Synchronize(&UpdateCaption2);

}
//---------------------------------------------------------------------------
UnicodeString ThreadQuery::GetListShop()
{
	UnicodeString result;
	int idx = 0;
	for (TListShop::iterator it_shop = list_shop->begin(); it_shop != list_shop->end(); ++it_shop)
	{
		if (idx < list_shop->size()-1)
		{
			result += "chekA.cod_shop = " + IntToStr(it_shop.operator *()->NumShop)+" OR ";
		}
		else
		{
			result += "chekA.cod_shop = " + IntToStr(it_shop.operator *()->NumShop);
		}
		idx++;

	}

	result+= " OR chekA.cod_shop= 191 OR chekA.cod_shop=121 OR chekA.cod_shop=123 OR chekA.cod_shop=236 OR chekA.cod_shop=239 OR chekA.cod_shop=196 OR chekA.cod_shop=114 OR chekA.cod_shop=124";
	return  result;
}
//---------------------------------------------------------------------------
UnicodeString ThreadQuery::GetListShop(UnicodeString Name_field)
{
	UnicodeString result;
	int idx = 0;
	for (TListShop::iterator it_shop = list_shop->begin(); it_shop != list_shop->end(); ++it_shop)
	{
		if (idx < list_shop->size()-1)
		{
			result += Name_field+" = " + IntToStr(it_shop.operator *()->NumShop)+" OR ";
		}
		else
		{
			result += Name_field+" = " + IntToStr(it_shop.operator *()->NumShop);
		}
		idx++;

	}

	result+= " OR "+Name_field+"= 191 OR "+Name_field+"=121 OR "+Name_field+"=123 OR " +Name_field+"=236 OR " + Name_field+"=239 OR " + Name_field+"=196 OR " + Name_field+"=114 OR " + Name_field+"=124";
	return  result;
}
//---------------------------------------------------------------------------
void ThreadQuery::writeoff()//Списания
{

	message = "_____________________Start query writeoff (Списания)";
	Synchronize(&UpdateCaption2);

	UniQuery1->SQL->Clear();
	UniQuery1->SQL->Add("SELECT chekB.chek_sn AS chek_sn, chekB.cas_n AS cas_n, chekB.bar_cod AS bar_cod, chekB.quantity AS quantity, chekB.retail_price AS retail_price, chekB.discount AS discount, chekA.dat_chek AS dat_chek, chekA.timer AS timer, chekA.dk_dcod AS dk_dcod, chekA.cod_shop AS cod_shop");
	UniQuery1->SQL->Add("FROM  chekB FULL OUTER JOIN ");
	UniQuery1->SQL->Add("chekA ON chekB.chek_sn = chekA.chek_sn AND chekB.cas_n = chekA.cas_n");
	UniQuery1->SQL->Add("WHERE (chekA.sum_b = 0) AND ( "+GetListShop()+" ) AND chekA.business_date >= convert(datetime,'"+StartDT.FormatString("DD/MM/YYYY")+"',104) AND chekA.business_date <= convert(datetime,'"+EndDT.FormatString("DD/MM/YYYY")+"',104)");
	UniQuery1->SQL->Add("ORDER BY chekB.chek_sn , chekB.cas_n");

	UniQuery1->Execute();

	CountRecord = UniQuery1->RecordCount;
	message = "Count : " + IntToStr(CountRecord);
	Synchronize(&UpdateCaption2);

	if (!UniQuery1->IsEmpty())
	{

		int id_document = 0;
		UnicodeString last_check_sn;
		while (!UniQuery1->Eof)
		{
			CountRecord--;
			Synchronize(&UpdateCaptionDelay);

			if (last_check_sn != UniQuery1->FieldByName("chek_sn")->AsString)
			{
			   id_document = 0;
			   last_check_sn = UniQuery1->FieldByName("chek_sn")->AsString;
			}



			TDateTime DT_ = UniQuery1->FieldByName("dat_chek")->AsDateTime;
			DT_ = EncodeDate(YearOf(DT_), MonthOf(DT_), 1);

			UnicodeString price = "0";
			if (UniQuery1->FieldByName("bar_cod")->AsString != "")
			{
				UniQuery3->SQL->Clear();
				UniQuery3->SQL->Add("SELECT * FROM cost WHERE dt1 = convert(datetime,'"+DT_.FormatString("DD/MM/YYYY")+"',104) AND cost.cod_shop = "+UniQuery1->FieldByName("cod_shop")->AsString+" AND cost.cod_good = "+UniQuery1->FieldByName("bar_cod")->AsString);
				UniQuery3->Execute();

				if (!UniQuery3->IsEmpty())
				{
					price = UniQuery3->FieldByName("av_cost_price")->AsString;
				}
				else
				{
					UniQuery3->SQL->Clear();
					UniQuery3->SQL->Add("SELECT * FROM cost WHERE dt1 = convert(datetime,'"+DT_.FormatString("DD/MM/YYYY")+"',104) AND cost.cod_shop = 104 AND cost.cod_good = "+UniQuery1->FieldByName("bar_cod")->AsString);
					UniQuery3->Execute();
					if (!UniQuery3->IsEmpty())
					{
						price = UniQuery3->FieldByName("av_cost_price")->AsString;
					}
                }
			}


			if (UniQuery1->FieldByName("bar_cod")->AsString == "822174" && false)
			{
				message = "price : " + price + "  dt: " + DT_.FormatString("YYYY-MM-DD");
				Synchronize(&UpdateCaption2);
			}

			UnicodeString writeoff_reason1 = GetReason1ByReason2(UniQuery1->FieldByName("dk_dcod")->AsInteger);

			float price_float = StrToFloat(price);
			int quantity = UniQuery1->FieldByName("quantity")->AsInteger;
			float summ = price_float * quantity;

			UnicodeString msg;
			msg += "1;"; 																				//id базы данных
			msg += UniQuery1->FieldByName("chek_sn")->AsString + UniQuery1->FieldByName("cas_n")->AsString + IntToStr(id_document)+";";   //id списания
			msg += IntToStr(id_document)+";";  															//id документа
			msg += UniQuery1->FieldByName("chek_sn")->AsString+";";   									//номер документа
			msg += ConvertShopNum(UniQuery1->FieldByName("cod_shop")->AsString)+";"; 					//id ресторана
			msg += ConvertToDateTime(UniQuery1->FieldByName("dat_chek")->AsDateTime,
				   UniQuery1->FieldByName("timer")->AsString).FormatString("YYYY-MM-DD HH:MM:SS")+";"; 	//дата/время
			msg += UniQuery1->FieldByName("bar_cod")->AsString+";"; 									//id позиции меню
			msg += UniQuery1->FieldByName("quantity")->AsString+";"; 									//количество
			msg += ReplaceComma(FormatFloat("0.###",summ))+";"; 										//сумма
			msg += "333" + UniQuery1->FieldByName("dk_dcod")->AsString+";"; 							//id метода оплаты
			msg += "0;"; 																				//автоматически
			msg += "pcs;"; 																				//единица измерения
			msg += writeoff_reason1+";";
			msg += UniQuery1->FieldByName("dk_dcod")->AsString;                                     //GUID причины списания 2


			WriteToFileDelayed("writeoff.csv",msg.Trim());
			UniQuery1->Next();
			id_document++;
		}
		Synchronize(&UpdateCaption);

	}

	WriteToFileDelayed_WriteBufferLeftOvers();

	message = "End writeoff (Списания)";
	Synchronize(&UpdateCaption2);

}
//---------------------------------------------------------------------------
UnicodeString ThreadQuery::GetReason1ByReason2(int reason2)
{
	UnicodeString result;
	switch (reason2)
	{
		case 20:
		{
			break;
		}
		case 15:
		{

			break;
		}
		case 24:
		{
			result = "40";
			break;
		}
		case 30:
		{
			result = "392";
			break;
		}
		case 21:
		{
			result = "35";
			break;
		}
		case 28:
		{
			result = "39";
			break;
		}
		case 29:
		{
			result = "391";
			break;
		}
		case 25:
		{
			result = "38";
			break;
		}
		case 16:
		{
			result = "39";
			break;
		}
		case 12:
		{

			break;
		}
		case 19:
		{
			result = "39";
			break;
		}
		case 17:
		{
			result = "31";
			break;
		}
		case 14:
		{
			result = "36";
			break;
		}
		case 26:
		{
			result = "351";
			break;
		}
		case 11:
		{
			result = "35";
			break;
		}
		case 18:
		{
			result = "32";
			break;
		}
		case 27:
		{
			result = "352";
			break;
		}
		case 22:
		{
			result = "35";
			break;
		}
		case 13:
		{
			result = "36";
			break;
		}
		case 23:
		{
			result = "35";
			break;
		}
	}

	return result;
}
//---------------------------------------------------------------------------
void ThreadQuery::writeoff_reason2()
{
	message = "_____________________Start writeoff_reason1 (справочник причин списания)";
	Synchronize(&UpdateCaption2);


	UniQuery2->SQL->Clear();
	UniQuery2->SQL->Add("SELECT AlohaCompId, Name FROM [Diogen].[dbo].[AlohaMenuComps] WHERE AlohaCompId >= 11 AND AlohaCompId <= 30");

	UniQuery2->Execute();

	if (!UniQuery2->IsEmpty())
	{

		while (!UniQuery2->Eof)
		{
			UnicodeString msg;
			msg += "1;";
			msg += UniQuery2->FieldByName("AlohaCompId")->AsString+";";
			msg += UniQuery2->FieldByName("Name")->AsString;


			WriteToFile("writeoff_reason2.csv",msg.Trim());
			UniQuery2->Next();

		}
	}

	UnicodeString msg;
	msg += "1;";
	msg += "3666;";
	msg += "Питание персонал";


	WriteToFile("writeoff_reason2.csv",msg.Trim());


	message = "End writeoff_reason1 (справочник причин списания)";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
void ThreadQuery::empl() //Сотрудники
{
	message = "_____________________Start query empl (Сотрудники)";
	Synchronize(&UpdateCaption2);

	UniQuery1->SQL->Clear();
	UniQuery1->SQL->Add("SELECT EMPLOYEE_ID, LAST_NAME, MIDDLE_NAME, FIRST_NAME, SUBDIVISION_ID FROM EMPLOYEE");
	UniQuery1->Execute();


	CountRecord = UniQuery1->RecordCount;
	message = "Count Record: " + IntToStr(CountRecord);
	Synchronize(&UpdateCaption2);
	UnicodeString name = "";

	if (!UniQuery1->IsEmpty())
	{
		UnicodeString msg;
		while (!UniQuery1->Eof)
		{
			CountRecord--;
			Synchronize(&UpdateCaption);

			name = UniQuery1->FieldByName("LAST_NAME")->AsString + " " + UniQuery1->FieldByName("FIRST_NAME")->AsString + " " + UniQuery1->FieldByName("MIDDLE_NAME")->AsString;

            msg = "";
			msg += "1;"+ 															//id базы данных
			msg += UniQuery1->FieldByName("EMPLOYEE_ID")->AsString+";"+   			//id сотрудника
			msg += name+";"+   														//имя сотрудника
			msg += "3;";   														    //статус
			msg += UniQuery1->FieldByName("SUBDIVISION_ID")->AsString+";"; 	        //id ресторана
			msg += UniQuery1->FieldByName("EMPLOYEE_ID")->AsString; 		        //код сотрудника

			WriteToFile("empl.csv",msg.Trim());
			UniQuery1->Next();

		}

	}

	message = "End  empl (Сотрудники)";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
void ThreadQuery::pmnt() //Платежи
{
	message = "_____________________Start query pmnt (Платежи)";
	Synchronize(&UpdateCaption2);

 	double sum_plast = 0;
	double sum_cash = 0;
	double sum_30 = 0;
	double sum_card_present = 0;

    double sum = 0;
	CountRecord = 0;
	for (TListOrder::iterator it_order = list_order->begin(); it_order != list_order->end(); ++it_order)
	{

		CountRecord++;
		Synchronize(&UpdateCaptionDelay);

		TListCheck *list_check = (*it_order)->list_check;
		int id_pr = 0;

		for (TListCheck::iterator it_check = list_check->begin(); it_check != list_check->end(); ++it_check)
		{
			UnicodeString check_sn = it_check.operator *()->NumCheck;
			UnicodeString cas_n = it_check.operator *()->cas_n;

			TList *list_payment = GetPayments(check_sn, cas_n);

			for (int i = 0; i < list_payment->Count; i++)
			{
				TPayment *payment = (TPayment *)list_payment->Items[i];
				//payment->amount = abs(payment->amount);
				UnicodeString msg;
				msg += "1;";   															//id базы данных
				msg += (*it_order)->NumOrder+";";   						            //id визита
				msg += ConvertShopNum((*it_order)->NumShop)+";";   						//id ресторана
				msg += IntToStr(id_pr)+";";   											//id платежа внутри визита
				msg += "0;";   						    								//id заказа
				msg += IntToStr(payment->id)+";";   									//id метода оплаты
				msg += ReplaceComma(FormatFloat("0.###",payment->amount))+";";			//оплаченная сумма
				msg += "0;";   						            					    //предоплата
				msg += check_sn;                                                        //id чека

				sum += payment->amount;

				AddData_pmnt((*it_order)->NumShop,(*it_order)->BusinessDate, payment->id, payment->amount);

				WriteToFileDelayed("pmnt.csv",msg.Trim());
				id_pr++;



				switch (payment->id)
				{
					case 8 : sum_plast += payment->amount ;
							 break;
					case 13 : sum_cash += payment->amount ;
							 break;
					case 30 : sum_30 += payment->amount ;
							 break;
					case 25 : sum_card_present += payment->amount ;
							 break;
				}



			}



		}

	}
	Synchronize(&UpdateCaption);
	WriteToFileDelayed_WriteBufferLeftOvers();

    message = "------------------------------------";
	Synchronize(&UpdateCaption2);

	message = "Summ plast: " + FormatFloat("0.##",sum_plast) + " Summ cash: " + FormatFloat("0.##",sum_cash) + " Summ 30: " + FormatFloat("0.##",sum_30)+ " Summ Card Present: " + FormatFloat("0.##",sum_card_present);
	Synchronize(&UpdateCaption2);

	message = "Count: "+IntToStr(CountRecord);
	Synchronize(&UpdateCaption2);

	message = "Sum: "+FloatToStr(sum);
	Synchronize(&UpdateCaption2);

	message = "End pmnt (Платежи)";
	Synchronize(&UpdateCaption2);

    ShowData_pmnt();
}
//---------------------------------------------------------------------------
void ThreadQuery::hall() // Залы
{
	message = "_____________________Start hall (Залы)";
	Synchronize(&UpdateCaption2);

	ListHall = new TList();

	int idx = 1;


	std::vector<TPairData> list_new_cat;
	std::vector<TPairData> list_new_cat2;
	std::vector<TPairData> list_new_cat3;
    std::vector<TPairData> list_new_cat4;


	for (TListShop::iterator it_shop = list_shop->begin(); it_shop != list_shop->end(); ++it_shop)
	{
		for (TListTypeOrder::iterator it_typeorder = list_typeorder->begin(); it_typeorder != list_typeorder->end(); ++it_typeorder)
		{
			UnicodeString msg = "";
			msg += "1;";   						   													//id базы данных
			msg += IntToStr(idx)+";";     															//id зала
			msg += it_typeorder.operator *()->Name + " " + (*it_shop)->NameShop +";";               //название зала
			msg += "3;";   																			//статус
			msg += IntToStr((*it_shop)->NumShop)+";";   									        //id ресторана
			msg += "0;";   																	        //порядок сортировки
			msg += IntToStr((*it_shop)->NumShop)+";";                                               //id подразделения
			msg += "0";                                                                             //вместимость гостей


			if ((*it_typeorder)->id == 23 )//чтоб id залов не изменилось перенесем новые в конец
			{
				int id_t = (*it_typeorder)->id;
				UnicodeString name_type_oper = (*it_typeorder)->Name;
				list_new_cat.push_back(std::pair<int,UnicodeString>(id_t,name_type_oper));
				continue;
			}

			if ((*it_typeorder)->id == 24 )//чтоб id залов не изменилось перенесем новые в конец
			{
				int id_t = (*it_typeorder)->id;
				UnicodeString name_type_oper = (*it_typeorder)->Name;
				list_new_cat2.push_back(std::pair<int,UnicodeString>(id_t,name_type_oper));
				continue;
			}

			if ((*it_typeorder)->id == 25 )//чтоб id залов не изменилось перенесем новые в конец
			{
				int id_t = (*it_typeorder)->id;
				UnicodeString name_type_oper = (*it_typeorder)->Name;
				list_new_cat3.push_back(std::pair<int,UnicodeString>(id_t,name_type_oper));
				continue;
			}

			if ((*it_typeorder)->id == 26 )//чтоб id залов не изменилось перенесем новые в конец
			{
				int id_t = (*it_typeorder)->id;
				UnicodeString name_type_oper = (*it_typeorder)->Name;
				list_new_cat4.push_back(std::pair<int,UnicodeString>(id_t,name_type_oper));
				continue;
			}

			WriteToFile("hall.csv",msg.Trim());
			idx++;

			THall *hall = new THall();
			hall->id = (*it_typeorder)->id;
			hall->Name = (*it_typeorder)->Name;
			hall->NumShop = (*it_shop)->NumShop;
			ListHall->Add(hall);

		}
	}


	// Новые залы переместим в конец списка чтоб у старых не сбилась нумерация
	for (TListShop::iterator it_shop = list_shop->begin(); it_shop != list_shop->end(); ++it_shop)
	{
		std::pair<int,UnicodeString> item;
		try
		{
			item = list_new_cat.at(0);
		}
		catch (...)
		{
            continue;
		}


		UnicodeString msg = "";
		msg += "1;";   						   													//id базы данных
		msg += IntToStr(idx)+";";     															//id зала
		msg += item.second + " " + (*it_shop)->NameShop +";";                                   //название зала
		msg += "3;";   																			//статус
		msg += IntToStr((*it_shop)->NumShop)+";";   									        //id ресторана
		msg += "0;";   																	        //порядок сортировки
		msg += IntToStr((*it_shop)->NumShop)+";";                                               //id подразделения
		msg += "0";                                                                             //вместимость гостей


		WriteToFile("hall.csv",msg.Trim());
		idx++;

		THall *hall = new THall();
		hall->id = item.first;
		hall->Name = item.second;
		hall->NumShop = (*it_shop)->NumShop;
		ListHall->Add(hall);

	}

	// Новые залы переместим в конец списка чтоб у старых не сбилась нумерация
	for (TListShop::iterator it_shop = list_shop->begin(); it_shop != list_shop->end(); ++it_shop)
	{
		std::pair<int,UnicodeString> item;
		try
		{
			item = list_new_cat2.at(0);
		}
		catch (...)
		{
			continue;
		}


		UnicodeString msg = "";
		msg += "1;";   						   													//id базы данных
		msg += IntToStr(idx)+";";     															//id зала
		msg += item.second + " " + (*it_shop)->NameShop +";";                                   //название зала
		msg += "3;";   																			//статус
		msg += IntToStr((*it_shop)->NumShop)+";";   									        //id ресторана
		msg += "0;";   																	        //порядок сортировки
		msg += IntToStr((*it_shop)->NumShop)+";";                                               //id подразделения
		msg += "0";                                                                             //вместимость гостей


		WriteToFile("hall.csv",msg.Trim());
		idx++;

		THall *hall = new THall();
		hall->id = item.first;
		hall->Name = item.second;
		hall->NumShop = (*it_shop)->NumShop;
		ListHall->Add(hall);

	}

 	// Новые залы переместим в конец списка чтоб у старых не сбилась нумерация
	for (TListShop::iterator it_shop = list_shop->begin(); it_shop != list_shop->end(); ++it_shop)
	{
		std::pair<int,UnicodeString> item;
		try
		{
			item = list_new_cat3.at(0);
		}
		catch (...)
		{
			continue;
		}


		UnicodeString msg = "";
		msg += "1;";   						   													//id базы данных
		msg += IntToStr(idx)+";";     															//id зала
		msg += item.second + " " + (*it_shop)->NameShop +";";                                   //название зала
		msg += "3;";   																			//статус
		msg += IntToStr((*it_shop)->NumShop)+";";   									        //id ресторана
		msg += "0;";   																	        //порядок сортировки
		msg += IntToStr((*it_shop)->NumShop)+";";                                               //id подразделения
		msg += "0";                                                                             //вместимость гостей


		WriteToFile("hall.csv",msg.Trim());
		idx++;

		THall *hall = new THall();
		hall->id = item.first;
		hall->Name = item.second;
		hall->NumShop = (*it_shop)->NumShop;
		ListHall->Add(hall);

	}

     	// Новые залы переместим в конец списка чтоб у старых не сбилась нумерация
	for (TListShop::iterator it_shop = list_shop->begin(); it_shop != list_shop->end(); ++it_shop)
	{
		std::pair<int,UnicodeString> item;
		try
		{
			item = list_new_cat4.at(0);
		}
		catch (...)
		{
			continue;
		}


		UnicodeString msg = "";
		msg += "1;";   						   													//id базы данных
		msg += IntToStr(idx)+";";     															//id зала
		msg += item.second + " " + (*it_shop)->NameShop +";";                                   //название зала
		msg += "3;";   																			//статус
		msg += IntToStr((*it_shop)->NumShop)+";";   									        //id ресторана
		msg += "0;";   																	        //порядок сортировки
		msg += IntToStr((*it_shop)->NumShop)+";";                                               //id подразделения
		msg += "0";                                                                             //вместимость гостей


		WriteToFile("hall.csv",msg.Trim());
		idx++;

		THall *hall = new THall();
		hall->id = item.first;
		hall->Name = item.second;
		hall->NumShop = (*it_shop)->NumShop;
		ListHall->Add(hall);

	}

	message = "End hall (Залы)";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
int ThreadQuery::GetIdHall(int idTable, int idShop)
{
	int Type = 6;

	if ( (idTable >= 231 && idTable <= 240) || ((idTable >= 900 && idTable <= 909)) || ((idTable >= 169 && idTable <= 173)))
	{
	   Type = 1;   ////Яндекс еда
	}
	if (idTable >= 200 && idTable <= 207)
	{
	   Type = 2; ////Delivery club
	}
	if (idTable >= 208 && idTable <= 209)
	{
	   Type = 2; ////Delivery club самовывоз
	}
	if (idTable >= 245 && idTable <= 245)
	{
	   Type = 3; ////Поднеси
	}

	if (idTable >= 177 && idTable <= 179)
	{
	   Type = 7;  ////Домиленд
	}
	if (idTable >= 174 && idTable <= 176)
	{
	   Type = 8;  ////Myply
	}
	if (idTable >= 163 && idTable <= 168)
	{
	   Type = 9;  ////Яндекс-еда самовынос
	}
	if (idTable >= 180 && idTable <= 184)
	{
	   Type = 10;  ////Доставка с сайта  Пешком
	}
	if ((idTable >= 185 && idTable <= 195) || (idTable >= 920 && idTable <= 929) || (idTable >= 951 && idTable <= 960))
	{
	   Type = 11;  ////Доставка с сайта  Автомобиль
	}
	if ((idTable >= 196 && idTable <= 199) || (idTable >= 930 && idTable <= 934)  || (idTable >= 935 && idTable <= 940) )
	{
	   Type = 12;  ////Доставка с сайта  Самовынос
	}
	if (idTable >= 221 && idTable <= 224)
	{
	   Type = 13;  ////Звонок гостя, Доставка пешком
	}
	if (idTable >= 215 && idTable <= 220)
	{
	   Type = 14;  ////Звонок гостя, Доставка авто
	}
	if (idTable >= 146 && idTable <= 150)
	{
	   Type = 15;  ////Звонок гостя, Самовынос
	}
	if (idTable >= 210 && idTable <= 213)
	{
	   Type = 16;  //WA, Доставка пешком
	}
	if (idTable >= 225 && idTable <= 230)
	{
	   Type = 17;  //WA, Доставка авто
	}
	if (idTable >= 151 && idTable <= 155)
	{
	   Type = 18;  //WA, Самовынос
	}
	if ((idTable >= 156 && idTable <= 160) || idTable == 214 || (idTable >= 241 && idTable <= 244))
	{
	   Type = 19;  //Сам пришел, сам унес
	}
	if (idTable >= 251 && idTable <= 254)
	{
	   Type = 20;  //Кофе с собой
	}
	if (idTable >= 246 && idTable <= 248)
	{
	   Type = 21;  ////ASAP (теперь КофеПопути)
	}

	if (CheckTableOfVeranda(idTable,idShop))
	{
	   Type = 22;  //Веранда
	}


    // New
	//if (idTable >= 951 && idTable <= 960)
	//{
	//   Type = 23;  //Интернет магазин (Филиас-онлайн)
	//}


	if (CheckTableOfBar(idTable,idShop))
	{
	   Type = 24;  //Бар
	}

	//if (idTable >= 941 && idTable <= 942)
	if (idTable >= 941 && idTable <= 944)
	{
	   Type = 25;  ////мир сити
	}

	if (idTable >= 945 && idTable <= 950)      // Added 08.02
	{
	   Type = 27;  ////Бронибой
	}


	if ((idTable >= 100 && idTable <= 102) || (idTable >= 104 && idTable <= 107))
	{
	   Type = 26;  //Без Тарелок
	}

	for (int i = 0; i < ListHall->Count; i++)
	{
		THall *hall = (THall*)ListHall->Items[i];

		if (hall->id == Type && hall->NumShop == idShop)
		{
			return i+1;
		}

	}

}
//---------------------------------------------------------------------------
bool ThreadQuery::CheckTableOfBar(int idTable, int idShop)
{
	bool result = false;
	switch (idShop)
	{
		case 270 : //Весна
		{
			if ((idTable >= 72 && idTable <= 78))
			{
			   result = true;
			}
			break;
		}
	}
}
//---------------------------------------------------------------------------
bool ThreadQuery::CheckTableOfVeranda(int idTable, int idShop)
{
	bool result = false;
	switch (idShop)
	{
		case 205 : //Аврора
		{
			if ((idTable >= 50 && idTable <= 54) || (idTable >= 35 && idTable <= 42) )
			{
			   result = true;
			}
			break;
		}
		case 371 : //Новая площадь
		{
			if ((idTable >= 701 && idTable <= 716))
			{
			   result = true;
			}
			break;
		}
		case 390 : //Белая площадь
		{
			if ((idTable >= 97 && idTable <= 127))
			{
			   result = true;
			}
			break;
		}
		case 177 : //Неглинная
		{
			if ((idTable >= 101 && idTable <= 112))
			{
			   result = true;
			}
			break;
		}
		case 255 : //Жажда Крови
		{
			if ((idTable >= 31 && idTable <= 51) )
			{
			   result = true;
			}
			break;
		}
		case 180 : //Осенняя
		{
			if ((idTable >= 70 && idTable <= 90))
			{
			   result = true;
			}
			break;
		}
		case 104 : //Кофейня на Никитской
		{
			if ((idTable >= 70 && idTable <= 103))
			{
			   result = true;
			}
			break;
		}
		case 111 : //Четыре Ветра
		{
			if ((idTable >= 701 && idTable <= 717))
			{
			   result = true;
			}
			break;
		}
		case 130 : //Кофейня на Кудринской
		{
			if ((idTable >= 50 && idTable <= 63))
			{
			   result = true;
			}
			break;
		}
		case 260 : //Якиманка
		{
			if ((idTable >= 30 && idTable <= 38) || (idTable >= 40 && idTable <= 43) || (idTable >= 50 && idTable <= 52))
			{
			   result = true;
			}
			break;
		}

		case 270 : //Весна
		{
			if ((idTable >= 40 && idTable <= 51) || (idTable >= 60 && idTable <= 71) )
			{
			   result = true;
			}
			break;
		}
		case 235 : //Павлово
		{
			if ((idTable >= 37 && idTable <= 57) )
			{
			   result = true;
			}
			break;
		}
		case 285 : //Усачевский
		{
			if ((idTable >= 40 && idTable <= 53) || (idTable >= 60 && idTable <= 64) || (idTable >= 70 && idTable <= 74))
			{
			   result = true;
			}
			break;
		}
		case 290 : //Кофейня в ТРЦ Хорошо
		{
			if ((idTable >= 30 && idTable <= 43))
			{
			   result = true;
			}
			break;
		}
		case 295 : //КОМСОМОЛЬСКИЙ
		{
			if ((idTable >= 50 && idTable <= 66) || (idTable >= 24 && idTable <= 29) )
			{
			   result = true;
			}
			break;
		}
		case 310 : //Ресторан Внуково Аутлет
		{
			if ((idTable >= 41 && idTable <= 66))
			{
			   result = true;
			}
			break;
		}
		case 311 : //Новая Рига Аутлет
		{
			if ((idTable >= 60 && idTable <= 77))
			{
			   result = true;
			}
			break;
		}
		case 350 : //Метрополис
		{
			if ((idTable >= 49 && idTable <= 64))
			{
			   result = true;
			}
			break;
		}
		case 370 : //Кутузовский
		{
			if ((idTable >= 100 && idTable <= 138))
			{
			   result = true;
			}
			break;
		}
		case 375 : //Тверская
		{
			if ((idTable >= 35 && idTable <= 46))
			{
			   result = true;
			}
			break;
		}

		case 395 : //Покровка
		{
			if ((idTable >= 27 && idTable <= 55))
			{
			   result = true;
			}
			break;
		}
		case 122 : //Wall Street
		{
			if ((idTable >= 51 && idTable <= 72) )
			{
			   result = true;
			}
			break;
		}
		case 280 : //Отель
		{
			if ((idTable >= 41 && idTable <= 57) )
			{
			   result = true;
			}
			break;
		}
		case 195 : //Вишневый Сад
		{
			if ((idTable >= 101 && idTable <= 135) )
			{
			   result = true;
			}
			break;
		}
		case 190 : //Депо
		{
			if ((idTable >= 33 && idTable <= 52) )
			{
			   result = true;
			}
			break;
		}
	}

	return result;
}
//---------------------------------------------------------------------------
void ThreadQuery::table() //Столы
{
	message = "_____________________Start table (Столы) город";
	Synchronize(&UpdateCaption2);

	UniQuery1->SQL->Clear();
	UniQuery1->SQL->Add("SELECT stol FROM chekA WHERE " +GetListShop()+ " GROUP BY stol ORDER BY stol" );
	UniQuery1->Execute();

	UnicodeString msg;



	for (TListShop::iterator it_shop = list_shop->begin(); it_shop != list_shop->end(); ++it_shop)
	{
		UniQuery1->First();
		int CountGuestResult = 0;

		while (!UniQuery1->Eof)
		{
			CountRecord++;
			Synchronize(&UpdateCaptionDelay);

			int capacity = 1;
			UniQuery2->SQL->Clear();
			UniQuery2->SQL->Add("SELECT SeatsNum FROM [Diogen].[dbo].[AlohaTableInfo] WHERE Dep =" + IntToStr(it_shop.operator *()->NumShop) + " AND TableNum = " + UniQuery1->FieldByName("stol")->AsString);
			UniQuery2->Execute();
			if (!UniQuery2->IsEmpty())
			{
				capacity = UniQuery2->FieldByName("SeatsNum")->AsInteger;
			}

			msg = "";
			msg += "1;";   						   															//id базы данных
			msg += UniQuery1->FieldByName("stol")->AsString+";"; 											//id стола
			msg += IntToStr(GetIdHall(UniQuery1->FieldByName("stol")->AsInteger,it_shop.operator *()->NumShop))+";";    //id зала
			msg += UniQuery1->FieldByName("stol")->AsString+";"; 											//номер стола
			msg += IntToStr(capacity)+";";   																//вместимость
			msg += "3;";   																					//статус
			msg += IntToStr(it_shop.operator *()->NumShop);   												//id ресторана


			WriteToFileDelayed("table.csv",msg.Trim());
			UniQuery1->Next();
		}
		Synchronize(&UpdateCaption);

	}

	WriteToFileDelayed_WriteBufferLeftOvers();
	message = "End table (Столы)";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
void ThreadQuery::table_aeroport() //Столы
{
	message = "_____________________Start table (Столы) аэропорт";
	Synchronize(&UpdateCaption2);

	int CountGuestResult = 0;
	UnicodeString msg;
	for (TListShop::iterator it_shop = list_shop->begin(); it_shop != list_shop->end(); ++it_shop)
	{
		int capacity = 1;
		int TableNum = 1;

		msg = "";
		msg += "1;";   						   										//id базы данных
		msg += IntToStr(TableNum)+";"; 												//id стола
		msg += IntToStr(GetIdHall(TableNum,
								  it_shop.operator *()->NumShop))+";";     	 		//id зала
		msg += IntToStr(TableNum)+";"; 											    //номер стола
		msg += IntToStr(capacity)+";";   											//вместимость
		msg += "3;";   																//статус
		msg += IntToStr(it_shop.operator *()->NumShop);   							//id ресторана

		WriteToFileDelayed("table.csv",msg.Trim());
	}

	WriteToFileDelayed_WriteBufferLeftOvers();
	message = "End table (Столы)";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
void ThreadQuery::discount() //Скидки
{
	message = "_____________________Start discount (Скидки)";
	Synchronize(&UpdateCaption2);

	UniQuery2->SQL->Clear();
	UniQuery2->SQL->Add("SELECT Id, AlohaCompId, Name FROM [Diogen].[dbo].[AlohaMenuComps]");
	UniQuery2->Execute();


	while (!UniQuery2->Eof)
	{
		UnicodeString msg;
		msg += "1;"+ 															//id базы данных
		msg += UniQuery2->FieldByName("AlohaCompId")->AsString+";";   			//id скидки
		msg += UniQuery2->FieldByName("Name")->AsString+";";   					//название
		msg += "3;";   															//статус


		WriteToFile("discount.csv",msg.Trim());
		UniQuery1->Next();
	}

	message = "End discount (Скидки)";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
void ThreadQuery::cost() //Себестоимость
{
	message = "_____________________Start cost (Себестоимость)";
	Synchronize(&UpdateCaption2);

	if (no_cost)
	{
		message = "Себестоимость отключена";
		Synchronize(&UpdateCaption2);
		return;
	}

	TDateTime DTS = IncMonth(StartDT, -2);


	UniQuery1->SQL->Clear();
	UniQuery1->SQL->Add("declare @dt1 datetime;set @dt1 = convert(datetime,'"+DTS.FormatString("DD/MM/YYYY")+"',104);declare @dt2 datetime;set @dt2 = convert(datetime,'"+EndDT.FormatString("DD/MM/YYYY")+"',104);SELECT * FROM cost WHERE dt1 >= @dt1 AND dt1 <= @dt2 AND ( "+GetListShop("cost.cod_shop")+");");
	UniQuery1->Execute();

	CountRecord = UniQuery1->RecordCount;
	message = "Count checks in DB: " + IntToStr(CountRecord);
	Synchronize(&UpdateCaption2);

	while (!UniQuery1->Eof)
	{
		CountRecord--;
		Synchronize(&UpdateCaptionDelay);


		UnicodeString LocalCode = UniQuery1->FieldByName("cod_good")->AsString;
		double av_cost_price = UniQuery1->FieldByName("av_cost_price")->AsFloat;

		UniQuery4->SQL->Clear();
		UniQuery4->SQL->Add("SELECT cod_good FROM barcod WHERE bar_cod  = '" + UniQuery1->FieldByName("cod_good")->AsString + "'");
		UniQuery4->Execute();

		if (!UniQuery4->IsEmpty())
		{
			LocalCode = UniQuery4->FieldByName("cod_good")->AsWideString;
		}

		UniQuery3->SQL->Clear();
		UniQuery3->SQL->Add("SELECT * FROM [dbo].[katalog] WHERE cod_good='" + LocalCode + "' AND wflag = 1;");
		UniQuery3->Execute();

		if (!UniQuery3->IsEmpty())
		{
		  double volume = UniQuery3->FieldByName("volume")->AsFloat;

		  if (volume > 0)
		  {
			 av_cost_price = av_cost_price * volume;
		  }

		}

		if (av_cost_price != 0)
		{
			UnicodeString msg;
			msg += "1;"+ 															       //id базы данных
			msg += UniQuery1->FieldByName("dt1")->AsDateTime.FormatString("YYYY-MM")+";";  //месяц в формате ГГГГ-ММ
			msg += ConvertShopNum(UniQuery1->FieldByName("cod_shop")->AsString)+";";   	   //id ресторана
			msg += UniQuery1->FieldByName("cod_good")->AsString+";";   					   //id позиции меню
			msg += ReplaceComma(FloatToStr(av_cost_price));                  		       //себестоимость

			WriteToFileDelayed("cost.csv",msg.Trim());
		}

		UniQuery1->Next();
	}
	Synchronize(&UpdateCaption);
	WriteToFileDelayed_WriteBufferLeftOvers();

	message = "End cost (Себестоимость)";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
void ThreadQuery::ExportData()
{
	message = "_____________________Start ExportData ";
	Synchronize(&UpdateCaption2);

	UniQuery1->SQL->Clear();
	UniQuery1->SQL->Add("SELECT     katalog.name AS Expr1, katalog.massa AS Expr2, barcod.bar_cod AS Expr3");
	UniQuery1->SQL->Add("FROM         katalog LEFT OUTER JOIN");
	UniQuery1->SQL->Add("                      barcod ON katalog.cod_good = barcod.cod_good");
	UniQuery1->SQL->Add("WHERE     (katalog.massa > 0)");
	UniQuery1->Execute();

	CountRecord = UniQuery1->RecordCount;
	message = "Count checks in DB: " + IntToStr(CountRecord);
	Synchronize(&UpdateCaption2);

	UnicodeString msg;
	msg += "bar_cod		";
	msg += "name		";
	msg += "massa		";
	msg += "av_cost_price		";

	WriteToFile("export.txt",msg);


	while (!UniQuery1->Eof)
	{
		CountRecord--;
		Synchronize(&UpdateCaption);

		UniQuery3->SQL->Clear();
		UniQuery3->SQL->Add("SELECT * FROM [dbo].[chekB] WHERE bar_cod = '" + UniQuery1->FieldByName("Expr3")->AsString+"'");

		message = UniQuery3->SQL->Text;
		Synchronize(&UpdateCaption2);

		UniQuery3->Execute();



		if (!UniQuery3->IsEmpty())
		{
			message = "No Emty";
			Synchronize(&UpdateCaption2);

/*
    			UniQuery3->SQL->Clear();
    			UniQuery3->SQL->Add("SELECT  av_cost_price FROM cost WHERE cod_good = " + UniQuery1->FieldByName("Expr3")->AsString);
    			UniQuery3->Execute();
*/


			UnicodeString msg;
	/*		msg.SetLength(200);
			msg.Insert(UniQuery1->FieldByName("Expr3")->AsString, 1);
			msg.Insert(UniQuery1->FieldByName("Expr1")->AsString, 15);
			msg.Insert(UniQuery1->FieldByName("Expr2")->AsString, 60);
			msg.Insert(UniQuery3->FieldByName("av_cost_price")->AsString, 150);
			msg.Insert(char(13), 200);
	*/

			msg += UniQuery1->FieldByName("Expr3")->AsString+";		";
			msg += UniQuery1->FieldByName("Expr1")->AsString+";		";
			msg += UniQuery1->FieldByName("Expr2")->AsString+";		";
			//msg += UniQuery3->FieldByName("av_cost_price")->AsString+" ";

			WriteToFile("export.txt",msg.Trim());
		}

		UniQuery1->Next();
	}




	message = "End ExportData ";
	Synchronize(&UpdateCaption2);
}
//---------------------------------------------------------------------------
UnicodeString ThreadQuery::AddTime(UnicodeString Time, int Interval)
{
	TDateTime  time = StrToTime(Time);
	time=IncMinute(time,Interval);

	return time.FormatString("HH:MM:SS");
}
//---------------------------------------------------------------------------
UnicodeString ThreadQuery::AddDateTime(UnicodeString Date,UnicodeString Time)
{
	TDateTime  DT = StrToDateTime(Date + " " + Time);
	return DT.FormatString("YYYY-MM-DD HH:MM:SS");
}
//---------------------------------------------------------------------------
void ThreadQuery::WriteToFile(UnicodeString NameFile,UnicodeString msg)
{
	UnicodeString Patch = ExtractFilePath(ParamStr(0));
	Patch += "//"+NameFile;


	ofstream file;
	file.open(Patch.c_str(),ios::app);
	if (!file) return ;
	if (!file.is_open()) return ;


	AnsiString	ResultString = msg.Trim();

	setlocale(LC_ALL, "rus");

	file << ResultString << "\n";

	file.close();

}
// !!!! Обязательно при использовании этой функции в конце цикла вызывать WriteToFileDelay_WriteBufferLeftOvers
//      Иначе может потеряться Big Data
 void ThreadQuery::WriteToFileDelayed(UnicodeString NameFile,UnicodeString msg)
{
	file_name_prev = NameFile;

	if(file_msg_buffer != "")
		file_msg_buffer += "\n";

	file_msg_buffer += msg;

	file_msg_count++;
	if(file_msg_count > file_msg_max)
	{
		WriteToFile(NameFile, file_msg_buffer);
		file_msg_buffer = "";
		file_msg_count = 0;
	}
}
 void ThreadQuery::WriteToFileDelayed_WriteBufferLeftOvers()
{
	if(file_name_prev != "")
	{   if(file_msg_buffer != "")
			WriteToFile(file_name_prev, file_msg_buffer);
    	file_msg_buffer = "";
		file_name_prev = "";
		file_msg_count = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall ThreadQuery::UpdateCaption()
{
   Form1->Edit1->Text = IntToStr(CountRecord);
}
void __fastcall ThreadQuery::UpdateCaptionDelay()
{
   caption_now_redraw_ms = MilliSecondOf(Now());
   if(caption_now_redraw_ms > caption_prev_redraw_ms + caption_rewraw_delay_ms
		||
	  caption_now_redraw_ms < caption_prev_redraw_ms)
   {
		Form1->Edit1->Text = IntToStr(CountRecord);
		caption_prev_redraw_ms = caption_now_redraw_ms;
   }
}
//---------------------------------------------------------------------------
void __fastcall ThreadQuery::UpdateCaption2()
{
   UnicodeString msg  =Now().FormatString("DD/MM/YYYY HH:mm:SS")+"     "+message;
   Form1->Memo1->Lines->Add(message);
   WriteToFile("Log MOZG.txt",msg);
}
void __fastcall ThreadQuery::UpdateCaption2NoLog()
{
   Form1->Memo1->Lines->Add(message);
}
