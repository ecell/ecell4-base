#ifndef ECELL4_SPATIOCYTE_REACTIONS_HPP
#define ECELL4_SPATIOCYTE_REACTIONS_HPP

#include <memory>
#include "Voxel.hpp"
#include <ecell4/core/ReactionRule.hpp>
#include <ecell4/core/VoxelPool.hpp>

namespace ecell4
{

namespace spatiocyte
{

class ReactionInfo {
public:
    struct Item
    {
        Item(ParticleID pid, const Species &species, const Voxel &voxel)
            : pid(pid), species(species), voxel(voxel)
        {
        }

        ParticleID pid;
        Species species;
        Voxel voxel;
    };

    typedef std::vector<Item> container_type;

public:
    ReactionInfo() : t_(0), reactants_(), products_() {}

    ReactionInfo(const Real t) : t_(t), reactants_(), products_() {}

    ReactionInfo(const Real t, const container_type &reactants,
                 const container_type &products)
        : t_(t), reactants_(reactants), products_(products)
    {
    }

    ReactionInfo(const ReactionInfo &another)
        : t_(another.t()), reactants_(another.reactants()),
          products_(another.products())
    {
    }

    Real t() const { return t_; }

    bool has_occurred() const
    {
        return reactants_.size() > 0 || products_.size() > 0;
    }

    const container_type &reactants() const { return reactants_; }

    void add_reactant(const Item &item) { reactants_.push_back(item); }

    const container_type &products() const { return products_; }

    void add_product(const Item &item) { products_.push_back(item); }

protected:
    Real t_;
    container_type reactants_, products_;
};

// Application of reactions

class SpatiocyteWorld;

ReactionInfo apply_a2b(std::shared_ptr<SpatiocyteWorld> world,
                       const ReactionInfo::Item &reactant_item,
                       const Species &product_species);

ReactionInfo apply_a2bc(std::shared_ptr<SpatiocyteWorld> world,
                        const ReactionInfo::Item &reactant_item,
                        const Species &product_species0,
                        const Species &product_species1);

ReactionInfo
apply_second_order_reaction(std::shared_ptr<SpatiocyteWorld> world,
                            const ReactionRule &reaction_rule,
                            const ReactionInfo::Item &reactant_item0,
                            const ReactionInfo::Item &reactant_item1);

ReactionInfo apply_vanishment(std::shared_ptr<SpatiocyteWorld> world,
                              const ReactionInfo::Item &reactant_item0,
                              const ReactionInfo::Item &reactant_item1);

ReactionInfo apply_ab2c(std::shared_ptr<SpatiocyteWorld> world,
                        const ReactionInfo::Item &reactant_item0,
                        const ReactionInfo::Item &reactant_item1,
                        const Species &product_species);

ReactionInfo apply_ab2cd(std::shared_ptr<SpatiocyteWorld> world,
                         const ReactionInfo::Item &reactant_item0,
                         const ReactionInfo::Item &reactant_item1,
                         const Species &product_species0,
                         const Species &product_species1);

} // namespace spatiocyte

} // namespace ecell4

#endif /* ECELL4_SPATIOCYTE_REACTIONS_HPP */
