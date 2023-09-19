#include <algorithm>
#include <vector>

template <typename T, typename Allocator = std::allocator<T>>
class Deque {
 public:
  Deque() {
    elements_amount_ = 0;
    front_element_.bucket = 1;
    front_element_.index = 0;
    back_element_.bucket = 1;
    back_element_.index = 0;
    const size_t kInitialBucketAmount = 3;
    external_.resize(kInitialBucketAmount, nullptr);
  }

  Deque(const Allocator& allocator) {
    allocator_ = allocator;
    elements_amount_ = 0;
    front_element_.bucket = 1;
    front_element_.index = 0;
    back_element_.bucket = 1;
    back_element_.index = 0;
    const size_t kInitialBucketAmount = 3;
    external_.resize(kInitialBucketAmount, nullptr);
  }

  Deque(const Deque& other) {
    allocator_ = allocator_traits::select_on_container_copy_construction(
        other.allocator_);
    try {
      external_.resize(other.external_.size(), nullptr);
      elements_amount_ = 0;
      front_element_ = {1, 0};
      back_element_ = {1, 0};
      for (auto iter = other.cbegin(); iter != other.cend(); iter++) {
        (*this).push_back(*iter);
      }
    } catch (...) {
      destroy_elements();
      deallocate_buckets();
      throw;
    }
  }

  Deque(Deque&& other) {
    allocator_ = std::move(other.allocator_);
    external_ = std::move(other.external_);
    elements_amount_ = other.elements_amount_;
    front_element_ = other.front_element_;
    back_element_ = other.back_element_;
    other.elements_amount_ = 0;
  }

  Deque(size_t count, const Allocator& allocator = Allocator()) {
    try {
      allocator_ = allocator;
      prepare_for_count_elements(count + 1);
      for (size_t i = 0; i < count; i++) {
        (*this).emplace_back();
      }
    } catch (...) {
      destroy_elements();
      deallocate_buckets();
      throw;
    }
  }

  Deque(size_t count, const T& value,
        const Allocator& allocator = Allocator()) {
    try {
      T value_for = value;
      allocator_ = allocator;
      prepare_for_count_elements(count + 1);
      for (size_t i = 0; i < count; i++) {
        (*this).push_back(value);
      }
    } catch (...) {
      destroy_elements();
      deallocate_buckets();
      throw;
    }
  }

  Deque(std::initializer_list<T> list,
        const Allocator& allocator = Allocator()) {
    allocator_ = allocator;
    prepare_for_count_elements(list.size() + 1);
    for (const auto& element : list) {
      (*this).push_back(element);
    }
  }

  ~Deque() {
    destroy_elements();
    deallocate_buckets();
  }

  void swap(Deque& other) {
    std::swap(external_, other.external_);
    std::swap(allocator_, other.allocator_);
    std::swap(elements_amount_, other.elements_amount_);
    std::swap(front_element_, other.front_element_);
    std::swap(back_element_, other.back_element_);
  }

  Deque& operator=(const Deque& other) {
    try {
      auto temporary = other;
      swap(temporary);
      if (allocator_traits::propagate_on_container_copy_assignment::value) {
        allocator_ = other.allocator_;
      }
    } catch (...) {
      throw;
    }
    return *this;
  }

  Deque& operator=(Deque&& other) {
    try {
      auto temporary = std::move(other);
      swap(temporary);
      if (allocator_traits::propagate_on_container_move_assignment::value) {
        allocator_ = std::move(other.allocator_);
      }
    } catch (...) {
      throw;
    }
    return *this;
  }

  size_t size() const { return elements_amount_; }

  bool empty() const { return elements_amount_ == 0; }

  const T& operator[](size_t index) const {
    Position element_position = get_element_position(index);
    return external_[element_position.bucket][element_position.index];
  }

  T& operator[](size_t index) {
    Position element_position = get_element_position(index);
    return external_[element_position.bucket][element_position.index];
  }

  const T& at(size_t index) const {
    if (index >= elements_amount_) {
      throw std::out_of_range("Index out of range");
    }
    return (*this)[index];
  }

  T& at(size_t index) {
    if (index >= elements_amount_) {
      throw std::out_of_range("Index out of range");
    }
    return (*this)[index];
  }

  void push_back(const T& value) {
    if (elements_amount_ != 0) {
      if ((back_element_.bucket == external_.size() - 1) &&
          (back_element_.index == kBucketSize - 1)) {
        external_.resize(external_.size() +
                             (back_element_.bucket - front_element_.bucket + 1),
                         nullptr);
        back_element_.index = 0;
        back_element_.bucket++;
      } else if (back_element_.index == kBucketSize - 1) {
        back_element_.index = 0;
        back_element_.bucket++;
      } else {
        back_element_.index++;
      }
    }
    try {
      elements_amount_++;
      if (external_[back_element_.bucket] == nullptr) {
        external_[back_element_.bucket] =
            allocator_traits::allocate(allocator_, kBucketSize);
      }
      allocator_traits::construct(
          allocator_, &external_[back_element_.bucket][back_element_.index],
          value);
    } catch (...) {
      elements_amount_--;
      if (elements_amount_ != 0) {
        back_element_ = get_before_position(back_element_);
      }
      throw;
    }
  }

  void push_back(T&& value) {
    if (elements_amount_ != 0) {
      if ((back_element_.bucket == external_.size() - 1) &&
          (back_element_.index == kBucketSize - 1)) {
        external_.resize(external_.size() +
                             (back_element_.bucket - front_element_.bucket + 1),
                         nullptr);
        back_element_.index = 0;
        back_element_.bucket++;
      } else if (back_element_.index == kBucketSize - 1) {
        back_element_.index = 0;
        back_element_.bucket++;
      } else {
        back_element_.index++;
      }
    }
    try {
      elements_amount_++;
      if (external_[back_element_.bucket] == nullptr) {
        external_[back_element_.bucket] =
            allocator_traits::allocate(allocator_, kBucketSize);
      }
      allocator_traits::construct(
          allocator_, &external_[back_element_.bucket][back_element_.index],
          std::move(value));
    } catch (...) {
      elements_amount_--;
      if (elements_amount_ != 0) {
        back_element_ = get_before_position(back_element_);
      }
      throw;
    }
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    if (elements_amount_ != 0) {
      if ((back_element_.bucket == external_.size() - 1) &&
          (back_element_.index == kBucketSize - 1)) {
        external_.resize(external_.size() +
                             (back_element_.bucket - front_element_.bucket + 1),
                         nullptr);
        back_element_.index = 0;
        back_element_.bucket++;
      } else if (back_element_.index == kBucketSize - 1) {
        back_element_.index = 0;
        back_element_.bucket++;
      } else {
        back_element_.index++;
      }
    }
    try {
      elements_amount_++;
      if (external_[back_element_.bucket] == nullptr) {
        external_[back_element_.bucket] =
            allocator_traits::allocate(allocator_, kBucketSize);
      }
      allocator_traits::construct(
          allocator_, &external_[back_element_.bucket][back_element_.index],
          std::forward<Args>(args)...);
    } catch (...) {
      elements_amount_--;
      if (elements_amount_ != 0) {
        back_element_ = get_before_position(back_element_);
      }
      throw;
    }
  }

  void pop_back() {
    allocator_traits::destroy(
        allocator_, &external_[back_element_.bucket][back_element_.index]);
    if (elements_amount_ != 1) {
      if (back_element_.index == 0) {
        back_element_.index = kBucketSize - 1;
        back_element_.bucket--;
      } else {
        back_element_.index--;
      }
    }
    elements_amount_--;
  }

  void push_front(const T& value) {
    if (elements_amount_ != 0) {
      if ((front_element_.bucket == 0) && (front_element_.index == 0)) {
        std::reverse(external_.begin(), external_.end());
        size_t additional_buckets = back_element_.bucket + 1;
        external_.resize(external_.size() + additional_buckets, nullptr);
        std::reverse(external_.begin(), external_.end());
        back_element_.bucket += additional_buckets;
        front_element_.bucket += additional_buckets;
        front_element_.index = kBucketSize - 1;
        front_element_.bucket--;
      } else if (front_element_.index == 0) {
        front_element_.index = kBucketSize - 1;
        front_element_.bucket--;
      } else {
        front_element_.index--;
      }
    }
    if (external_[front_element_.bucket] == nullptr) {
      external_[front_element_.bucket] =
          allocator_traits::allocate(allocator_, kBucketSize);
    }
    try {
      elements_amount_++;
      allocator_traits::construct(
          allocator_, &external_[front_element_.bucket][front_element_.index],
          value);
    } catch (...) {
      elements_amount_--;
      if (elements_amount_ != 0) {
        front_element_ = get_next_position(front_element_);
      }
      throw;
    }
  }

  void push_front(T&& value) {
    if (elements_amount_ != 0) {
      if ((front_element_.bucket == 0) && (front_element_.index == 0)) {
        std::reverse(external_.begin(), external_.end());
        size_t additional_buckets = back_element_.bucket + 1;
        external_.resize(external_.size() + additional_buckets, nullptr);
        std::reverse(external_.begin(), external_.end());
        back_element_.bucket += additional_buckets;
        front_element_.bucket += additional_buckets;
        front_element_.index = kBucketSize - 1;
        front_element_.bucket--;
      } else if (front_element_.index == 0) {
        front_element_.index = kBucketSize - 1;
        front_element_.bucket--;
      } else {
        front_element_.index--;
      }
    }
    if (external_[front_element_.bucket] == nullptr) {
      external_[front_element_.bucket] =
          allocator_traits::allocate(allocator_, kBucketSize);
    }
    try {
      elements_amount_++;
      allocator_traits::construct(
          allocator_, &external_[front_element_.bucket][front_element_.index],
          std::move(value));
    } catch (...) {
      elements_amount_--;
      if (elements_amount_ != 0) {
        front_element_ = get_next_position(front_element_);
      }
      throw;
    }
  }

  template <typename... Args>
  void emplace_front(Args&&... args) {
    if (elements_amount_ != 0) {
      if ((front_element_.bucket == 0) && (front_element_.index == 0)) {
        std::reverse(external_.begin(), external_.end());
        size_t additional_buckets = back_element_.bucket + 1;
        external_.resize(external_.size() + additional_buckets, nullptr);
        std::reverse(external_.begin(), external_.end());
        back_element_.bucket += additional_buckets;
        front_element_.bucket += additional_buckets;
        front_element_.index = kBucketSize - 1;
        front_element_.bucket--;
      } else if (front_element_.index == 0) {
        front_element_.index = kBucketSize - 1;
        front_element_.bucket--;
      } else {
        front_element_.index--;
      }
    }
    if (external_[front_element_.bucket] == nullptr) {
      external_[front_element_.bucket] =
          allocator_traits::allocate(allocator_, kBucketSize);
    }
    try {
      elements_amount_++;
      allocator_traits::construct(
          allocator_, &external_[front_element_.bucket][front_element_.index],
          std::forward<Args>(args)...);
    } catch (...) {
      elements_amount_--;
      if (elements_amount_ != 0) {
        front_element_ = get_next_position(front_element_);
      }
      throw;
    }
  }

  void pop_front() {
    allocator_traits::destroy(
        allocator_, &external_[front_element_.bucket][front_element_.index]);
    if (elements_amount_ != 1) {
      if (front_element_.index == kBucketSize - 1) {
        front_element_.index = 0;
        front_element_.bucket++;
      } else {
        front_element_.index++;
      }
    }
    elements_amount_--;
  }

  template <bool IsConst>
  class CommonIterator {
   public:
    using type = std::conditional_t<IsConst, const T, T>;
    using value_type = type;
    using pointer = type*;
    using reference = type&;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = int64_t;

    CommonIterator() = delete;

    CommonIterator(const CommonIterator& other) = default;

    CommonIterator& operator=(const CommonIterator& other) = default;

    CommonIterator(type** bucket_pointer, size_t element_index)
        : bucket_pointer_(bucket_pointer), element_index_(element_index) {}

    type& operator*() { return (*bucket_pointer_)[element_index_]; }

    type* operator->() { return &((*bucket_pointer_)[element_index_]); }

    type& operator*() const { return (*bucket_pointer_)[element_index_]; }

    type* operator->() const { return &((*bucket_pointer_)[element_index_]); }

    CommonIterator& operator++() {
      if (element_index_ == kBucketSize - 1) {
        bucket_pointer_++;
        element_index_ = 0;
      } else {
        element_index_++;
      }
      return *this;
    }

    CommonIterator operator++(int) {
      CommonIterator temporary = *this;
      ++(*this);
      return temporary;
    }

    CommonIterator& operator--() {
      if (element_index_ == 0) {
        bucket_pointer_--;
        element_index_ = kBucketSize - 1;
      } else {
        element_index_--;
      }
      return *this;
    }

    CommonIterator operator--(int) {
      CommonIterator temporary = *this;
      --(*this);
      return temporary;
    }

    CommonIterator operator+(int shift) const {
      if (shift >= 0) {
        CommonIterator temporary = *this;
        size_t bucket_shift = shift / kBucketSize;
        shift -= bucket_shift * kBucketSize;
        if (shift + element_index_ >= kBucketSize) {
          bucket_shift++;
        }
        temporary.element_index_ = (shift + element_index_) % kBucketSize;
        temporary.bucket_pointer_ += bucket_shift;
        return temporary;
      }
      return *this - (-shift);
    }

    CommonIterator& operator+=(int shift) {
      CommonIterator temporary = *this + shift;
      *this = temporary;
      return *this;
    }

    CommonIterator& operator-=(int shift) {
      CommonIterator temporary = *this - shift;
      *this = temporary;
      return *this;
    }

    CommonIterator operator-(int shift) const {
      if (shift >= 0) {
        CommonIterator temporary = *this;
        size_t bucket_shift = shift / kBucketSize;
        shift -= bucket_shift * kBucketSize;
        if (static_cast<int>(element_index_) - shift < 0) {
          bucket_shift++;
        }
        temporary.element_index_ =
            (element_index_ - shift + kBucketSize) % kBucketSize;
        temporary.bucket_pointer_ -= bucket_shift;
        return temporary;
      }
      return *this + (-shift);
    }

    std::ptrdiff_t operator-(const CommonIterator& other) const {
      if (*this <= other) {
        if ((*this).bucket_pointer_ == other.bucket_pointer_) {
          return -(other.element_index_ - (*this).element_index_);
        }
        return -((other.bucket_pointer_ - (*this).bucket_pointer_ - 1) *
                     kBucketSize +
                 other.element_index_ + kBucketSize - (*this).element_index_);
      }
      if ((*this).bucket_pointer_ == other.bucket_pointer_) {
        return -other.element_index_ + (*this).element_index_;
      }
      return ((*this).bucket_pointer_ - other.bucket_pointer_ - 1) *
                 kBucketSize -
             other.element_index_ + kBucketSize + (*this).element_index_;
    }

    bool operator==(const CommonIterator& other) const {
      return (*this).bucket_pointer_ == other.bucket_pointer_ &&
             (*this).element_index_ == other.element_index_;
    }

    bool operator!=(const CommonIterator& other) const {
      return (*this).bucket_pointer_ != other.bucket_pointer_ ||
             (*this).element_index_ != other.element_index_;
    }

    bool operator<(const CommonIterator& other) const {
      return (*this).bucket_pointer_ < other.bucket_pointer_ ||
             ((*this).bucket_pointer_ == other.bucket_pointer_ &&
              (*this).element_index_ <= other.element_index_);
    }

    bool operator<=(const CommonIterator& other) const {
      return (*this == other) || (*this < other);
    }

    bool operator>(const CommonIterator& other) const {
      return other.bucket_pointer_ < (*this).bucket_pointer_ ||
             (other.bucket_pointer_ == (*this).bucket_pointer_ &&
              other.element_index_ <= (*this).element_index_);
    }

    bool operator>=(const CommonIterator& other) const {
      return (*this == other) || (*this > other);
    }

    size_t get_index() { return element_index_; }

    type** get_bucket_pointer() { return bucket_pointer_; }

   private:
    type** bucket_pointer_;
    size_t element_index_;
  };

  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() const {
    return CommonIterator<false>(&external_[front_element_.bucket],
                                 front_element_.index);
  }

  iterator end() const {
    if (elements_amount_ == 0) {
      return begin();
    }
    Position end_position = get_next_position(back_element_);
    return CommonIterator<false>(&external_[end_position.bucket],
                                 end_position.index);
  }

  iterator begin() {
    return CommonIterator<false>(&external_[front_element_.bucket],
                                 front_element_.index);
  }

  iterator end() {
    if (elements_amount_ == 0) {
      return begin();
    }
    Position end_position = get_next_position(back_element_);
    return CommonIterator<false>(&external_[end_position.bucket],
                                 end_position.index);
  }

  const_iterator cbegin() const {
    return CommonIterator<true>((const T**)(&external_[front_element_.bucket]),
                                front_element_.index);
  }

  const_iterator cend() const {
    if (elements_amount_ == 0) {
      return cbegin();
    }
    Position end_position = get_next_position(back_element_);
    return CommonIterator<true>((const T**)(&external_[end_position.bucket]),
                                end_position.index);
  }

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

  // NOLINTNEXTLINE
  void insert(iterator it, const T& value) {
    Position inserting_position = {it.get_bucket_pointer() -
                                       begin().get_bucket_pointer() +
                                       front_element_.bucket,
                                   it.get_index()};
    push_back(value);
    if (elements_amount_ == 1) {
      return;
    }
    Position position_to_copy = back_element_;
    if (external_[position_to_copy.bucket] == nullptr) {
      external_[position_to_copy.bucket] =
          allocator_traits::allocate(allocator_, kBucketSize);
    }
    while (position_to_copy != inserting_position) {
      Position previous_position = get_before_position(position_to_copy);
      try {
        external_[position_to_copy.bucket][position_to_copy.index] =
            external_[previous_position.bucket][previous_position.index];
      } catch (...) {
        throw;
      }
      position_to_copy = previous_position;
    }
    external_[inserting_position.bucket][inserting_position.index] = value;
  }

  // NOLINTNEXTLINE
  void erase(iterator it) {
    Position element_position = {it.get_bucket_pointer() -
                                     begin().get_bucket_pointer() +
                                     front_element_.bucket,
                                 it.get_index()};
    if (element_position != back_element_) {
      Position next_element;
      do {
        next_element = get_next_position(element_position);
        if (external_[next_element.bucket] == nullptr) {
          external_[next_element.bucket] =
              allocator_traits::allocate(allocator_, kBucketSize);
        }
        try {
          external_[element_position.bucket][element_position.index] =
              external_[next_element.bucket][next_element.index];
        } catch (...) {
          throw;
        }
        element_position = next_element;
      } while (next_element != back_element_);
    }
    pop_back();
  }

  using allocator_type = Allocator;
  using allocator_traits = std::allocator_traits<Allocator>;

  allocator_type get_allocator() { return allocator_; }

 private:
  struct Position {
    size_t bucket;
    size_t index;

    Position() {
      bucket = 0;
      index = 0;
    }

    Position(size_t bucket, size_t index) : bucket(bucket), index(index) {}

    bool operator==(const Position& other) const {
      return bucket == other.bucket && index == other.index;
    }

    bool operator!=(const Position& other) const {
      return bucket != other.bucket || index != other.index;
    }

    Position(const Position& other) {
      bucket = other.bucket;
      index = other.index;
    }

    Position& operator=(const Position& other) {
      bucket = other.bucket;
      index = other.index;
      return *this;
    }
  };

  allocator_type allocator_;

  static const size_t kBucketSize = 64;
  static const size_t kBucketBlocksAmount = 3;
  std::vector<T*> external_;

  size_t elements_amount_;
  Position front_element_;
  Position back_element_;

  Position get_next_position(const Position& element_position) const {
    if (element_position.index == kBucketSize - 1) {
      return {element_position.bucket + 1, 0};
    }
    return {element_position.bucket, element_position.index + 1};
  }

  Position get_before_position(const Position& element_position) const {
    if (element_position.index == 0) {
      return {element_position.bucket - 1, kBucketSize - 1};
    }
    return {element_position.bucket, element_position.index - 1};
  }

  Position get_element_position(size_t index) const {
    size_t shifted_element_bucket = front_element_.bucket + index / kBucketSize;
    size_t shifted_element_index =
        ((index + 1) % kBucketSize - 1 + kBucketSize) % kBucketSize;
    if (shifted_element_index + front_element_.index >= kBucketSize) {
      shifted_element_bucket++;
    }
    shifted_element_index =
        (shifted_element_index + front_element_.index) % kBucketSize;
    return {shifted_element_bucket, shifted_element_index};
  }

  void destroy_elements() {
    size_t initial_size = elements_amount_;
    for (size_t i = 0; i < initial_size; i++) {
      (*this).pop_back();
    }
  }

  void deallocate_buckets() {
    for (size_t i = 0; i < external_.size(); i++) {
      if (external_[i] != nullptr) {
        allocator_traits::deallocate(allocator_, external_[i], kBucketSize);
        external_[i] = nullptr;
      }
    }
  }

  void prepare_for_count_elements(size_t count) {
    elements_amount_ = 0;
    size_t required_buckets_amount = count / kBucketSize;
    required_buckets_amount += count % kBucketSize == 0 ? 0 : 1;
    external_.resize(required_buckets_amount * kBucketBlocksAmount, nullptr);
    front_element_ = {required_buckets_amount, 0};
    back_element_ = {required_buckets_amount, 0};
  }
};
