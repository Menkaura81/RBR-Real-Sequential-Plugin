#ifndef __TESTPLUGIN_H_INCLUDED
#define __TESTPLUGIN_H_INCLUDED

// You need to define this before the includes, or use /D __SUPPORT_PLUGIN__=1 
// in the C/C++ / Commandline project configuration
#define __SUPPORT_PLUGIN__ 1

#include "IPlugin.h"
#include "IRBRGame.h"
#include "PluginHelpers.h"
#include "stdlib.h"

#include "Gendef.h" // 2D Gfx header
#include <stdio.h>

const unsigned int NUM_SELECTIONS = 2;

extern int Keys[10];
extern char jname[255];
extern int cindex;
extern bool onoff;
extern int listenid;
extern int numj;
extern bool listening;
extern void SwitchJoystick(HWND hDlg, int jid);
extern HWND hWin;
extern bool KeyDown(DWORD KeyID);
extern void Save();

//------------------------------------------------------------------------------------------------//

class CTestPlugin : public IPlugin
{
public:
	CTestPlugin	( IRBRGame* pGame )	
		:	m_pGame	( pGame )
		,	m_iSelection( 0 )
		,	m_iMap	( 10 )
		,	m_dTime ( 0 )
		,	m_dSec ( 0 )
		,	m_dsTime ( 0 )
		,	m_dsSec ( 0 )
		,	m_iSelections ( 0 )

	{
		FILE *fp = fopen( "rbrplugin.log", "wt" );
		if( fp )
		{
			fflush( fp );
			fclose( fp );
		}
	}

	virtual ~CTestPlugin( void )	
	{
	}

	//------------------------------------------------------------------------------------------------//

	virtual const char* GetName( void )	
	{
		return "RealSequentialPlugin";
	}

	//------------------------------------------------------------------------------------------------//

	virtual void DrawResultsUI( void )
	{
		// Do nothing

	}

	//------------------------------------------------------------------------------------------------//

	virtual void DrawFrontEndPage( void )
	{
		if (listening)
		{
			for (int i = 0; i < 331; ++i)
			{
				if (KeyDown(i) && !(i==28) ) { Keys[listenid] = i; listening = false; }
			}
		}

		m_iSelections = 6;
		m_pGame->DrawBlackOut( 0.0f, 0.0f, 0.0f, 0.0f );
		m_pGame->DrawSelection( 0.0f, 68.0f + ( static_cast< float >( m_iSelection ) * 21.0f ), 250.0f );
		m_pGame->SetMenuColor( IRBRGame::MENU_SELECTION );	
		m_pGame->DrawFlatBox( 250.0f, 49.0f, 3.0f, 271.0f);
		m_pGame->SetMenuColor( IRBRGame::MENU_ICON );		
		m_pGame->SetMenuColor( IRBRGame::MENU_HEADING );	
		m_pGame->SetFont( IRBRGame::FONT_BIG );
		m_pGame->WriteText( 73.0f, 49.0f, "RBR Real Sequential Plugin" );
		
		const char* aszSelections[] =
		{
			"Enabled",
			"Save Changes",
			"Controller",
			"Gear Up",
			"Gear Down",
			"Safety Trigger",
		};

		m_pGame->SetMenuColor( IRBRGame::MENU_TEXT );
		for( unsigned int i = 0; i < m_iSelections; ++i )
		{
			m_pGame->WriteText( 73.0f, 70.0f + ( static_cast< float >( i ) * 21.0f ), aszSelections[ i ] );


			if (i > 2 && i < 6)
			{
				if (Keys[i-3] < 256)
					sprintf(m_szTemp, "Keyboard Button %i", Keys[i-3]);
				if (Keys[i-3] > 256 && Keys[i-3] < 265)
					sprintf(m_szTemp, "Mouse Button %i", Keys[i-3] - 256);
				if (Keys[i-3] > 264)
					sprintf(m_szTemp, "Joystick Button %i", Keys[i-3] - 264);
			}
			else
			{
				if (i == 0)
				{
					if (onoff)
						sprintf(m_szTemp,"True");
					else
						sprintf(m_szTemp,"False");
				}						
				if (i == 1)
				{
					if (m_dsSec)
					{
						if (GetTickCount() - m_dsTime > 1000)
						{
							m_dsTime = GetTickCount();
							--m_dsSec;
						}
						sprintf(m_szTemp, "File Saved...", m_dsSec);
					}
					else
					{
						m_szTemp[0] = 0;
					}
				}
				if (i == 2)
				{
					if (numj > 0)
						sprintf(m_szTemp, "Index: %i  Name: %s", cindex, jname);
					else
						sprintf(m_szTemp, "No Controllers Found");
				}
			}
					
			if (listening && i == listenid+3)
			{
				if (GetTickCount() - m_dTime > 1000)
				{
					m_dTime = GetTickCount();
					--m_dSec;
				}
				if (m_dSec == 0) listening = false;
				sprintf(m_szTemp, "Press A Key... %d", m_dSec);
			}

			m_pGame->WriteText( 260.0f, 70.0f + ( static_cast< float >( i ) * 21.0f ), m_szTemp );
		}		
	}

	//------------------------------------------------------------------------------------------------//

	virtual void HandleFrontEndEvents( char txtKeyboard, bool bUp, bool bDown, 
		bool bLeft, bool bRight, bool bSelect )
	{
		if (listening) return;

		if( bSelect )
		{
			if( m_iSelection > 2 && m_iSelection < 6 )
			{
				m_dSec = 6;
				listenid = m_iSelection-3;
				listening = true;
			}
			else
			{
				if (m_iSelection == 1)
				{
					Save();
					m_dsSec = 3;
				}
			}
		}

		if( bUp )
		{
			--m_iSelection;
			if( m_iSelection < 0 )
				m_iSelection = m_iSelections - 1;
		}
		
		if( bDown )
		{
			++m_iSelection;
			if( m_iSelection >= m_iSelections )
				m_iSelection  = 0;
		}
		//switch joystick
		if( bLeft && m_iSelection == 2 )
		{
			--cindex;
			if (cindex < 0) cindex = 0;
			SwitchJoystick(hWin, cindex);
		}
		if( bRight && m_iSelection == 2 )
		{
			++cindex;
			if (cindex > numj-1) cindex = numj-1;
			SwitchJoystick(hWin, cindex);
		}
		//switch onoff
		if( bLeft && m_iSelection == 0 )
		{
			onoff = !onoff;
		}
		if( bRight && m_iSelection == 0 )
		{
			onoff = !onoff;
		}		
	}

	//------------------------------------------------------------------------------------------------//
	virtual void TickFrontEndPage( float fTimeDelta )
	{
		//Do nothing
	}

	//------------------------------------------------------------------------------------------------//
	/// Is called when the player timer starts (after GO! or in case of a false start)
	virtual void StageStarted ( int iMap, const char* ptxtPlayerName, bool bWasFalseStart )
	{
		// Do nothing
	}

	//------------------------------------------------------------------------------------------------//
	/// Is called when player finishes stage ( fFinishTime is 0.0f if player failed the stage )
	virtual void HandleResults ( float fCheckPoint1, float fCheckPoint2, float fFinishTime, const char* ptxtPlayerName )
	{
		//Do nothing
	}

	//------------------------------------------------------------------------------------------------//
	/// Is called when a player passed a checkpoint 
	virtual void CheckPoint ( float fCheckPointTime, int   iCheckPointID, const char* ptxtPlayerName )
	{
		// Do nothing
	}

private:
	IRBRGame*		m_pGame;
	int				m_iSelection;
	int				m_iSelections;
	int				m_iMap;
	DWORD			m_dTime;
	DWORD			m_dSec;
	DWORD			m_dsTime;
	DWORD			m_dsSec;
	char			m_szTemp[255];
	float			m_fResults[ 3 ];
	char			m_szPlayerName[ 32 ];
};

#endif