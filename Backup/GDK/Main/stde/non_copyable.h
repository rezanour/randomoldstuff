// A simple base class which prevents copy constructor and assignment operator from being generated
#pragma once

namespace stde
{
    class non_copyable
    {
    public:
        non_copyable() {}

    private:
        non_copyable(const non_copyable&);
        non_copyable& operator= (const non_copyable&);
    };
}

