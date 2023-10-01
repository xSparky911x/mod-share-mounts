/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "Spell.h"

class ShareMounts_PlayerScript : public PlayerScript
{
public:
    ShareMounts_PlayerScript() : PlayerScript("ShareMounts_PlayerScript") { }

    void OnLogin(Player* player) override
    {
        if (!sConfigMgr->GetOption<bool>("ShareMounts.Enable", false)) {
            return;
        }

        uint32 accountId = player->GetSession()->GetAccountId();

        QueryResult mountQuery = CharacterDatabase.Query("SELECT DISTINCT `spell` FROM `character_spell` WHERE `guid` IN (SELECT `guid` FROM `characters` WHERE `account` = {})", accountId);

        if (!mountQuery) {
            return;
        }

        std::set<uint32> allianceMountIds = { 458, 470, 472, 6648, 6777, 6898, 6899, 8394, 10789, 10793, 10873, 10969, 15779, 16055, 16056, 16058, 16059, 16060, 16082, 16083, 17229, 17453, 17454, 17455, 17456, 17459, 17460, 17461, 22717, 22719, 22720, 22723, 23219, 23220, 23221, 23222, 23223, 23225, 23227, 23228, 23229, 23238, 23239, 23240, 23338, 23510, 34406, 34407, 34896, 34897, 34898, 34899, 35710, 35711, 35712, 35713, 35714, 39315, 39317, 39318, 39319, 48027, 59785, 59791, 59799, 60114, 60118, 60424, 61425, 61465, 61470, 63232, 63636, 63637, 63638, 63639, 65637, 65638, 65640, 65642, 65643, 66847, 68057, 68187, 32235,32239,32240,32242,32289,32290,32292,33388,33391,34090,34091,54197,61229,61996,66087 };
        std::set<uint32> hordeMountIds = { 32243, 32244, 32245, 32246, 32295, 32296, 32297, 33388, 33391, 34090, 34091, 54197, 61230, 61997, 66088, 580, 6653, 6654, 8395, 10796, 10799, 16080, 16081, 16084, 17450, 17462, 17463, 17464, 17465, 18989, 18990, 18991, 18992, 22718, 22721, 22722, 22724, 23241, 23242, 23243, 23246, 23247, 23248, 23249, 23250, 23251, 23252, 23509, 33660, 34795, 34896, 34897, 34898, 34899, 35018, 35020, 35022, 35025, 35027, 35028, 39315, 39317, 39318, 39319, 55531, 59788, 59793, 59797, 60116, 60119, 61447, 61467, 61469, 63635, 63640, 63641, 63642, 63643, 64657, 64658, 64659, 64977, 65639, 65641, 65644, 65645, 65646, 66091, 66846, 68056, 68188 };

        std::set<uint32> ignoreSpells = { };

        do {
            Field* fields = mountQuery->Fetch();
            uint32 spellId = fields[0].Get<uint32>();

            const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(spellId);

            if (spellInfo->Attributes == 269844752) {

                if (ignoreSpells.count(spellId) == 0) {
                    if (!sConfigMgr->GetOption<bool>("ShareMounts.BlockFactions", false)) {
                        player->learnSpell(spellId);
                    }
                    else {
                        if (allianceMountIds.count(spellId) > 0 && player->GetTeamId() == TEAM_ALLIANCE) {
                            player->learnSpell(spellId);
                        }
                        else if (hordeMountIds.count(spellId) > 0 && player->GetTeamId() == TEAM_HORDE) {
                            player->learnSpell(spellId);
                        }
                        else if (allianceMountIds.count(spellId) == 0 && hordeMountIds.count(spellId) == 0) {
                            player->learnSpell(spellId);
                        }
                    }
                }
            }
        } while (mountQuery->NextRow());
    }

private:
        bool isSpellMount(const SpellInfo* spellInfo)
        {
            return spellInfo->IsProfessionOrRiding() || spellInfo->IsProfession();
        }
};

void AddSC_mod_share_mounts()
{
    new ShareMounts_PlayerScript();
}
