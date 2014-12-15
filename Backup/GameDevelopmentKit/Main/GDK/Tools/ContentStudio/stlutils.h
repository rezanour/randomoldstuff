#pragma once

namespace std
{
    template <typename E, typename C>
    size_t split(std::basic_string<E> const& s,
                 C &container,
                 E const delimiter,
                 bool keepBlankFields = true)
    {
        size_t n = 0;
        std::basic_string<E>::const_iterator it = s.begin(), end = s.end(), first;
        for (first = it; it != end; ++it)
        {
            // Examine each character and if it matches the delimiter
            if (delimiter == *it)
            {
                if (keepBlankFields || first != it)
                {
                    // extract the current field from the string and
                    // append the current field to the given container
                    container.push_back(std::basic_string<E>(first, it));
                    ++n;
                
                    // skip the delimiter
                    first = it + 1;
                }
                else
                {
                    ++first;
                }
            }
        }
        if (keepBlankFields || first != it)
        {
            // extract the last field from the string and
            // append the last field to the given container
            container.push_back(std::basic_string<E>(first, it));
            ++n;
        }
        return n;
    }

    std::wstring AnsiToWide(std::string s);
    std::wstring DirectoryRootFromPath(std::wstring path);
}

