/**
 * @file Parallel.h
 * @brief Minimal index-parallel loop with cooperative cancellation.
 */
#pragma once

#include <algorithm>
#include <atomic>
#include <thread>
#include <vector>

namespace argus {

/**
 * @brief Resolve a requested worker count against the machine's capabilities.
 * @param requested Desired thread count, or 0 to auto-detect.
 * @return A thread count of at least 1.
 */
inline int resolveThreadCount(int requested)
{
    if (requested > 0)
        return requested;
    const unsigned hc = std::thread::hardware_concurrency();
    return hc ? static_cast<int>(hc) : 4;
}

/**
 * @brief Run @p fn for every index in <tt>[0, count)</tt> across @p threads workers.
 *
 * Deliberately hand-rolled rather than QtConcurrent: callers write into their own
 * preallocated slots so there is no locking, no future plumbing, and the cancel
 * flag is polled between items rather than between chunks.
 *
 * @param count   Number of items to process.
 * @param threads Worker threads to spawn; clamped to <tt>[1, count]</tt>.
 * @param cancel  Optional flag polled before each item; may be @c nullptr.
 * @param fn      Callable invoked as <tt>fn(int index)</tt>, concurrently.
 * @note @p fn must be safe to call from several threads at once.
 * @note Returns once every worker has joined, including on cancellation.
 */
template <typename Fn>
void parallelFor(int count, int threads, const std::atomic_bool *cancel, Fn fn)
{
    if (count <= 0)
        return;

    threads = std::max(1, std::min(threads, count));
    if (threads == 1) {
        for (int i = 0; i < count; ++i) {
            if (cancel && cancel->load(std::memory_order_relaxed))
                return;
            fn(i);
        }
        return;
    }

    std::atomic<int> next{0};
    std::vector<std::thread> pool;
    pool.reserve(static_cast<size_t>(threads));

    for (int t = 0; t < threads; ++t) {
        pool.emplace_back([&] {
            for (;;) {
                if (cancel && cancel->load(std::memory_order_relaxed))
                    return;
                const int i = next.fetch_add(1, std::memory_order_relaxed);
                if (i >= count)
                    return;
                fn(i);
            }
        });
    }

    for (auto &t : pool)
        t.join();
}

/**
 * @brief Index-parallel loop that also tells each worker which slot it is.
 *
 * The plain @ref parallelFor is enough when workers share nothing. This variant
 * exists for the opposite case: when every thread needs its own expensive,
 * non-shareable resource — an inference session, a database connection — the
 * loop has to hand out a stable slot so each thread can pick its own.
 *
 * @param count   Number of items to process.
 * @param threads Worker threads to spawn; clamped to <tt>[1, count]</tt>.
 * @param cancel  Optional flag polled before each item; may be @c nullptr.
 * @param fn      Callable invoked as <tt>fn(int index, int worker)</tt>, where
 *                @c worker is in <tt>[0, threads)</tt> and is never used by two
 *                threads at once.
 */
template <typename Fn>
void parallelForWorker(int count, int threads, const std::atomic_bool *cancel, Fn fn)
{
    if (count <= 0)
        return;

    threads = std::max(1, std::min(threads, count));
    if (threads == 1) {
        for (int i = 0; i < count; ++i) {
            if (cancel && cancel->load(std::memory_order_relaxed))
                return;
            fn(i, 0);
        }
        return;
    }

    std::atomic<int> next{0};
    std::vector<std::thread> pool;
    pool.reserve(static_cast<size_t>(threads));

    for (int t = 0; t < threads; ++t) {
        pool.emplace_back([&, t] {
            for (;;) {
                if (cancel && cancel->load(std::memory_order_relaxed))
                    return;
                const int i = next.fetch_add(1, std::memory_order_relaxed);
                if (i >= count)
                    return;
                fn(i, t);
            }
        });
    }

    for (auto &th : pool)
        th.join();
}

} // namespace argus
