#ifndef ECELL4_HCP_LATTICE_SPACE_HPP
#define ECELL4_HCP_LATTICE_SPACE_HPP

#include "VoxelSpaceBase.hpp"

namespace ecell4
{

class HCPLatticeSpace : public VoxelSpaceBase
{
public:
    typedef VoxelSpaceBase base_type;

public:
    HCPLatticeSpace(const Real3 &edge_lengths, const Real &voxel_radius,
                    const bool is_periodic)
        : base_type(voxel_radius)
    {
        set_lattice_properties(edge_lengths, is_periodic);
    }

    virtual ~HCPLatticeSpace()
    {
        ; // do nothing
    }

    virtual void reset(const Real3 &edge_lengths, const Real &voxel_radius,
                       const bool is_periodic)
    {
        voxel_radius_ = voxel_radius;

        set_lattice_properties(edge_lengths, is_periodic);
    }

    void set_lattice_properties(const Real3 &edge_lengths,
                                const bool is_periodic);

    /**
     * Primitives
     */

    const Real3 &edge_lengths() const { return edge_lengths_; }

    virtual const Integer col_size() const { return col_size_ - 2; }

    virtual const Integer row_size() const { return row_size_ - 2; }

    virtual const Integer layer_size() const { return layer_size_ - 2; }

    /**
     Coordinate transformations
     */
    coordinate_type global2coordinate(const Integer3 &global) const
    {
        const Integer3 g(global.col + 1, global.row + 1, global.layer + 1);
        return g.row + row_size_ * (g.col + col_size_ * g.layer);
    }

    Integer3 coordinate2global(const coordinate_type &coord) const
    {
        const Integer NUM_COLROW(row_size_ * col_size_);
        const Integer LAYER(coord / NUM_COLROW);
        const Integer SURPLUS(coord - LAYER * NUM_COLROW);
        const Integer COL(SURPLUS / row_size_);
        const Integer3 global(COL, SURPLUS - COL * row_size_, LAYER);
        const Integer3 retval(global.col - 1, global.row - 1, global.layer - 1);
        return retval;
    }

    Real3 coordinate2position(const coordinate_type &coord) const
    {
        return global2position(coordinate2global(coord));
    }

    coordinate_type position2coordinate(const Real3 &pos) const
    {
        return global2coordinate(position2global(pos));
    }

    Real3 global2position(const Integer3 &global) const
    {
        // the center point of a voxel
        const Real3 pos(
            global.col * HCP_X, (global.col % 2) * HCP_L + HCP_Y * global.layer,
            (global.row * 2 + (global.layer + global.col) % 2) * voxel_radius_);
        return pos;
    }

    Integer3 position2global(const Real3 &pos) const
    {
        const Integer col(round(pos[0] / HCP_X));
        const Integer layer(round((pos[1] - (col % 2) * HCP_L) / HCP_Y));
        const Integer row(
            round((pos[2] / voxel_radius_ - ((layer + col) % 2)) / 2));
        const Integer3 global(col, row, layer);
        return global;
    }

    Integer num_neighbors(const coordinate_type &coord) const
    {
        if (!is_inside(coord))
            return 0;
        return 12;
    }

    coordinate_type periodic_transpose(const coordinate_type &coord) const
    {
        Integer3 global(coordinate2global(coord));

        global.col = global.col % col_size();
        global.row = global.row % row_size();
        global.layer = global.layer % layer_size();

        global.col = global.col < 0 ? global.col + col_size() : global.col;
        global.row = global.row < 0 ? global.row + row_size() : global.row;
        global.layer =
            global.layer < 0 ? global.layer + layer_size() : global.layer;

        return global2coordinate(global);
    }

protected:
    coordinate_type get_neighbor_(const coordinate_type &coord,
                                  const Integer &nrand) const
    {
        const Integer NUM_COLROW(col_size_ * row_size_);
        const Integer NUM_ROW(row_size_);
        const bool odd_col(((coord % NUM_COLROW) / NUM_ROW) & 1);
        const bool odd_lay((coord / NUM_COLROW) & 1);

        if (!is_inside(coord))
            throw NotFound("There is no neighbor voxel.");

        switch (nrand)
        {
        case 0:
            return coord - 1;
        case 1:
            return coord + 1;
        case 2:
            return coord + (odd_col ^ odd_lay) - NUM_ROW - 1;
        case 3:
            return coord + (odd_col ^ odd_lay) - NUM_ROW;
        case 4:
            return coord + (odd_col ^ odd_lay) + NUM_ROW - 1;
        case 5:
            return coord + (odd_col ^ odd_lay) + NUM_ROW;
        case 6:
            return coord - (2 * odd_col - 1) * NUM_COLROW - NUM_ROW;
        case 7:
            return coord - (2 * odd_col - 1) * NUM_COLROW + NUM_ROW;
        case 8:
            return coord + (odd_col ^ odd_lay) - NUM_COLROW - 1;
        case 9:
            return coord + (odd_col ^ odd_lay) - NUM_COLROW;
        case 10:
            return coord + (odd_col ^ odd_lay) + NUM_COLROW - 1;
        case 11:
            return coord + (odd_col ^ odd_lay) + NUM_COLROW;
        }
        throw NotFound("Invalid argument: nrand");
    }

public:
    bool is_in_range(const coordinate_type &coord) const
    {
        return coord >= 0 && coord < row_size_ * col_size_ * layer_size_;
    }

    virtual bool is_inside(const coordinate_type &coord) const
    {
        const Integer3 global(coordinate2global(coord));
        return global.col >= 0 && global.col < col_size() && global.row >= 0 &&
               global.row < row_size() && global.layer >= 0 &&
               global.layer < layer_size();
    }

    virtual Integer size() const { return row_size_ * col_size_ * layer_size_; }

    virtual Integer3 shape() const
    {
        return Integer3(col_size_, row_size_, layer_size_);
    }

    virtual Integer actual_size() const
    {
        return col_size() * row_size() * layer_size();
    }

protected:
    Real3 edge_lengths_;
    Real HCP_L, HCP_X, HCP_Y;
    Integer row_size_, layer_size_, col_size_;
};

} // namespace ecell4

#endif /* ECELL4_LATTICE_SPACE_BASE_HPP */
