#include "big_integer.hpp"

BigInt::BigInt() {
  sign_ = Zero;
  body_size_ = 1;
  body_.push_back(0);
}

BigInt::BigInt(std::string string) {
  sign_ = Zero;
  body_size_ = 1;
  body_.push_back(0);
  if (!(string[0] == '0' && string.length() == 1) && string != "-0") {
    if (string[0] == '-') {
      sign_ = Minus;
      string.erase(0, 1);
    } else {
      sign_ = Plus;
    }
    int64_t base = kRadixBase;
    for (char symbol : string) {
      ShortMul(base);
      ShortAdd(symbol - '0');
    }
  }
}

BigInt::BigInt(int64_t integer) {
  int64_t integer_copy = integer;
  if (integer == 0) {
    sign_ = Zero;
    body_size_ = 1;
    body_.push_back(0);
  } else {
    if (integer > 0) {
      sign_ = Plus;
    } else {
      sign_ = Minus;
      integer_copy *= -1;
    }
    uint64_t push_back_value = integer_copy;
    if (integer == INT64_MIN) {
      push_back_value = kMaxValue;
    }
    if (push_back_value < static_cast<int64_t>(kBase)) {
      body_size_ = 1;
      body_.push_back(push_back_value);
    } else {
      body_size_ = 2;
      body_.push_back(push_back_value % kBase);
      body_.push_back(push_back_value / kBase);
    }
  }
}

BigInt::BigInt(size_t body_size, uint64_t filling_value) {
  sign_ = Zero;
  body_size_ = body_size;
  body_.resize(body_size, filling_value);
}

BigInt& BigInt::operator=(const BigInt& other) = default;

BigInt::BigInt(const BigInt& other) {
  sign_ = other.sign_;
  body_size_ = other.body_size_;
  body_ = other.body_;
}

BigInt& BigInt::operator+=(const BigInt& other) {
  BigInt result = *this + other;
  *this = result;
  return *this;
}

BigInt& BigInt::operator-=(const BigInt& other) {
  BigInt result = *this - other;
  *this = result;
  return *this;
}

BigInt& BigInt::operator*=(const BigInt& other) {
  BigInt result = *this * other;
  *this = result;
  return *this;
}

BigInt& BigInt::operator/=(const BigInt& other) {
  BigInt result = *this / other;
  *this = result;
  return *this;
}

BigInt& BigInt::operator%=(const BigInt& other) {
  BigInt result = *this % other;
  *this = result;
  return *this;
}

BigInt& BigInt::operator++() {
  BigInt result = *this + 1;
  *this = result;
  return *this;
}

BigInt BigInt::operator++(int) {
  BigInt temporary = *this;
  *this += 1;
  return temporary;
}

BigInt& BigInt::operator--() {
  BigInt result = *this - 1;
  *this = result;
  return *this;
}

BigInt BigInt::operator--(int) {
  BigInt temporary = *this;
  *this -= 1;
  return temporary;
}

BigInt BigInt::operator-() const {
  BigInt result = *this;
  result.sign_ *= Minus;
  return result;
}

uint64_t BigInt::ShortDivisionMod(uint32_t value) {
  uint64_t carry = 0;
  for (int i = static_cast<int>(body_size_) - 1; i >= 0; i--) {
    uint64_t current = body_[i] + carry * kBase;
    body_[i] = current / value;
    carry = current % value;
  }
  TrimZeroes();
  if (IsZero()) {
    sign_ = BigInt::Zero;
  }
  return carry;
}

std::ostream& operator<<(std::ostream& os, const BigInt& integer) {
  if (integer.IsZero()) {
    os << 0;
    return os;
  }
  BigInt integer_copy = integer;
  std::string answer;
  uint32_t base = BigInt::kRadixBase;
  while (!integer_copy.IsZero()) {
    answer += std::to_string(integer_copy.ShortDivisionMod(base));
  }
  if (integer.sign_ == BigInt::Minus) {
    answer += "-";
  }
  std::reverse(answer.begin(), answer.end());
  os << answer;
  return os;
}

std::istream& operator>>(std::istream& is, BigInt& integer) {
  std::string input;
  is >> input;
  integer = BigInt(input);
  return is;
}

int BigInt::GetSign() const { return sign_; }

bool BigInt::IsZero() const { return (body_size_ == 1 && body_[0] == 0); }

BigInt operator+(const BigInt& left, const BigInt& right) {
  if (left.GetSign() * right.GetSign() == -1) {
    BigInt left_abs = left.GetAbsolute();
    BigInt right_abs = right.GetAbsolute();
    if (left_abs == right_abs) {
      return {};
    }
    if (left < right && left_abs < right_abs) {
      return BigInt::SignedSub(right, left, BigInt::Plus);
    }
    if (left > right && left_abs > right_abs) {
      return BigInt::SignedSub(left, right, BigInt::Plus);
    }
    if (left < right && left_abs > right_abs) {
      return BigInt::SignedSub(left, right, BigInt::Minus);
    }
    if (left > right && left_abs < right_abs) {
      return BigInt::SignedSub(right, left, BigInt::Minus);
    }
  }
  int sign = left.GetSign();
  if (left.GetSign() == 0 || right.GetSign() == 0) {
    sign += right.GetSign();
  }
  return BigInt::SignedAdd(left, right, sign);
}

BigInt operator-(const BigInt& left, const BigInt& right) {
  if (left.GetSign() * right.GetSign() == -1) {
    int sign = BigInt::Plus;
    if (right.GetSign() == BigInt::Plus) {
      sign = BigInt::Minus;
    }
    return BigInt::SignedAdd(left, right, sign);
  }
  if (right.GetSign() == BigInt::Zero) {
    return left;
  }
  if (left.GetSign() == BigInt::Zero) {
    return -right;
  }
  BigInt left_abs = left.GetAbsolute();
  BigInt right_abs = right.GetAbsolute();
  if (left_abs >= right_abs) {
    return BigInt::SignedSub(left, right, left.GetSign());
  }
  return BigInt::SignedSub(right, left, BigInt::Minus * left.GetSign());
}

size_t BigInt::GetMaxEven(size_t size1, size_t size2) {
  return (size1 >= size2) ? (size1 + (size1 % 2)) : (size2 + (size2 % 2));
}

BigInt BigInt::TrimBack(size_t trim_length) const {
  if (body_size_ > trim_length) {
    BigInt new_integer = *this;
    new_integer.body_.resize(new_integer.body_size_ - trim_length);
    new_integer.body_size_ -= trim_length;
    return new_integer;
  }
  return {};
}

BigInt BigInt::TrimFront(size_t trim_length) const {
  if (body_size_ > trim_length) {
    BigInt new_integer = *this;
    new_integer.body_.erase(new_integer.body_.begin(),
                            new_integer.body_.begin() + trim_length);
    new_integer.body_size_ -= trim_length;
    return new_integer;
  }
  return {};
}

void BigInt::Shift(size_t shift_length) {
  for (size_t i = 0; i < shift_length; i++) {
    body_.insert(body_.begin(), 0);
    body_size_++;
  }
}

BigInt BigInt::MultiplyChunks(const BigInt& left, const BigInt& right) {
  BigInt left_copy = left;
  left_copy.TrimZeroes();
  left_copy.sign_ = Plus;
  BigInt right_copy = right;
  right_copy.TrimZeroes();
  right_copy.sign_ = Plus;
  size_t size = BigInt::GetMaxEven(left_copy.body_size_, right_copy.body_size_);
  BigInt left_aligned = left_copy.Align(size - left_copy.body_size_);
  BigInt right_aligned = right_copy.Align(size - right_copy.body_size_);
  BigInt x1 = left_aligned.TrimFront(left_aligned.body_size_ / 2);
  BigInt y1 = left_aligned.TrimBack(left_aligned.body_size_ / 2);
  BigInt x2 = right_aligned.TrimFront(right_aligned.body_size_ / 2);
  BigInt y2 = right_aligned.TrimBack(right_aligned.body_size_ / 2);
  BigInt z0 = RecursiveMultiplication(y1, y2);
  BigInt z2 = RecursiveMultiplication(x1, x2);
  x1 += y1;
  x2 += y2;
  BigInt z1 = RecursiveMultiplication(x1, x2);
  BigInt answer = z1;
  answer -= z0;
  answer -= z2;
  answer.Shift(size / 2);
  answer += z0;
  z2.Shift(size);
  answer += z2;
  answer.sign_ = left.sign_ * right.sign_;
  answer.TrimZeroes();
  return answer;
}

BigInt BigInt::RecursiveMultiplication(const BigInt& left,
                                       const BigInt& right) {
  if (left.body_size_ == 1 && right.body_size_ == 1) {
    BigInt answer = left;
    answer.ShortMul(right.body_[0]);
    answer.sign_ = left.sign_ * right.sign_;
    return answer;
  }
  return MultiplyChunks(left, right);
}

BigInt operator*(const BigInt& left, const BigInt& right) {
  return BigInt::RecursiveMultiplication(left, right);
}

uint64_t BigInt::BinSearchDivision(const BigInt& left, const BigInt& right) {
  if (left.GetAbsolute() < right.GetAbsolute()) {
    return 0;
  }
  uint64_t down = 0;
  uint64_t up = kBase;
  uint64_t middle;
  uint64_t previous_middle = 0;
  while (down < up) {
    middle = (up + down) / 2;
    BigInt right_copy = right;
    right_copy.ShortMul(middle);
    if (left < right_copy) {
      up = middle;
    } else if (left > right_copy) {
      down = middle;
    } else {
      down = up;
    }
    if (middle == previous_middle) {
      break;
    }
    previous_middle = middle;
  }
  return middle;
}

BigInt BigInt::Division(const BigInt& left, const BigInt& right) {
  BigInt left_c = left;
  left_c.sign_ *= left_c.sign_;
  BigInt cur_div;
  size_t length_difference = 0;
  if (left.body_size_ == right.body_size_) {
    cur_div = left_c;
  } else {
    cur_div = left_c.TrimFront(left_c.body_size_ - right.body_size_);
    length_difference = left_c.body_size_ - right.body_size_;
  }
  BigInt right_absolute = right.GetAbsolute();
  if (cur_div < right_absolute) {
    cur_div = left_c.TrimFront(left_c.body_size_ - right.body_size_ - 1);
    length_difference--;
  }
  BigInt result = 1;
  for (size_t i = 0; i < length_difference; i++) {
    result.body_[0] = BinSearchDivision(cur_div, right_absolute);
    BigInt current_subtrahend = right_absolute;
    current_subtrahend.ShortMul(result.body_[0]);
    cur_div -= current_subtrahend;
    cur_div.Shift(1);
    cur_div.ShortAdd(left.body_[length_difference - i - 1]);
    result.Shift(1);
  }
  result.body_[0] = BinSearchDivision(cur_div, right_absolute);
  result.sign_ = left.sign_ * right.sign_;
  return result;
}

BigInt operator/(const BigInt& left, const BigInt& right) {
  if (left.IsZero() || right.IsZero()) {
    return {};
  }
  BigInt left_abs = left.GetAbsolute();
  BigInt right_abs = right.GetAbsolute();
  if (left_abs < right_abs) {
    return {};
  }
  if (left_abs == right_abs) {
    return {left.GetSign() * right.GetSign()};
  }
  return BigInt::Division(left, right);
}

BigInt operator%(const BigInt& left, const BigInt& right) {
  if (right.IsZero() || left.IsZero() || left == right) {
    return {};
  }
  if (left.GetAbsolute() < right.GetAbsolute()) {
    return left.GetAbsolute();
  }
  BigInt result = left / right;
  result *= right;
  result -= left;
  result = -result;
  result.TrimZeroes();
  return result;
}

bool operator<(const BigInt& left, const BigInt& right) {
  return BigInt::GetDifferenceSign(left, right) == BigInt::Minus;
}

bool operator<=(const BigInt& left, const BigInt& right) {
  return BigInt::GetDifferenceSign(left, right) != BigInt::Plus;
}

bool operator>(const BigInt& left, const BigInt& right) {
  return BigInt::GetDifferenceSign(left, right) == BigInt::Plus;
}

bool operator>=(const BigInt& left, const BigInt& right) {
  return BigInt::GetDifferenceSign(left, right) != BigInt::Minus;
}

bool operator==(const BigInt& left, const BigInt& right) {
  return BigInt::GetDifferenceSign(left, right) == BigInt::Zero;
}

bool operator!=(const BigInt& left, const BigInt& right) {
  return BigInt::GetDifferenceSign(left, right) != BigInt::Zero;
}

void BigInt::ShortAdd(uint32_t value) {
  uint64_t carry = value;
  for (size_t i = 0; i < body_size_; i++) {
    uint64_t current = body_[i] + carry;
    body_[i] = current & (kBase - 1);
    carry = current >> kBaseDegree;
  }
  if (carry > 0) {
    body_.push_back(carry);
    body_size_++;
  }
}

void BigInt::ShortMul(uint32_t value) {
  uint64_t carry = 0;
  for (size_t i = 0; i < body_size_; i++) {
    uint64_t current = body_[i] * value + carry;
    body_[i] = current & (kBase - 1);
    carry = current >> kBaseDegree;
  }
  if (carry > 0) {
    body_.push_back(carry);
    body_size_++;
  }
}

int BigInt::GetDifferenceSign(const BigInt& left, const BigInt& right) {
  if (left.sign_ < right.sign_) {
    return Minus;
  }
  if (left.sign_ > right.sign_) {
    return Plus;
  }
  if (left.body_size_ > right.body_size_) {
    return left.sign_;
  }
  if (left.body_size_ < right.body_size_) {
    return Minus * left.sign_;
  }
  int i = 1;
  while (left.body_size_ - i > 0 &&
         left.body_[left.body_size_ - i] == right.body_[right.body_size_ - i]) {
    i++;
  }
  if (left.body_[left.body_size_ - i] > right.body_[right.body_size_ - i]) {
    return left.sign_;
  }
  if (left.body_[left.body_size_ - i] < right.body_[right.body_size_ - i]) {
    return Minus * left.sign_;
  }
  return Zero;
}

BigInt BigInt::GetAbsolute() const { return (sign_ < 0) ? -(*this) : *this; }

BigInt BigInt::SignedAdd(const BigInt& left, const BigInt& right, int sign) {
  size_t result_size =
      (left.body_size_ > right.body_size_) ? left.body_size_ : right.body_size_;
  BigInt result = BigInt(result_size, 0);
  uint64_t carry = 0;
  for (size_t i = 0; i < result_size; i++) {
    uint64_t current = carry;
    if (i < left.body_size_) {
      current += left.body_[i];
    }
    if (i < right.body_size_) {
      current += right.body_[i];
    }
    result.body_[i] = current & (kBase - 1);
    carry = current >> kBaseDegree;
  }
  if (carry > 0) {
    result.body_.push_back(carry);
    result.body_size_++;
  }
  if (result.IsZero()) {
    sign = 0;
  }
  result.sign_ = sign;
  return result;
}

BigInt BigInt::SignedSub(const BigInt& left, const BigInt& right, int sign) {
  size_t result_size = left.body_size_;
  BigInt result = BigInt(result_size, 0);
  uint64_t carry = 0;
  BigInt aligned_right = right.Align(left.body_size_ - right.body_size_);
  for (size_t i = 0; i < result_size; i++) {
    uint64_t previous_carry = carry;
    if (left.body_[i] < (aligned_right.body_[i] + carry)) {
      carry = 1;
    } else {
      carry = 0;
    }
    result.body_[i] = left.body_[i] + carry * kBase - previous_carry;
    result.body_[i] -= aligned_right.body_[i];
  }
  result.TrimZeroes();
  if (result.IsZero()) {
    sign = 0;
  }
  result.sign_ = sign;
  return result;
}

void BigInt::TrimZeroes() {
  while (body_size_ != 1) {
    if (body_.back() == 0) {
      body_.pop_back();
      body_size_--;
    } else {
      break;
    }
  }
}

BigInt BigInt::Align(size_t alignment_length) const {
  BigInt new_integer = *this;
  new_integer.body_.resize(body_size_ + alignment_length, 0);
  new_integer.body_size_ += alignment_length;
  return new_integer;
}
