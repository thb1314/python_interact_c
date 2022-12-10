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