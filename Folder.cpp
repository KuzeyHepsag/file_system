#include "Folder.h"
#include "File.h"
#include "Link.h"
#include <cstddef>
#include <iostream>


Folder::Folder(const std::string &name, Node *parent) : Node(name, parent) {}
 
Folder::~Folder() {
    for (size_t i=0; i<children_.size();i++){
        delete children_[i];
    }
}
 
NodeType Folder::node_type() const { return NodeType::Folder; }
 
Node *Folder::clone(Node *parent) const {
    Folder *new_folder= new Folder(name_, parent);
    for (size_t i=0; i<children_.size();i++){
        new_folder->children_.push_back(children_[i]->clone(new_folder));
    }
    return new_folder;
}
 
Node *Folder::add_file(const std::string &name, const std::string &content) {
    if (name.empty()){
        throw std::invalid_argument("File name cannot be empty.");
    }
    for (size_t i=0;i<children_.size();i++){
        if (children_[i]->get_name() == name && children_[i]->node_type()==NodeType::File){
            throw std::invalid_argument("A file with the same name already exists.");
        }
    }
    Node *new_file =new File(name,this,content);
    children_.push_back(new_file);
    return new_file;
}

Node *Folder::add_folder(const std::string &name) {
    if (name.empty()){
        throw std::invalid_argument("Folder name cannot be empty.");
    }
    for (size_t i=0;i<children_.size();i++){
        if (children_[i]->get_name()==name && children_[i]->node_type()==NodeType::Folder){
            throw std::invalid_argument("A folder with the same name already exists.");
        }
    }
    Node* new_folder=new Folder(name,this);
    children_.push_back(new_folder);
    return new_folder;
}

Node *Folder::add_link(const std::string &name, Node *target) {
    if (name.empty()) {
        throw std::invalid_argument("Link name cannot be empty."); 
    }
    if (target == nullptr) {
        throw std::invalid_argument("Target cannot be nullptr."); 
    }
    for (size_t i=0;i<children_.size();i++){
        if (children_[i]->get_name()==name && children_[i]->node_type()==NodeType::Link){
            throw std::invalid_argument("A link with the same name already exists.");
        }
    }
    Node *new_link=new Link(name,this,target);
    children_.push_back(new_link);
    return new_link;
}

void Folder::add_child(Node *child) {
    if (child) children_.push_back(child);
}
 
Node *Folder::remove_child(const std::string &name) {
    if (name.empty()){
        throw std::invalid_argument("Name cannot be empty.");
    }
    for (size_t i=0;i<children_.size();i++){
        if (children_[i]->get_name()==name){
            Node *removed_node=children_[i];
            for (size_t j = i; j < children_.size() - 1; j++) {
                children_[j] = children_[j + 1];
            }
            children_.pop_back();
            return removed_node;
        }
    }
    throw std::invalid_argument("Node not found.");
}
 
Node *Folder::find_child(const std::string &name) const {
    for (size_t i=0;i<children_.size();i++){
        if (children_[i]->get_name()==name) return children_[i];
    }
    return nullptr;
}

std::vector<std::string> Folder::list_children() const {
    std::vector<std::string> child_names;
    for (size_t i=0;i<children_.size();i++){
        child_names.push_back(children_[i]->get_name());
    }
    return child_names;
} 