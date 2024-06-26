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

#ifndef PFS_BLOCK_HPP
#define PFS_BLOCK_HPP

#include <fcntl.h>

#include "types.hpp"
#include "block_queue.hpp"

namespace pfs {

class block final
{
public:
    static const std::string BLOCK_DIR;

public:
    block(const block&) = default;
    block(block&&)      = default;

    block& operator=(const block&) = delete;
    block& operator=(block&&) = delete;

    bool operator<(const block& rhs) const;

public: // Properties
    const std::string& name() const;
    const std::string& dir() const;

    size_t get_size() const;
    dev_t get_dev() const;

    block_stat get_stat() const;
    block_queue get_queue() const;

private:
    friend class sysfs;
    block(const std::string& sysfs_root, const std::string& name, int sysfs_fd = AT_FDCWD);

private:
    const int _sysfs_fd;
    const std::string _name;
    const std::string _block_root;
};

} // namespace pfs

#endif // PFS_BLOCK_HPP
