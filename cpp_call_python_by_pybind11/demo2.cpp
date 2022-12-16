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

    py::exec(R"(
        kwargs = dict(name="World", number=42)
        message = "Hello, {name}! The answer is {number}".format(**kwargs)
        print(message)
    )");

    return 0;
}