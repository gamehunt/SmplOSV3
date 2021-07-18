#include <stdio.h>
#include <string.h>

#include <dev/log.h>
#include <dev/vfs.h>
#include <memory.h>

static fs_type_t*  fs_types[256];
static uint8_t     last_fs_type = 0;

static vfs_node_t* vfs_root = 0;

static uint8_t vfs_typeid = 0;

vfs_node_t* vfs_create_node(const char* name, void* inode, uint8_t type, vfs_node_t* parent){
    vfs_node_t* node   = kmalloc(sizeof(vfs_node_t));
    node->name         = name;
    node->fs_type      = type;
    node->inode        = inode;
    node->device       = 0;
    node->childs_count = 0;
    node->childs       = 0;

    node->dirent       = kmalloc(sizeof(vfs_dirent_t));

    if(parent){
        vfs_add_child(parent, node);
    }

    return node;
}

vfs_node_t* vfs_get_root(){
    return vfs_root;
}

uint8_t vfs_get_type(){
    return vfs_typeid;
}


uint8_t     vfs_register_type(fs_type_t* type){
    fs_types[last_fs_type] = type;
    last_fs_type++;
    return last_fs_type - 1;
}

uint8_t     vfs_add_child(vfs_node_t* parent, vfs_node_t* child){
    if(!parent->dirent->childs_count){
        parent->dirent->childs = kmalloc(sizeof(vfs_node_t*));
    }else{
        parent->dirent->childs = krealloc(parent->childs, sizeof(vfs_node_t*)*(parent->childs_count + 1));
    }
    parent->dirent->childs[parent->childs_count] = child;
    parent->dirent->childs_count++;
    child->dirent->parent = parent;
    return 0;
}

vfs_dirent_t* kreaddir(const char* path){
    vfs_node_t* node = kseek(path);
    if(node){
        if(fs_types[node->fs_type]->readdir){
            return fs_types[node->fs_type]->readdir(node);
        }else{
            return fs_types[vfs_typeid]->readdir(node);
        }
    }
    return 0;
}

uint8_t kmount(const char* path, const char* device, uint8_t type){
    vfs_node_t* node = kseek(path);
    if(!node){
        node = kcreate(path, type);
    }
    vfs_node_t* device_node = kseek(device);
    if(device){
        if(fs_types[node->fs_type]->mount){
            fs_types[node->fs_type]->mount(node, device_node);
        }else{
            fs_types[vfs_typeid]->mount(node, device_node);    
        }
        info("Mounted %s to %s with type %d", device, path, type);
        return 0;
    }else{
        error("Failed to mount %s to %s: device not exists", device, path);
        return 1;
    }
    
}

uint64_t    kread(vfs_node_t* node,uint64_t offset, uint64_t size, uint8_t* buffer){
    if(fs_types[node->fs_type]->read){
        return fs_types[node->fs_type]->read(node, offset, size, buffer);
    }else{
        return fs_types[vfs_typeid]->read(node, offset, size, buffer);
    }
}

uint64_t    kwrite(vfs_node_t* node,uint64_t offset, uint64_t size, uint8_t* buffer){
    if(fs_types[node->fs_type]->write){
        return fs_types[node->fs_type]->write(node, offset, size, buffer);
    }else{
        return fs_types[vfs_typeid]->write(node, offset, size, buffer);
    }
}

vfs_node_t* kcreate(const char* path, uint8_t type){
    vfs_node_t* node = kseek(path);
    if(!node){
        node = vfs_get_root();
        char* copy = strdup(path);
        char * path_part = strtok(copy, "/"); 
        while (path_part != NULL)
        {
            vfs_node_t* prev_node = node;
            node = fs_types[vfs_typeid]->seek(path_part, node);
            if(!node){
                node = fs_types[vfs_typeid]->create(strdup(path_part), prev_node);
            }
            path_part = strtok(NULL, "/");
        }
        kfree(copy);        
    }else{
        warning("Failed to create %s, node exists", path);
    }
    return node;
}

vfs_node_t* kseek(const char* path){
  char * copy = strdup(path);
  char * path_part = strtok(copy, "/"); 
  vfs_node_t* root = vfs_get_root();
  while (path_part != NULL)
  {
      if(fs_types[root->fs_type]->seek){
          root = fs_types[root->fs_type]->seek(path_part, root);
      }else{
          root = fs_types[vfs_typeid]->seek(path_part, root);
      }
      if(!root){
          kfree(copy);
          return 0;
      }
      path_part = strtok(NULL, "/");
  }
  kfree(copy);
  return root;
}  


static vfs_node_t* vfs_create(const char* name, vfs_node_t* parent){
    return vfs_create_node(name, 0, vfs_typeid, parent);
}

static uint8_t vfs_mount(vfs_node_t* to, vfs_node_t* from){
    to->device  = from;
    to->fs_type = from->fs_type;
}

static uint64_t vfs_read(vfs_node_t* node,uint64_t offset, uint64_t size, uint8_t* buffer){
    if(node->inode){
        memcpy(buffer, (void*)((uint64_t)node->inode + offset), size);
        return size;
    }
    return 0;
}

static uint64_t vfs_write(vfs_node_t* node,uint64_t offset, uint64_t size, uint8_t* buffer){
    if(node->inode){
        memcpy((void*)((uint64_t)node->inode + offset), buffer, size);
        return size;
    }
    return 0;
}

static vfs_node_t* vfs_seek(const char* path, vfs_node_t* node){
    for(int i=0;i<node->dirent->childs_count;i++){
        if(!strcmp(path, node->dirent->childs[i]->name)){
            return node->dirent->childs[i];
        }
    }
    return 0;
}  

static vfs_dirent_t* vfs_readdir(vfs_node_t* node){
    return node->dirent;
}

uint8_t vfs_init(){

    fs_type_t* type = kmalloc(sizeof(fs_type_t));

    type->read    = vfs_read;
    type->write   = vfs_write;
    type->mount   = vfs_mount;
    type->seek    = vfs_seek;
    type->create  = vfs_create;
    type->readdir = vfs_readdir;

    vfs_typeid = vfs_register_type(type);

    vfs_root = vfs_create_node("[root]", 0, vfs_typeid, 0);

    kcreate("/dev",  vfs_typeid);
    kcreate("/proc", vfs_typeid);

    info("Initialized VFS");

    //vfs_print_tree();
}

static void vfs_print_node(vfs_node_t* node, int spaces){
    char buffer[2048];
    memset(buffer, '\0', 2048);
    for(int i=0;i<spaces;i++){
        buffer[i] = '-';
    }
    printf("%s%s\n\r", buffer, node->name);
    for(int i=0;i<node->childs_count;i++){
        vfs_print_node(node->childs[i], spaces+1);
    }
}

void  vfs_print_tree(){
    debug("VFS tree:");
    vfs_print_node(vfs_get_root(), 0);
}
