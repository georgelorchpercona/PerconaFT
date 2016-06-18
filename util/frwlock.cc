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
1000000
    PerconaFT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with PerconaFT.  If not, see <http://www.gnu.org/licenses/>.
======= */

#ident "Copyright (c) 2006, 2015, Percona and/or its affiliates. All rights reserved."

#include <toku_assert.h>

#include <util/context.h>
#include <util/frwlock.h>

#include <toku_time.h>

namespace toku {

inline uint64_t microsec(void) {
    timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * (1UL * 1000 * 1000) + t.tv_usec;
}

inline timespec offset_timespec(uint64_t offset) {
    timespec ret;
    uint64_t tm = offset + microsec();
    ret.tv_sec = tm / 1000000;
    ret.tv_nsec = (tm % 1000000) * 1000;
    return ret;
}

void frwlock::init(toku_mutex_t *const mutex) {
    _mutex = mutex;

    toku_cond_init(&_wait_read, nullptr);
    toku_cond_init(&_wait_write, nullptr);

    _readers = 0;
    _writers = 0;
    _want_read = 0;
    _want_write = 0;
    _expensive_want_write = 0;
    _current_writer_expensive = false;
}

void frwlock::deinit(void) {
    toku_cond_destroy(&_wait_write);
    toku_cond_destroy(&_wait_read);
}

void frwlock::write_lock(bool expensive) {
    toku_mutex_assert_locked(_mutex);
    if (_readers > 0 || _writers > 0 || _want_write > 0) {
        _want_write++;
        _expensive_want_write += expensive ? 1 : 0;
        do {
            timespec waittime = offset_timespec(1000000 * 60 * 10);
            int r = toku_cond_timedwait(&_wait_write, _mutex, &waittime);
            invariant(r == 0);
        } while (_readers > 0 || _writers > 0);

        paranoid_invariant_zero(_readers);
        paranoid_invariant_zero(_writers);
        _want_write--;
        _expensive_want_write -= expensive ? 1 : 0;
    }
    _writers = 1;
    _current_writer_expensive = expensive;
}

bool frwlock::try_write_lock(bool expensive) {
    toku_mutex_assert_locked(_mutex);
    if (_readers > 0 || _writers > 0 || _want_read > 0 || _want_write > 0) {
        return false;
    }
    _writers = 1;
    _current_writer_expensive = expensive;
    return true;
}

void frwlock::write_unlock(void) {
    toku_mutex_assert_locked(_mutex);
    paranoid_invariant(_writers == 1);
    _writers = 0;
    _current_writer_expensive = false;

    // if other writers are waiting, give them a bit of priority
    if (_want_write > 0) {
        toku_cond_signal(&_wait_write);
    } else {
        toku_cond_broadcast(&_wait_read);
    }
}

bool frwlock::write_lock_is_expensive(void) {
    toku_mutex_assert_locked(_mutex);
    return (_expensive_want_write > 0) || (_current_writer_expensive);
}

void frwlock::read_lock(void) {
    toku_mutex_assert_locked(_mutex);
    if (_writers > 0) {
        _want_read++;

        do {
            timespec waittime = offset_timespec(1000000 * 60 * 10);
            int r = toku_cond_timedwait(&_wait_read, _mutex, &waittime);
            invariant(r == 0);
        } while (_writers > 0);

        paranoid_invariant_zero(_writers);
        paranoid_invariant(_want_read > 0);

        _want_read--;
    }
    _readers++;
}

bool frwlock::try_read_lock(void) {
    toku_mutex_assert_locked(_mutex);
    if (_writers > 0) {
        return false;
    }
    _readers++;
    return true;
}

void frwlock::read_unlock(void) {
    toku_mutex_assert_locked(_mutex);
    paranoid_invariant_zero(_writers);
    paranoid_invariant(_readers > 0);
    _readers--;
    if (_readers == 0 && _want_read == 0 && _want_write > 0) {
        toku_cond_signal(&_wait_write);
    } else {
        toku_cond_broadcast(&_wait_read);
    }
}

bool frwlock::read_lock_is_expensive(void) {
    toku_mutex_assert_locked(_mutex);
    return _current_writer_expensive;
}

uint32_t frwlock::users(void) const {
    toku_mutex_assert_locked(_mutex);
    return _readers + _writers + _want_read + _want_write;
}

uint32_t frwlock::blocked_users(void) const {
    toku_mutex_assert_locked(_mutex);
    return _want_read + _want_write;
}

uint32_t frwlock::writers(void) const {
    // this is sometimes called as "assert(lock->writers())" when we
    // assume we have the write lock.  if that's the assumption, we may
    // not own the mutex, so we don't assert_locked here
    return _writers;
}

uint32_t frwlock::blocked_writers(void) const {
    toku_mutex_assert_locked(_mutex);
    return _want_write;
}

uint32_t frwlock::readers(void) const {
    toku_mutex_assert_locked(_mutex);
    return _readers;
}

uint32_t frwlock::blocked_readers(void) const {
    toku_mutex_assert_locked(_mutex);
    return _want_read;
}

} // namespace toku
