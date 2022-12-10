#include <vector>

// swig std::vector会自动与python中list类型绑定
int list_sum(const std::vector<int>& buffer) {
    int ret = 0;
    for(auto item: buffer)
        ret += item;
    return ret;
}