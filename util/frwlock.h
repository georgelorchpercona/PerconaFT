/* -*- mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
// vim: ft=cpp:expandtab:ts=8:sw=4:softtabstop=4:
#ident "$Id$"
/*======
This file is part of PerconaFT.


Copyright (c) 2006, 2015, Percona and/or its affiliates. All rights reserved.

    PerconaFT is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License, version 2,
    as published by the Free Software Foundation.

    PerconaFT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PerconaFT.  If not, see <http://www.gnu.org/licenses/>.

----------------------------------------

    PerconaFT is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License, version 3,
    as published by the Free Software Foundation.

    PerconaFT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with PerconaFT.  If not, see <http://www.gnu.org/licenses/>.
======= */

#ident "Copyright (c) 2006, 2015, Percona and/or its affiliates. All rights reserved."

#pragma once

#include <toku_portability.h>
#include <toku_pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/context.h>

//TODO: update comment, this is from rwlock.h

namespace toku {

class frwlock {
public:

    void init(toku_mutex_t *const mutex);
    void deinit(void);

    void write_lock(bool expensive);
    bool try_write_lock(bool expensive);
    void write_unlock(void);
    // returns true if acquiring a write lock will be expensive
    bool write_lock_is_expensive(void);

    void read_lock(void);
    bool try_read_lock(void);
    void read_unlock(void);
    // returns true if acquiring a read lock will be expensive
    bool read_lock_is_expensive(void);

    uint32_t users(void) const;
    uint32_t blocked_users(void) const;
    uint32_t writers(void) const;
    uint32_t blocked_writers(void) const;
    uint32_t readers(void) const;
    uint32_t blocked_readers(void) const;

private:
    toku_mutex_t *_mutex;
    toku_cond_t _wait_read;
    toku_cond_t _wait_write;

    uint32_t _readers;
    uint32_t _writers;
    uint32_t _want_read;
    uint32_t _want_write;
    // number of writers waiting that are expensive
    // MUST be < m_num_want_write
    uint32_t _expensive_want_write;
    bool _current_writer_expensive;
};

ENSURE_POD(frwlock);

} // namespace toku

// include the implementation here
// #include "frwlock.cc"
