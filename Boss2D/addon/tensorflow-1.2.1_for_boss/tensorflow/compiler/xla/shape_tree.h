/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_COMPILER_XLA_SHAPE_TREE_H_
#define TENSORFLOW_COMPILER_XLA_SHAPE_TREE_H_

#include <functional>
#include <memory>
#include <vector>

#include "tensorflow/compiler/xla/layout_util.h"
#include "tensorflow/compiler/xla/ptr_util.h"
#include "tensorflow/compiler/xla/shape_util.h"
#include "tensorflow/compiler/xla/status_macros.h"
#include "tensorflow/compiler/xla/xla_data.pb.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/lib/gtl/array_slice.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"

namespace xla {

namespace internal {

// Internal representation of each node in a ShapeTree.
template <typename T>
struct ShapeTreeNode {
  // Data corresponding to this node.
  T data;

  // Children of this node.
  std::vector<std::unique_ptr<ShapeTreeNode>> children;

  explicit ShapeTreeNode(const T& data) : data(data) {}

  ShapeTreeNode(const ShapeTreeNode& other)
      : data(other.data), children(other.children.size()) {
    for (size_t i = 0; i < children.size(); ++i) {
      children[i] = MakeUnique<ShapeTreeNode>(*other.children[i]);
    }
  }

  ShapeTreeNode& operator=(const ShapeTreeNode& other) {
    if (this != &other) {
      data = other.data;
      children.resize(other.children.size());
      for (size_t i = 0; i < children.size(); ++i) {
        children[i] = MakeUnique<ShapeTreeNode>(*other.children[i]);
      }
    }
    return *this;
  }
};

}  // namespace internal

// A ShapeTree<T> is a recursive data structure which mirrors the structure of a
// XLA shape and holds a value of type T for each subshape (i.e. tuple or array)
// in the shape. For array shapes, a ShapeTree trivially holds a single value of
// type T.
//
// For tuple shapes which can be an arbitrary tree with arrays at the leaves, a
// ShapeTree is an identically structured tree with data elements of type T at
// every node. I.e. the root is a tuple by definition, all interior nodes are
// also tuples, and all leaves are arrays.
//
// Like the Shape data structure, this is a tree and tuple elements cannot be
// duplicated. That is, every distinct ShapeIndex in the Shape has a unique T
// object.
template <typename T>
class ShapeTree {
 public:
  // Default constructor creates a tree with a nil shape (i.e. an empty tuple).
  ShapeTree() : ShapeTree(ShapeUtil::MakeNil()) {}
  // Create ShapeTree with the given shape, and default T values for all nodes.
  explicit ShapeTree(const Shape& shape) : ShapeTree(shape, T()) {}
  // Create ShapeTree with the given shape, and init_value for all nodes.
  ShapeTree(const Shape& shape, const T& init_value);

  ShapeTree(const ShapeTree& other) = default;
  ShapeTree& operator=(const ShapeTree& other) = default;

  // Returns the data element associated with the array in the shape at the
  // given index (see ShapeUtil::GetSubshape for how indexes are defined).
  const T& element(const ShapeIndex& index) const;
  T* mutable_element(const ShapeIndex& index);

  // Return the shape represented with this ShapeTree.
  const Shape& shape() const { return shape_; }

  // Returns true if the node at the given index is a leaf node (an array
  // shape).
  bool IsLeaf(const ShapeIndex& index) const {
    return Lookup(index)->children.empty();
  }

  // Recursively traverses the shape and calls the given function at each
  // element. The function has the following arguments:
  //
  //   index : the index of the element in the shape. See ShapeUtil::GetSubshape
  //           for definition of index.
  //   is_leaf : Whether this element is a leaf element in the shape. That is,
  //             whether this index corresponds to an array and not a (nested)
  //             tuple element.
  //   data : The data value at this elemnt.
  //
  // If any call to the given function returns a non-OK status, then traversal
  // is aborted and the status value is returned.
  using VisitorFunction = std::function<Status(
      const ShapeIndex& /*index*/, bool /*is_leaf*/, const T& /*data*/)>;
  Status ForEachElement(const VisitorFunction& func) const;

  using MutableVisitorFunction = std::function<Status(
      const ShapeIndex& /*index*/, bool /*is_leaf*/, T* /*data*/)>;
  Status ForEachMutableElement(const MutableVisitorFunction& func);

 private:
  using Node = internal::ShapeTreeNode<T>;

  // Initialize node->children based on 'shape'. All children are assigned the
  // the given 'init_value'.
  void InitChildren(const Shape& shape, const T& init_value, Node* node);

  // Helpers for traversing the shape via ForEachElement. The helpers
  // recursively traverse the subtree rooted at "index" (defined as in
  // ShapeUtil::GetSubshape).
  static Status ForEachHelper(const VisitorFunction& func, const Node& node,
                              ShapeIndex* index);
  static Status ForEachMutableHelper(const MutableVisitorFunction& func,
                                     Node* node, ShapeIndex* index);

  // Return the tree node at the given index.
  Node* Lookup(const ShapeIndex& index);
  const Node* Lookup(const ShapeIndex& index) const;

  // The root node, which contains all other nodes.
  Node root_;

  // The XLA shape mirrored in this ShapeTree.
  Shape shape_;
};

template <typename T>
void ShapeTree<T>::InitChildren(const Shape& shape, const T& init_value,
                                Node* node) {
  if (ShapeUtil::IsTuple(shape)) {
    for (int i = 0; i < ShapeUtil::TupleElementCount(shape); ++i) {
      node->children.emplace_back(new Node(init_value));
      InitChildren(shape.tuple_shapes(i), init_value,
                   node->children.back().get());
    }
  }
}

template <typename T>
ShapeTree<T>::ShapeTree(const Shape& shape, const T& init_value)
    : root_(init_value), shape_(shape) {
  // The shape_ field is just used to hold the structure of the shape.
  // It should not be relied upon to store layout information.
  LayoutUtil::ClearLayout(&shape_);
  InitChildren(shape_, init_value, &root_);
}

template <typename T>
const T& ShapeTree<T>::element(const ShapeIndex& index) const {
  return Lookup(index)->data;
}

template <typename T>
T* ShapeTree<T>::mutable_element(const ShapeIndex& index) {
  return &Lookup(index)->data;
}

template <typename T>
internal::ShapeTreeNode<T>* ShapeTree<T>::Lookup(const ShapeIndex& index) {
  Node* node = &root_;
  for (const int64 i : index) {
    CHECK_GE(i, 0);
    CHECK_LT(i, node->children.size());
    node = node->children[i].get();
  }
  return node;
}

template <typename T>
const internal::ShapeTreeNode<T>* ShapeTree<T>::Lookup(
    const ShapeIndex& index) const {
  return const_cast<ShapeTree*>(this)->Lookup(index);
}

/* static */
template <typename T>
Status ShapeTree<T>::ForEachHelper(const VisitorFunction& func,
                                   const Node& node, ShapeIndex* index) {
  TF_RETURN_IF_ERROR(func(*index, node.children.empty(), node.data));
  for (int64 i = 0; i < node.children.size(); ++i) {
    index->push_back(i);
    TF_RETURN_IF_ERROR(ForEachHelper(func, *node.children[i], index));
    index->pop_back();
  }
  return Status::OK();
}

/* static */
template <typename T>
Status ShapeTree<T>::ForEachMutableHelper(const MutableVisitorFunction& func,
                                          Node* node, ShapeIndex* index) {
  TF_RETURN_IF_ERROR(func(*index, node->children.empty(), &node->data));
  for (int64 i = 0; i < node->children.size(); ++i) {
    index->push_back(i);
    TF_RETURN_IF_ERROR(
        ForEachMutableHelper(func, node->children[i].get(), index));
    index->pop_back();
  }
  return Status::OK();
}

template <typename T>
Status ShapeTree<T>::ForEachElement(const VisitorFunction& func) const {
  ShapeIndex index;
  return ForEachHelper(func, root_, &index);
}

template <typename T>
Status ShapeTree<T>::ForEachMutableElement(const MutableVisitorFunction& func) {
  ShapeIndex index;
  return ForEachMutableHelper(func, &root_, &index);
}

}  // namespace xla

#endif  // TENSORFLOW_COMPILER_XLA_SHAPE_TREE_H_
