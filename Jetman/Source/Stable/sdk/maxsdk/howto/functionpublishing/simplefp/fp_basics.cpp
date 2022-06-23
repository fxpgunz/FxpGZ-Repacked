#include "fp_basics.h"

const Class_ID fp_basics_CLASS_ID(0x3f869fdf, 0x63e46b8e);

//================================================
//Global utility plugin skeleton
//================================================
class fp_basic_gup : public GUP {
	public:

		static HWND hParams;

		// GUP Methods
		virtual DWORD	Start	( );
		virtual void	Stop	( );
		virtual void DeleteThis ( );
		DWORD_PTR	Control			( DWORD parameter );
		
		// Loading/Saving
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

		//Constructor/Destructor
		fp_basic_gup();
		~fp_basic_gup();		
};
//------------------------------------
//member implementations
//------------------------------------
fp_basic_gup::fp_basic_gup()
{

}
fp_basic_gup::~fp_basic_gup()
{

}

DWORD fp_basic_gup::Start( ) {
	return GUPRESULT_KEEP; // Activate and Stay Resident
}

void fp_basic_gup::Stop( ) {
	// TODO: Do plugin un-initialization here
}

void fp_basic_gup::DeleteThis() {
	delete this;
}

DWORD_PTR fp_basic_gup::Control( DWORD ) {
	return 0;
}

IOResult fp_basic_gup::Save(ISave* ) {
	return IO_OK;
}

IOResult fp_basic_gup::Load(ILoad* ) {
	return IO_OK;
}

//================================================
//Our class descriptor
//================================================
class fp_basicsClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL) { return new fp_basic_gup(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return GUP_CLASS_ID; }
	Class_ID		ClassID() { return fp_basics_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("fpbasics"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle
	

};

static fp_basicsClassDesc fp_basicsDesc;
ClassDesc2* Getfp_basicsDesc() { return &fp_basicsDesc; }


//================================================
//Function Publishing implementations
//================================================


const Interface_ID FP_BASIC_INTERFACE(0x7d0c759f, 0x7714b4b);

//This is a contructor for class FP_Basic that takes a variable number of arugments.
FP_Basic fp_basic_desc(
	FP_BASIC_INTERFACE, //Interface_ID
	_T("fpbasics"),		//Internal Fixed Name
	IDS_CLASS_NAME,		//localized string resource ID
	&fp_basicsDesc,		//owning class descriptor
	FP_CORE,			//Flags

		//Functions -------------------------
		//Function that takes two numbers and multiplies them together
		//Note the scripter visible name of the function is passed in as a string
		IFP_Basic::em_products, _T("products"), 0, TYPE_FLOAT, 0, 2,
			_T("float_X"), 0, TYPE_FLOAT,
			_T("float_Y"), 0, TYPE_FLOAT,

		//Function that displays a message box
		IFP_Basic::em_message, _T("Message"), 0, TYPE_VOID, 0, 0,

		//Properties ------------------------
		//Property description that can has read / write functionality
		properties,
			IFP_Basic::em_getNum, IFP_Basic::em_setNum, _T("Number"), 0 , TYPE_FLOAT,
			IFP_Basic::em_getPosition, IFP_Basic::em_setPosition, _T("Position"), 0, TYPE_POINT2_BV,
			IFP_Basic::em_getLocation, IFP_Basic::em_setLocation, _T("Location"), 0, TYPE_POINT2_BR,
		p_end
);

// Custom initialization Method called when FP_Basic is constructed
void FP_Basic::init()
{
	// Do something here
	m_Num = 0.0f;
}

//--------------------------------------------------------
//Maxscript usage:
//--------------------------------------------------------
//fpbasics.products 2.5 4
//--> 10.0
float FP_Basic::products(float x, float y)
{
	return x * y;
}

//--------------------------------------------------------
//Maxscript usage:
//--------------------------------------------------------
//fpbasics.message()
//--> A standard Windows message box will then appear.
void FP_Basic::message()
{
	MessageBox(NULL, _T("This was called via a void member function."), _T("Function Publishing Demonstration"), MB_OK);
}

//--------------------------------------------------------
//Maxscript usage:
//--------------------------------------------------------
//fpbasics.number
//--> 0.0
float FP_Basic::GetNum()
{
	return m_Num;
}

//--------------------------------------------------------
//Maxscript usage:
//--------------------------------------------------------
//fpbasics.number
//--> 0.0
//fpbasics.number = 4.5
//--> 4.5
//fpbasics.number
//--> 4.5
void FP_Basic::SetNum(float x)
{
	m_Num = x;
}

//--------------------------------------------------------
//Maxscript usage:
//--------------------------------------------------------
//fpbasics.position
//--> 0.0
//fpbasics.position = point2 45 67
//--> [45,67]
Point2 FP_Basic::GetPosition()
{
	return m_Point;
}

//--------------------------------------------------------
//Maxscript usage:
//--------------------------------------------------------
//fpbasics.position
//--> 0.0
//fpbasics.position = point2 45 67
//--> [45,67]
void FP_Basic::SetPosition( Point2 pos )
{
	m_Point = pos;
}
//--------------------------------------------------------
//Maxscript usage:
//--------------------------------------------------------
//fpbasics.location
//--> 0.0
//fpbasics.location = point2 45 67
//--> [45,67]
Point2& FP_Basic::GetLocation()
{
	return m_Location;
}
//--------------------------------------------------------
//Maxscript usage:
//--------------------------------------------------------
//fpbasics.location
//--> 0.0
//fpbasics.location = point2 45 67
//--> [45,67]
void FP_Basic::SetLocation( Point2& pos )
{
	m_Location = pos;
}