/**
 * @file AsyncDelay.h
 *
 * @brief Provides a library for managing asynchronous delays and timeouts in
 * microcontrollers.
 *
 * @author boolscope
 */
#ifndef _ASYNC_DELAY_H
#define _ASYNC_DELAY_H

#include <limits.h>

/**
 * @brief Type definition for a callback function with no arguments and no
 * return value.
 */
typedef void (*CallbackFunction)();

/**
 * @class AsyncDelay
 * @brief This class facilitates creating non-blocking delays and timeouts.
 *
 * The AsyncDelay class allows you to create delays and timeouts that are
 * non-blocking, meaning your program can perform other tasks while waiting.
 * This improves the efficiency of the program, especially when running on
 * a single-threaded microcontroller environment.
 */
class AsyncDelay {
private:
    /** @brief The number of times the AsyncDelay instance has been triggered.
     */
    unsigned long count = 0;

    /** @brief The time interval (in milliseconds) after which the AsyncDelay
     * object becomes ready.
     */
    unsigned long interval = 0;

    /** @brief The last time (in milliseconds) the AsyncDelay object was
     * triggered or initialized.
     */
    unsigned long timestamp = 0;

    /**
     * @brief Indicates whether the timer is paused.
     *
     * This member variable keeps track of whether the timer is currently
     * paused. By default, it is set to true because the default interval is 0.
     */
    bool isPaused = true;  // because default interval is 0

    /**
     * @brief Stores the callback function to be invoked when the timer expires.
     *
     * This member variable holds a pointer to a callback function that is
     * invoked when the timer expires.
     */
    CallbackFunction callbackFunction = nullptr;

public:
    // Minimum allowed interval in milliseconds.
    static const unsigned long MIN_INTERVAL = 0;  // 0ms is allowed too

    // Maximum allowed interval in milliseconds.
    static const unsigned long MAX_INTERVAL = 36000000;  // 10 hours

    /** @brief Constructs a new AsyncDelay object.
     *
     * Initializes a new AsyncDelay object and optionally sets the delay
     * interval during object creation.
     *
     * @param[in] interval The delay time in milliseconds. Defaults to 0.
     */
    AsyncDelay(unsigned long interval = 0);

    /** @brief Destructor.
     *
     * Destroys the AsyncDelay object, performing any necessary cleanup.
     */
    ~AsyncDelay() = default;

    /** @brief Configures the delay interval for the AsyncDelay object.
     *
     * Sets the amount of time (in milliseconds) the AsyncDelay object will
     * wait before transitioning to an active state.
     *
     * @param[in] interval The desired delay time in milliseconds.
     */
    void setInterval(unsigned long interval);

    /**
     * @brief Pauses the timer.
     *
     * This method pauses the timer, stopping it from reaching the 'done' state
     * until it is resumed.
     */
    void pause();

    /**
     * @brief Resumes the timer.
     *
     * This method resumes a paused timer, allowing it to reach the 'done'
     * state.
     */
    void resume();

    /**
     * @brief Sets the callback function for the timer.
     *
     * This method sets a callback function that will be invoked when the timer
     * reaches the 'done' state.
     *
     * @param[in] cbFn The callback function.
     */
    void setCallback(CallbackFunction cbFn);

    /**
     * @brief Checks if a callback function is set.
     *
     * This method returns true if a callback function has been set for this
     * timer.
     *
     * @return True if a callback function is set, false otherwise.
     */
    bool hasCallback();

    /**
     * @brief Retrieves the current callback function.
     *
     * This method returns the current callback function set for this timer.
     *
     * @return The current callback function.
     */
    CallbackFunction getCallback();

    /** @brief Retrieves the configured delay interval of the AsyncDelay obj.
     *
     * Gets the amount of time (in milliseconds) that the AsyncDelay object
     * will wait before becoming active.
     *
     * @return The configured delay time in milliseconds.
     */
    unsigned long getInterval();

    /** @brief Resets the internal timestamp to the current time.
     *
     * Updates the internal timestamp with the current time from millis().
     * This effectively resets any counting towards the next activation period.
     *
     * @return void
     */
    void resetTime();

    /** @brief Calculates the time elapsed since the last reset.
     *
     * This function returns the difference, in milliseconds, between the
     * current system time (as obtained by millis()) and the internal timestamp
     * set by the last call to resetTime() or during object initialization.
     * This can be useful for understanding how close the object is to
     * transitioning to its 'ready' or 'done' state.
     *
     * @return The time difference in milliseconds.
     */
    unsigned long getDelta();

    /** @brief Checks if the loop object's delay interval has expired.
     *
     * @retval true if the loop object's delay interval has expired.
     * @retval false otherwise.
     *
     * This function will not return true if the interval is set to zero.
     * If a callback function is set, it will not be invoked by this method.
     *
     * Unlike isReady(), this function does not automatically reset the timer.
     * It's useful for scenarios where you have a longer block of code and you
     * want manual control over the timer reset.
     *
     * @code
     * if (loop.isDone()) {
     *   // Long-running code here...
     *   loop.resetTime();  // Manual reset
     * }
     * @endcode
     *
     * If the timer is not manually reset, the function will enter an "open
     * state", meaning it will continuously return true if interval is
     * non-zero.
     */
    bool isDone();

    /** @brief Checks if the loop object is ready to execute its task.
     *
     * @retval true if the loop object's delay interval has expired.
     * @retval false otherwise.
     *
     * If a callback function is set, it will be invoked automatically when
     * this function returns true.
     *
     * This function will not return true if the interval is zero.
     *
     * Unlike isDone(), this function automatically resets the timer when
     * it returns true. This is useful for quick operations.
     *
     * @code
     * if (loop.isReady()) {
     *   // Quick operation here...
     * }
     * @endcode
     *
     * Note: This function may not be suitable for long-running code blocks,
     * as it resets the timer automatically, reducing the effective delay
     * before the next check.
     *
     * @code
     * // loop.getInterval() == 300
     * if (loop.isReady()) {
     *   delay(200);
     * } else {
     *   // This block will only execute for 100ms instead of 300ms.
     * }
     * @endcode
     */
    bool isReady();

    /** @brief Gets the number of times the loop object has become active.
     *
     * This function returns the count of times the object's delay interval has
     * expired.
     *
     * @return The number of times the loop object has been ready.
     */
    unsigned long getCount();

    /** @brief Resets the activation count to zero.
     *
     * Resets the internal counter that keeps track of the number of times
     * the loop object has become active.
     *
     * @return void
     */
    void resetCount();

    /** @brief Checks if the loop object has been active an even number of
     * times.
     *
     * @retval true If the internal counter is an even number.
     * @retval false Otherwise, including when the counter is zero.
     */
    bool isEven();

    /** @brief Checks if the loop object has been active an odd number of times.
     *
     * @retval true If the internal counter is an odd number.
     * @retval false Otherwise, including when the counter is zero.
     */
    bool isOdd();

    /** @brief Checks if the loop object has never been ready.
     *
     * @retval true If the internal counter is zero, indicating the object has
     * never been ready.
     * @retval false Otherwise.
     */
    bool isNever();
};

#endif  // _ASYNC_DELAY_H
