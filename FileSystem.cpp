#include "FileSystem.h"
#include "File.h"
#include "Link.h"
#include <iostream>


void print_tree_helper(Node* node, int indent) {
    if (node == nullptr) return;
    for (int i = 0; i < indent; i++) {
        std::cout << "  ";
    }
    std::cout << node->get_name() << std::endl;
    if (node->node_type() == NodeType::Folder) {
        Folder* folder = (Folder*)node;
        std::vector<std::string> children = folder->list_children();
        for (std::string childName : children) {
            Node* child = folder->find_child(childName);
            print_tree_helper(child, indent + 1);
        }
    }
}

FileSystem::FileSystem() {
    root_ = new Folder("/", nullptr);
}

FileSystem::FileSystem(const FileSystem &obj) {
    if (obj.root_ != nullptr) {
        root_ = (Folder*)(obj.root_->clone(nullptr));
    } else {
        root_ = nullptr;
    }
}

FileSystem &FileSystem::operator=(const FileSystem &other) {
    if (this != &other) {
        delete root_;
        if (other.root_ != nullptr) {
            root_ = (Folder*)(other.root_->clone(nullptr));
        } else {
            root_ = nullptr;
        }
    }
    return *this;
}

Node *FileSystem::resolve_path(const std::string &path) const {
    if (path.empty() || path[0] != '/') return nullptr;
    if (path == "/") return root_;
    Node *current = root_;
    std::string token = "";
    for (size_t i = 1; i <= path.length(); i++) {
        if (i == path.length() || path[i] == '/') {
            if (!token.empty()) {
                while (current && current->node_type() == NodeType::Link) {
                    current = ((Link*)current)->get_target();
                }
                if (!current || current->node_type() != NodeType::Folder) return nullptr;
                current = ((Folder*)current)->find_child(token);
                if (!current) return nullptr;
                token = "";
            }
        } else {
            token += path[i];
        }
    }
    return current;
}

void FileSystem::recursive_delete(Node *node) {
    if (!node) return;
    delete node;
}

FileSystem::~FileSystem() {
    delete root_;
}

void FileSystem::create_file(const std::string &path, const std::string &content) {
    if (path.empty()) throw std::invalid_argument("Invalid path.");
    int j=path.size()-1;
    while (j>-1){
        if (path[j]=='/') break;
        j--;
    }
    std::string parentPath;
    if (j==-1) throw std::invalid_argument("Invalid path.");
    if (j==0) parentPath="/";
    if (j!=0) parentPath=path.substr(0,j);
    std::string fileName = path.substr(j + 1);

    Node* parentNode = resolve_path(parentPath);
    while (parentNode && parentNode->node_type() == NodeType::Link) {
        parentNode = ((Link*)parentNode)->get_target();
    }
    if (parentNode == nullptr || parentNode->node_type() != NodeType::Folder) {
        throw std::runtime_error("No such file or directory.");
    }

    Folder* parentFolder = (Folder*)parentNode;
    if (parentFolder->find_child(fileName) != nullptr) {
        throw std::runtime_error("A file with this name already exists.");
    }
    parentFolder->add_file(fileName, content);
}

void FileSystem::create_folder(const std::string &path) {
    if (path.empty() || path == "/") {
        throw std::invalid_argument("Invalid path.");
    }
    int j=path.size()-1;
    while (j>-1){
        if (path[j]=='/') break;
        j--;
    }
    if (j==-1) throw std::invalid_argument("Invalid path.");
    std::string parentPath;
    if (j==0) parentPath="/";
    if (j!=0) parentPath=path.substr(0,j);
    std::string folderName = path.substr(j + 1);
    Node* parentNode = resolve_path(parentPath);
    while (parentNode && parentNode->node_type() == NodeType::Link) {
        parentNode = ((Link*)parentNode)->get_target();
    }
    if (parentNode == nullptr || parentNode->node_type() != NodeType::Folder) {
        throw std::runtime_error("No such file or directory.");
    }

    Folder* parentFolder = (Folder*)parentNode;
    if (parentFolder->find_child(folderName) != nullptr) {
        throw std::runtime_error("A folder with this name already exists.");
    }
    parentFolder->add_folder(folderName);
}

void FileSystem::create_link(const std::string &path, const std::string &target_path) {
    if (path.empty() || path == "/"){
        throw std::invalid_argument("Invalid path.");
    }
    int j=path.size()-1;
    while (j>-1){
        if (path[j]=='/') break;
        j--;
    }
    std::string parentPath;
    if (j==-1) throw std::invalid_argument("Invalid path.");
    if (j==0) parentPath="/";
    if (j!=0) parentPath=path.substr(0,j);
    std::string linkName=path.substr(j+1);
    Node *parentNode=resolve_path(parentPath);
    while (parentNode && parentNode->node_type()==NodeType::Link){
        parentNode=((Link*)parentNode)->get_target();
    }
    if (parentNode==nullptr || parentNode->node_type() != NodeType::Folder) {
        throw std::runtime_error("No such file or directory.");
    }
    Node *target_node=resolve_path(target_path);
    if (!target_node) throw std::runtime_error("No such file or directory.");
    Folder *parentFolder=(Folder*)parentNode;
    if (parentFolder->find_child(linkName)!=nullptr){
        throw std::runtime_error("A link with this name already exists.");
    }
    parentFolder->add_link(linkName,target_node);
}

bool FileSystem::remove_node(const std::string &path) {
    if (path == "/" || path.empty()) return false;
    int j=path.size()-1;
    while (j>-1){
        if (path[j]=='/') break;
        j--;
    }
    std::string parentPath;
    if (j==-1) return false;
    if (j==0) parentPath="/";
    if (j!=0) parentPath=path.substr(0,j);
    std::string nodeName = path.substr(j + 1);
    Node *parentNode = resolve_path(parentPath);
    while (parentNode && parentNode->node_type() == NodeType::Link) {
        parentNode = ((Link*)parentNode)->get_target();
    }
    if (!parentNode || parentNode->node_type() != NodeType::Folder) return false;

    Folder *parentFolder = (Folder*)(parentNode);
    Node *toDelete = parentFolder->find_child(nodeName);
    if (toDelete == nullptr) return false;

    parentFolder->remove_child(nodeName);
    recursive_delete(toDelete);
    return true;
}

void FileSystem::move_node(const std::string &src_path, const std::string &dest_path) {
    int j=dest_path.size()-1;
    while (j>-1){
        if (dest_path[j]=='/') break;
        j--;
    }
    std::string destParentPath;
    Node* srcNode = resolve_path(src_path);
    if (j==-1) throw std::invalid_argument("Invalid path.");
    if (j==0) destParentPath="/";
    if (j!=-1) destParentPath=dest_path.substr(0,j);
    std::string newName = dest_path.substr(j+1);
    if (srcNode == nullptr) {
        throw std::runtime_error("No such file or directory.");
    }
    Node* destParentNode = resolve_path(destParentPath);
    while (destParentNode && destParentNode->node_type() == NodeType::Link) {
        destParentNode = ((Link*)destParentNode)->get_target();
    }
    if (destParentNode == nullptr || destParentNode->node_type() != NodeType::Folder) {
        throw std::runtime_error("No such file or directory.");
    }
    if (srcNode->get_parent() == nullptr) {
        throw std::runtime_error("Cannot move root.");
    }

    Folder* oldParent = (Folder*)(srcNode->get_parent());
    Folder* newParent = (Folder*)(destParentNode);

    oldParent->remove_child(srcNode->get_name());
    srcNode->set_name(newName);
    srcNode->set_parent(newParent);
    newParent->add_child(srcNode);
}

void FileSystem::copy_node(const std::string &src_path, const std::string &dest_path) {
    Node* srcNode = resolve_path(src_path);
    int j=dest_path.size();
    while (j>-1){
        if (dest_path[j]=='/') break;
        j--;
    }
    if (j==-1) throw std::invalid_argument("Invalid path.");
    std::string destParentPath;
    if (j==0) destParentPath="/";
    if (j!=0) destParentPath=dest_path.substr(0,j);
    std::string newName=dest_path.substr(j+1);
    Node* destParentNode = resolve_path(destParentPath);
    while (destParentNode && destParentNode->node_type() == NodeType::Link) {
        destParentNode = ((Link*)destParentNode)->get_target();
    }
    if (srcNode == nullptr || destParentNode == nullptr || destParentNode->node_type() != NodeType::Folder) {
        throw std::runtime_error("No such file or directory.");
    }
    
    if (!srcNode->get_parent()) throw std::runtime_error("Cannot copy root.");

    Folder* destFolder = (Folder*)(destParentNode);
    Node* existing = destFolder->find_child(newName);
    if (existing != nullptr && existing->node_type() == srcNode->node_type()) {
        Node* toRemove = destFolder->remove_child(newName);
        recursive_delete(toRemove);
    }

    Node* clonedNode = srcNode->clone(destFolder);
    clonedNode->set_name(newName);
    destFolder->add_child(clonedNode);
}

Node *FileSystem::find_node(const std::string &path) const {
    return resolve_path(path);
}

std::vector<std::string> FileSystem::list_directory(const std::string &path) const {
    Node *targetNode = resolve_path(path);
    while (targetNode && targetNode->node_type() == NodeType::Link) {
        targetNode = ((Link*)targetNode)->get_target();
    }
    if (targetNode != nullptr && targetNode->node_type() == NodeType::Folder) {
        Folder *folder = (Folder*)(targetNode);
        return folder->list_children();
    }
    return std::vector<std::string>();
}

void FileSystem::print_tree(const std::string &path, int indent) const {
    Node* startNode = resolve_path(path);
    print_tree_helper(startNode, indent);
}