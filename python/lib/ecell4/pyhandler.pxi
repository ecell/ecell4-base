
from cpython cimport PyObject, Py_XINCREF, Py_XDECREF

#ctypedef void (*Pyhandler_Functype_single_argument_func_type)(void*)

cdef void pyhandler_inc_ref(void* obj):
    Py_XINCREF(<PyObject*>obj)

cdef void pyhandler_dec_ref(void* obj):
    Py_XDECREF(<PyObject*>obj)

cdef class PyObjectHandler:
    def __cinit__(self):
        self.thisptr = shared_ptr[Cpp_PyObjectHandler](
                <Cpp_PyObjectHandler*>(new Cpp_PyObjectHandler( 
                    <PyObjectHandler_single_argument_func_type>pyhandler_inc_ref, 
                    <PyObjectHandler_single_argument_func_type>pyhandler_dec_ref) ) )

    def __dealloc__(self):
        #del self.thisptr
        pass

    def is_available(self):
        self.thisptr.get().is_available()

    


