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
