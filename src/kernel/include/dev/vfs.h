#ifndef K_SMPLOS_VFS_H
#define K_SMPLOS_VFS_H

#if defined(K_SMPLOS_KERNEL)
    #include <util.h>
#else
    #include <kernel/util.h>
#endif

struct vfs_node;

typedef struct{
    struct vfs_node** childs;
    struct vfs_node*  parent;
    uint32_t          childs_count;
}vfs_dirent_t;
struct vfs_node{
    const char*       name;
    uint8_t           fs_type;
    void*             inode;
    struct vfs_node*  device;
    vfs_dirent_t*     dirent;
};
typedef struct vfs_node vfs_node_t;

typedef struct{
    uint8_t* (*mount)(vfs_node_t* root, vfs_node_t* device);
    uint64_t   (*read)(vfs_node_t*, uint64_t, uint64_t, uint8_t* buffer);
    uint64_t   (*write)(vfs_node_t*, uint64_t, uint64_t, uint8_t* buffer);
    vfs_dirent_t* (*readdir)(vfs_node_t*);
    vfs_node_t* (*seek)(const char*, vfs_node_t*);
    vfs_node_t* (*create)(const char*, vfs_node_t*);
}fs_type_t;

CH_START

vfs_node_t* vfs_create_node(const char* name, void* inode, uint8_t type, vfs_node_t* parent);
uint8_t     vfs_register_type(fs_type_t*); 
uint8_t     vfs_add_child(vfs_node_t* parent, vfs_node_t* child); 

vfs_node_t* vfs_get_root();
uint8_t     vfs_get_type();

uint8_t       kmount(const char* path, const char* device, uint8_t type);
uint64_t      kread(vfs_node_t* node,uint64_t offset, uint64_t size, uint8_t* buffer);
uint64_t      kwrite(vfs_node_t* node,uint64_t offset, uint64_t size, uint8_t* buffer);
vfs_node_t*   kcreate(const char* path, uint8_t type);
vfs_dirent_t* kreaddir(const char* path);
vfs_node_t*   kseek(const char* path); 

uint8_t     vfs_init();

void        vfs_print_tree();

CH_END

#endif