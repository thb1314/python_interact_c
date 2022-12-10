# 总结Python和C/C++的交互方式

## 一. 前言

本文旨在总结Python如何调用C/C++，以及在C/C++代码中如何调用Python，所给案例较为简单，抛砖引玉，欢迎读者自行拓展。

> 实验环境
>
> - System: Ubuntu 22.04.1 LTS  
> - GCC version: 11.2.0  
> - Python version: miniconda安装的python3.7.13  
>
> 本文的所有需要安装的包为了可复现性，都指定了版本，如果读者需要使用最新版本还请去掉版本限制。

本文代码开源地址：

[https://github.com/thb1314/python_interact_c](https://github.com/thb1314/python_interact_c)

## 二. Python调用C

### 2.1 采用ctypes库调用C编译产生的动态接库

#### 2.1.1 预备知识 so文件的生成与使用

这里以对数组进行求和的函数为例，采用gcc作为编译器，样例代码在ctypes文件夹

`sum.c`文件

```c
int sum(int* buffer, int len) {
    int ret = 0;
    for(int i = 0; i < len; ++i)
        ret += buffer[i];
    return ret;
}
```

shell指令

```
gcc sum.c -fPIC -shared -std=c99 -o libsum.so
```

使用`sum.so`文件，编写`test.c`内容如下

```c
#include <stdio.h>

int sum(int* buffer, int len);

int main(void) {
    const int length = 10;
    int buffer[length];
    int buffer_sum = 0;
    for(int i = 0; i < length; ++i) {
        buffer[i] = i;
    }
    buffer_sum = sum(buffer, length);
    printf("buffer_sum = %d\n", buffer_sum); 
    return 0;
}
```

shell指令

```bash
# 方式一
gcc -o test test.c libsum.so
# 方式二
gcc -o test test.c -L. -lsum

# 运行test
./test
```

对于以上指令还请读者自行查阅相关资料了解其中含义

#### 2.1.2 Python Ctypes库调用动态链接库so中的函数

ctypes 是Python的外部函数库。它提供了与 C语言兼容的数据类型，并允许调用 DLL 或共享库中的函数。可使用该模块以纯 Python 形式对这些库进行封装。  

还是以调用libsum.so中的sum函数为例，编写`ctypes_sum.py`对libsum.so中的sum函数进行封装，内容如下

```python
import ctypes


# windows平台用  ctypes.WinDLL
libsum_so = ctypes.CDLL("./libsum.so") 


def sum(int_list):
    for item in int_list:
        assert isinstance(item, int)
    func = libsum_so.sum
    length = len(int_list)
    first_arg_type = ctypes.c_int * length
    first_arg = first_arg_type()
    for i in range(length):
        first_arg[i] = int_list[i]
    return int(func(first_arg, length))


if __name__ == "__main__":
    print(sum([5, 6, 7, 8]))
```



### 2.2 采用pybind11创建C++代码的Python接口

Pybind11 是一个轻量级的 C++ 库，用于将你的 C++ 代码暴露给 Python 调用（反之也可，但主要还是前者）。Pybind11 借鉴了 `Boost::Python` 库的设计，但使用了更为简洁的实现方式，使用了大量 C++11 的新特性，更易于使用。  

官方文档：[https://pybind11.readthedocs.io/en/stable/index.html](https://pybind11.readthedocs.io/en/stable/index.html)

#### 2.2.1 Pybind11特点

Pybind11 允许你在 C++ 代码中使用如下特性，并在 Python 中方便地调用。

- 允许函数参数使用自定义类型（包括值、指针和引用）；
- 类成员函数与静态函数；
- 函数重载；
- 类成员与静态成员；
- 异常；
- 枚举；
- 回调函数；
- 迭代器和范围(`range`);
- 自定义 `operator`；
- 继承（包括多重继承）；
- STL 中的数据结构；
- 智能指针；
- 带有引用计数的内部引用；
- 在 C++ 中定义虚方法，并在 Python 中进行扩展；

Pybind11 的优点有：

- 兼容性强，支持 Python2.7、Python3.x、PyPy (PyPy2.7 >= 5.7)；
- 可以在 C++ 中使用 lambda 表达式，并在 Python 中使用捕获的变量；
- 大量使用移动特性，保证数据转移时的性能；
- 可以很方便地通过 Python buffer protocol 进行数据类型的转移；
- 可以很方便地对函数进行向量化加速；
- 支持使用 Python 的切片语法；
- Pybind11 是 header-only 的，只需要包含头文件即可；
- 相比于 `Boost::Python`，生成的二进制文件体积更小；
- 函数签名通过 `constexper` 提前计算，进一步减小二进制文件体积；
- C++ 中的类型可以很容易地进行序列化/反序列化；

pybind11的安装：

```bash
pip install pybind11==2.10.1
```

#### 2.2.2 代码示例

样例代码在pybind11_sum文件夹

首先创建c++文件`pybind11_sum.cpp`

```c++
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace py = pybind11;

// 引入 pybind11/stl.h 后 std::vector会自动与python中list类型绑定
int sum(const std::vector<int>& buffer) {
    int ret = 0;
    for(auto item: buffer)
        ret += item;
    return ret;
}

// pybind11_sum 这里约定要与文件名相同
PYBIND11_MODULE(pybind11_sum, m) {
    m.doc() = "pybind11 sum plugin"; // optional module docstring
    m.def("sum", &sum, "A function which calulate sum in buffer",
          py::arg("buffer"));
}
```

然后运行shell命令

```bash
g++ -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) pybind11_sum.cpp -o pybind11_sum$(python3-config --extension-suffix)
```

测试文件`test_pybind.py`

```python
import pybind11_sum

print(pybind11_sum)
print(dir(pybind11_sum))
print(pybind11_sum.sum([5, 6, 7]))
```



#### 2.2.3 应用案例

目前市面上大部分 AI 计算框架,如 TensorFlow、Pytorch、阿里 X-Deep Learning、百度 PaddlePaddle 等,均使用 pybind11来提供 C++到 Python 端接口封装。

Pytorch pybind extension文档地址

[https://pytorch.org/tutorials/advanced/cpp_extension.html](https://pytorch.org/tutorials/advanced/cpp_extension.html)

### 2.3 使用Python的C-API创建Python扩展

#### 2.3.1 代码演示

样例代码在python_api_sum文件夹

直接使用Python开放的api也可以创建直接import的动态库

创建python_api_sum.c

```c++
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
```

在shell窗口执行如下指令：

```bash
gcc -Wall -shared  -std=c99 -fPIC $(python3-config --includes) $(python3-config --ldflags) python_api_sum.c -o python_api_sum$(python3-config --extension-suffix)
```

测试代码文件`test_python_api.py`

```python
import python_api_sum

print(python_api_sum)
print(dir(python_api_sum))
print(python_api_sum.sum([5, 6, 7, 8]))
```



#### 2.3.2 应用举例

从上面可以看出，该方法比较复杂，且较为底层。越底层的东西在用的好的情况下自然效率越高，所以比如在pytorch官方源码中就对一些算子运算的底层设计采用该方式做封装，比如 [https://github.com/pytorch/pytorch/blob/master/tools/autograd/templates/python_sparse_functions.cpp](https://github.com/pytorch/pytorch/blob/master/tools/autograd/templates/python_sparse_functions.cpp)



### 2.4 使用SWIG编译

SWIG是简单包装器和接口生成器，是一个适用于多种语言的工具。一方面，它让你可以使用C/C++编写扩展代码；另一方面，它自动包装这些代码，并且让这些代码在Java、Python、Tcl、Perl和Ruby等高级语言中运行。

官方网址：[http://www.swig.org](http://www.swig.org)

文档地址：https://www.swig.org/doc.html

#### 2.4.1 代码示例

安装方式

```.cbash
# 如果下载不下来还请挂个梯子
wget https://github.com/swig/swig/archive/refs/tags/v4.1.1.zip

unzip v4.1.1.zip
cd swig-4.1.1
sudo apt install automake
sudo apt install bison
./autogen.sh
mkdir -p ../swig
./configure --prefix="`pwd`/../swig" --without-pcre --without-perl5
make
make install
export PATH="`pwd`/../swig/bin:$PATH"
```

编写`example1.c`

```c
/* File : example1.c */
double My_variable = 3.0;

/* Compute factorial of n */
int fact(int n)
{
    if (n <= 1)
        return 1;
    else
        return n * fact(n - 1);
}

/* Compute n mod m */
int my_mod(int n, int m)
{
    return (n % m);
}

```

编写`example1.i`

```c
/* File : example1.i */
%module example
%{
/* Put headers and other declarations here */
extern double My_variable;
extern int fact(int);
extern int my_mod(int n, int m);
%}
extern double My_variable;
extern int fact(int);
extern int my_mod(int n, int m);
```

执行swig命令生成`example1_wrap.c`

```bash
swig -python -interface example example1.i
```

生成python扩展，这里实际上与Python API方式相同，swig就是做了这样的事情，自动生成了这部分代码。

```bash
gcc -Wall -shared  -std=c99 -fPIC $(python3-config --includes) $(python3-config --ldflags) example1.c example1_wrap.c -o example$(python3-config --extension-suffix)
```



上面是Swig官方的例子，我们再看一个对list求和的例子

首先定义.h文件`example2_sum.h`

```cpp
#include <vector>

// swig std::vector会自动与python中list类型绑定
int list_sum(const std::vector<int>& buffer) {
    int ret = 0;
    for(auto item: buffer)
        ret += item;
    return ret;
}
```

然后给出`.i`文件说明需要导出的东西(`example2_sum.i`)

```
%module py_swig_sum
%{
#include "example2_sum.h"
%}

%include "std_vector.i"
// Instantiate templates 
namespace std {
    %template(IntVector) vector<int>;
}
// Include the header file with above prototypes
%include "example2_sum.h"

```

然后输入swig转换指令

```bash
swig -c++ -python -interface swig_sum -o example2_sum_wrap.cpp example2_sum.i
```

最后编译python库

```
g++ -Wall -shared  -std=c++11 -fPIC $(python3-config --includes) $(python3-config --ldflags) example2_sum_wrap.cpp -o swig_sum$(python3-config --extension-suffix)
```

测试代码如下

```python
import py_swig_sum

print(py_swig_sum)
print(dir(py_swig_sum))
print(py_swig_sum.list_sum([5,6,7]))

iv = py_swig_sum.IntVector(4)
for i in range(4):
    iv[i] = i + 1

print(py_swig_sum.list_sum(iv))
```

更多信息还请读者阅读文档，个人感觉swig还是做得不错的。

### 2.5 使用Cython

**关于 Cython，我们必须要清楚两件事：**

1）Cython 是一门编程语言，它将 C 和 C++ 的静态类型系统融合在了 Python 身上。Cython 源文件的后缀是 .pyx，它是 Python 的一个超集，语法是 Python 语法和 C 语法的混血。当然我们说它是 Python 的一个超集，因此你写纯 Python 代码也是可以的。

2）当我们编写完 Cython 代码时，需要先将 Cython 代码翻译成高效的 C 代码，然后再将 C 代码编译成 Python 的扩展模块。

如上，正因为Cython是一门编程语言，因此不是很推荐学习（笑哭）。

在早期，编写 Python 扩展都是拿 C 去写，但是这对开发者有两个硬性要求：一个是熟悉 C，另一个是要熟悉解释器提供的 C API，这对开发者是一个非常大的挑战。此外，拿 C 编写代码，开发效率也非常低。

而 Cython 的出现则解决了这一点，Cython 和 Python 的语法非常相似，我们只需要编写 Cython 代码，然后再由 Cython 编译器将 Cython 代码翻译成 C 代码即可。所以从这个角度上说，拿 C 写扩展和拿 Cython 写扩展是等价的。

至于如何将 Cython 代码翻译成 C 代码，则依赖于相应的编译器，这个编译器本质上就是 Python 的一个第三方模块。它就相当于是一个翻译官，既然用 C 写扩展是一件痛苦的事情，那就拿 Cython 去写，写完了再帮你翻译成 C。

因此 Cython 的强大之处就在于它将 Python 和 C 结合了起来，可以让你像写 Python 代码一样的同时还可以获得 C 的高效率。所以我们看到 Cython 相当于是高级语言 Python 和低级语言 C 之间的一个融合，因此有人也称 Cython 是 "克里奥尔编程语言"（creole programming language）。



Cython安装

```bash
pip install cython==0.29.32
```



#### 2.5.1 代码示例

我们依然给出的是对list元素进行sum的案例，样例代码见`cython_demo'`

**python代码示例**

`sum.pyx`

```pyx
def py_list_sum(buffer):
    return sum(buffer)
```

创建包安装文件`setup.py`

```python
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import glob
import os

pyx_filelist = glob.glob(os.path.join(os.path.realpath("./"), "**", "*.pyx"), recursive=True)

extension_name = "sum_py"
ext_modules = [
    Extension(extension_name,pyx_filelist)
]

setup(
    name = "Hello pyx",
    version="1.0",
    cmdclass = {'install': build_ext},
    ext_modules = ext_modules
)
```

构建命令

```bash
 python setup.py install --inplace
```

本文作者对cython的语法实在不感兴趣，估略去后续探索。

## 三. C/C++调用Python

### 3.1 采用Python原生API

c++调用 python ，本质上是在 c++ 中启动了一个 python 解释器，由解释器对 python 相关的代码进行执行，执行完毕后释放资源，达到调用目的。

代码示例如下（在`c_call_python_by_pyapi`文件夹）：

首先创建`main.cpp`

```c++
#include <Python.h>

int main(int argc, char *argv[]) {
  // 初始化python解释器.C/C++中调用Python之前必须先初始化解释器
  Py_Initialize();
  // 执行一个简单的执行python脚本命令
  PyRun_SimpleString("print('hello world')\n");
  PyRun_SimpleString("import sys");
  PyRun_SimpleString("sys.path.append('.')");
  
  PyObject* pModule = PyImport_ImportModule("sum");
  if( pModule == NULL ){
		cout <<"module not found" << endl;
		return 1;
  }
    // 4、调用函数
    PyObject* pFunc = PyObject_GetAttrString(pModule, "say");
    if( !pFunc || !PyCallable_Check(pFunc)){
        cout <<"not found function add_num" << endl;
        return 0;
    }
    // 
    PyObject_CallObject(pFunc, NULL);
  // 撤销Py_Initialize()和随后使用Python/C API函数进行的所有初始化
  Py_Finalize();
  return 0;
}
```

这里给出`sum.py`内容

```python
def py_list_sum(buffer):
    return sum(buffer)
```



shell编译命令如下

```bash
# 采用一个低版本的g++
/usr/bin/gcc main.c -std=c99 $(python3-config --cflags) $(python3-config --ldflags) -fno-lto -o main

./main
```



### 3.2 采用Pybind11扩展调用python

pybind11通过简单的C++包装公开了Python类型和函数，这使得我们可以方便的在C++中调用Python代码，而无需借助Python C API。

`demo1.cpp`

```c++
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <iostream>


namespace py = pybind11;

int main(int argc, char *argv[]) {
  py::object sum = py::module_::import("sum");
  py::object py_list_sum = sum.attr("py_list_sum");
  int result = py_list_sum(std::vector<int>{1,2,3,4,5}).cast<int>();
  std::cout << "result:" << result << std::endl;
  return 0;
}

```

`demo2.cpp`

```cpp
#include <iostream>
#include <pybind11/embed.h>

namespace py = pybind11;

int main()
{
    std::cout << "Hello PyBind World" << std::endl;

    // start the interpreter and keep it alive
    py::scoped_interpreter guard{}; 
    py::module math = py::module::import("math");
    py::object result = math.attr("sqrt")(25);
    std::cout << "Sqrt of 25 is: " << result.cast<float>() << std::endl;
    return 0;
}
```

TODO：这里报错了。后续解决

```bash
g++ -std=c++11 -Wl,-Bstatic $(python3 -m pybind11 --includes) $(python3-config --includes) $(python3-config --ldflags) -fno-lto demo2.cpp -o demo2
```

## 四. 效率对比

由于我们的功能实现的较为简单，这里给出b站上一位up主给的关于效率问题做的总结

[https://www.bilibili.com/video/BV1Ng41167t6/](https://www.bilibili.com/video/BV1Ng41167t6/)  

| Solution     | Brief Description                                    | Scenarios                                                  |
| ------------ | ---------------------------------------------------- | ---------------------------------------------------------- |
| Python C API | 需要写C，复杂但性能最优                              | 追求极致性能                                               |
| ctypes       | 需要在Python里面写/生成boilerplate code              | 调用已有的动态库，且不涉及复杂数据结构，就调用几个function |
| SWIG         | 自动生成ext module的binding code，要写inferface file | 需要支持各种语言的binding，只需要写一份inferface文件       |
| pybind11     | 好写，支持C++feature，性能有牺牲                     | 有复杂的数据结构，Python和C/C++交互较多                    |

## 五. 总结

本文给出Python调用C/C++编写的扩展以及C/C++如何调用Python脚本和库的若干方法，并给出一些具体的应用案例。

从个人角度来讲，我更喜欢Pybind11和Python C API这两种方式。

Python API特点

- 速度快
- 对CPython的源码尤其是引用计数部分需要有深层次了解

Pybind11特点

- 完美融合c++11特性，无需掌握额外语法
- 速度相比于Python API欠佳，但是针对比如一些算子开发，当调用耗时不是主要占比的时候，该方式还是值得推荐。





