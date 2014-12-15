#ifndef _BSP_H_
#define _BSP_H_

// General BSP methods for traversing, etc...

int BSPFindSubsector(vector2_t position);
void BSPTraverseForRendering(vector2_t position, vector2_t fovLeft, vector2_t fovRight, int* subsectors, int maxSubsectors, int* numSubsectors);

#endif // _BSP_H_