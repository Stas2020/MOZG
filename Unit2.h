//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "Unit1.h"
#include <vector>
#include <map>
#include "Uni.hpp"
#include "UniProvider.hpp"
#include "SQLServerUniProvider.hpp"
#include <fstream>
//---------------------------------------------------------------------------
typedef struct
{
		int NumShop;
		TDateTime BusinessDate;
		double sum_plast;
		double sum_cash;
		double sum_30;
		double sum_card_present;
}TShopPayment;



//---------------------------------------------------------------------------

typedef struct
{
	int NumTable;
	TDateTime Start;
	TDateTime End;
	UnicodeString NumCheck;
	int NumShop;
	int CountGuest;
	int idWaiter;
	TDateTime BusinessDate;
	double SumPayment;
	int TypeOrder;
	UnicodeString Discription;
	int id_discount;
	UnicodeString cas_n;
	int TableId;

}TCheck;
typedef std::vector<TCheck*> TListCheck;


typedef struct
{
	int NumTable;
    UnicodeString NumOrder;
	UnicodeString NumFirstCheck;
	UnicodeString NumLastCheck;
	int NumShop;
	int CountGuest;
	int idWaiter;
	int TypeOrder;
	TDateTime Start;
	TDateTime End;
	TDateTime BusinessDate;
	double SumPaymentOrder;
    int TableId;

	TListCheck *list_check;

} TOrder;
typedef std::vector<TOrder*> TListOrder;


typedef struct
{
	int id;
	UnicodeString Name;

}TTypeOrder;
typedef std::vector<TTypeOrder*> TListTypeOrder;



typedef struct
{
	int id;
	double amount;
	double part;
}TPayment;


typedef struct
{
	int NumShop;
	UnicodeString NameShop;
}TShop;
typedef std::vector<TShop*> TListShop;


typedef struct
{
	int id;
	UnicodeString Name;
	int NumShop;

}THall;

typedef struct
{
	UnicodeString id;
	UnicodeString desc;

}TWriteoff_reason;

typedef std::vector<std::shared_ptr<TWriteoff_reason>> TListTWriteoff_reason;

// <TableId,CountGuest>
typedef std::map<int,int> TListGuestOfTebleId;
// <DepNum,CountGuest in table>
typedef std::map<int,TListGuestOfTebleId*> TListDepNum;

typedef std::map<TDateTime,TListDepNum*> TListDateTime;

typedef std::pair<int,UnicodeString> TPairData;


class ThreadQuery : public TThread
{
private:
	TList *List_pbnd;
	TList *List_pmnt;

	TListOrder *list_order;
	TListShop *list_shop;
	TListTypeOrder *list_typeorder;

	TList *ListHall;

    TStringList *list_group_menu;

	int CountRecord;
	UnicodeString message;
	void __fastcall UpdateCaption();
	void __fastcall UpdateCaption2();

	void cot(); //Типы заказа
	void cot_aeroport();
	void order(); //Заказы
	void pbnd(); //Проводки
	void item_size(); //Приказы (типы скидок)
	void curr(); //  Методы оплат
	void item(); //  Позиции меню
    void group(); // Группы меню
	void rest(); //Рестораны
    void rest_aeroport(); //Рестораны
	void rcpt(); //Чеки
	void writeoff();//Списания
	void writeoff_reason2(); //справочник причин списания 1
	void empl(); //Сотрудники
	void pmnt(); //Платежи
	void cost(); //Себестоимость

	void hall(); // Залы
	void table(); //Столы
	void table_aeroport(); //Столы

	void discount(); //Скидки

	void __fastcall exit(); //exit

	UnicodeString GetReason1ByReason2(int reason2);


	bool CheckTableOfVeranda(int idTable, int idShop);
	bool CheckTableOfBar(int idTable, int idShop);

    UnicodeString GenerateUID(void);
	void AddCheckToOrderList(TCheck *check);
    void AddCheckToOrderListAeroport(TCheck *check);
	void AddCheckToOrderListTabId(TCheck *check);
	bool DateTimeInRange_(TDateTime DT,	TDateTime Start, TDateTime End);
	TDateTime ConvertToDateTime(TDateTime DT, UnicodeString Time);
	TDateTime AddTime(TDateTime DT,UnicodeString Time, int Interval);
	void SetTypeOrder(TCheck *check);
	int GetIntervalDateTime(TDateTime Start, TDateTime End);

	TList *GetPayments(UnicodeString check_sn, UnicodeString cas_n);
	int GetPrePayments(int payment_id);

	UnicodeString GetListShop();
	UnicodeString GetListShop(UnicodeString Name_field);
	int GetIdHall(int idTable, int idShop);

	UnicodeString ConvertShopNum(UnicodeString value);
	UnicodeString ConvertShopNum(int value);
    UnicodeString ConvertShopNumForGuest(int value);

	TUniConnection *UniConnection1;
	TUniConnection *UniConnection2;
	TUniQuery *UniQuery1;
	TUniQuery *UniQuery2;
	TUniQuery *UniQuery3;
    TUniQuery *UniQuery4;
	bool ConnectSQL1();
	bool ConnectSQL2();

	void WriteToFile(UnicodeString NameFile, UnicodeString msg);
	UnicodeString AddTime(UnicodeString Time, int Interval);
	UnicodeString AddDateTime(UnicodeString Date,UnicodeString Time);

	TListDateTime *ListDateTime;
	void GetAllGuestFromDb();
	int GetCountGuest(TDateTime BusinessDate, int DepNum, int TableId);
	int GetCountGuestAeroport(TDateTime BusinessDate, int DepNum, int TableId);

	TStringList *ListPartionRatio;
	void AddPartionRatio(UnicodeString Barcode);
	bool CheckPartionRatio(UnicodeString Barcode);


	UnicodeString ReplaceComma(UnicodeString value);

	void ExportData();

	void AddData_pbnd(int NumShop,TDateTime b_date, int payment_id, double payment);
	void AddData_pmnt(int NumShop,TDateTime b_date, int payment_id, double payment);
	void ShowData_pbnd();
	void ShowData_pmnt();



protected:
	void __fastcall Execute();
public:
		TDateTime StartDT;
		TDateTime EndDT;
		bool Close;
		UnicodeString type_export;
		bool no_guest;

	__fastcall ThreadQuery(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
