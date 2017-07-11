

namespace Voxel
{
	
struct FChunk;
struct FProcMeshSection;

class IMeshBuilder
{
	virtual void BuildMeshForChunk(FChunk* pChunk, FProcMeshSection& meshSection) = 0;
};
	
}