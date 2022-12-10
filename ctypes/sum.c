int sum(int* buffer, int len) {
    int ret = 0;
    for(int i = 0; i < len; ++i)
        ret += buffer[i];
    return ret;
}