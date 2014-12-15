#pragma once

template <class TDerived>
class BaseObject : public std::enable_shared_from_this<TDerived>
{
protected:
    BaseObject() {}

private:
    // block copy
    BaseObject(const BaseObject&);
    BaseObject& operator= (const BaseObject&);

    // block move
    BaseObject(const BaseObject&&);
    BaseObject& operator= (BaseObject&&);
};
