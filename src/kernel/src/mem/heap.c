#include <memory.h>
#include <util.h>
#include <stddef.h>
#include <dev/log.h>
#include <panic.h>

static void *heap_top = (void *)HEAP_START;
static uint64_t heap_end;
static memblock_t *free_list = 0;

void *kalloc(uint64_t start, uint64_t size, uint8_t contigious)
{
    return kalloc_f(start, size, contigious, PAGE_FLAG_WRITABLE);
}

void *kalloc_f(uint64_t start, uint64_t size, uint8_t contigious, uint16_t flags)
{
    SUPPRESS_UNUSED(contigious);
    for (uint32_t i = 0; i < size; i++)
    {
        mapf(falloc(), start + i * 4096, flags);
    }
    return (void *)start;
}

static memblock_t *find_free_block(uint64_t size)
{
    if (!free_list)
    {
        return 0;
    }
    memblock_t *block = free_list;
    while (block)
    {
        if (block->size == size)
        {
            return block;
        }
        if (block->size > size + sizeof(memblock_t))
        {
            uint64_t o_size = block->size;
            block->size = size;
            memblock_t *new_block = (memblock_t *)((uint64_t)PTR(block) + size);
            new_block->magic[0] = 'M';
            new_block->magic[1] = 'E';
            new_block->magic[2] = 'M';
            new_block->size = o_size - size - sizeof(memblock_t);
            if (block->next)
            {
                block->next->prev = new_block;
            }
            new_block->next = block->next;
            new_block->prev = block;
            block->next = new_block;
            return block;
        }
        block = block->next;
    }
    return 0;
}

void *kmalloc(uint64_t size)
{
    memblock_t *free_block = find_free_block(size);
    if (!free_block)
    {
        //info("%")
        if ((uint64_t)heap_top + size + sizeof(memblock_t) > heap_end)
        {
            panic(NULL, "Out of memory");
            return 0;
        }
        free_block = heap_top;
        free_block->size = size;
        free_block->magic[0] = 'M';
        free_block->magic[1] = 'E';
        free_block->magic[2] = 'M';
        free_block->next = 0xAAAAAAAABBBBBBBB;
        free_block->prev = 0xCCCCCCCCDDDDDDDD;
        heap_top = (void *)((uint64_t)heap_top + sizeof(memblock_t) + size);
        return PTR(free_block);
    }
    if (free_block->next && free_block->prev)
    {
        free_block->prev->next = free_block->next->prev;
    }
    else if (free_block->prev)
    {
        free_block->prev->next = 0;
    }
    else if (free_block->next)
    {
        free_block->next->prev = 0;
    }
    if (free_block == free_list)
    {
        free_list = free_block->next;
    }
    return PTR(free_block);
}

static void merge_blocks()
{
    memblock_t *block = free_list;
    while (block)
    {
        while ((uint64_t)PTR(block) + block->size == (uint64_t)block->next)
        {
            block->size += block->next->size + sizeof(memblock_t);
            block->next = block->next->next;
            block->next->prev = block;
        }
        block = block->next;
    }
}

void kfree(void *mem)
{
    memblock_t *hdr = HDR(mem);
    if (!free_list)
    {
        free_list = hdr;
        free_list->prev = 0;
        free_list->next = 0;
    }
    else
    {
        if ((uint64_t)free_list > (uint64_t)hdr)
        {
            hdr->next = free_list;
            hdr->prev = 0;
            free_list->prev = hdr;
            free_list = hdr;
        }
        else
        {
            memblock_t *parent = free_list;
            while (parent->next && (uint64_t)parent->next < (uint64_t)hdr)
            {
                parent = parent->next;
            }
            if (parent->next == 0)
            {
                parent->next = hdr;
                hdr->prev = parent;
                hdr->next = 0;
            }
            else
            {
                // B1 P X B2
                parent->next->prev = hdr;
                hdr->next = parent->next;
                parent->next = hdr;
                hdr->prev = parent;
            }
        }
        merge_blocks();
    }
}

void print_free_list()
{
    memblock_t *block = free_list;
    while (block)
    {
        info("-----%llx-----", block);
        info("Magic: %c%c%c", block->magic[0], block->magic[1], block->magic[2]);
        info("Size: %d (%d KB)", block->size, block->size / 1024);
        info("Prev: 0x%llx", block->prev);
        info("Next: 0x%llx", block->next);
        block = block->next;
    }
}

void init_heap(uint64_t size)
{
    kalloc_f(HEAP_START, size / 4096, 0, PAGE_FLAG_WRITABLE | PAGE_FLAG_GLOBAL);
    heap_end = HEAP_START + size * 4096;
    info("Initialized heap of size %d MB", size / 1024 / 1024);
}

void *krealloc(void *old, uint64_t new_size)
{
    memblock_t *block = HDR(old);
    void *new_mem = kmalloc(new_size);
    memcpy(new_mem, old, new_size > block->size ? block->size : new_size);
    kfree(old);
    return new_mem;
}

void *kvalloc(uint64_t size, uint64_t alignment)
{
    void *p1;  // original block
    void **p2; // aligned block
    int offset = alignment - 1 + sizeof(void *);
    if ((p1 = (void *)kmalloc(size + offset)) == NULL)
    {
        return NULL;
    }
    p2 = (void **)(((size_t)(p1) + offset) & ~(alignment - 1));
    p2[-1] = p1;
    return p2;
}

void kvfree(void *aligned_ptr)
{
    int offset = *(((char *)aligned_ptr) - 1);
    kfree(((char *)aligned_ptr) - offset);
}
