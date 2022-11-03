#include "utils.hxx"

auto
split(const std::string& s, const std::string& delimiter)
  -> std::vector<std::string>
{
  auto res = std::vector<std::string>{};

  auto start = 0U;
  auto end = s.find(delimiter);

  while (end != std::string::npos) {
    res.push_back(s.substr(start, end - start));
    start = end + delimiter.length();
    end = s.find(delim, start);
  }

  return res;
}

auto
is_prefix(const std::string& s, const std::string& pre) -> bool
{
  return s.compare(0, pre.size(), pre) == 0;
}