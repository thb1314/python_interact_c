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