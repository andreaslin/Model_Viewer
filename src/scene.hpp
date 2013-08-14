#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include <map>

class SceneNode {
public:
	SceneNode(const std::string& name);
	virtual ~SceneNode();

	virtual void walk_gl(bool picking = false) const;

	const Matrix4x4& get_transform() const { return m_trans; }
	const Matrix4x4& get_inverse() const { return m_invtrans; }
  
	void set_transform(const Matrix4x4& m)
	{
		m_trans = m;
		m_invtrans = m.invert();
	}

	void set_transform(const Matrix4x4& m, const Matrix4x4& i)
	{
		m_trans = m;
		m_invtrans = i;
	}

	void add_child(SceneNode* child)
	{
		m_children.push_back(child);
	}

	void remove_child(SceneNode* child)
	{
		m_children.remove(child);
	}

	// Callbacks to be implemented.
	// These will be called from Lua.
	void rotate(char axis, double angle);
	void scale(const Vector3D& amount);
	void translate(const Vector3D& amount);

	// Returns true if and only if this node is a JointNode
	virtual bool is_joint() const;

	// Return name of the node
	std::string get_name() { return m_name; }

	// Rotation variables for checking limits
	Vector3D rotation;


	// Record information needs for undo/redo action
	struct Info {
		Matrix4x4 old_m_trans;
		Vector3D old_rotation;
	};

	void set_rotation( const Vector3D &r ) {
		rotation = r;
	}

	const Vector3D& get_rotation() const { return rotation; }

	// Finding all joint nodes for reset in viewer
	void findJoints( std::map<unsigned int, Info> &sj );

	// Inform the node to recalculate its normal
	void hasChanged() { changed = true; }

protected:
  
	// Useful for picking
	int m_id;
	std::string m_name;

	// Transformations
	Matrix4x4 m_trans;
	Matrix4x4 m_invtrans;

	// Hierarchy
	typedef std::list<SceneNode*> ChildList;
	ChildList m_children;

	// Identify whether the node has been changed for a new display list
	mutable bool changed;

};

class JointNode : public SceneNode {
public:
	JointNode(const std::string& name);
	virtual ~JointNode();

	virtual void walk_gl(bool bicking = false) const;

	virtual bool is_joint() const;

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	void set_pick();
	bool get_pick();

	struct JointRange {
		double min, init, max;
	};

	void checkLimits(); 		// Check the limits of x and y angle

protected:
	JointRange m_joint_x, m_joint_y;
	bool picked;                    // Inidicate whether the joint is picked or not
};

class GeometryNode : public SceneNode {
public:
	GeometryNode(const std::string& name,
				 Primitive* primitive);
	virtual ~GeometryNode();

	virtual void walk_gl(bool picking = false) const;

	const Material* get_material() const;
	Material* get_material();

	void set_material(Material* material)
	{
		m_material = material;
	}

protected:
	Material* m_material;
	Primitive* m_primitive;
};

#endif
