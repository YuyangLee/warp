/** Copyright (c) 2022 NVIDIA CORPORATION.  All rights reserved.
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */

#pragma once

#include "volume.h"

// Helper functions for cpp/cu files, not to be exposed to user kernels

namespace wp
{

namespace volume
{

inline CUDA_CALLABLE pnanovdb_leaf_handle_t get_leaf(const pnanovdb_buf_t buf, const uint32_t leaf_id)
{
    const pnanovdb_tree_handle_t tree = get_tree(buf);
    const uint64_t first_leaf_offset = pnanovdb_tree_get_node_offset_leaf(buf, tree);
    const uint32_t leaf_stride = PNANOVDB_GRID_TYPE_GET(get_grid_type(buf), leaf_size);
    return {pnanovdb_address_offset64(tree.address, first_leaf_offset + uint64_t(leaf_id) * leaf_stride)};
}

inline CUDA_CALLABLE pnanovdb_coord_t leaf_origin(const pnanovdb_buf_t buf, const pnanovdb_leaf_handle_t leaf)
{
    pnanovdb_coord_t origin = pnanovdb_leaf_get_bbox_min(buf, leaf);
    // mask out last three bits corresponding to voxel coordinates within leaf
    constexpr uint32_t MASK = (1u << 3u) - 1u;
    origin.x &= ~MASK;
    origin.y &= ~MASK;
    origin.z &= ~MASK;
    return origin;
}

inline CUDA_CALLABLE uint64_t leaf_voxel_index(const pnanovdb_buf_t buf, const uint32_t leaf_id,
                                               const pnanovdb_coord_t &ijk)
{
    const uint32_t grid_type = get_grid_type(buf);

    const pnanovdb_leaf_handle_t leaf = get_leaf(buf, leaf_id);
    const pnanovdb_address_t value_address = pnanovdb_leaf_get_value_address(grid_type, buf, leaf, &ijk);
    return volume::get_grid_voxel_index(grid_type, buf, value_address, ijk) - 1;
}

inline CUDA_CALLABLE pnanovdb_coord_t leaf_offset_to_local_coord(uint32_t offset)
{
    pnanovdb_coord_t coord;
    coord.x = (offset >> 6) & 7;
    coord.y = (offset >> 3) & 7;
    coord.z = (offset >> 0) & 7;
    return coord;
}

} // namespace volume

} // namespace wp
