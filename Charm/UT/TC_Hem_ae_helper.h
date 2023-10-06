#pragma once

namespace Hem { class Bill; class Unbill; };

Hem::Bill createBill( const std::wstring& text );
Hem::Unbill createUnbill( const std::wstring& text );
