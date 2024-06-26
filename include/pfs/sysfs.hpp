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

#ifndef PFS_SYSFS_HPP
#define PFS_SYSFS_HPP

#include <fcntl.h>
#include <unistd.h>

#include <set>
#include <string>

#include "block.hpp"

namespace pfs {

class sysfs final
{
public:
    static const std::string DEFAULT_ROOT;

public:
    sysfs(const std::string& root = DEFAULT_ROOT);
    sysfs(int root_fd);

    sysfs(const sysfs&) = default;
    sysfs(sysfs&&)      = default;

    sysfs& operator=(const sysfs&) = delete;
    sysfs& operator=(sysfs&&) = delete;

public: // Block API
    block get_block(const std::string& block_name) const;
    std::set<block> get_blocks() const;

private:
    const int _root_fd = AT_FDCWD;
    const std::string _root;
};

} // namespace pfs

#endif // PFS_SYSFS_HPP
