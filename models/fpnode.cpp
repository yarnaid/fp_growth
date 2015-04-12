#include "fpnode.h"

FPNode::FPNode(const Item& item, const std::shared_ptr<FPNode>& parent) :
    item( item ), frequency( 1 ), node_link( nullptr ), parent( parent ),
    children(), depth(0), token(item.token) {}
