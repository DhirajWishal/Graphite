// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include <mutex>

/**
 * Guarded class.
 * This class stores a single variable of the given type and accessing it requires locking a mutex.
 * There are unsafe methods for performance concerns but it's recommended to use the safe functions when accessing the variable, especially when attached
 * to another thread.
 *
 * @tparam Type The type to store.
 * @tparam Mutex The mutex type. Default is std::mutex.
 */
template<class Type, class Mutex = std::mutex>
class Guarded final
{
public:
	/**
	 * Default constructor.
	 */
	Guarded() = default;

	/**
	 * Copy constructor for the variable.
	 *
	 * @param value The value to be copied.
	 */
	explicit Guarded(const Type& value) : m_Variable(value) {}

	/**
	 * Move constructor for the variable.
	 *
	 * @param value The value to be moved.
	 */
	explicit Guarded(Type&& value) : m_Variable(std::move(value)) {}

	/**
	 * Default copy constructor.
	 */
	Guarded(const Guarded&) = default;

	/**
	 * Default move constructor.
	 */
	Guarded(Guarded&&) noexcept = default;

	/**
	 * Default destructor.
	 */
	~Guarded() = default;

	/**
	 * Safely access the stored variable.
	 *
	 * @tparam Function The callback function type.
	 * @param function The function that will be called.
	 * @return The return of the function.
	 */
	template<class Function>
	decltype(auto) access(Function&& function)
	{
		const auto lock = std::scoped_lock(m_Mutex);
		return function(m_Variable);
	}

	/**
	 * Get the internally stored variable reference.
	 * Note that this function call is unsafe!
	 *
	 * @return The variable reference.
	 */
	[[nodiscard]] Type& getUnsafe() { return m_Variable; }

	/**
	 * Get the internally stored variable const reference.
	 * Note that this function call is unsafe!
	 *
	 * @return The variable const reference.
	 */
	[[nodiscard]] const Type& getUnsafe() const { return m_Variable; }

	/**
	 * Set the a value to the stored variable.
	 * This function is safe.
	 *
	 * @param value The value to set.
	 */
	void set(const Type& value)
	{
		const auto lock = std::scoped_lock(m_Mutex);
		m_Variable = value;
	}

	/**
	 * Set the a value to the stored variable.
	 * This function is safe.
	 *
	 * @param value The value to set.
	 */
	void set(Type&& value)
	{
		const auto lock = std::scoped_lock(m_Mutex);
		m_Variable = std::move(value);
	}

	/**
	 * Set the a value to the stored variable.
	 * Note that this function is unsafe!
	 *
	 * @param value The value to set.
	 */
	void setUnsafe(const Type& value)
	{
		m_Variable = value;
	}

	/**
	 * Set the a value to the stored variable.
	 * Note that this function is unsafe!
	 *
	 * @param value The value to set.
	 */
	void setUnsafe(Type&& value)
	{
		m_Variable = std::move(value);
	}

	/**
	 * Default copy assign operator.
	 */
	Guarded& operator=(const Guarded&) = default;

	/**
	 * Default move assign operator.
	 */
	Guarded& operator=(Guarded&&) noexcept = default;

	/**
	 * Value copy assign operator.
	 * 
	 * @param value the value to assign.
	 * @return The guarded lock reference.
	 */
	Guarded& operator=(const Type& value)
	{
		set(value);
		return *this;
	}

	/**
	 * Value move assign operator.
	 *
	 * @param value the value to assign.
	 * @return The guarded lock reference.
	 */
	Guarded& operator=(Type&& value) noexcept
	{
		set(std::move(value));
		return *this;
	}

private:
	Type m_Variable;
	Mutex m_Mutex;
};