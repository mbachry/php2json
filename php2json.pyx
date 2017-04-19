from cpython.mem cimport PyMem_Malloc, PyMem_Free


cdef extern from "lib/php2json.h":
    int _php2json(char* src, char* dst, char *err, int err_len)


def php2json(a):
    cdef int l = len(a)
    cdef char err[256]
    cdef char *dst = <char *>PyMem_Malloc(l * 2)
    cdef int res = _php2json(a, dst, err, sizeof(err))
    if res < 0:
        PyMem_Free(dst)
        raise ValueError(err.decode('utf8', 'replace'))
    cdef object s = dst
    PyMem_Free(dst)
    return s
