/* Copyright (C) 2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "../sc_defines.h"

// **** This script is still under Developement ****

struct MANGOS_DLL_DECL generic_meleeAI : public ScriptedAI
{
    generic_meleeAI(Creature *c) : ScriptedAI(c) {}

    Unit* pTarget;

    void AttackStart(Unit *who)
    {
        if (!who)
            return;

        if (m_creature->getVictim() == NULL && who->isTargetableForAttack() && who != m_creature)
        {
            //Begin attack
            DoStartMeleeAttack(who);
            pTarget = who;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who)
            return;

        if (who->isTargetableForAttack() && IsVisible(who) && who->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(who))
        {
            if ( m_creature->getVictim() == NULL)
            {
                if(who->HasStealthAura())
                    who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                //Begin attack
                DoStartMeleeAttack(who);
                pTarget = who;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //If we had a target and it wasn't cleared then it means the player died from some unknown soruce
        //But we still need to reset
        if (m_creature->isAlive() && pTarget && !m_creature->getVictim())
        {
            pTarget = NULL;
            DoStopAttack();
            DoGoHome();
            return;
        }

        //Check if we have a current target
        if( m_creature->getVictim() && m_creature->isAlive())
        {
            //Check if we should stop attacking because our victim is no longer attackable or we are to far from spawnpoint
            if (needToStop() || CheckTether())
            {
                pTarget = NULL;
                DoStopAttack();
                DoGoHome();
                return;
            }
            
            //If we are within range melee the target
            if( m_creature->IsWithinDist(m_creature->getVictim(), ATTACK_DIST))
            {
                if( m_creature->isAttackReady() )
                {
                    Unit* newtarget = m_creature->SelectHostilTarget();
                    if(newtarget)
                        AttackStart(newtarget);

                    //Check if we have any melee spells avialable (warning this only works with Extended script)
                    SpellEntry const *info = SelectSpell(m_creature->getVictim());

                    //30% chance to replace our white hit with a melee special
                    if (info)
                    {
                        DoCastSpell(m_creature->getVictim(), info);
                    }
                    else
                        m_creature->AttackerStateUpdate(m_creature->getVictim());

                    m_creature->resetAttackTimer();
                }
            }

            //If we are still alive and we lose our victim it means we killed them
            if(m_creature->isAlive() && !m_creature->getVictim())
            {
                pTarget = NULL;
                DoStopAttack();
                DoGoHome();
            }
        }
    }
}; 
CreatureAI* GetAI_generic_melee(Creature *_Creature)
{
    return new generic_meleeAI (_Creature);
}


void AddSC_generic_melee()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="generic_melee";
    newscript->GetAI = GetAI_generic_melee;
    m_scripts[nrscripts++] = newscript;
}