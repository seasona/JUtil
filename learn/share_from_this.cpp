#include <iostream>
#include <memory>

class Value : public std::enable_shared_from_this<Value> {
public:
    std::shared_ptr<Value> setValueThis(bool value) {
        value_ = value;
        return shared_from_this();
    }

    void setValue(bool value) { value_ = value; }

    bool getValue() { return value_; }

private:
    bool value_;
};

void useThis(std::shared_ptr<Value> vp) {
    std::cout << vp->getValue() << std::endl;
}

int main() {
    /// std::share_from can be used in class to transfer the shared_ptr of this.
    useThis(std::make_shared<Value>()->setValue(true)); // is wrong
    useThis(std::make_shared<Value>()->setValueThis(true));// right
    return 0;
}