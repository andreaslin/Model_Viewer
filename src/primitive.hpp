#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

class Primitive {
public:
	Primitive();
  virtual ~Primitive();
  virtual void walk_gl(bool picking) const = 0;
	void hasChanged();
protected:
	mutable bool changed;
	mutable bool old_picking;
};


class Sphere : public Primitive {
public:
  virtual ~Sphere();
  virtual void walk_gl(bool picking) const;
private:
	mutable GLuint mysphereID;
};

#endif
