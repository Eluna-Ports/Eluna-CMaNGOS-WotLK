/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
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

#ifndef MANGOS_SPAWN_GROUP_DEFINE_H
#define MANGOS_SPAWN_GROUP_DEFINE_H

#include "Platform/Define.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>

struct FormationEntry;
class FormationData;
class FormationSlotData;

typedef std::shared_ptr<FormationSlotData> FormationSlotDataSPtr;
typedef std::shared_ptr<FormationData> FormationDataSPtr;
typedef std::shared_ptr<FormationEntry> FormationEntrySPtr;
typedef std::map<uint32, FormationSlotDataSPtr> FormationSlotMap;

struct SpawnGroupRandomEntry
{
    uint32 Id;
    uint32 Entry;
    uint32 MinCount; // Minimum of given entry before randomization is used
    uint32 MaxCount; // Maximum of given entry alive spawned in world, 0 is infinite
    uint32 Chance;
};

struct SpawnGroupDbGuids
{
    uint32 Id;
    uint32 DbGuid;
    int32 SlotId;
    uint32 Chance;
    uint32 OwnEntry;
    bool RandomEntry;
};

struct SpawnGroupSquad
{
    uint32 SquadId;
    std::map<uint32, uint32> GuidToEntry;
};

enum SpawnGroupType
{
    SPAWN_GROUP_CREATURE = 0,
    SPAWN_GROUP_GAMEOBJECT = 1,
};

enum SpawnGroupFlags // flags that are common between both
{
    SPAWN_GROUP_DESPAWN_ON_COND_FAIL = 0x08,
};

enum CreatureGroupFlags
{
    CREATURE_GROUP_AGGRO_TOGETHER   = 0x01,
    CREATURE_GROUP_RESPAWN_TOGETHER = 0x02,
    CREATURE_GROUP_EVADE_TOGETHER   = 0x04,
    // SPAWN_GROUP_DESPAWN_ON_COND_FAIL
    CREATURE_GROUP_FORMATION_MIRRORING = 0x10, // mirrors position during linear path movement
};

enum CreatureGroupEvent : uint32
{
    CREATURE_GROUP_EVENT_AGGRO,
    CREATURE_GROUP_EVENT_EVADE,
    CREATURE_GROUP_EVENT_HOME,
    CREATURE_GROUP_EVENT_RESPAWN,
    CREATURE_GROUP_EVENT_MEMBER_DIED,
};

struct SpawnGroupEntry
{
    uint32 Id;
    std::string Name;
    SpawnGroupType Type;
    uint32 MaxCount; // Maximum active alive entities spawned in world
    int32 WorldStateCondition; // Worldstate value when set to 1 enables spawning of given group and 0 disables spawning
    int32 WorldStateExpression; // Exclusive with condition
    uint32 Flags;
    uint32 StringId;
    std::optional<uint32> RespawnOverrideMin;
    std::optional<uint32> RespawnOverrideMax;
    bool Active;
    bool Large;
    bool EnabledByDefault;
    bool HasChancedSpawns;
    std::vector<SpawnGroupDbGuids> DbGuids;
    std::vector<SpawnGroupRandomEntry> RandomEntries;
    std::vector<SpawnGroupRandomEntry*> EquallyChanced;
    std::vector<SpawnGroupRandomEntry*> ExplicitlyChanced;
    std::vector<uint32> LinkedGroups;

    std::vector<SpawnGroupSquad> Squads;

    // may be nullptr
    std::unique_ptr<FormationEntry> Formation;

    int32 GetFormationSlotId(uint32 dbGuid) const
    {
        auto const& itr = std::find_if(DbGuids.begin(), DbGuids.end(), [dbGuid](SpawnGroupDbGuids const& x) { return x.DbGuid == dbGuid; });
        return itr != DbGuids.end() ? (*itr).SlotId : -1;
    }

    bool IsMirroring() const { return Flags & CREATURE_GROUP_FORMATION_MIRRORING; }
};

// Formation defines
enum SpawnGroupFormationType : uint32
{
    SPAWN_GROUP_FORMATION_TYPE_RANDOM              = 0,
    SPAWN_GROUP_FORMATION_TYPE_SINGLE_FILE         = 1,
    SPAWN_GROUP_FORMATION_TYPE_SIDE_BY_SIDE        = 2,
    SPAWN_GROUP_FORMATION_TYPE_LIKE_GEESE          = 3,
    SPAWN_GROUP_FORMATION_TYPE_FANNED_OUT_BEHIND   = 4,
    SPAWN_GROUP_FORMATION_TYPE_FANNED_OUT_IN_FRONT = 5,
    SPAWN_GROUP_FORMATION_TYPE_CIRCLE_THE_LEADER   = 6,

    SPAWN_GROUP_FORMATION_TYPE_COUNT               = 7
};

enum SpawnGroupFormationSlotType : uint32
{
    SPAWN_GROUP_FORMATION_SLOT_TYPE_STATIC = 0,
    SPAWN_GROUP_FORMATION_SLOT_TYPE_SCRIPT = 1,
    SPAWN_GROUP_FORMATION_SLOT_TYPE_PLAYER = 2,
};

enum SpawGroupFormationOptions : uint32
{
    SPAWN_GROUP_FORMATION_OPTION_NONE                                       = 0x00,
    SPAWN_GROUP_FORMATION_OPTION_FOLLOWERS_WILL_NOT_PATHFIND_TO_LOCATION    = 0x01, // NYI - need examples where used vs normal
    SPAWN_GROUP_FORMATION_OPTION_KEEP_CONPACT                               = 0x02,
};

struct FormationEntry
{
    uint32 GroupId                  = 0;
    SpawnGroupFormationType Type    = SPAWN_GROUP_FORMATION_TYPE_RANDOM;
    uint32 MovementIdOrWander       = 0;      
    uint32 MovementType             = 0;
    float Spread                    = 3.0f;
    uint32 Options                  = 0;
    std::string Comment             = "No comment";
    bool IsDynamic                  = false;
};

struct SpawnGroupEntryContainer
{
    std::map<uint32, SpawnGroupEntry> spawnGroupMap;
    std::map<std::pair<uint32, uint32>, SpawnGroupEntry*> spawnGroupByGuidMap;
};

#endif