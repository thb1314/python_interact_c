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
