#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class BigInt {
 public:
  BigInt();

  BigInt(std::string string);

  BigInt(int64_t integer);

  BigInt(size_t body_size, uint64_t filling_value);

  BigInt& operator=(const BigInt& other);

  BigInt(const BigInt& other);

  BigInt& operator+=(const BigInt& other);

  BigInt& operator-=(const BigInt& other);

  BigInt& operator*=(const BigInt& other);

  BigInt& operator/=(const BigInt& other);

  BigInt& operator%=(const BigInt& other);

  BigInt& operator++();

  BigInt operator++(int);

  BigInt& operator--();

  BigInt operator--(int);

  BigInt operator-() const;

  friend std::ostream& operator<<(std::ostream& os, const BigInt& integer);

  friend std::istream& operator>>(std::istream& is, BigInt& integer);

  static int GetDifferenceSign(const BigInt& left, const BigInt& right);

  int GetSign() const;

  bool IsZero() const;

  BigInt GetAbsolute() const;

  static BigInt SignedAdd(const BigInt& left, const BigInt& right, int sign);

  static BigInt SignedSub(const BigInt& left, const BigInt& right, int sign);

  uint64_t ShortDivisionMod(uint32_t value);

  static BigInt RecursiveMultiplication(const BigInt& left,
                                        const BigInt& right);

  static BigInt MultiplyChunks(const BigInt& left, const BigInt& right);

  static BigInt Division(const BigInt& left, const BigInt& right);

  static uint64_t BinSearchDivision(const BigInt& left, const BigInt& right);

  static size_t GetMaxEven(size_t size1, size_t size2);

  void TrimZeroes();

  enum SignValues { Minus = -1, Zero = 0, Plus = 1 };

  static const uint64_t kBase = 4294967296;
  static const uint64_t kBaseDegree = 32;
  static const uint64_t kRadixBase = 10;
  static const uint64_t kMaxValue = 9223372036854775808U;

 private:
  std::vector<uint64_t> body_;
  size_t body_size_;
  int sign_;

  void ShortAdd(uint32_t value);

  void ShortMul(uint32_t value);

  BigInt Align(size_t alignment_length) const;

  BigInt TrimFront(size_t trim_length) const;

  BigInt TrimBack(size_t trim_length) const;

  void Shift(size_t shift_length);
};

BigInt operator+(const BigInt& left, const BigInt& right);

BigInt operator-(const BigInt& left, const BigInt& right);

BigInt operator*(const BigInt& left, const BigInt& right);

BigInt operator/(const BigInt& left, const BigInt& right);

BigInt operator%(const BigInt& left, const BigInt& right);

bool operator<(const BigInt& left, const BigInt& right);

bool operator<=(const BigInt& left, const BigInt& right);

bool operator>(const BigInt& left, const BigInt& right);

bool operator>=(const BigInt& left, const BigInt& right);

bool operator==(const BigInt& left, const BigInt& right);

bool operator!=(const BigInt& left, const BigInt& right);
