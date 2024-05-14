#pragma once

inline constexpr unsigned char operator"" _uc(unsigned long long arg) noexcept
{
	return static_cast<unsigned char>(arg);
}