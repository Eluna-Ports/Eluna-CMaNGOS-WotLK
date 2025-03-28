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

#ifndef _VMAPMANAGER2_H
#define _VMAPMANAGER2_H

#include "IVMapManager.h"

#include <G3D/Vector3.h>

#include <unordered_map>
#include <mutex>

//===========================================================

#define MAP_FILENAME_EXTENSION2 ".vmtree"

#define FILENAMEBUFFER_SIZE 500

/**
This is the main Class to manage loading and unloading of maps, line of sight, height calculation and so on.
For each map or map tile to load it reads a directory file that contains the ModelContainer files used by this map or map tile.
Each global map or instance has its own dynamic BSP-Tree.
The loaded ModelContainers are included in one of these BSP-Trees.
Additionally a table to match map ids and map names is used.
*/

//===========================================================

namespace VMAP
{
    class StaticMapTree;
    class WorldModel;

    class ManagedModel
    {
        public:
            ManagedModel() : iModel(nullptr), iRefCount(0) {}
            void setModel(WorldModel* model) { iModel = model; }
            WorldModel* getModel() const { return iModel; }
            void incRefCount() { ++iRefCount; }
            int decRefCount() { return --iRefCount; }
        protected:
            WorldModel* iModel;
            int iRefCount;
    };

    typedef std::unordered_map<uint32, StaticMapTree*> InstanceTreeMap;
    typedef std::unordered_map<std::string, ManagedModel> ModelFileMap;

    class VMapManager2 : public IVMapManager
    {
        private:
            std::mutex m_vmStaticMapMutex;
            std::mutex m_vmModelMutex;

            bool m_thread_safe_environment;

        protected:
            // Tree to check collision
            ModelFileMap iLoadedModelFiles;
            InstanceTreeMap iInstanceMapTrees;

            bool _loadMap(uint32 pMapId, const std::string& basePath, uint32 tileX, uint32 tileY);
            /* void _unloadMap(uint32 pMapId, uint32 x, uint32 y); */

        public:
            // public for debug
            G3D::Vector3 convertPositionToInternalRep(float x, float y, float z) const;
            InstanceTreeMap::const_iterator GetMapTree(uint32 mapId) const;
            static std::string getMapFileName(unsigned int pMapId);

            VMapManager2();
            ~VMapManager2();

            void InitializeThreadUnsafe(const std::vector<uint32>& mapIds) override;

            VMAPLoadResult loadMap(const char* pBasePath, unsigned int pMapId, int x, int y) override;
            bool IsTileLoaded(uint32 mapId, uint32 x, uint32 y) const override;

            void unloadMap(unsigned int pMapId, int x, int y) override;
            void unloadMap(unsigned int pMapId) override;

            bool isInLineOfSight(unsigned int pMapId, float x1, float y1, float z1, float x2, float y2, float z2, bool ignoreM2Model) override;
            /**
            fill the hit pos and return true, if an object was hit
            */
            bool getObjectHitPos(unsigned int pMapId, float x1, float y1, float z1, float x2, float y2, float z2, float& rx, float& ry, float& rz, float pModifyDist) override;
            float getHeight(unsigned int pMapId, float x, float y, float z, float maxSearchDist) override;

            bool processCommand(char* /*pCommand*/) override { return false; }      // for debug and extensions

            bool getAreaInfo(unsigned int pMapId, float x, float y, float& z, uint32& flags, int32& adtId, int32& rootId, int32& groupId) const override;
            bool GetLiquidLevel(uint32 pMapId, float x, float y, float z, uint8 ReqLiquidType, float& level, float& floor, uint32& type) const override;

            WorldModel* acquireModelInstance(const std::string& basepath, const std::string& filename);
            void releaseModelInstance(const std::string& filename);

            // what's the use of this? o.O
            std::string getDirFileName(unsigned int pMapId, int /*x*/, int /*y*/) const override
            {
                return getMapFileName(pMapId);
            }
            bool existsMap(const char* pBasePath, unsigned int pMapId, int x, int y) override;

#ifdef MMAP_GENERATOR
        public:
            void getInstanceMapTree(InstanceTreeMap& instanceMapTree);
#endif
    };
}
#endif
