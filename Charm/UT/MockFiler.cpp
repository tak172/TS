#include "stdafx.h"
#include <boost/filesystem.hpp>
#include "MockFiler.h"

MockFiler::MockFiler()
    : NsCanopy::Filer( boost::filesystem::temp_directory_path().wstring(), 10 )
{
    clear();
}

MockFiler::~MockFiler()
{
     clear();
}

// удалить все файлы
void MockFiler::clear()
{
    NsCanopy::Filer::wipe_old_shift(0);
}
