#pragma once

struct MemoryBlock
{
    //TODO:36 some block of memory, either virtual address space or physically reserved

    MemoryBlock &next;
};

struct Arena
{
    //TODO:#36: custom memory arena allocator thingy!

    MemoryBlock &start;

    //TODO:#36 some kind of specifiable freelist
};
