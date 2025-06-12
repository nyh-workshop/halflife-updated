/***
 *
 *	Copyright (c) 1996-2025, Valve LLC. All rights reserved.
 *
 *	This product contains software technology licensed from Id
 *	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
 *	All Rights Reserved.
 *
 *   Use, distribution, and modification of this source code and/or resulting
 *   object code is restricted to non-commercial enhancements to products from
 *   Valve LLC.  All other use, distribution, or modification is prohibited
 *   without written permission from Valve LLC.
 *
 ****/

//
// stamina.cpp
//
// implementation of CHudStamina class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include "extdll.h"
#include "util.h"

#include <string.h>
#include <stdio.h>

DECLARE_MESSAGE(m_Stamina, Stamina)

bool CHudStamina::Init()
{
	m_iStamina = 0;

	m_drawStaminaMeter = false;

	HOOK_MESSAGE(Stamina);

	m_iFlags |= HUD_ACTIVE;

	gHUD.AddHudElem(this);

	// Possibly the "Alert" function not initialized, HUD inits at game startup!
	// ALERT(at_console, "Init stamina at HUD!\n");

	return true;
}

void CHudStamina::Reset()
{
	m_iStamina = 0;
	m_drawStaminaMeter = false;
}

bool CHudStamina::VidInit()
{
	// All these must be loaded from "sprites/hud.txt"!
	int spriteIndex0 = gHUD.GetSpriteIndex("h_stamina_icon");
	m_hSprite1 = gHUD.GetSprite(spriteIndex0);
	m_prc1 = &gHUD.GetSpriteRect(spriteIndex0);

	int spriteIndex1 = gHUD.GetSpriteIndex("h_stamina_fullBar");
	m_hSprite_bars = gHUD.GetSprite(spriteIndex1);
	m_prc_fullBar = &gHUD.GetSpriteRect(spriteIndex1);

	int spriteIndex2 = gHUD.GetSpriteIndex("h_stamina_emptyBar");
	m_hSprite_bars = gHUD.GetSprite(spriteIndex2);
	m_prc_emptyBar = &gHUD.GetSpriteRect(spriteIndex2);

	// Default for empty bar!
	m_prc_emptyBar->top = 0;
	m_prc_emptyBar->bottom = 32;
	m_prc_emptyBar->left = 0;
	m_prc_emptyBar->right = 128;

	// Default for full bar!
	m_prc_fullBar->top = 30;
	m_prc_fullBar->bottom = 60;
	m_prc_fullBar->left = 0;
	m_prc_fullBar->right = 128;
	
	return true;
}

bool CHudStamina::Draw(float flTime)
{
	int r, g, b, x, y, a;

	y = ScreenHeight - 84;
	x = 16;

	a = 128;
	
	UnpackRGB(r, g, b, RGB_YELLOWISH);

	ScaleColors(r, g, b, a);
	
	if (m_iStamina < 100)
	{
		SPR_Set(m_hSprite1, r, g, b);
		SPR_DrawAdditive(0, x, y, m_prc1);

		// Draw the empty bar first!
		SPR_Set(m_hSprite_bars, r, g, b);
		SPR_DrawAdditive(0, x + 48, y, m_prc_emptyBar);

		// Then fill it with the full bar, that is drawn partially, or fully. :D
		if (m_iStamina <= 100 && m_iStamina > 66)
		{
			// Full bar:
			m_prc_fullBar->right = 127;
		}
		else if (m_iStamina <= 66 && m_iStamina > 33)
		{
			// Two thirds:
			m_prc_fullBar->right = 84;
		}
		else if (m_iStamina <= 33 && m_iStamina > 0)
		{
			// One thirds:
			m_prc_fullBar->right = 44;
		}
		else
		{
			// Empty:
			m_prc_fullBar->right = 1;
			
		}
		SPR_Set(m_hSprite_bars, r, g, b);
		SPR_DrawAdditive(0, x + 48, y, m_prc_fullBar);
	}
	return true;
}

bool CHudStamina::MsgFunc_Stamina(const char* pszName, int iSize, void* pbuf)
{
	// Retrieve stamina amount from server.
	// Maybe call it as "GetStaminaAmountAtClientSide" ?
	// At UserMessages.h, it needs 12 characters for these kind of messages,
	// else it'll be complaining about "bogus calls"!
	// https://hlcoders.valvesoftware.narkive.com/epXjHYX8/bogus-message-type-0#
	BEGIN_READ(pbuf, iSize);
	m_iStamina = READ_BYTE();
	
	return true;
}
