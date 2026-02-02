// From official documentation, example header.

#ifndef CNAKE_H
#define CNAKE_H

#include <godot_cpp/core/class_db.hpp>

class GDExample : public Sprite2D {
	GDCLASS(GDExample, Sprite2D)

private:
	double time_passed;

protected:
	static void _bind_methods();

public:
	GDExample();
	~GDExample();

	void _process(double delta) override;
};

#endif // CNAKE_H