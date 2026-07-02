#include "Link.h"

/**
 * @brief Constructor for Link.
 *
 * Initializes a new Link object with the specified name, parent, and target.
 *
 * @param name The name of the link
 * @param parent Pointer to the parent node
 * @param target Pointer to the target node that this link will point to
 */
Link::Link(const std::string &name, Node *parent, Node *target) {}

/**
 * @brief Destructor for Link.
 *
 * Virtual destructor to ensure proper cleanup through base pointer.
 * Note: The link does not own the target node, so it does not delete it.
 */
Link::~Link() {}

/**
 * @brief Gets the type of the node.
 *
 * @return NodeType::Link
 */
NodeType Link::node_type() const {}

/**
 * @brief Creates a clone of the link.
 *
 * Creates a new Link object pointing to the same target as this link.
 *
 * @param parent The parent node for the cloned link (nullptr by default)
 * @return Pointer to the newly created Link clone
 */
Node *Link::clone(Node *parent) const {}

/**
 * @brief Gets the target node that this link points to.
 *
 * @return Pointer to the target node
 */
Node *Link::get_target() const {}
