cdef class Space:
    """An abstract base class of all worlds. This is for developers.

    Space()

    """

    def __init__(self):
        """Constructor"""
        pass

    def __cinit__(self):
        self.thisptr = new shared_ptr[Cpp_Space](
            <Cpp_Space*>(new Cpp_CompartmentSpaceVectorImpl(
                Cpp_Real3(1, 1, 1)))) #XXX: DUMMY

    def __dealloc__(self):
        del self.thisptr
