#pragma once

namespace TestLib {

class TestObject {
public:
	virtual std::string ToString() {
		return "This is TestObject";
	}
};

class InheritedA : public TestObject {
public:
	std::string ToString() override {
		return "This is InheritedA";
	}
};

class InheritedB : public TestObject {
public:
	std::string ToString() override {
		return "This is InheritedA";
	}
};

}