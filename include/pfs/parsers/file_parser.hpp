/*
 *  Copyright 2020-present Daniel Trugman
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef PFS_PARSERS_FILE_PARSER_HPP
#define PFS_PARSERS_FILE_PARSER_HPP

#include <fcntl.h>
#include <unistd.h>

#include <set>
#include <string>
#include <unordered_map>

#include "pfs/parser_error.hpp"
#include "pfs/defer.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

// When parsing key value text files, we might encounter different keys which
// should be parsed in the same manner. To relate those lines in the same
// manner, a remap function can be used.
using remap_function = const std::function<void(std::string&)>;

template <typename Output>
class file_parser
{
public:
    Output parse(int dirfd, const std::string& path,
                 const std::set<std::string>& keys = {})
    {
        static const char NEWLINE('\n');

        int fd = openat(dirfd, path.c_str(), O_RDONLY);
        if (fd < 0)
        {
            throw parser_error("Couldn't open file", path);
        }
        defer close_fd([fd] { close(fd); });

        FILE* fp = fdopen(fd, "r");
        if (!fp)
        {
            throw parser_error("Couldn't open file", path);
        }
        defer close_fp([fp] { fclose(fp); });

        Output output;
        ssize_t len = 0;
        size_t n = 0;
        char* linebuf = NULL;

        // free linebuf even if getline() fails
        defer free_line([linebuf] { free(linebuf); });
        while ((len = getline(&linebuf, &n, fp)) != -1)
        {
            while (len > 0 && linebuf[len - 1] == NEWLINE)
            {
                len--;
            }

            std::string line(linebuf, len);
            std::string key;
            std::string value;
            std::tie(key, value) = utils::split_once(line, _delim);
            if (key.empty())
            {
                throw parser_error("Corrupted line - Missing key", line);
            }

            utils::rtrim(key);
            if (_key_remap)
            {
                _key_remap(key);
            }

            // Value MIGHT be an empty value, for example:
            // Process without any groups

            // If caller specified a set of keys, get only those
            if (!keys.empty() && keys.find(key) == keys.end())
            {
                continue;
            }

            auto iter = _parsers.find(key);
            if (iter != _parsers.end())
            {
                utils::ltrim(value);
                auto& parser = iter->second;
                parser(value, output);
            }
        }

        return output;
    }

    Output parse(const std::string& path,
                 const std::set<std::string>& keys = {})
    {
        return parse(AT_FDCWD, path, keys);
    }

protected:
    using value_parser =
        std::function<void(const std::string& value, Output& out)>;
    using value_parsers = std::unordered_map<std::string, value_parser>;

    file_parser(const char delim, const value_parsers& parsers,
                remap_function key_remap = nullptr)
        : _delim(delim), _parsers(parsers), _key_remap(key_remap)
    {}

private:
    const char _delim;
    const value_parsers& _parsers;
    remap_function _key_remap;
};

} // namespace parsers
} // namespace impl
} // namespace pfs

#endif // PFS_PARSERS_FILE_PARSER_HPP
