#include "../Hem/Filer.h"

class MockFiler : public NsCanopy::Filer
{
public:
    MockFiler();
    ~MockFiler();
private:
    void clear();
};