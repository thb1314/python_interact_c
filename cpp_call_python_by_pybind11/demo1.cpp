#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>
#include <vector>
#include <iostream>


namespace py = pybind11;

int main(int argc, char *argv[]) {
  py::scoped_interpreter guard{}; 
  py::object sum = py::module_::import("sum");
  py::object py_list_sum = sum.attr("py_list_sum");
  int result = py_list_sum(std::vector<int>{1,2,3,4,5}).cast<int>();
  std::cout << "py_list_sum([1,2,3,4,5]) result:" << result << std::endl;
  return 0;
}
