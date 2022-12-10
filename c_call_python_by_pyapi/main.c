#include <Python.h>

#if PY_MAJOR_VERSION >= 3
#define PyInt_Check PyLong_Check
#define PyInt_AsLong PyLong_AsLong
#endif

int main(int argc, char *argv[]) {
  // 初始化python解释器.C/C++中调用Python之前必须先初始化解释器
  Py_Initialize();
  // 执行一个简单的执行python脚本命令
  PyRun_SimpleString("print('hello world')\n");
  PyRun_SimpleString("import sys");
  PyRun_SimpleString("sys.path.append('.')");
  
  PyObject* pModule = PyImport_ImportModule("sum");
  if( pModule == NULL ){
	  printf("module not found");
    Py_Finalize();
		return -1;
  }
  // 4、调用函数
  PyObject* pFunc = PyObject_GetAttrString(pModule, "py_list_sum");
  if(!pFunc || !PyCallable_Check(pFunc)){
      printf("py_list_sum not found");
      Py_Finalize();
      return -1;
  }
  
  PyObject* pArgs = PyTuple_New(1);
  PyTuple_SetItem(pArgs, 0, Py_BuildValue("[i,i,i]", 5, 6, 7));//0—序号 i表示创建int型变量
  // 等价于调用 py_list_sum([5, 6, 7])
  //调用函数，pArgs元素个数与被调函数参数个数一致
  PyObject* ret = PyObject_CallObject(pFunc, pArgs);
  long int ret_val = PyInt_AsLong(ret);
  printf("py_list_sum([5, 6, 7]) = %ld \n", ret_val);
  // 撤销Py_Initialize()和随后使用Python/C API函数进行的所有初始化
  Py_Finalize();
  return 0;
}