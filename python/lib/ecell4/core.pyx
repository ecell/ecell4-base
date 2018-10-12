from libcpp.string cimport string
from deprecated import deprecated

cdef string tostring(ustr):
    if isinstance(ustr, unicode):
        return <string>(ustr.encode('UTF-8'))
    else:
        return <string>(ustr)

include "RandomNumberGenerator.pxi"
include "UnitSpecies.pxi"
include "Species.pxi"
include "ReactionRule.pxi"
include "WorldInterface.pxi"
include "CompartmentSpace.pxi"  #XXX
include "ParticleSpace.pxi"  #XXX
include "Model.pxi"
include "NetworkModel.pxi"
include "NetfreeModel.pxi"
include "Real3.pxi"
include "Integer3.pxi"
include "Particle.pxi"
include "ParticleVoxel.pxi"
include "observers.pxi"
include "shapes.pxi"

def length_sq(p):
    """length_sq(p1) -> Real or Integer

    Return a square of a Euclidean norm of the given vector.

    """
    if isinstance(p, Real3):
        return real3_length_sq(<Real3>p)
    elif isinstance(p, Integer3):
        return integer3_length_sq(<Integer3>p)
    else:
        raise TypeError('Not implemented for this type')

def length(p):
    """length(p1) -> Real

    Return a Euclidean norm of the given vector.
    This is almost equivalent to call ``sqrt(length_sq(p1))``

    """
    if isinstance(p, Real3):
        return real3_length(<Real3>p)
    elif isinstance(p, Integer3):
        return integer3_length(<Integer3>p)
    else:
        raise TypeError('Not implemented for this type')

def dot_product(p1, p2):
    """dot_product(p1, p2) -> Real or Integer

    Return a dot product between two vectors

    """
    if isinstance(p1, Real3) and isinstance(p2, Real3):
        return real3_dot_product(<Real3>p1, <Real3>p2)
    elif isinstance(p1, Integer3) and isinstance(p2, Integer3):
        return integer3_dot_product(<Integer3>p1, <Integer3>p2)
    else:
        raise TypeError('Not implemented for this type')

from cython.operator cimport dereference as deref

cdef shared_ptr[Cpp_Model] Cpp_Model_from_Model(m):
    if isinstance(m, Model):
        return (<Model>m).thisptr
    elif isinstance(m, NetworkModel):
        return <shared_ptr[Cpp_Model]>((<NetworkModel>m).thisptr)
    elif isinstance(m, NetfreeModel):
        return <shared_ptr[Cpp_Model]>((<NetfreeModel>m).thisptr)
    else:
        raise ValueError, ("a wrong argument was given [%s]." % (type(m))
            + " the first argument must be Model, NetworkModel or NetfreeModel")

cimport extras

def load_version_information(filename):
    """Return a version information of HDF5 as a string."""
    cdef string cpp_filename = tostring(filename)
    return extras.load_version_information(cpp_filename).decode('UTF-8')

cimport functions

def cbrt(Real x):
    """cbrt(x) -> Real

    Return a cubic root of the given value.

    """
    return functions.cbrt(x)

cimport types

N_A = types.N_A
epsilon = types.epsilon

def _save_bd5(WorldInterface world, filename, group_index, object_name, spatial_unit, time_unit, bool trunc, bool with_radius):
    """This is a private function. See ecell4.extra.bdml.save_bd5"""
    cdef shared_ptr[Cpp_WorldInterface] _world = deref(world.thisptr)
    Cpp_save_bd5(deref(_world.get()), tostring(filename), group_index, tostring(object_name), tostring(spatial_unit), tostring(time_unit), trunc, with_radius)
