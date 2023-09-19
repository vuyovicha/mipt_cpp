#pragma once

#include <cstdint>
#include <cstdlib>

class Vector {
 public:
  Vector();

  Vector(int64_t x_coordinate, int64_t y_coordinate);

  Vector& operator=(const Vector& other);

  Vector(const Vector& other);

  int64_t operator*(const Vector& other) const;

  int64_t operator^(const Vector& other) const;

  Vector operator+(const Vector& other);

  Vector operator-(const Vector& other);

  Vector& operator+=(const Vector& other);

  Vector& operator-=(const Vector& other);

  Vector operator-() const;

  Vector& operator*=(const int64_t& value);

  int64_t GetX() const;

  int64_t GetY() const;

 private:
  int64_t x_;
  int64_t y_;
};

Vector operator*(const int64_t& value, const Vector& vector);

Vector operator*(const Vector& vector, const int64_t& value);

class Point;

class Segment;

class IShape {
 public:
  virtual void Move(const Vector& vector) = 0;

  virtual bool ContainsPoint(const Point& point) = 0;

  virtual bool CrossSegment(const Segment& segment) = 0;

  virtual IShape* Clone() = 0;

  virtual ~IShape() = default;
};

class Point : public IShape {
 public:
  Point();

  Point(int64_t x_coordinate, int64_t y_coordinate);

  int64_t GetX() const;

  int64_t GetY() const;

  Point& operator=(const Point& other);

  Point(const Point& other);

  void Move(const Vector& vector) override;

  bool ContainsPoint(const Point& point) override;

  bool CrossSegment(const Segment& segment) override;

  IShape* Clone() override;

  Point& operator+=(const Vector& vector);

 private:
  int64_t x_;
  int64_t y_;
};

Vector operator-(const Point& first, const Point& second);

class Segment : public IShape {
 public:
  Segment(const Point& first, const Point& second);

  Point GetA() const;

  Point GetB() const;

  Segment(const Segment& other);

  void Move(const Vector& vector) override;

  bool ContainsPoint(const Point& point) override;

  bool CrossSegment(const Segment& segment) override;

  IShape* Clone() override;

  static int64_t GetSign(int64_t value);

  static int64_t GetSign(double value);

 private:
  Point a_;
  Point b_;
};

class Line : public IShape {
 public:
  Line(const Point& first, const Point& second);

  int64_t GetA() const;

  int64_t GetB() const;

  int64_t GetC() const;

  Line(const Line& other);

  void Move(const Vector& vector) override;

  bool ContainsPoint(const Point& point) override;

  bool CrossSegment(const Segment& segment) override;

  IShape* Clone() override;

  static double FindIntersectionX(const Line& first, const Line& second);

 private:
  int64_t a_;
  int64_t b_;
  int64_t c_;
  Point first_point_;
  Point second_point_;

  void SetQuotients();
};

class Ray : public IShape {
 public:
  Ray(const Point& a_pt, const Point& b_pt);

  Point GetA() const;

  Vector GetVector() const;

  Ray(const Ray& other);

  void Move(const Vector& vector) override;

  bool ContainsPoint(const Point& point) override;

  bool CrossSegment(const Segment& segment) override;

  IShape* Clone() override;

 private:
  Point a_;
  Vector vector_;
  Point b_;
};

class Circle : public IShape {
 public:
  Circle(const Point& centre, size_t radius);

  Circle(const Circle& other);

  Point GetCentre() const;

  size_t GetRadius() const;

  void Move(const Vector& vector) override;

  bool ContainsPoint(const Point& point) override;

  bool PointOnCircle(const Point& point);

  bool PointInCircle(const Point& point);

  bool CrossSegment(const Segment& segment) override;

  IShape* Clone() override;

 private:
  Point centre_;
  size_t radius_;
};
