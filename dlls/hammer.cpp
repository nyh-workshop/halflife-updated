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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "gamerules.h"

LINK_ENTITY_TO_CLASS(weapon_hammer, CHammer);

void CHammer::Spawn()
{
	Precache();
	m_iId = WEAPON_HAMMER;
	SET_MODEL(ENT(pev), "models/v_hammer.mdl");
	m_iClip = -1;

	FallInit(); // get ready to fall down.
}

void CHammer::Precache()
{
	PRECACHE_MODEL("models/v_hammer.mdl");
	PRECACHE_MODEL("models/p_hammer.mdl");
	PRECACHE_SOUND("sh15/ham_hitw.wav");
	PRECACHE_SOUND("sh15/ham_hitbod1.wav");
	PRECACHE_SOUND("sh15/ham_hitbod2.wav");
	PRECACHE_SOUND("sh15/ham_hitbod3.wav");
	PRECACHE_SOUND("sh15/ham_swing.wav");

	m_usHammer = PRECACHE_EVENT(1, "events/hammer.sc");
}

bool CHammer::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iId = WEAPON_HAMMER;
	p->iWeight = HAMMER_WEIGHT;
	return true;
}

bool CHammer::Deploy()
{
	isHammerLifted = false;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 5.0f, 8.0f);
	return DefaultDeploy("models/v_hammer.mdl", "models/p_hammer.mdl", HAMMER_DRAW, "hammer");
}

void CHammer::Holster()
{
	// Gotta try to fix that once it loads well without crashing!
	isHammerLifted = false;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim(HAMMER_HOLSTER);
}

void CHammer::ChargePrimaryAttack()
{
	bool inPriAttack = (m_pPlayer->pev->button & IN_ATTACK) == IN_ATTACK;
	// ALERT(at_console, "attack button status: %d\n", (int)(inPriAttack));
	if (inPriAttack)
	{
		// Lift that hammer to charge, don't swing it yet!
		// Calculate damage from charging time there!
		float timeFromFirstHammerLift = gpGlobals->time - m_flHammerFirstLiftedTime;

		if (timeFromFirstHammerLift >= 2.00f)
		{
			ALERT(at_console, "lifted more than 2.0secs!\n");
			m_iChargedHammerDamageMultiplier = LVL2_HAMMER_CHARGED_DMG_MULTIPLIER;
		}
		else if (timeFromFirstHammerLift >= 1.0f)
		{
			ALERT(at_console, "lifted more than 1.0secs!\n");
			m_iChargedHammerDamageMultiplier = LVL1_HAMMER_CHARGED_DMG_MULTIPLIER;
		}
		else
		{
			m_iChargedHammerDamageMultiplier = DEFAULT_HAMMER_CHARGED_DMG_MULTIPLIER;
		}

		pev->nextthink = gpGlobals->time + 0.10f;
	}
	else
	{
		// Release it now!
		SetThink(&CHammer::isSwingingAndCheckHit);
		pev->nextthink = gpGlobals->time + 0.1f;
		m_flNextPrimaryAttack = GetNextAttackDelay(HAMMER_NEXT_ATTACK_DELAY);
		isHammerLifted = false;
		m_flHammerFirstLiftedTime = 0.00f;
	}
}

void CHammer::ChargeSecondaryAttack()
{
	// Attack2 button scanning at afButtonPressed taken from gauss.cpp!
	bool inSecAttack = (m_pPlayer->m_afButtonPressed & IN_ATTACK2) == IN_ATTACK2;
	if (inSecAttack)
	{
		// Lift that hammer to charge, don't swing it yet!
		// Calculate damage from charging time there!
		float timeFromFirstHammerLift = gpGlobals->time - m_flHammerFirstLiftedTime;

		if (timeFromFirstHammerLift >= 2.00f)
		{
			ALERT(at_console, "lifted more than 2.0secs!\n");
			m_iChargedHammerDamageMultiplier = LVL2_HAMMER_CHARGED_DMG_MULTIPLIER / 10;
		}
		else if (timeFromFirstHammerLift >= 1.0f)
		{
			ALERT(at_console, "lifted more than 1.0secs!\n");
			m_iChargedHammerDamageMultiplier = LVL1_HAMMER_CHARGED_DMG_MULTIPLIER / 10;
		}
		else
		{
			m_iChargedHammerDamageMultiplier = DEFAULT_HAMMER_CHARGED_DMG_MULTIPLIER / 10;
		}

		pev->nextthink = gpGlobals->time + 0.10f;
	}
	else
	{
		// Release it now!
		SetThink(&CHammer::isSwingingAndCheckHit);
		pev->nextthink = gpGlobals->time + 0.1f;
		m_flNextSecondaryAttack = GetNextAttackDelay(HAMMER_NEXT_ATTACK_DELAY);
		isHammerLifted = false;
		m_flHammerFirstLiftedTime = 0.00f;
	}
}

// There are a lot of repeated code! I'm gonna consolidate them once they are working right!!
void CHammer::PrimaryAttack()
{
	if (!isHammerLifted)
	{
		m_flNextSecondaryAttack = GetNextAttackDelay(HAMMER_NEXT_ATTACK_DELAY);
		SendWeaponAnim(HAMMER_HOLSTER2);
		isHammerLifted = true;
		m_iAttackType = HAMMER_ATTACK_SWING;
		m_flHammerFirstLiftedTime = gpGlobals->time;
		m_iChargedHammerDamageMultiplier = DEFAULT_HAMMER_CHARGED_DMG_MULTIPLIER;
		SetThink(&CHammer::ChargePrimaryAttack);
		pev->nextthink = gpGlobals->time + 0.1f;
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 5.0f, 8.0f);
}

void CHammer::SecondaryAttack()
{
	if (!isHammerLifted)
	{
		m_flNextPrimaryAttack = GetNextAttackDelay(HAMMER_NEXT_ATTACK_DELAY);
		SendWeaponAnim(HAMMER_HOLSTER3);
		isHammerLifted = true;
		m_iAttackType = HAMMER_ATTACK_SLAM;
		m_flHammerFirstLiftedTime = gpGlobals->time;
		m_iChargedHammerDamageMultiplier = DEFAULT_HAMMER_CHARGED_DMG_MULTIPLIER;
		SetThink(&CHammer::ChargeSecondaryAttack);
		pev->nextthink = gpGlobals->time + 0.1f;
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 5.0f, 8.0f);
}

bool CHammer::Smack(CBaseEntity* pEntity, TraceResult& tr)
{
	// Looks like the v_hammer.mdl's animation isn't complete!
	// But I'll just leave it here, if I got the chance/time to fix it!
	PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_usHammer,
		0.0, g_vecZero, g_vecZero, 0, 0, 0,
		0.0, 0, 0.0);

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	// Hammer smacks an enemy, or a cold hard floor or wall?
	// If smacks an enemy, returns false, else true.
	ALERT(at_console, "hammer hit something!\n");

	// Todo: Need to include breakables there too... :D
	if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
	{
		ALERT(at_console, "hammer hit enemy!\n");
		// It should cut out that swinging sound when it hits something,
		// but I think I would just leave it out, just to preserve Scientist Hunt's one!
		m_pPlayer->m_iWeaponVolume = 256;

		switch (RANDOM_LONG(0, 2))
		{
		case 0:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "sh15/ham_hitbod1.wav", 1, ATTN_NORM);
			break;
		case 1:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "sh15/ham_hitbod2.wav", 1, ATTN_NORM);
			break;
		case 2:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "sh15/ham_hitbod3.wav", 1, ATTN_NORM);
			break;
		default:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "sh15/ham_hitbod1.wav", 1, ATTN_NORM);
			break;
		}

		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "sh15/ham_hitbod1.wav", 1, ATTN_NORM);

		CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);
		
		// Currently this is a prototype and future modifications include adding in a server side switch
		// to enable or disable this feature.
		switch (m_iAttackType)
		{
		case HAMMER_ATTACK_SWING:
			pHit->TakeDamage(pev, m_pPlayer->pev, 80.00f, DMG_NEVERGIB | DMG_CLUB);
			// Launches the enemy far far away...
			pHit->pev->velocity = pHit->pev->velocity + gpGlobals->v_forward * m_iChargedHammerDamageMultiplier + gpGlobals->v_up * m_iChargedHammerDamageMultiplier;
			if (FClassnameIs(pHit->pev, "monster_scientist"))
			{
				// ... if it's a scientist,
				// and let the enemy spins around. If crashes the wall, gib on touch wall.
				pHit->pev->avelocity = Vector(2000.0f, 2000.0f, 2000.0f);
				// Temporarily it's iuser4 and 0x01 to indicate "wrecked by hammer".
				// If this variable occupied by something else I'll use another one available.				
				pHit->pev->iuser4 = 0x01;
			}
			break;
		case HAMMER_ATTACK_SLAM:
			// Trash it into a billion pieces!
			pHit->TakeDamage(pev, m_pPlayer->pev, 1000.00f, DMG_ALWAYSGIB | DMG_CLUB);
			break;
		default:
			// Assert if goes there!
			break;
		}
		return false;
	}
	return true;
}

void CHammer::checkHit()
{
	// I lifted most of these code from crowbar.cpp!
	ALERT(at_console, "check hit now!\n");

	TraceResult tr;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

	if (tr.flFraction >= 1.0)
	{
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);
		if (tr.flFraction < 1.0)
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);
			if (!pHit || pHit->IsBSPModel())
				FindHullIntersection(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict());
			vecEnd = tr.vecEndPos; // This is the point on the actual surface (the hull could have hit space)
		}
		else
		{
			ALERT(at_console, "no hit there!\n");
			m_flTimeWeaponIdle = 1.0f;
			return;
		}
	}

	float flVol = 1.0;
	bool fHitWorld = true;

	CBaseEntity* pEntity = CBaseEntity::Instance(tr.pHit);

	if (pEntity)
	{
		fHitWorld = Smack(pEntity, tr);

		if (fHitWorld)
		{
			// Smacked onto a cold hard surface. Give it a bit of a sound pitch variety:
			ALERT(at_console, "hammer hit wall or floor!\n");
			m_pPlayer->m_iWeaponVolume = 512;
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "sh15/ham_hitw.wav", 1.00f, ATTN_NORM, 0, 100 + RANDOM_LONG(-10, 10));
		}
	}

	m_flTimeWeaponIdle = 1.0f;
}

void CHammer::isSwingingAndCheckHit()
{
	// Animate swing there and play swing sound, capture start swinging time:
	switch (m_iAttackType)
	{
	case HAMMER_ATTACK_SWING:
		SendWeaponAnim(HAMMER_ATTACK1);
		break;
	case HAMMER_ATTACK_SLAM:
		SendWeaponAnim(HAMMER_ATTACK2);
		break;
	default:
		// Shouldn't go there, should be asserting!
		SendWeaponAnim(HAMMER_ATTACK1);
		break;
	}

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "sh15/ham_swing.wav", 1, ATTN_NORM);
	m_flStartSwingTime = gpGlobals->time;

	// Scan for hit when animating - only do a hit when hammer swing is within approx. 20% of the animation.
	SetThink(&CHammer::checkHit);
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CHammer::WeaponIdle()
{
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	int iAnim = HAMMER_IDLE1;
	switch (RANDOM_LONG(0, 2))
	{
	case 0:
		iAnim = HAMMER_IDLE1;
		break;
	case 1:
		iAnim = HAMMER_IDLE2;
		break;
	case 2:
		iAnim = HAMMER_IDLE3;
		break;
	default:
		iAnim = HAMMER_IDLE1;
		break;
	}

	SendWeaponAnim(iAnim);

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 5.0f, 8.0f);
}