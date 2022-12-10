import py_swig_sum

print(py_swig_sum)
print(dir(py_swig_sum))
print(py_swig_sum.list_sum([5,6,7]))

iv = py_swig_sum.IntVector(4)
for i in range(4):
    iv[i] = i + 1

print(py_swig_sum.list_sum(iv))
