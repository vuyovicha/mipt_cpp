#include "geometry.hpp"

// Vector

Vector::Vector() {
  x_ = 0;
  y_ = 0;
}

Vector::Vector(int64_t x_coordinate, int64_t y_coordinate) {
  x_ = x_coordinate;
  y_ = y_coordinate;
}

Vector& Vector::operator=(const Vector& other) = default;

Vector::Vector(const Vector& other) {
  x_ = other.x_;
  y_ = other.y_;
}

int64_t Vector::operator*(const Vector& other) const {
  return x_ * other.x_ + y_ * other.y_;
}

int64_t Vector::operator^(const Vector& other) const {
  return x_ * other.y_ - y_ * other.x_;
}

Vector& Vector::operator+=(const Vector& other) {
  x_ += other.x_;
  y_ += other.y_;
  return *this;
}

Vector& Vector::operator-=(const Vector& other) {
  x_ -= other.x_;
  y_ -= other.y_;
  return *this;
}

Vector Vector::operator+(const Vector& other) {
  Vector result = *this;
  result += other;
  return result;
}

Vector Vector::operator-(const Vector& other) {
  Vector result = *this;
  result -= other;
  return result;
}

Vector& Vector::operator*=(const int64_t& value) {
  x_ *= value;
  y_ *= value;
  return *this;
}

Vector Vector::operator-() const {
  Vector result = *this;
  result *= -1;
  return result;
}

int64_t Vector::GetX() const { return x_; }

int64_t Vector::GetY() const { return y_; }

Vector operator*(const int64_t& value, const Vector& vector) {
  Vector result = vector;
  result *= value;
  return result;
}

Vector operator*(const Vector& vector, const int64_t& value) {
  Vector result = vector;
  result *= value;
  return result;
}

// Point

Point::Point() {
  x_ = 0;
  y_ = 0;
}

Point::Point(int64_t x_coordinate, int64_t y_coordinate) {
  x_ = x_coordinate;
  y_ = y_coordinate;
}

int64_t Point::GetX() const { return x_; }

int64_t Point::GetY() const { return y_; }

Point& Point::operator=(const Point& other) = default;

Point::Point(const Point& other) {
  x_ = other.x_;
  y_ = other.y_;
}

void Point::Move(const Vector& vector) { *this += vector; }

bool Point::ContainsPoint(const Point& point) {
  return x_ == point.x_ && y_ == point.y_;
}

bool Point::CrossSegment(const Segment& segment) {
  if (x_ == segment.GetA().x_ && y_ == segment.GetA().y_) {
    return true;
  }
  if (x_ == segment.GetB().x_ && y_ == segment.GetB().y_) {
    return true;
  }
  Vector first = Vector(segment.GetA().GetX() - x_, segment.GetA().GetY() - y_);
  Vector second =
      Vector(segment.GetB().GetX() - x_, segment.GetB().GetY() - y_);
  return (first ^ second) == 0 && (first * second) < 0;
}

IShape* Point::Clone() { return new Point(*this); }

Point& Point::operator+=(const Vector& vector) {
  x_ += vector.GetX();
  y_ += vector.GetY();
  return *this;
}

Vector operator-(const Point& first, const Point& second) {
  int64_t x_coordinate = first.GetX() - second.GetX();
  int64_t y_coordinate = first.GetY() - second.GetY();
  return Vector(x_coordinate, y_coordinate);
}

// Segment

Segment::Segment(const Point& first, const Point& second) {
  a_ = first;
  b_ = second;
}

Point Segment::GetA() const { return a_; }

Point Segment::GetB() const { return b_; }

void Segment::Move(const Vector& vector) {
  a_ += vector;
  b_ += vector;
}

bool Segment::ContainsPoint(const Point& point) {
  Point copy = point;
  return copy.CrossSegment(*this);
}

int64_t Segment::GetSign(int64_t value) {
  int64_t return_value = -1;
  if (value == 0) {
    return_value = 0;
  } else if (value > 0) {
    return_value = 1;
  }
  return return_value;
}

int64_t Segment::GetSign(double value) {
  int64_t return_value = -1;
  if (value == 0) {
    return_value = 0;
  } else if (value > 0) {
    return_value = 1;
  }
  return return_value;
}

bool Segment::CrossSegment(const Segment& segment) {
  if (a_.GetX() == b_.GetX() && a_.GetY() == b_.GetY()) {
    Point point = Point(a_.GetX(), a_.GetY());
    return point.CrossSegment(segment);
  }
  Vector from_this = Vector(a_.GetX() - b_.GetX(), a_.GetY() - b_.GetY());
  Vector from_segment = Vector(segment.GetA().GetX() - segment.GetB().GetX(),
                               segment.GetA().GetY() - segment.GetB().GetY());
  if ((from_this ^ from_segment) == 0) {
    return ContainsPoint(segment.GetA()) || ContainsPoint(segment.GetB());
  }
  Vector this_a_seg_a = Vector(a_.GetX() - segment.GetA().GetX(),
                               a_.GetY() - segment.GetA().GetY());
  Vector this_b_seg_a = Vector(b_.GetX() - segment.GetA().GetX(),
                               b_.GetY() - segment.GetA().GetY());
  Vector this_a_seg_b = Vector(a_.GetX() - segment.GetB().GetX(),
                               a_.GetY() - segment.GetB().GetY());
  Vector this_b_seg_b = Vector(b_.GetX() - segment.GetB().GetX(),
                               b_.GetY() - segment.GetB().GetY());
  Vector seg_a_this_a = Vector(segment.GetA().GetX() - a_.GetX(),
                               segment.GetA().GetY() - a_.GetY());
  Vector seg_b_this_a = Vector(segment.GetB().GetX() - a_.GetX(),
                               segment.GetB().GetY() - a_.GetY());
  Vector seg_a_this_b = Vector(segment.GetA().GetX() - b_.GetX(),
                               segment.GetA().GetY() - b_.GetY());
  Vector seg_b_this_b = Vector(segment.GetB().GetX() - b_.GetX(),
                               segment.GetB().GetY() - b_.GetY());
  return (GetSign(this_a_seg_a ^ this_b_seg_a) !=
          GetSign(this_a_seg_b ^ this_b_seg_b)) &&
         (GetSign(seg_a_this_a ^ seg_b_this_a) !=
          GetSign(seg_a_this_b ^ seg_b_this_b));
}

Segment::Segment(const Segment& other) {
  a_ = other.GetA();
  b_ = other.GetB();
}

IShape* Segment::Clone() { return new Segment(*this); }

// Line

void Line::SetQuotients() {
  a_ = first_point_.GetY() - second_point_.GetY();
  b_ = second_point_.GetX() - first_point_.GetX();
  c_ = (first_point_.GetX() - second_point_.GetX()) * first_point_.GetY() +
       (second_point_.GetY() - first_point_.GetY()) * first_point_.GetX();
}

Line::Line(const Point& first, const Point& second) {
  first_point_ = first;
  second_point_ = second;
  SetQuotients();
}

int64_t Line::GetA() const { return a_; }

int64_t Line::GetB() const { return b_; }

int64_t Line::GetC() const { return c_; }

void Line::Move(const Vector& vector) {
  first_point_ += vector;
  second_point_ += vector;
  SetQuotients();
}

bool Line::ContainsPoint(const Point& point) {
  return (a_ * point.GetX() + b_ * point.GetY() + c_) == 0;
}

bool Line::CrossSegment(const Segment& segment) {
  if (ContainsPoint(segment.GetA()) || ContainsPoint(segment.GetB())) {
    return true;
  }
  Vector this_first_seg_a = Vector(first_point_.GetX() - segment.GetA().GetX(),
                                   first_point_.GetY() - segment.GetA().GetY());
  Vector this_second_seg_a =
      Vector(second_point_.GetX() - segment.GetA().GetX(),
             second_point_.GetY() - segment.GetA().GetY());
  Vector this_first_seg_b = Vector(first_point_.GetX() - segment.GetB().GetX(),
                                   first_point_.GetY() - segment.GetB().GetY());
  Vector this_second_seg_b =
      Vector(second_point_.GetX() - segment.GetB().GetX(),
             second_point_.GetY() - segment.GetB().GetY());
  return Segment::GetSign(this_first_seg_a ^ this_second_seg_a) !=
         Segment::GetSign(this_first_seg_b ^ this_second_seg_b);
}

Line::Line(const Line& other) {
  first_point_ = other.first_point_;
  second_point_ = other.second_point_;
  a_ = other.a_;
  b_ = other.b_;
  c_ = other.c_;
}

IShape* Line::Clone() { return new Line(*this); }

double Line::FindIntersectionX(const Line& first, const Line& second) {
  int64_t a_first = first.a_;
  int64_t c_first = first.c_;
  int64_t a_second = second.a_;
  int64_t c_second = second.c_;
  a_first *= second.b_;
  c_first *= second.b_;
  a_second *= first.b_;
  c_second *= first.b_;
  return static_cast<double>(c_second - c_first) /
         static_cast<double>(a_first - a_second);
}

// Ray

Ray::Ray(const Point& a_pt, const Point& b_pt) {
  a_ = a_pt;
  vector_ = Vector(b_pt.GetX() - a_pt.GetX(), b_pt.GetY() - a_pt.GetY());
  b_ = b_pt;
}

Point Ray::GetA() const { return a_; }

Vector Ray::GetVector() const { return vector_; }

void Ray::Move(const Vector& vector) { a_ += vector; }

bool Ray::ContainsPoint(const Point& point) {
  Line line = Line(a_, b_);
  if (line.ContainsPoint(point)) {
    return (Vector(point.GetX() - a_.GetX(), point.GetY() - a_.GetY()) *
            vector_) >= 0;
  }
  return false;
}

Ray::Ray(const Ray& other) {
  a_ = other.a_;
  b_ = other.b_;
  vector_ = other.vector_;
}

IShape* Ray::Clone() { return new Ray(*this); }

bool Ray::CrossSegment(const Segment& segment) {
  Line line = Line(a_, b_);
  if (line.CrossSegment(segment)) {
    Vector segment_vector = segment.GetB() - segment.GetA();
    if ((segment_vector ^ vector_) == 0) {
      return ContainsPoint(segment.GetA()) || ContainsPoint(segment.GetB());
    }
    Line segment_line = Line(segment.GetA(), segment.GetB());
    double intersection_x = Line::FindIntersectionX(line, segment_line);
    return (intersection_x == static_cast<double>(a_.GetX())) ||
           (Segment::GetSign(intersection_x - static_cast<double>(a_.GetX())) ==
            Segment::GetSign(b_.GetX() - a_.GetX()));
  }
  return false;
}

// Circle

Circle::Circle(const Point& centre, size_t radius) {
  centre_ = centre;
  radius_ = radius;
}

Circle::Circle(const Circle& other) {
  centre_ = other.centre_;
  radius_ = other.radius_;
}

Point Circle::GetCentre() const { return centre_; }

size_t Circle::GetRadius() const { return radius_; }

IShape* Circle::Clone() { return new Circle(*this); }

void Circle::Move(const Vector& vector) { centre_ += vector; }

bool Circle::ContainsPoint(const Point& point) {
  return ((point.GetX() - centre_.GetX()) * (point.GetX() - centre_.GetX()) +
          (point.GetY() - centre_.GetY()) * (point.GetY() - centre_.GetY())) <=
         static_cast<int64_t>(radius_ * radius_);
}

bool Circle::PointOnCircle(const Point& point) {
  return ((point.GetX() - centre_.GetX()) * (point.GetX() - centre_.GetX()) +
          (point.GetY() - centre_.GetY()) * (point.GetY() - centre_.GetY())) ==
         static_cast<int64_t>(radius_ * radius_);
}

bool Circle::PointInCircle(const Point& point) {
  return ((point.GetX() - centre_.GetX()) * (point.GetX() - centre_.GetX()) +
          (point.GetY() - centre_.GetY()) * (point.GetY() - centre_.GetY())) <
         static_cast<int64_t>(radius_ * radius_);
}

bool Circle::CrossSegment(const Segment& segment) {
  if (PointOnCircle(segment.GetA()) || PointOnCircle(segment.GetB())) {
    return true;
  }
  if ((ContainsPoint(segment.GetA()) && !ContainsPoint(segment.GetB())) ||
      (!ContainsPoint(segment.GetA()) && ContainsPoint(segment.GetB()))) {
    return true;
  }
  if (PointInCircle(segment.GetA()) && PointInCircle(segment.GetB())) {
    return false;
  }
  Vector left = Vector(segment.GetA().GetX() - centre_.GetX(),
                       segment.GetA().GetY() - centre_.GetY());
  Vector right = Vector(segment.GetB().GetX() - centre_.GetX(),
                        segment.GetB().GetY() - centre_.GetY());
  int64_t squared_cross_product = (left ^ right) * (left ^ right);
  int64_t squared_segment_length =
      (segment.GetA().GetX() - segment.GetB().GetX()) *
          (segment.GetA().GetX() - segment.GetB().GetX()) +
      (segment.GetA().GetY() - segment.GetB().GetY()) *
          (segment.GetA().GetY() - segment.GetB().GetY());
  double squared_distance = static_cast<double>(squared_cross_product) /
                            static_cast<double>(squared_segment_length);
  return squared_distance <= static_cast<double>(radius_ * radius_);
}
