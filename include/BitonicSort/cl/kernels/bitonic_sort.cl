__kernel void bitonic_merge(
    __global int* data,
    __local int* local_data,
    int step,
    int stage
) {
    int gid = get_global_id(0);
    int lid = get_local_id(0);
    int group_size = get_local_size(0);

    if (stage < group_size) {

        local_data[lid] = data[gid];
        barrier(CLK_LOCAL_MEM_FENCE);

        int id2swap = lid ^ stage;
        if (id2swap > lid) {

            bool ascending = (gid & step) == 0;
            int val1 = local_data[lid];
            int val2 = local_data[id2swap];

            if ((val1 > val2) == ascending) {
                local_data[lid] = val2;
                local_data[id2swap] = val1;
            }

        }
        barrier(CLK_LOCAL_MEM_FENCE);
        data[gid] = local_data[lid];

    } else {

        int id2swap = gid ^ stage;
        if (id2swap > gid) {

            bool ascending = (gid & step) == 0;
            int val1 = data[gid];
            int val2 = data[id2swap];

            if ((val1 > val2) == ascending) {
                data[gid] = val2;
                data[id2swap] = val1;
            }
        }

    }
}