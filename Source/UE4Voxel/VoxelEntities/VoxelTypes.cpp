#include "VoxelTypes.h"
#include "VoxelWorld.h"

namespace Voxel
{
	
bool FChunk::MarkDirty()
{
	check(World != nullptr);
	if (IsDirty)
		return false;
	IsDirty = true;
	World->AddDirtyChunk(this);
	return true;
}


}//namespace Voxel

