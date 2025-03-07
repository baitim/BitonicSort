#define swap_xor(a, b) { a ^= b; b ^= a; a ^= b; }

__kernel void bitonic_merge(__global int* data, int step, int stage) {
    int gid = get_global_id(0);
    
    int id2swap = gid ^ stage;

    if (id2swap > gid) {
        bool ascending = ((gid & step) == 0);
        if ((data[gid] > data[id2swap]) == ascending)
            swap_xor(data[gid], data[id2swap]);
    }
}