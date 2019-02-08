from cython.operator cimport dereference as deref, preincrement as inc
from libcpp.string cimport string
from cython cimport address

cimport util
cimport context

import numbers
from cpython cimport bool as bool_t

class Quantity(object):

    def __init__(self, magnitude, units=""):
        self.magnitude = magnitude
        self.units = units

cdef Quantity_from_Cpp_Quantity_Real(Cpp_Quantity[Real] *value):
    return Quantity(value.magnitude, value.units.decode('UTF-8'))

cdef Quantity_from_Cpp_Quantity_Integer(Cpp_Quantity[Integer] *value):
    return Quantity(value.magnitude, value.units.decode('UTF-8'))

cdef Cpp_Quantity[Real] Cpp_Quantity_from_Quantity_Real(value):
    assert(isinstance(value, Quantity))
    return Cpp_Quantity[Real](<Real> value.magnitude, tostring(value.units))

cdef Cpp_Quantity[Integer] Cpp_Quantity_from_Quantity_Integer(value):
    assert(isinstance(value, Quantity))
    return Cpp_Quantity[Integer](<Integer> value.magnitude, tostring(value.units))

cdef boost_get_from_Cpp_Species_value_type(Cpp_Species_value_type value):
    cdef string* value_str = boost_get[string, string, Cpp_Quantity[Real], Cpp_Quantity[Integer], bool](address(value))
    if value_str != NULL:
        return deref(value_str).decode('UTF-8')
    cdef Cpp_Quantity[Real]* value_real = boost_get[Cpp_Quantity[Real], string, Cpp_Quantity[Real], Cpp_Quantity[Integer], bool](address(value))
    if value_real != NULL:
        # return deref(value_real).magnitude
        return Quantity_from_Cpp_Quantity_Real(value_real)
    cdef Cpp_Quantity[Integer]* value_int = boost_get[Cpp_Quantity[Integer], string, Cpp_Quantity[Real], Cpp_Quantity[Integer], bool](address(value))
    if value_int != NULL:
        # return deref(value_int).magnitude
        return Quantity_from_Cpp_Quantity_Integer(value_int)
    cdef bool* value_bool = boost_get[bool, string, Cpp_Quantity[Real], Cpp_Quantity[Integer], bool](address(value))
    if value_bool != NULL:
        return deref(value_bool)

    raise RuntimeError('Never get here. Unsupported return type was given.')

cdef class Species:
    """A class representing a type of molecules with attributes.

    Species(serial=None, radius=None, D=None, location=None)

    """

    def __init__(self, serial=None, radius=None, D=None, location=None):
        """Constructor.

        Parameters
        ----------
        serial : str, optional
            The serial name.
        radius : float, optional
            The radius of a molecule.
        D : foat, optional
            The diffusion rate of a molecule.
        location : str, optional
            The location of a molecule.

        """
        pass  # XXX: Only used for doc string

    def __cinit__(self, serial=None, radius=None, D=None, location=None):
        if serial is None and radius is None and D is None and location is None:
            self.thisptr = new Cpp_Species()
        elif serial is not None and radius is None and D is None and location is None:
            if isinstance(serial, Species):
                self.thisptr = new Cpp_Species(deref((<Species> serial).thisptr))
            elif isinstance(serial, (str, bytes)):
                self.thisptr = new Cpp_Species(tostring(serial))
            else:
                raise TypeError(
                    'Argument 1 must be string, Species or None.'
                    " '{}' was given [{}].".format(type(serial).__name__, serial))
        elif serial is not None and radius is not None and D is not None:
            if not isinstance(serial, (str, bytes)):
                raise TypeError(
                    "serial must be string. '{}' was given [{}].".format(type(serial).__name__, serial))
            if not isinstance(radius, (numbers.Real, Quantity)):
                raise TypeError(
                    "radius must be float. '{}' was given [{}].".format(type(radius).__name__, radius))
            if not isinstance(D, (numbers.Real, Quantity)):
                raise TypeError("D must be float. '{}' was given [{}].".format(type(D).__name__, D))
            if not type(radius) is type(D):
                raise TypeError(
                    "radius [{}] and D [{}] must have a same type ['{}' != '{}'].".format(
                        radius, D, type(radius).__name__, type(D).__name__))
            if location is not None and not isinstance(location, (str, bytes)):
                raise TypeError(
                    'location must be string.'
                    " '{}' was given [{}].".format(type(location).__name__, location))

            if isinstance(radius, Quantity):
                if location is None:
                    self.thisptr = new Cpp_Species(
                        tostring(serial), Cpp_Quantity_from_Quantity_Real(radius), Cpp_Quantity_from_Quantity_Real(D))
                else:
                    self.thisptr = new Cpp_Species(
                        tostring(serial), Cpp_Quantity_from_Quantity_Real(radius), Cpp_Quantity_from_Quantity_Real(D), tostring(location))
            else:
                if location is None:
                    self.thisptr = new Cpp_Species(
                        tostring(serial), <Real>radius, <Real>D)
                else:
                    self.thisptr = new Cpp_Species(
                        tostring(serial), <Real>radius, <Real>D, tostring(location))
        else:
            raise TypeError('A wrong list of arguments was given. See help(Species).')

    def __dealloc__(self):
        del self.thisptr

    def __richcmp__(Species self, Species rhs, int op):
        cdef int compare
        if deref(self.thisptr) > deref(rhs.thisptr):
            compare = 1
        elif deref(self.thisptr) < deref(rhs.thisptr):
            compare = -1
        else: # self == rhs
            compare = 0
        return util.richcmp_helper(compare, op)

    def __hash__(self):
        return hash(self.thisptr.serial().decode('UTF-8'))

    def serial(self):
        """Return the serial name as an unicode string."""
        return self.thisptr.serial().decode('UTF-8')

    def get_attribute(self, name):
        """get_attribute(name) -> str, float, int, or bool

        Return an attribute.
        If no corresponding attribute is found, raise an error.

        Parameters
        ----------
        name : str
            The name of an attribute.

        Returns
        -------
        value : str, float, int, or bool
            The value of the attribute.

        """
        return boost_get_from_Cpp_Species_value_type(self.thisptr.get_attribute(tostring(name)))

    def set_attribute(self, name, value):
        """set_attribute(name, value)

        Set an attribute.
        If existing already, the attribute will be overwritten.

        Parameters
        ----------
        name : str
            The name of an attribute.
        value : str, float, int, or bool
            The value of an attribute.

        """
        if isinstance(value, str):
            self.thisptr.set_attribute(tostring(name), tostring(value))
        elif isinstance(value, bool_t):
            self.thisptr.set_attribute(tostring(name), <bool> value)
        elif isinstance(value, numbers.Integral):
            self.thisptr.set_attribute(tostring(name), <Integer> value)
        elif isinstance(value, numbers.Real):
            self.thisptr.set_attribute(tostring(name), <Real> value)
        elif isinstance(value, Quantity) and isinstance(value.magnitude, numbers.Integral):
            self.thisptr.set_attribute(tostring(name), Cpp_Quantity_from_Quantity_Integer(value))
        elif isinstance(value, Quantity) and isinstance(value.magnitude, numbers.Real):
            self.thisptr.set_attribute(tostring(name), Cpp_Quantity_from_Quantity_Real(value))
        else:
            raise TypeError(
                'Type [{}] is not supported. str, int, float or bool must be given.'.format(
                    type(value)))

    def remove_attribute(self, name):
        """remove_attribute(name)

        Remove an attribute.
        If no corresponding attribute is found, raise an error.

        Parameters
        ----------
        name : str
            The name of an attribute to be removed.

        """
        self.thisptr.remove_attribute(tostring(name))

    def has_attribute(self, name):
        """has_attribute(name) -> bool

        Return if the attribute exists or not.

        Parameters
        ----------
        name : str
            The name of an attribute.

        Returns
        -------
        bool:
            True if the attribute exists, False otherwise.

        """
        return self.thisptr.has_attribute(tostring(name))

    def list_attributes(self):
        """list_attributes() -> [(str, str)]

        List all attributes.

        Returns
        -------
        list:
            A list of pairs of name and value.
            ``name`` and ``value`` are given as unicode strings.

        """
        cdef vector[pair[string, Cpp_Species_value_type]] attrs = self.thisptr.list_attributes()
        res = []
        cdef vector[pair[string, Cpp_Species_value_type]].iterator it = attrs.begin()
        while it != attrs.end():
            res.append((deref(it).first.decode('UTF-8'), boost_get_from_Cpp_Species_value_type(deref(it).second)))
            inc(it)
        return res

    def add_unit(self, UnitSpecies usp):
        """add_unit(usp)

        Append an ``UnitSpecies`` to the end.

        Parameters
        ----------
        usp : UnitSpecies
            An ``UnitSpecies`` to be added.

        """
        self.thisptr.add_unit(deref(usp.thisptr))

    def count(self, Species sp):
        """count(sp) -> Integer

        Count the number of matches for a target given as a ``Species``.

        Parameters
        ----------
        sp : Species
            A target to be count.

        Returns
        -------
        Integer:
            The number of matches.

        """
        return self.thisptr.count(deref(sp.thisptr))

    def units(self):
        """units() -> [UnitSpecies]

        Return a list of all ``UnitSpecies`` contained.

        """
        cdef vector[Cpp_UnitSpecies] usps = self.thisptr.units()
        retval = []
        cdef vector[Cpp_UnitSpecies].iterator it = usps.begin()
        while it != usps.end():
            retval.append(UnitSpecies_from_Cpp_UnitSpecies(
            <Cpp_UnitSpecies*>(address(deref(it)))))
            inc(it)
        return retval

    def D(self, value):
        """D(string) -> Species

        set attribute 'D', and return self.

        """
        cdef Cpp_Species *sp = self.thisptr.D_ptr(tostring(value))
        assert sp == self.thisptr
        return self

    def radius(self, value):
        """radius(string) -> Species

        set attribute 'radius', and return self.

        """
        cdef Cpp_Species *sp = self.thisptr.radius_ptr(tostring(value))
        assert sp == self.thisptr
        return self

    def location(self, value):
        """location(string) -> Species

        set attribute 'location', and return self.

        """
        cdef Cpp_Species *sp = self.thisptr.location_ptr(tostring(value))
        assert sp == self.thisptr
        return self

    def dimension(self, value):
        """dimension(string) -> Species

        set attribute 'dimension', and return self.

        """
        cdef Cpp_Species *sp = self.thisptr.dimension_ptr(tostring(value))
        assert sp == self.thisptr
        return self

    def __reduce__(self):
        return (__rebuild_species, (self.serial(), self.list_attributes()))

def __rebuild_species(serial, attrs):
    sp = Species(serial)
    for key, val in attrs:
        sp.set_attribute(key, val)
    return sp

cdef Species Species_from_Cpp_Species(Cpp_Species *sp):
    cdef Cpp_Species *new_obj = new Cpp_Species(deref(sp))
    r = Species()
    del r.thisptr
    r.thisptr = new_obj
    return r

# def spmatch(Species pttrn, Species sp):
#     """spmatch(pttrn, sp) -> bool
# 
#     Return if a pattern matches the target ``Species`` or not.
# 
#     Parameters
#     ----------
#     pttrn : Species
#         A pattern.
#     sp : Species
#         A target.
# 
#     Returns
#     -------
#     bool:
#         True if ``pttrn`` matches ``sp`` at least one time, False otherwise.
# 
#     """
#     return context.spmatch(deref(pttrn.thisptr), deref(sp.thisptr))

def count_species_matches(Species pttrn, Species sp):
    """count_species_matches(pttrn, sp) -> Integer

    Count the number of matches for a pattern given as a ``Species``.

    Parameters
    ----------
    pttrn : Species
        A pattern.
    sp : Species
        A target.

    Returns
    -------
    Integer:
        The number of matches.

    """
    return context.count_species_matches(deref(pttrn.thisptr), deref(sp.thisptr))

def format_species(Species sp):
    """format_species(sp) -> Species

    Return a species uniquely reformatted.

    """
    cdef Cpp_Species newsp = context.format_species(deref(sp.thisptr))
    return Species_from_Cpp_Species(address(newsp))

# def unique_serial(Species sp):
#     """unique_serial(sp) -> str
# 
#     Return a serial of a species uniquely reformatted.
#     This is equivalent to call ``format_species(sp).serial()``
# 
#     """
#     return context.unique_serial(deref(sp.thisptr)).decode('UTF-8')
