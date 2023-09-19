#pragma once
#include <iterator>
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 public:
  List() {
    fake_node_ = node_allocator_traits::allocate(allocator_, 1);
    fake_node_->next = nullptr;
    fake_node_->previous = nullptr;
  }

  List(size_t count, const T& value, const Allocator& allocator = Allocator()) {
    allocator_ = node_allocator(allocator);
    fake_node_ = node_allocator_traits::allocate(allocator_, 1);
    fake_node_->next = nullptr;
    fake_node_->previous = nullptr;
    BaseNode* previous = fake_node_;
    for (size_t i = 0; i < count; i++) {
      Node* new_node = node_allocator_traits::allocate(allocator_, 1);
      try {
        node_allocator_traits::construct(allocator_, new_node, previous,
                                         fake_node_, value);
      } catch (...) {
        node_allocator_traits::deallocate(allocator_, new_node, 1);
        this->~List();
        throw;
      }
      previous->next = new_node;
      fake_node_->previous = new_node;
      previous = new_node;
      size_++;
    }
  }

  explicit List(size_t count, const Allocator& allocator = Allocator()) {
    allocator_ = node_allocator(allocator);
    fake_node_ = node_allocator_traits::allocate(allocator_, 1);
    fake_node_->next = nullptr;
    fake_node_->previous = nullptr;
    BaseNode* previous = fake_node_;
    for (size_t i = 0; i < count; i++) {
      Node* new_node = node_allocator_traits::allocate(allocator_, 1);
      try {
        node_allocator_traits::construct(allocator_, new_node, previous,
                                         fake_node_);
      } catch (...) {
        node_allocator_traits::deallocate(allocator_, new_node, 1);
        this->~List();
        throw;
      }
      previous->next = new_node;
      fake_node_->previous = new_node;
      previous = new_node;
      size_++;
    }
  }

  List(const List& other) {
    allocator_ = node_allocator_traits::select_on_container_copy_construction(
        other.allocator_);
    fake_node_ = node_allocator_traits::allocate(allocator_, 1);
    fake_node_->next = nullptr;
    fake_node_->previous = nullptr;
    BaseNode* previous = fake_node_;
    Node* other_current_node = static_cast<Node*>(other.fake_node_->next);
    for (size_t i = 0; i < other.size_; i++) {
      Node* new_node = node_allocator_traits::allocate(allocator_, 1);
      try {
        node_allocator_traits::construct(allocator_, new_node, previous,
                                         fake_node_, other_current_node->value);
      } catch (...) {
        node_allocator_traits::deallocate(allocator_, new_node, 1);
        this->~List();
        throw;
      }

      previous->next = new_node;
      fake_node_->previous = new_node;
      previous = new_node;
      size_++;
      other_current_node = static_cast<Node*>(other_current_node->next);
    }
  }

  List(std::initializer_list<T> list,
       const Allocator& allocator = Allocator()) {
    allocator_ = node_allocator(allocator);
    fake_node_ = node_allocator_traits::allocate(allocator_, 1);
    fake_node_->next = nullptr;
    fake_node_->previous = nullptr;
    BaseNode* previous = fake_node_;
    for (const auto& element : list) {
      Node* new_node = node_allocator_traits::allocate(allocator_, 1);
      try {
        node_allocator_traits::construct(allocator_, new_node, previous,
                                         fake_node_, element);
      } catch (...) {
        node_allocator_traits::deallocate(allocator_, new_node, 1);
        this->~List();
        throw;
      }
      previous->next = new_node;
      fake_node_->previous = new_node;
      previous = new_node;
      size_++;
    }
  }

  ~List() { delete_elements(); }

  void swap(List& other) {
    BaseNode* temp_fake_node = fake_node_;
    node_allocator temp_allocator = allocator_;
    size_t temp_size = size_;
    fake_node_ = other.fake_node_;
    allocator_ = other.allocator_;
    size_ = other.size_;
    other.fake_node_ = temp_fake_node;
    other.allocator_ = temp_allocator;
    other.size_ = temp_size;
  }

  List& operator=(const List& other) {
    node_allocator allocator_copy = allocator_;
    try {
      auto temporary = other;
      swap(temporary);
      if (node_allocator_traits::propagate_on_container_copy_assignment::
              value) {
        allocator_ = other.allocator_;
      } else {
        allocator_ = allocator_copy;
      }
    } catch (...) {
      throw;
    }
    return *this;
  }

  bool empty() const { return size_ == 0; }

  size_t size() const { return size_; }

  T& front() { return (static_cast<Node*>(fake_node_->next))->value; }

  const T& front() const {
    return (static_cast<Node*>(fake_node_->next))->value;
  }

  T& back() { return (static_cast<Node*>(fake_node_->previous))->value; }

  const T& back() const {
    return (static_cast<Node*>(fake_node_->previous))->value;
  }

  void push_back(const T& value) {
    Node* new_node = node_allocator_traits::allocate(allocator_, 1);
    if (size_ == 0) {
      try {
        node_allocator_traits::construct(allocator_, new_node, fake_node_,
                                         fake_node_, value);
      } catch (...) {
        node_allocator_traits::deallocate(allocator_, new_node, 1);
        throw;
      }
      fake_node_->next = new_node;
      fake_node_->previous = new_node;
    } else {
      BaseNode* old_back_node = fake_node_->previous;
      try {
        node_allocator_traits::construct(allocator_, new_node, old_back_node,
                                         fake_node_, value);
      } catch (...) {
        node_allocator_traits::deallocate(allocator_, new_node, 1);
        throw;
      }
      fake_node_->previous = new_node;
      old_back_node->next = new_node;
    }
    size_++;
  }

  void push_back(T&& value) {
    Node* new_node = node_allocator_traits::allocate(allocator_, 1);
    if (size_ == 0) {
      try {
        node_allocator_traits::construct(allocator_, new_node, fake_node_,
                                         fake_node_, std::move(value));
      } catch (...) {
        node_allocator_traits::deallocate(allocator_, new_node, 1);
        throw;
      }
      fake_node_->next = new_node;
      fake_node_->previous = new_node;
    } else {
      BaseNode* old_back_node = fake_node_->previous;
      try {
        node_allocator_traits::construct(allocator_, new_node, old_back_node,
                                         fake_node_, std::move(value));
      } catch (...) {
        node_allocator_traits::deallocate(allocator_, new_node, 1);
        throw;
      }
      fake_node_->previous = new_node;
      old_back_node->next = new_node;
    }
    size_++;
  }

  void push_front(const T& value) {
    Node* new_node = node_allocator_traits::allocate(allocator_, 1);
    if (size_ == 0) {
      try {
        node_allocator_traits::construct(allocator_, new_node, fake_node_,
                                         fake_node_, value);
      } catch (...) {
        node_allocator_traits::deallocate(allocator_, new_node, 1);
        throw;
      }
      fake_node_->next = new_node;
      fake_node_->previous = new_node;
    } else {
      BaseNode* old_front_node = fake_node_->next;
      try {
        node_allocator_traits::construct(allocator_, new_node, fake_node_,
                                         old_front_node, value);
      } catch (...) {
        node_allocator_traits::deallocate(allocator_, new_node, 1);
        throw;
      }
      fake_node_->next = new_node;
      old_front_node->previous = new_node;
    }
    size_++;
  }

  void push_front(T&& value) {
    try {
      T copy = std::move(value);
    } catch (...) {
      this->~List();
      throw;
    }
    Node* new_node = node_allocator_traits::allocate(allocator_, 1);
    if (size_ == 0) {
      node_allocator_traits::construct(allocator_, new_node, fake_node_,
                                       fake_node_, std::move(value));
      fake_node_->next = new_node;
      fake_node_->previous = new_node;
    } else {
      BaseNode* old_front_node = fake_node_->next;
      node_allocator_traits::construct(allocator_, new_node, fake_node_,
                                       old_front_node, std::move(value));
      fake_node_->next = new_node;
      old_front_node->previous = new_node;
    }
    size_++;
  }

  void pop_back() {
    BaseNode* back_node = fake_node_->previous;
    BaseNode* node_before_back = back_node->previous;
    node_allocator_traits::destroy(allocator_, static_cast<Node*>(back_node));
    node_allocator_traits::deallocate(allocator_, static_cast<Node*>(back_node),
                                      1);
    node_before_back->next = fake_node_;
    fake_node_->previous = node_before_back;
    size_--;
  }

  void pop_front() {
    BaseNode* front_node = fake_node_->next;
    BaseNode* node_after_front = front_node->next;
    node_allocator_traits::destroy(allocator_, static_cast<Node*>(front_node));
    node_allocator_traits::deallocate(allocator_,
                                      static_cast<Node*>(front_node), 1);
    node_after_front->previous = fake_node_;
    fake_node_->next = node_after_front;
    size_--;
  }

 private:
  struct BaseNode {
    BaseNode* previous;
    BaseNode* next;

    BaseNode(BaseNode* previous, BaseNode* next)
        : previous(previous), next(next) {}
  };

  struct Node : BaseNode {
    T value;

    Node(BaseNode* previous, BaseNode* next) : BaseNode(previous, next) {}

    Node(BaseNode* previous, BaseNode* next, const T& value)
        : BaseNode(previous, next), value(value) {}
  };

  using allocator_traits = std::allocator_traits<Allocator>;
  using node_allocator = typename allocator_traits::template rebind_alloc<Node>;
  using node_allocator_traits =
      typename allocator_traits::template rebind_traits<Node>;

 public:
  template <bool IsConst>
  class CommonIterator {
   public:
    using type = std::conditional_t<IsConst, const T, T>;
    using value_type = type;
    using pointer = type*;
    using reference = type&;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = int64_t;

    CommonIterator() = delete;

    CommonIterator(const CommonIterator& other) = default;

    CommonIterator& operator=(const CommonIterator& other) = default;

    CommonIterator(BaseNode* node_pointer) : node_pointer_(node_pointer) {}

    type& operator*() { return (static_cast<Node*>(node_pointer_))->value; }

    type* operator->() { return &((static_cast<Node*>(node_pointer_))->value); }

    type& operator*() const {
      return (static_cast<Node*>(node_pointer_))->value;
    }

    type* operator->() const {
      return &((static_cast<Node*>(node_pointer_))->value);
    }

    CommonIterator& operator++() {
      node_pointer_ = node_pointer_->next;
      return *this;
    }

    CommonIterator operator++(int) {
      CommonIterator temporary = *this;
      ++(*this);
      return temporary;
    }

    CommonIterator& operator--() {
      node_pointer_ = node_pointer_->previous;
      return *this;
    }

    CommonIterator operator--(int) {
      CommonIterator temporary = *this;
      --(*this);
      return temporary;
    }

    bool operator==(const CommonIterator& other) const {
      return node_pointer_ == other.node_pointer_;
    }

    bool operator!=(const CommonIterator& other) const {
      return node_pointer_ != other.node_pointer_;
    }

   private:
    BaseNode* node_pointer_;
  };

  using value_type = T;
  using allocator_type = Allocator;
  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() const { return CommonIterator<false>(fake_node_->next); }

  iterator end() const { return CommonIterator<false>(fake_node_); }

  iterator begin() { return CommonIterator<false>(fake_node_->next); }

  iterator end() { return CommonIterator<false>(fake_node_); }

  const_iterator cbegin() const {
    return CommonIterator<true>(fake_node_->next);
  }

  const_iterator cend() const { return CommonIterator<true>(fake_node_); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  reverse_iterator rend() { return reverse_iterator(begin()); }

  reverse_iterator rbegin() const { return reverse_iterator(end()); }

  reverse_iterator rend() const { return reverse_iterator(begin()); }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin());
  }

  node_allocator get_allocator() { return allocator_; }

 private:
  size_t size_ = 0;
  BaseNode* fake_node_;
  node_allocator allocator_;

  void delete_elements() {
    if (fake_node_->next != nullptr) {
      Node* current = static_cast<Node*>(fake_node_->next);
      while (true) {
        BaseNode* next = current->next;
        node_allocator_traits::destroy(allocator_, current);
        node_allocator_traits::deallocate(allocator_, current, 1);
        if (next != fake_node_) {
          current = static_cast<Node*>(next);
        } else {
          break;
        }
      }
    }
    node_allocator_traits::deallocate(allocator_,
                                      static_cast<Node*>(fake_node_), 1);
    size_ = 0;
  }
};
