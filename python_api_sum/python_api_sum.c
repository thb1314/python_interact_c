#include <Python.h>

#if PY_MAJOR_VERSION >= 3
#define PyInt_Check PyLong_Check
#define PyInt_AsLong PyLong_AsLong
#endif

static PyObject* list_sum(PyObject *self, PyObject *args)
{
    PyObject *pList;
    PyObject *pItem;
    Py_ssize_t n = 0;
    int result = 0;
    if(!PyArg_ParseTuple(args, "O!", &PyList_Type, &pList))
    {
        return NULL;
    }
    n = PyList_Size(pList);
    for (int i=0; i<n; i++) {
        pItem = PyList_GetItem(pList, i);
        if(!PyInt_Check(pItem)) {
            PyErr_SetString(PyExc_TypeError, "list items must be integers.");
            return NULL;
        }
        result += PyInt_AsLong(pItem);
    }
    
    return Py_BuildValue("i", result);
}

static PyMethodDef methods[] = {
   { "sum", (PyCFunction)list_sum, METH_VARARGS, "sum method" },
   { NULL, NULL, 0, NULL }
};

static struct PyModuleDef python_api_sum_module = {
    PyModuleDef_HEAD_INIT,
    "python_api_sum",
    "Python interface for the array sum",
    -1,
    methods
};
 
PyMODINIT_FUNC PyInit_python_api_sum(void)
{
   return PyModule_Create(&python_api_sum_module);
}