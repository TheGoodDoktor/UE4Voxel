#pragma once

class VoxelEngine;

namespace Voxel
{
	class IWorldBuilder
	{
	public:
		virtual bool Init(VoxelEngine *pEngine) = 0;
	};
}