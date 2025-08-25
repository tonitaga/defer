/*
 * Copyright (c) 2025 Gubaydullin Nurislam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

/**
 * @file
 * @brief Defines a `defer` statement for C++, similar to Go's `defer`.
 * @details This header provides a RAII-based utility to execute a function
 *          or lambda upon leaving the current scope, regardless of how it is exited
 *          (normal return, exception, etc.).
 */

#ifndef DEFER_INCLUDE_DEFER_DEFER_HPP
#define DEFER_INCLUDE_DEFER_DEFER_HPP

#include <functional>

/**
 * @brief Namespace containing the implementation details for the `defer` utility.
 * @warning Symbols in this namespace are for internal use and should not be used directly.
 */
namespace defer { namespace detail {

/// @brief Alias for the type of task (a void-returning, no-argument function) to be deferred.
using DeferTaskType = std::function<void()>;

/**
 * @brief RAII guard class that executes a stored task upon its destruction.
 * @details The destructor of this class is noexcept and will catch any exceptions
 *          thrown by the deferred task to prevent termination during stack unwinding.
 */
class Defer
{
public:
    /**
     * @brief Constructs the Defer object, taking ownership of the provided task.
     * @param task The callable task to be executed upon destruction.
     */
    Defer(DeferTaskType task)
        : m_task(std::move(task))
    {};

    /**
     * @brief Destructor. Executes the deferred task.
     * @note The destructor is explicitly marked `noexcept`. Any exceptions thrown
     *       by the deferred task are caught and silently discarded to ensure
     *       safe execution during stack unwinding (e.g., if an exception is already active).
     */
    ~Defer() noexcept
    {
        try
        {
            m_task();
        }
        catch (...)
        {} // Exceptions from the deferred task are suppressed.
    }

private:
    DeferTaskType m_task; ///< @brief The stored task to be executed.
};

/**
 * @brief An empty tag type used to enable a clean syntax for the `defer` macro.
 * @details This struct has no functionality of its own. It exists solely to be
 *          used with the overloaded `operator|` to create a `Defer` object.
 */
struct DeferTaskTag {};

/**
 * @brief Overloaded operator that creates a Defer object from a tag and a task.
 * @details This function enables the syntax `DeferTaskTag{} | [&]{...}` which is
 *          used internally by the `defer` macro.
 * @param tag Ignored tag parameter (used for syntactic purposes).
 * @param task The lambda or function to be deferred.
 * @return A fully constructed `Defer` object ready to execute the task on scope exit.
 */
Defer operator|(DeferTaskTag, DeferTaskType task)
{
    return Defer(std::move(task));
}

}} // namespace defer::detail

// Internal macro helpers for generating unique variable names.
#define __DEFER_CONCAT__(a, b)          __DEFER_CONCAT_HELPER_1__(a, b)
#define __DEFER_CONCAT_HELPER_1__(a, b) __DEFER_CONCAT_HELPER_2__(a, b)
#define __DEFER_CONCAT_HELPER_2__(a, b) a ## b

#define __DEFER_NEWBIE__ __DEFER_CONCAT__(__defer_newbie_, __LINE__)

/**
 * @def defer
 * @brief Macro to defer execution of a statement until the end of the current scope.
 * @par Usage:
 * @code{.cpp}
 * {
 *     FILE* f = fopen("file.txt", "r");
 *     if (!f) return;
 *     defer { fclose(f); }; // This will run at the end of the scope.
 *
 *     // ... work with the file ...
 *     // fclose(f) is called automatically here, even if an exception is thrown.
 * }
 * @endcode
 * @details The macro creates an anonymous RAII object. The provided lambda is
 *          executed when this object goes out of scope. The lambda captures variables
 *          by reference (`[&]`) to have access to the local context.
 * @warning Because the lambda captures by reference, it is crucial that the
 *          lifetime of all captured references exceeds the lifetime of the scope
 *          in which the `defer` statement is declared. Deferring operations on
 *          stack variables that go out of scope earlier will lead to undefined behavior.
 */
#define defer const auto __DEFER_NEWBIE__ = ::defer::detail::DeferTaskTag{} | [&]() mutable -> void

#endif // DEFER_INCLUDE_DEFER_DEFER_HPP