#include "AsyncDelay.h"

#include <Arduino.h>

/**
 * @brief Constructs a new AsyncDelay object and sets its interval.
 *
 * The constructor calls the setInterval method, initializing the delay
 * interval as well as resetting the internal timestamp.
 *
 * @param[in] interval The delay time in milliseconds. Defaults to 0.
 */
AsyncDelay::AsyncDelay(unsigned long interval) {
    setInterval(interval);
}

/**
 * @brief Sets the delay interval for the AsyncDelay object and resets the
 * timer.
 *
 * This method updates the delay interval for the object and automatically
 * resets its internal timestamp to the current system time.
 *
 * @param[in] interval The new delay time in milliseconds.
 */
void AsyncDelay::setInterval(unsigned long interval) {
    // Check for boundary conditions
    if (interval < MIN_INTERVAL) {
        this->interval = MIN_INTERVAL;
    } else if (interval > MAX_INTERVAL) {
        this->interval = MAX_INTERVAL;
    } else {
        this->interval = interval;
    }

    this->resetTime();
}

/**
 * @brief Pauses the delay timer.
 *
 * This method pauses the timer, effectively stopping the counting of time
 * towards the delay interval.
 */
void AsyncDelay::pause() {
    this->isPaused = true;
}

/**
 * @brief Resumes the delay timer.
 *
 * This method resumes the timer if it was paused. It also resets the internal
 * timestamp to the current system time.
 */
void AsyncDelay::resume() {
    this->isPaused = false;
    this->resetTime();
}

/**
 * @brief Sets the callback function to be executed when the delay interval is
 * reached.
 *
 * The provided function will be called automatically when the method
 * `isDone()` returns true.
 *
 * @param[in] cbFn The function to be called as a callback.
 */
void AsyncDelay::setCallback(CallbackFunction cbFn) {
    this->callbackFunction = cbFn;
}

/**
 * @brief Checks if a callback function has been set.
 *
 * This method returns `true` if a callback function has been set using the
 * `setCallback` method, `false` otherwise.
 *
 * @return `true` if a callback function exists, `false` otherwise.
 */
bool AsyncDelay::hasCallback() {
    return this->callbackFunction != nullptr;
}

/**
 * @brief Retrieves the current callback function.
 *
 * This method returns the current callback function set for this AsyncDelay
 * object. If no callback function has been set, it returns nullptr.
 *
 * @return The current callback function or nullptr if no callback function has
 * been set.
 */
CallbackFunction AsyncDelay::getCallback() {
    return this->hasCallback() ? this->callbackFunction : nullptr;
}

/**
 * @brief Returns the delay interval of the AsyncDelay object.
 *
 * This method retrieves the delay interval, in milliseconds, that the
 * object is configured to wait.
 *
 * @return The configured delay time in milliseconds.
 */
unsigned long AsyncDelay::getInterval() {
    return this->interval;
}

/**
 * @brief Resets the internal timestamp to the current system time.
 *
 * This method updates the internal timestamp to the current system time,
 * effectively resetting the timer.
 */
void AsyncDelay::resetTime() {
    this->timestamp = millis();
    if (this->interval == 0) {
        this->isPaused = true;
    } else {
        this->isPaused = false;
    }
}

/**
 * @brief Calculates the elapsed time since the last timestamp update.
 *
 * This method returns the elapsed time, in milliseconds, since the last
 * timestamp update. It accounts for the rollover behavior of Arduino's
 * millis() function.
 *
 * @note The millis() function resets to zero approximately every 49.7 days.
 *
 * @return The elapsed time in milliseconds.
 */
unsigned long AsyncDelay::getDelta() {
    // The millis method resets to zero when the ULONG_MAX range is reached.
    // Therefore, if the last fixation of time was close to the moment of
    // resetting the delta can get a value with a very large range
    // close to ULONG_MAX.
    //
    // We can determine the overflow of millis (approximately every 49.7 days) -
    // if millis is less than the timestamp.
    unsigned long m = millis();
    return m < this->timestamp ? ULONG_MAX - this->timestamp + m
                               : m - this->timestamp;
}

/**
 * @brief Checks if the delay interval has been reached or exceeded.
 *
 * This method checks whether the delay interval has been reached or exceeded,
 * without automatically resetting the internal timestamp. If the interval is
 * reached and a callback function has been set, the callback function will
 * be invoked.
 *
 * This is useful for executing large blocks of code where the timer should be
 * reset manually. If the timer is not reset manually, this function will
 * continuously return true as long as the interval is non-zero.
 *
 * @code
 * if (loop.isDone()) {
 *   // Long-running code
 *   loop.resetTime();  // Manual timer reset
 * }
 * @endcode
 *
 * @note Never returns true if the interval is zero.
 *
 * @return `true` if the delay interval is reached or exceeded (and invokes the
 * callback function if set), `false` otherwise.
 */
bool AsyncDelay::isDone() {
    // If the interval is set to zero, the "ready" state can never occur.
    if (this->isPaused || this->interval == 0) {
        return false;
    }

    // If the loop object is active, then the count is incremented.
    if (this->getDelta() >= this->interval) {
        // Increment the count and call the callback function, if it exists.
        this->count++;

        // Call the callback function, if it exists.
        if (this->callbackFunction != nullptr) {
            this->callbackFunction();
        }

        return true;
    }

    return false;
}

/**
 * @brief Checks if the delay interval has been reached or exceeded.
 *
 * This method checks whether the delay interval has been reached or exceeded,
 * without automatically resetting the internal timestamp. If the interval is
 * reached and a callback function has been set, the callback function will
 * be invoked.
 *
 * This is useful for executing large blocks of code where the timer should be
 * reset manually. If the timer is not reset manually, this function will
 * continuously return true as long as the interval is non-zero.
 *
 * @code
 * if (loop.isDone()) {
 *   // Long-running code
 *   loop.resetTime();  // Manual timer reset
 * }
 * @endcode
 *
 * @note Never returns true if the interval is zero.
 *
 * @return `true` if the delay interval is reached or exceeded (and invokes the
 * callback function if set), `false` otherwise.
 */
bool AsyncDelay::isReady() {
    bool result = this->isDone();
    if (result) {
        this->resetTime();
    }

    return result;
}

/**
 * @brief Retrieves the count of times the loop object has been active.
 *
 * This method returns the number of times the object's timer has reached or
 * exceeded the specified delay interval. Essentially, it tells you how many
 * times either `isDone()` or `isReady()` has returned `true`.
 *
 * @return The count of times the loop object has been active.
 */
unsigned long AsyncDelay::getCount() {
    return this->count;
}

/**
 * @brief Resets the activation count to zero.
 *
 * This method resets the internal counter that keeps track of how many times
 * the loop object has been active (i.e., the timer has reached or exceeded the
 * specified delay interval).
 */
void AsyncDelay::resetCount() {
    this->count = 0;
}

/**
 * @brief Checks if the count of activations is even.
 *
 * This method returns `true` if the internal counter is an even number and
 * non-zero.
 *
 * @note Always returns `false` if the counter is zero.
 *
 * @return `true` if the count is even and non-zero, `false` otherwise.
 */
bool AsyncDelay::isEven() {
    return this->count % 2 == 0 && this->count != 0;
}

/**
 * @brief Checks if the count of activations is odd.
 *
 * This method returns `true` if the internal counter is an odd number and
 * non-zero.
 *
 * @note Always returns `false` if the counter is zero.
 *
 * @return `true` if the count is odd and non-zero, `false` otherwise.
 */
bool AsyncDelay::isOdd() {
    return this->count % 2 != 0 && this->count != 0;
}

/**
 * @brief Checks if the loop object has never been active.
 *
 * This method returns `true` if the internal counter is zero, indicating that
 * the loop object has never reached or exceeded the specified delay interval.
 *
 * @return `true` if the loop object has never been active, `false` otherwise.
 */
bool AsyncDelay::isNever() {
    return this->count == 0;
}
