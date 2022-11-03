#pragma once

#include <string>
#include <vector>

auto
split(const std::string& s, const std::string& delimiter)
  -> std::vector<std::string>;

auto
is_prefix(const std::string& s, const std::string& pre) -> bool;
