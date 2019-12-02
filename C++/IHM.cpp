//*********************************************************************************************
//* Programme : TP_Sondeur_GPS.cpp date : 18/11/2019
//*---------------------------------------------------------------------------------------------------------
//* derni�re mise a jour : 02/12/2019
//*
//*Programmeurs : Fontaine Thibaud/Cosman Vincent/ Jouen Matthias
//*classe : BTSSN2
//*--------------------------------------------------------------------------------------------------------
//* BUT :  Ce programme permet de recpuerer des trames envoy�es par un GPS et de les
//* traiter afin d'en tirer des informations sur nottament des coordonn�es,
//* une temp�rature et une profondeur...
//*Programmes associ�s : AUCUN
//*********************************************************************************************
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "IHM.h"
#include <string.h>
#include <deque>
#include <vector>
#include <sstream>
#define __WIN__
#include "mysql.h"//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
// Thread permettant de recuperer les donn�es du gps en boucle afin d'avoir
// toutes les trames en meme temps
DWORD WINAPI TForm1::RecvThread(LPVOID params)
{
	TForm1 * form = (TForm1*)params;

	while(1)
	{
		WaitForSingleObject(form->sync, INFINITE);
		form->port->lireport();

		std::deque<char> & buf = form->port->getRecvBuf();
		std::vector<char> trame;

		bool bFound = false;
		int idx = -1;
		// test si on est en fin de ligne de trame
		for(int i = 0; i < buf.size(); i++)
		{
			trame.push_back(buf[i]);
			if(buf[i] == '\n' || buf[i] == '\r')
			{
				idx = i + 1;
				bFound = true;
				break;
			}
		}
		// si on est en fin de ligne supprime la 1ere valeur
		if(bFound)
		{
			for(int i = 0; i < idx; i++)
			{
				buf.pop_front();
			}

			while(buf.size() > 0 && buf[0] != '$')
			{
                buf.pop_front();
            }

			form->trames.push_back(trame);
        }

		ReleaseMutex(form->sync);
	}
}

//---------------------------------------------------------------------------
// ouverture de l'IHM
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	port = new PortRS();
	//cacher les label et les M�mos
	Label1->Visible = false;
	Label2->Visible = false;
	Label3->Visible = false;
	Label4->Visible = false;
	Label5->Visible = false;
	MemoGeant->Visible = false;
	Lati->Visible = false;
	Longi->Visible = false;
	Temp->Visible = false;
	Profond->Visible = false;
	sync = CreateMutex(NULL, FALSE, NULL);

}
//---------------------------------------------------------------------------

// bouton ouvrir port
// permet la connexion avec le port COM
void __fastcall TForm1::OuvrirPort1Click(TObject *Sender)
{
	int error;
	error = port->ouvrirport();  //ouverture du port
	if(error != 0)
	{
		char buffer[20];

		itoa(GetLastError(),buffer,10);   // here 10 means decimal
		MessageBox(NULL, buffer, "Fail", 0);
	}
	Button3->Enabled = false;
	//affichage des Labels et des M�mos
	Label1->Visible = true;
	Label2->Visible = true;
	Label3->Visible = true;
	Label4->Visible = true;
	Label5->Visible = true;
	MemoGeant->Visible = true;
	Lati->Visible = true;
	Longi->Visible = true;
	Profond->Visible = true;
    Temp->Visible = true;


	DWORD threadId;
	CreateThread(NULL, 0, &RecvThread, this, 0, &threadId);
	Timer1->Enabled = true;
}
//---------------------------------------------------------------------------
// fonction permettant de recuperer les donn�es depuis le port RS232
void __fastcall TForm1::LirePort1Click(TObject *Sender)
{
	int i=0;
	port->lireport();
	std::deque<char> & buf = port->getRecvBuf();

	UnicodeString str = "";
	MemoGeant->Visible = true;
}

//---------------------------------------------------------------------------

// fermeture de l'IHM
void __fastcall TForm1::FermerPort1Click(TObject *Sender)
{
	Button4->Enabled = false;
	Button3->Enabled = true;
	Form1->Close();
}
//---------------------------------------------------------------------------

// Traduction de la trame pour r�cuperer uniquement les coordonn�es
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
	WaitForSingleObject(sync, INFINITE);

	//creation des variables qui contiendront les valeurs
	UnicodeString longitudedegres = "";
	UnicodeString longitudesecondes = "";
	UnicodeString longitudeminutes = "";
	UnicodeString latitudedegres = "";
	UnicodeString latitudesecondes = "";
	UnicodeString latitudeminutes = "";
	UnicodeString Profondeur = "";
	UnicodeString Temperature = "";
	UnicodeString str = "";

	std::stringstream stringbuilder;


	for(int j = 0; j < trames.size(); j++)
	{
		std::vector<char> buf = trames[j];
		int i = 0;
		do
		{
        //Traitement de la trame GPGGA qui contient les valeurs des coordonn�es
		// verification de la bonne trame puis d�coupage
		if (buf[i] == '$' && buf[i+1] == 'G' && buf[i+2] == 'P' && buf[i+3] == 'G' && buf[i+4] == 'G' && buf[i+5] == 'A') {
			int j=i+6;
			if (buf[j]== ','){
				int z=j+8;
				for (int k = 0; k < 10; k++)
				{
					if (k==1){
						latitudedegres +=buf[z-1];
					}
					if (k==2) {
					  // z++;
						latitudedegres +=buf[z-1];
					}
					if (k==3)  {
						latitudeminutes += buf[z-1];
					}
					if (k==4) {
						latitudeminutes += buf[z-1];
						latitudeminutes = (latitudeminutes/60);
					}
					if (k==6) {
						latitudesecondes += buf[z-1];
					}
					if (k==7) {
						latitudesecondes += buf[z-1];
						latitudesecondes = (latitudesecondes/3600);
					}
					if (k==9) {
						z++;
					}
					z++;
				}
				z++;
				//ajout des trames d�coup�s dans une nouvelle variable
				latitude = (latitudedegres.ToDouble()+latitudeminutes.ToDouble()+latitudesecondes.ToDouble());
				for (int k = 10; k < 21; k++)
				{

					if (k==11){
						longitudedegres +=buf[z-1];
					}
					if (k==12) {
						longitudedegres +=buf[z-1];
					}
					if (k==13)  {
						longitudedegres += buf[z-1];
					}
					if (k==14) {
						longitudeminutes += buf[z-1];
					}
					if (k==15) {
						longitudeminutes += buf[z-1];
						longitudeminutes = (longitudeminutes/60);
					}
					if (k==17) {
						longitudesecondes += buf[z-1];
					}
					if (k==18) {
						longitudesecondes +=buf[z-1];
						longitudesecondes = (longitudesecondes/3600);
					}
					z++;
				}
			//ajout des trames d�coup�s dans une nouvelle variable
			longitude = (longitudedegres.ToDouble()+longitudeminutes.ToDouble()+longitudesecondes.ToDouble());
			}
		// ajout des coordonn�es dans 2 memo diff�rent
		Lati->Lines->Add(latitude);
		Longi->Lines->Add(longitude);
		}

	//Traitement de la trame SDMTW qui contient la valeur de la Temperature
	if (buf[i] == '$' && buf[i+1] == 'S' && buf[i+2] == 'D' && buf[i+3] == 'M' && buf[i+4] == 'T' && buf[i+5] == 'W'){
		int j = i+7;
		for (int t = 0; t < 4; t++)
		{
		   Temperature += buf[j];
		   j++;
		}
	Temp->Lines->Add(Temperature); //ajout de la valeur temperature dans le memo
	}

	//Traitement de la trame SDDPT qui contient la valeur de la Profondeur
	if (buf[i] == '$' && buf[i+1] == 'S' && buf[i+2] == 'D' && buf[i+3] == 'D' && buf[i+4] == 'P' && buf[i+5] == 'T'){
		int a = i+9;
		for (int p = 0; p < 2; p++)
		{
		   Profondeur += buf[a];
		   a++;
		}
	Profond->Lines->Add(Profondeur); //affichage de la profondeur dans le memo
	}

	str += buf[i];
	i++;

	}while (i < buf.size());

	MemoGeant->Lines->Add(str); //affichage de toutes les trames dans le grand memo
	}

	trames.clear();

	ReleaseMutex(sync);
}


//---------------------------------------------------------------------------


